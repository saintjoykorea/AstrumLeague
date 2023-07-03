package kr.co.brgames.cdk;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.KeyguardManager;
import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.ClipboardManager;
import android.content.ClipData;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.Color;
import android.media.AudioManager;
import android.net.Uri;
import android.net.UrlQuerySanitizer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Process;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class CSActivity extends Activity {
    private static final String TAG = "CSActivity";

    private CSView _view;
    private ViewGroup _viewGroup;
    private Handler _handler;
    private List<Runnable> _finishRunnables;
    private List<ActivityResultListener> _activityResultListeners;
    private List<RequestPermissionsResultListener> _requestPermissionsResultListeners;
    private List<ActivityRestartListener> _activityRestartListeners;
    private String _version;
	private String _storagePath;
    private Boolean _visible;
    private CSKeyboardHeightProvider _keyboardHeightProvider;
    
    private static CSActivity _sharedActivity;
    public static CSActivity sharedActivity() {
        return _sharedActivity;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (_sharedActivity != null) {
            Log.e(TAG, "duplicate activity");
            System.exit(0);
            return;
        }
        _sharedActivity = this;
        
        CSExceptionHandler.start();

        _handler = new Handler();
        _activityResultListeners = new ArrayList<>();
        _requestPermissionsResultListeners = new ArrayList<>();
        _activityRestartListeners = new ArrayList<>();

        hideSoftKey();
		
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            _storagePath = getExternalFilesDir(null).getAbsolutePath();
        }
        else {
            _storagePath = getFilesDir().getAbsolutePath();
        }

        _view = new CSView();

        ViewGroup.LayoutParams frameLayoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        _viewGroup = new FrameLayout(this);
        _viewGroup.setLayoutParams(frameLayoutParams);
        _viewGroup.setBackgroundColor(Color.BLACK);
        _viewGroup.addView(_view);
        setContentView(_viewGroup);

        _keyboardHeightProvider = new CSKeyboardHeightProvider();

        _view.getLayoutParams().width = _view.width();
        _view.getLayoutParams().height = _view.height();

        _view.post(() -> _keyboardHeightProvider.start());

        Bundle bundle = getIntent().getExtras();
        if (bundle != null) {
            _view.onReceiveNotification(bundleToJSON(bundle));
        }

        _visible = true;
        CSNotification.createChannel();

        getWindow().setSharedElementsUseOverlay(false);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        parseUrl(getIntent());
    }

    private void hideSoftKey() {
        final int flags = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | 4096;     //View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

        getWindow().getDecorView().setSystemUiVisibility(flags);

        // Code below is to handle presses of Volume up or Volume down.
        // Without this, after pressing volume buttons, the navigation bar will
        // show up and won't hide
        final View decorView = getWindow().getDecorView();

        decorView.setOnSystemUiVisibilityChangeListener(visibility -> {
            if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                decorView.setSystemUiVisibility(flags);
            }
        });
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }

    @Override
    public void onLowMemory() {
        _view.onMemoryWarning(CSView.MemoryWarningLevelCritical);

        super.onLowMemory();
    }

    @Override
    @SuppressLint("SwitchIntDef")
    public void onTrimMemory(int level) {
        switch (level) {
            case TRIM_MEMORY_RUNNING_MODERATE:
                _view.onMemoryWarning(CSView.MemoryWarningLevelNormal);
                break;
            case TRIM_MEMORY_MODERATE:
            case TRIM_MEMORY_RUNNING_LOW:
                _view.onMemoryWarning(CSView.MemoryWarningLevelLow);
                break;
            case TRIM_MEMORY_COMPLETE:
            case TRIM_MEMORY_RUNNING_CRITICAL:
                _view.onMemoryWarning(CSView.MemoryWarningLevelCritical);
                break;
        }
        super.onTrimMemory(level);
    }

    private String bundleToJSON(Bundle bundle) {
        JSONObject object = new JSONObject();

        Set<String> keys = bundle.keySet();
        for (String key : keys) {
            Object value = bundle.get(key);

            if (value != null) {
                try {
                    object.put(key, value.toString());
             } catch (JSONException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
        return object.toString();
    }

    public static final String NOTIFICATION_ACTION = "com.google.android.c2dm.intent.RECEIVE";

    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (action.equals(Intent.ACTION_USER_PRESENT)) {
                _view.onResume();
            } else if (action.equals(NOTIFICATION_ACTION)) {
                Bundle bundle = intent.getExtras();

                if (bundle != null) {
                    _view.onReceiveNotification(bundleToJSON(bundle));
                }
            }
        }
    };

    @Override
    public void onBackPressed() {
        _view.onBackPressed();
    }

    @Override
    protected void onResume() {
        super.onResume();

        _visible = true;

        IntentFilter filter = new IntentFilter();
        filter.addAction(NOTIFICATION_ACTION);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        registerReceiver(receiver, filter);

        KeyguardManager keyguardManager = (KeyguardManager) getSystemService(Context.KEYGUARD_SERVICE);
        if (!keyguardManager.inKeyguardRestrictedInputMode()) {
            _view.onResume();
        }
    }

    @Override
    protected void onPause() {
        unregisterReceiver(receiver);
        _visible = false;
        _view.onPause();
        super.onPause();
    }

    @Override
    public void onDestroy() {
        _keyboardHeightProvider.close();

        _finishRunnables = new ArrayList<>();
        _handler.removeCallbacksAndMessages(null);

        _view.onDestroy();

        for (Runnable r : _finishRunnables) r.run();

        super.onDestroy();

        _visible = false;

        _finishRunnables = null;
        _sharedActivity = null;

        Process.killProcess(Process.myPid());
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        parseUrl(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        for (ActivityResultListener listener : new ArrayList<>(_activityResultListeners)) {
            listener.onActivityResult(requestCode, resultCode, data);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onRestart() {
        _visible = true;

        for (ActivityRestartListener listener : new ArrayList<>(_activityRestartListeners)) {
            listener.onActivityRestart();
        }
        super.onRestart();
    }

    public void addActivityResultListener(ActivityResultListener listener) {
        if (!_activityResultListeners.contains(listener)) {
            _activityResultListeners.add(listener);
        }
    }

    public void removeActivityResultListener(ActivityResultListener listener) {
        _activityResultListeners.remove(listener);
    }

    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        for (RequestPermissionsResultListener listener : new ArrayList<>(_requestPermissionsResultListeners)) {
            listener.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    public void addRequestPermissionsResultListener(RequestPermissionsResultListener listener) {
        if (!_requestPermissionsResultListeners.contains(listener)) {
            _requestPermissionsResultListeners.add(listener);
        }
    }

    public void removeRequestPermissionsResultListener(RequestPermissionsResultListener listener) {
        _requestPermissionsResultListeners.remove(listener);
    }

    public void addActivityRestartListener(ActivityRestartListener listener) {
        if (!_activityRestartListeners.contains(listener)) {
            _activityRestartListeners.add(listener);
        }
    }

    public void removeActivityRestartListener(ActivityRestartListener listener) {
        _activityRestartListeners.remove(listener);
    }

    public void queueEvent(Runnable r) {
        if (_finishRunnables == null) _handler.post(r);
        else _finishRunnables.add(r);
    }

    public void queueEvent(Runnable r, int delay) {
        if (_finishRunnables == null) _handler.postDelayed(r, delay);
        else _finishRunnables.add(r);
    }

    public CSView view() {
        return _view;
    }

    public ViewGroup viewGroup() { return _viewGroup; }
	
	public String storagePath() {
		return _storagePath;
	}

    public interface ActivityResultListener {
        void onActivityResult(int requestCode, int resultCode, Intent data);
    }

    public interface RequestPermissionsResultListener {
        void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults);
    }

    public interface ActivityRestartListener {
        void onActivityRestart();
    }

    public static void finishShared() {
        _sharedActivity.queueEvent(() -> {
            _sharedActivity.moveTaskToBack(true);	// 태스크를 백그라운드로 이동
            _sharedActivity.finishAndRemoveTask();			// 액티비티 종료 + 태스크 리스트에서 지우기
            //Process.killProcess(Process.myPid());         // finish를 하면 onDestroy가 호출되고 onDestroy가 종료시 프로세스틀 삭제. onDestroy 에서 에러 발생시 문제 여지 있음
        });
    }

    public static void shareUrl(final String title, final String text, final String url) {
        _sharedActivity.queueEvent(() -> {
            Intent intent = new Intent(Intent.ACTION_SEND);

            intent.addCategory(Intent.CATEGORY_DEFAULT);
            intent.putExtra(Intent.EXTRA_TEXT, text + "\n" + url);
            intent.setType("text/plain");

            _sharedActivity.startActivity(Intent.createChooser(intent, title));
        });
    }
	
    public static void setVersion(String version) {
        _sharedActivity._version = version;
    }

    public String version() {
        return _version;
    }

    public static void openURL(String url) {
        Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        try {
            _sharedActivity.startActivity(i);
        }
        catch (ActivityNotFoundException e) {
            Log.e(TAG, e.getMessage(), e);
        }
    }

    private static CharSequence clipboardReturn;
    public static String clipboard() {
        Runnable r = () -> {
            ClipboardManager clipboardManager = (ClipboardManager)_sharedActivity.getSystemService(CLIPBOARD_SERVICE);
            ClipData clip = clipboardManager.getPrimaryClip();
            if (clip != null && clip.getItemCount() > 0) {
                ClipData.Item item = clip.getItemAt(0);
                clipboardReturn = item.getText();
            }
            else {
                clipboardReturn = null;
            }
            synchronized (_sharedActivity) {
                _sharedActivity.notify();
            }
        };
        synchronized (_sharedActivity) {
            _sharedActivity.queueEvent(r);
            try {
                _sharedActivity.wait();
            }
            catch (InterruptedException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
        return clipboardReturn != null ? clipboardReturn.toString() : null;
    }

    public static void setClipboard(final String text) {
        _sharedActivity.queueEvent(() -> {
            ClipboardManager clipboardManager = (ClipboardManager)_sharedActivity.getSystemService(CLIPBOARD_SERVICE);
            ClipData clip = ClipData.newPlainText("clip", text);
            clipboardManager.setPrimaryClip(clip);
        });
    }

    public static void setResolution(final int resolution) {
        _sharedActivity.queueEvent(() -> _sharedActivity._view.setResolution(resolution));
    }

    public static int resolution() {
        return _sharedActivity._view.resolution();
    }

    protected void parseUrl(Intent intent) {
        if (intent == null) {
            return;
        }

        Uri uri = intent.getData();
        if (uri == null) {
            return;
        }

        UrlQuerySanitizer sanitizer = new UrlQuerySanitizer(uri.toString());
        JSONObject json = new JSONObject();

        List<UrlQuerySanitizer.ParameterValuePair> params = sanitizer.getParameterList();
        for (int i = 0; i < params.size(); i++) {
            UrlQuerySanitizer.ParameterValuePair pair = params.get(i);

            try {
                json.put(pair.mParameter, pair.mValue);
         } catch (JSONException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }

        _view.onReceiveUrl(uri.toString(), json.toString());
    }

    public String resourceString(String id) {
        Resources resources = getResources();
        String pkgName = getPackageName();

        int resId = resources.getIdentifier(id, "string", pkgName);

        return (resId == 0) ? null : resources.getString(resId);
    }

    public int resourceAsId(String id) {
        Resources resources = getResources();
        String pkgName = getPackageName();

        return resources.getIdentifier(id, "drawable", pkgName);
    }

    public boolean isVisible() {
        return _visible;
    }

    static {
        System.loadLibrary("cdkapp");
    }
}
