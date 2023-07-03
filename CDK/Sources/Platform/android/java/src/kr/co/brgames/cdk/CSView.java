package kr.co.brgames.cdk;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

@SuppressLint("ViewConstructor")
public class CSView extends GLSurfaceView {
    private final CSViewSize _size;
    private final List<Runnable> _events;
    private int _status;
    private int _resolution;

    private static final int STATUS_INIT = 0;
    private static final int STATUS_PAUSED = 1;
    private static final int STATUS_ACTIVE = 2;
    private static final int STATUS_DESTROYED = 3;

    public static final int RESOLUTION_720 = 0;
    public static final int RESOLUTION_1080 = 1;
    public static final int RESOLUTION_FIT = 2;
    
    private static final String TAG = "CSView";

    public CSView() {
        super(CSActivity.sharedActivity());

        _events = new ArrayList<>();

        ActivityManager activityManager =
                (ActivityManager) CSActivity.sharedActivity().getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo =
                activityManager.getDeviceConfigurationInfo();
        int glesver = configurationInfo.reqGlEsVersion >= 0x30000 ? 3 : 2;
        Log.v(TAG, String.format("opengl es version : %d", glesver));
        setEGLContextClientVersion(glesver);
        setFocusableInTouchMode(true);
        setRenderer(renderer);

        _size = new CSViewSize(this);

        loadResolution();

        applyResolution();

        setPreserveEGLContextOnPause(true);
    }

    private File resolutionFile() {
        return new File(CSActivity.sharedActivity().storagePath() + "/resolution.xmf");
    }

