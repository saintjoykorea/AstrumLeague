package kr.co.brgames.cdk.extension;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.Application;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.AudioManager;
import android.media.MediaRecorder;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.SparseIntArray;
import android.view.Surface;
import android.widget.ToggleButton;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import kr.co.brgames.cdk.CSActivity;

class CSScreenRecordImpl implements Application.ActivityLifecycleCallbacks, CSActivity.RequestPermissionsResultListener, CSActivity.ActivityResultListener {
    private static final int RC_PERMISSIONS = 10400;

    private String _path;
    private int mScreenDensity;
    private MediaProjectionManager mProjectionManager;
    private int _displayWidth = 720;
    private int _displayHeight = 480;
    int _audioMode = AudioManager.MODE_NORMAL;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private MediaProjectionCallback mMediaProjectionCallback;
    private ToggleButton mToggleButton;
    private MediaRecorder mMediaRecorder;
    private SimpleDateFormat mSimpleDateFormat;
    private boolean _recording;
    private boolean _micEnabled;

    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();

    static {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    public CSScreenRecordImpl() {
        CSActivity.sharedActivity().getApplication().registerActivityLifecycleCallbacks(this);
        CSActivity.sharedActivity().addRequestPermissionsResultListener(this);
        CSActivity.sharedActivity().addActivityResultListener(this);

        DisplayMetrics metrics = new DisplayMetrics();
        CSActivity.sharedActivity().getWindowManager().getDefaultDisplay().getMetrics(metrics);
        //DISPLAY_WIDTH = metrics.widthPixels;
        //DISPLAY_HEIGHT = metrics.heightPixels;
        mScreenDensity = metrics.densityDpi;

        mMediaRecorder = new MediaRecorder();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            mProjectionManager = (MediaProjectionManager) CSActivity.sharedActivity().getSystemService
                    (Context.MEDIA_PROJECTION_SERVICE);

        }

        _recording = false;
        
        mSimpleDateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode != RC_PERMISSIONS) {
            Log.e("CSScreenRecord.java", "Unknown request code: " + requestCode);
            return;
        }
        if (resultCode != CSActivity.sharedActivity().RESULT_OK) {
            Log.e("CSScreenRecord.java", "Unknown request code: " + resultCode);
            mMediaRecorder.reset();
            _recording = false;
            return;
        }

