package kr.co.brgames.cdk.extension;

import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import kr.co.brgames.cdk.CSActivity;

class CSAndroidPermissionsImpl implements CSActivity.ActivityResultListener, CSActivity.RequestPermissionsResultListener {
    private static final int RC_REQUEST_PERMISSIONS = 10000;
    private static final int RC_ACTION_APPLICATION_DETAILS_SETTINGS = 10001;

    private static final String TAG = "CSAndroidPermissions";

    private List<String> _requestedPermissions;
    private List<String> _essentialPermissions;

    public CSAndroidPermissionsImpl() {
        CSActivity.sharedActivity().addActivityResultListener(this);
        CSActivity.sharedActivity().addRequestPermissionsResultListener(this);

        _requestedPermissions = new ArrayList<>();
        _essentialPermissions = new ArrayList<>();
    }

    public void dispose() {
        CSActivity.sharedActivity().removeActivityResultListener(this);
        CSActivity.sharedActivity().removeRequestPermissionsResultListener(this);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == RC_ACTION_APPLICATION_DETAILS_SETTINGS) {
            requestPermissions();
        }
    }

    public void addEssentialPermission(String permission) {
        _essentialPermissions.add(permission);

        Log.i(TAG, "essential permission:" + permission);
    }

    public void checkPermissions() {
        if (Build.VERSION.SDK_INT >= 23) {
            doCheckPermissions();
        }
        else {
            checkDone(CSAndroidPermissions.RESULT_GRANTED);
        }
    }

    private void doCheckPermissions() {
        PackageInfo info = null;

        _requestedPermissions.clear();

        try {
            info = CSActivity.sharedActivity().getPackageManager().getPackageInfo(CSActivity.sharedActivity().getPackageName(), PackageManager.GET_PERMISSIONS);
        }
        catch (Exception e) {
            Log.e(TAG, e.getMessage(), e);
        }

        if (info == null || info.requestedPermissions == null) {
            checkDone(CSAndroidPermissions.RESULT_GRANTED);
            return;
        }

        boolean retry = false;
        for (String permission : info.requestedPermissions) {
            if (ContextCompat.checkSelfPermission(CSActivity.sharedActivity(), permission) == PackageManager.PERMISSION_DENIED) {
                try {
                    //한 번 거부되고 선택사항이면 재요청하지 않음
                    if (ActivityCompat.shouldShowRequestPermissionRationale(CSActivity.sharedActivity(), permission)) {
						//사용자가 명시적으로 거부한 경우
                        if (_essentialPermissions.contains(permission)) {		
                            retry = true;
                            _requestedPermissions.add(permission);
                        }
                    } else {
                        _requestedPermissions.add(permission);
                    }
                }
                catch (Exception e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }

        if (_requestedPermissions.isEmpty()) checkDone(CSAndroidPermissions.RESULT_GRANTED);
        else if (retry) checkDone(CSAndroidPermissions.RESULT_RETRY);
        else requestPermissions();
    }

    public void requestPermissions() {
        if (!_requestedPermissions.isEmpty()) {
            for (String permission : _requestedPermissions) {
                Log.i(TAG, "request permission:" + permission);
            }
            ActivityCompat.requestPermissions(CSActivity.sharedActivity(), _requestedPermissions.toArray(new String[0]), RC_REQUEST_PERMISSIONS);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == RC_REQUEST_PERMISSIONS) {
            _requestedPermissions.clear();
            for (int i = 0; i < grantResults.length; i++) {
                if (grantResults[i] == PackageManager.PERMISSION_DENIED && _essentialPermissions.contains(permissions[i])) {
                    _requestedPermissions.add(permissions[i]);

                    Log.i(TAG, "denied permission:" + permissions[i]);
                }
            }

            int result;
            if (_requestedPermissions.isEmpty()) result = CSAndroidPermissions.RESULT_GRANTED;
            else {
                result = CSAndroidPermissions.RESULT_DENIED;
                for (String permission : _requestedPermissions) {
                    if (ActivityCompat.shouldShowRequestPermissionRationale(CSActivity.sharedActivity(), permission)) {
                        result = CSAndroidPermissions.RESULT_RETRY;
                        break;
                    }
                }
            }
            Log.i(TAG, "permission result:" + result);

            checkDone(result);
        }
    }

    public void showMoveToOptions(String title, String message, String positive, String negative) {
        android.app.AlertDialog.Builder builder = new  android.app.AlertDialog.Builder(CSActivity.sharedActivity(), AlertDialog.THEME_DEVICE_DEFAULT_LIGHT);
        builder.setTitle(title)
                .setMessage(message)
                .setPositiveButton(positive, (dialogInterface, i) -> {
                    Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
                    intent.setData(Uri.parse("package:" + CSActivity.sharedActivity().getPackageName()));
                    CSActivity.sharedActivity().startActivityForResult(intent, RC_ACTION_APPLICATION_DETAILS_SETTINGS);
                })
                .setNegativeButton(negative, (dialogInterface, i) -> CSActivity.finishShared())
                .setCancelable(false)
                .show();
    }

    private void checkDone(final int result) {
        CSActivity.sharedActivity().view().queueEvent(() -> CSAndroidPermissions.nativeCheckDone(result));
    }
}

public class CSAndroidPermissions {
    private static CSAndroidPermissionsImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSAndroidPermissionsImpl());
    }

    public static void dispose() {
        if (_impl != null) {
            CSActivity.sharedActivity().queueEvent(() -> {
                _impl.dispose();
                _impl = null;
            });
        }
    }

    public static void addEssentialPermission(final String essentialPermission) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.addEssentialPermission(essentialPermission));
    }

    public static void checkPermissions() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.checkPermissions());
    }

    public static void requestPermissions() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.requestPermissions());
    }

    public static void showMoveToOptions(final String title, final String message, final String positive, final String negative) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.showMoveToOptions(title, message, positive, negative));
    }

    public static native void nativeCheckDone(int result);

    public static final int RESULT_GRANTED = 0;
    public static final int RESULT_RETRY = 1;
    public static final int RESULT_DENIED = 2;
}