    private void loadResolution() {
        File f = resolutionFile();

        _resolution = RESOLUTION_1080;

        if (f.exists()) {
            try {
                try (InputStream is = new FileInputStream(f)) {
                    _resolution = is.read();
                }
            } catch (IOException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
    }

    private void saveResolution() {
        File f = resolutionFile();

        if (_resolution != RESOLUTION_1080) {
            try {
                try (OutputStream os = new FileOutputStream(f)) {
                    os.write(_resolution);
                }
            } catch (IOException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
        else {
            f.delete();
        }
    }

    private void applyResolution() {
        int width, height;

        switch(_resolution) {
            case RESOLUTION_720:
                width = 720;
                height = 1280;
                break;
            case RESOLUTION_1080:
                width = 1080;
                height = 1920;
                break;
            default:
                getHolder().setSizeFromLayout();
                Log.i(TAG, String.format("resolution:%d, %d, %d", _resolution, _size.width(), _size.height()));
                return;
        }
        if (_size.isLandscape()) {
            int temp = width;
            width = height;
            height = temp;
        }

        float rate = Math.max((float)width / _size.width(), (float)height / _size.height());

        if (rate < 1.0f) {
            width = (int)Math.ceil(_size.width() * rate);
            height = (int)Math.ceil(_size.height() * rate);
            getHolder().setFixedSize(width, height);
        }
        else {
            width = _size.width();
            height = _size.height();
            getHolder().setSizeFromLayout();
        }

        Log.i(TAG, String.format("resolution:%d, %d, %d ", _resolution, width, height));
    }

    public void setResolution(int resolution) {
        if (_resolution != resolution) {
            _resolution = resolution;

            applyResolution();

            saveResolution();
        }
    }

    public int resolution() {
        return _resolution;
    }

    public int width() {
        return _size.width();
    }

    public int height() {
        return _size.height();
    }

    @Override
    public void onPause() {
        Runnable r = () -> {
            if (_status == STATUS_ACTIVE) {
                nativePause();
                _status = STATUS_PAUSED;
            }
            synchronized (CSView.this) {
                CSView.this.notify();
            }
        };

        synchronized (this) {
            super.queueEvent(r);
            try {
                wait();
            }
            catch (InterruptedException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
        super.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    public void onDestroy() {
        Runnable r = () -> {
            if (_status != STATUS_DESTROYED) {
                _status = STATUS_DESTROYED;
                nativeDestroy();
            }
            synchronized (CSView.this) {
                CSView.this.notify();
            }
        };
        synchronized (this) {
            super.queueEvent(r);
            try {
                wait();
            }
            catch (InterruptedException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
        CSURLConnectionBridge.dispose();
    }

    public static int MemoryWarningLevelNormal = 0;
    public static int MemoryWarningLevelLow = 1;
    public static int MemoryWarningLevelCritical = 2;

    public void onMemoryWarning(final int level) {
        Log.i(TAG, "onMemoryWarning:" + level);

        Runnable r = () -> {
            switch (_status) {
                case STATUS_PAUSED:
                case STATUS_ACTIVE:
                    nativeMemoryWarning(level);
                    break;
            }
        };
        super.queueEvent(r);
    }

    @Override
    public void queueEvent(Runnable r) {
        synchronized (_events) {
            _events.add(r);
        }
    }

    public void onReceiveNotification(final String query) {
        queueEvent(() -> nativeReceiveQuery(null, query));
    }

    public void onReceiveUrl(final String url, final String query) {
        queueEvent(() -> nativeReceiveQuery(url, query));
    }

    public void onBackPressed() {
        queueEvent(CSView::nativeBackKey);
    }

    public void onKeyboardHeightChanged(final int height) {
        queueEvent(() -> nativeKeyboardHeightChanged(height));
    }
    // /////////////////////////////////////////////////////////////////////////
    // touch event
    // /////////////////////////////////////////////////////////////////////////

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_POINTER_DOWN:
                {
                    final int idx = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                    final int ident = event.getPointerId(idx);
                    final float x = event.getX(idx);
                    final float y = event.getY(idx);

                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesBegan(ident, x, y);
                        }
                    });
                }
                break;
            case MotionEvent.ACTION_DOWN:
                {
                    final int ident = event.getPointerId(0);
                    final float x = event.getX(0);
                    final float y = event.getY(0);

                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesBegan(ident, x, y);
                        }
                    });
                }
                break;
            case MotionEvent.ACTION_MOVE:
                {
                    int count = event.getPointerCount();

                    final int[] ids = new int[count];
                    final float[] xs = new float[count];
                    final float[] ys = new float[count];

                    for (int i = 0; i < count; i++) {
                        ids[i] = event.getPointerId(i);
                        xs[i] = event.getX(i);
                        ys[i] = event.getY(i);
                    }
                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesMoved(ids, xs, ys);
                        }
                    });
                }
                break;
            case MotionEvent.ACTION_POINTER_UP:
                {
                    final int idx = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                    final int ident = event.getPointerId(idx);
                    final float x = event.getX(idx);
                    final float y = event.getY(idx);

                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesEnded(ident, x, y);
                        }
                    });
                }
                break;
            case MotionEvent.ACTION_UP:
                {
                    // there are only one finger on the screen
                    final int ident = event.getPointerId(0);
                    final float x = event.getX(0);
                    final float y = event.getY(0);

                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesEnded(ident, x, y);
                        }
                    });
                    performClick();
                }
                break;
            case MotionEvent.ACTION_CANCEL:
                {
                    int count = event.getPointerCount();

                    final int[] ids = new int[count];
                    final float[] xs = new float[count];
                    final float[] ys = new float[count];

                    for (int i = 0; i < count; i++) {
                        ids[i] = event.getPointerId(i);
                        xs[i] = event.getX(i);
                        ys[i] = event.getY(i);
                    }
                    super.queueEvent(() -> {
                        if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                            nativeTouchesCancelled(ids, xs, ys);
                        }
                    });
                }
                break;
        }
        return true;
    }

    @Override
    public boolean performClick() {
        return super.performClick();
    }

    private final GLSurfaceView.Renderer renderer = new GLSurfaceView.Renderer() {
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            if (_status == STATUS_ACTIVE || _status == STATUS_PAUSED) {
                nativeReload();
            }
        }

        public void onSurfaceChanged(GL10 gl, int w, int h) {
            switch (_status) {
                case STATUS_DESTROYED:
                    break;
                case STATUS_INIT:
                    _status = STATUS_ACTIVE;
                    nativeStart(_size.width(), _size.height(), w, h, _size.horizontalEdge(), _size.verticalEdge(),
						CSActivity.sharedActivity().getAssets(), CSActivity.sharedActivity().storagePath());
                    break;
                case STATUS_PAUSED:
                    _status = STATUS_ACTIVE;
                    nativeResume();
                default:
                    nativeResize(w, h);
                    break;
            }
        }

        public void onDrawFrame(GL10 gl) {
            List<Runnable> events = null;
            synchronized (_events) {
                if (!_events.isEmpty()) {
                    events = new ArrayList<>(_events);
                    _events.clear();
                }
            }
            if (events != null) {
                for (Runnable r : events) {
                    r.run();
                }
            }
            nativeTimeout();
        }
    };

    private static native void nativeStart(int width, int height, int bufferWidth, int bufferHeight, int horizontalEdge, int verticalEdge, Object assetManager, String storagePath);

    private static native void nativeResize(int width, int height);

    private static native void nativeTimeout();

    private static native void nativePause();

    private static native void nativeResume();

    private static native void nativeReload();

    private static native void nativeDestroy();

    private static native void nativeMemoryWarning(int level);

    private static native void nativeReceiveQuery(String url, String json);

    private static native void nativeTouchesBegan(int id, float x, float y);

    private static native void nativeTouchesEnded(int id, float x, float y);

    private static native void nativeTouchesMoved(int[] id, float[] x, float[] y);

    private static native void nativeTouchesCancelled(int[] id, float[] x, float[] y);

    private static native void nativeBackKey();

    private static native void nativeKeyboardHeightChanged(int height);

}