        mMediaProjectionCallback = new MediaProjectionCallback();
        mMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);
        mMediaProjection.registerCallback(mMediaProjectionCallback, null);

        mVirtualDisplay = createVirtualDisplay();
        mMediaRecorder.start();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void shareScreen() {
        if (mMediaProjection == null) {
            CSActivity.sharedActivity().startActivityForResult(mProjectionManager.createScreenCaptureIntent(), RC_PERMISSIONS);
            return;
        }
        mVirtualDisplay = createVirtualDisplay();
        mMediaRecorder.start();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private VirtualDisplay createVirtualDisplay() {
        return mMediaProjection.createVirtualDisplay("MainActivity",
                _displayWidth, _displayHeight, mScreenDensity,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
                mMediaRecorder.getSurface(), null /*Callbacks*/, null
                /*Handler*/);
    }


    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void initRecorder() {
        try {
            _path = String.format("%s/%s.mp4",
                                        Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES),
                                        mSimpleDateFormat.format(Calendar.getInstance().getTime()));


            AudioManager audioManager = (AudioManager) CSActivity.sharedActivity().getSystemService(Context.AUDIO_SERVICE);
            _audioMode = audioManager.getMode();

            if (!_micEnabled) {
                audioManager.setMode(AudioManager.MODE_IN_COMMUNICATION);
            }

            mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
            mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
            mMediaRecorder.setOutputFile(_path);
            mMediaRecorder.setVideoSize(_displayWidth, _displayHeight);
            mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
            mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            mMediaRecorder.setVideoEncodingBitRate((int)(_displayWidth * _displayHeight * 11.4));
            mMediaRecorder.setVideoFrameRate(30);
            int rotation = CSActivity.sharedActivity().getWindowManager().getDefaultDisplay().getRotation();
            int orientation = ORIENTATIONS.get(rotation );
            mMediaRecorder.setOrientationHint(orientation);
            mMediaRecorder.prepare();

            Log.i("CSScreenRecord.java", "Recording Start:" + _path);
        } catch (IOException e) {
            Log.e("CSScreenRecord.java", "Recording Start Error", e);
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private class MediaProjectionCallback extends MediaProjection.Callback {
        @Override
        public void onStop() {
            if (mToggleButton.isChecked()) {
                mToggleButton.setChecked(false);
                mMediaRecorder.stop();
                mMediaRecorder.reset();

                Log.i("CSScreenRecord.java", "Recording Stopped");
            }
            mMediaProjection = null;
            stopScreenSharing();
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void stopScreenSharing() {
        if (mVirtualDisplay == null) {
            return;
        }
        mVirtualDisplay.release();
        //mMediaRecorder.release(); //If used: mMediaRecorder object cannot
        // be reused again
        ContentValues values = new ContentValues();
        values.put(MediaStore.Video.Media.DATA, _path);
        ContentResolver resolver = CSActivity.sharedActivity().getContentResolver();
        resolver.insert(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, values);


        AudioManager audioManager = (AudioManager) CSActivity.sharedActivity().getSystemService(Context.AUDIO_SERVICE);
        audioManager.setMode(_audioMode);
        destroyMediaProjection();
    }

    public void onDestroy() {
        CSActivity.sharedActivity().removeActivityResultListener(this);
        CSActivity.sharedActivity().getApplication().unregisterActivityLifecycleCallbacks(this);
        CSActivity.sharedActivity().removeRequestPermissionsResultListener(this);
        destroyMediaProjection();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void destroyMediaProjection() {
        if (mMediaProjection != null) {
            mMediaProjection.unregisterCallback(mMediaProjectionCallback);
            mMediaProjection.stop();
            mMediaProjection = null;
        }
        Log.i("CSScreenRecord.java", "MediaProjection Stopped");
    }

    public boolean isRecording() {
        return _recording;
    }

    public boolean isEnabled() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return true;
        }
        return false;
    }

    public static boolean canQualitySelect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return true;
        }
        return false;
    }

    public static boolean canMicSelect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return true;
        }
        return false;
    }


    public void recordStart(int displayWidth, int displayHeight, final boolean micEnabled) {
        _displayWidth = displayWidth;
        _displayHeight = displayHeight;
        _micEnabled = micEnabled;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            if (!checkPermissionFromDevice()) {
                requestPermissionFromDevice();
                return;
            }

            if (!_recording) {
                initRecorder();
                shareScreen();
            }
            _recording = true;
        }
    }

    public void recordEnd() {
        if (_recording) {
            mMediaRecorder.stop();
            mMediaRecorder.reset();
            Log.i("CSScreenRecord.java", "Stopping Recording");
            stopScreenSharing();

        }
        _recording = false;
    }

    private void requestPermissionFromDevice() {
        ActivityCompat.requestPermissions(CSActivity.sharedActivity(),new String[]{
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.RECORD_AUDIO},
                RC_PERMISSIONS);
    }

    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode){
            case RC_PERMISSIONS:
            {
                boolean result = true;
                for (int grantResult : grantResults) {
                    if (grantResult == PackageManager.PERMISSION_DENIED) {
                        result = false;
                    }
                }
                if (result) {
                    recordStart(_displayWidth, _displayHeight, _micEnabled);
                }
            }
            break;
        }
    }

    private boolean checkPermissionFromDevice() {
        int recorder_permssion =ContextCompat.checkSelfPermission(CSActivity.sharedActivity(),Manifest.permission.RECORD_AUDIO);
        int write_permssion =ContextCompat.checkSelfPermission(CSActivity.sharedActivity(),Manifest.permission.WRITE_EXTERNAL_STORAGE);
        return (recorder_permssion == PackageManager.PERMISSION_GRANTED && write_permssion == PackageManager.PERMISSION_GRANTED);
    }

    // Application.ActivityLifecycleCallbacks

    @Override
    public void onActivityCreated(Activity activity, Bundle savedInstanceState) { }

    @Override
    public void onActivityStarted(Activity activity) { }

    @Override
    public void onActivityResumed(Activity activity) { }

    @Override
    public void onActivityPaused(Activity activity) { }

    @Override
    public void onActivityStopped(Activity activity) { }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) { }

    @Override
    public void onActivityDestroyed(Activity activity) {
        if (activity != CSActivity.sharedActivity()) {
            return;
        }
		onDestroy();
    }
}

public class CSScreenRecord {
    private static CSScreenRecordImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSScreenRecordImpl());
    }

    public static boolean isRecording() {
        return _impl.isRecording();
    }
    public static boolean isEnabled() {
        return _impl.isEnabled();
    }
    public static boolean canQualitySelect() {
        return _impl.canQualitySelect();
    }
    public static boolean canMicSelect() {
        return _impl.canMicSelect();
    }

    public static void recordStart(final int displayWidth, final int displayHeight, final boolean micEnabled) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.recordStart(displayWidth, displayHeight, micEnabled));
    }

    public static void recordEnd() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.recordEnd());
    }
}
