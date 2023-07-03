package kr.co.brgames.cdk;

import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Point;
import android.os.Build;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.ViewConfiguration;
import android.content.res.Resources;

public class CSViewSize {
    private int _width;
    private int _height;
    private int _horizontalEdge;
    private int _verticalEdge;

    private static final String TAG = "CSViewSize";

    public CSViewSize(View view) {
        for (FixedDeviceInfo info : fixedDeviceInfos) {
            if (info.match()) {
                Log.i(TAG, String.format("init for fixed screen (%s %s)", Build.BRAND, Build.MODEL));

                _width = info.width();
                _height = info.height();
                _horizontalEdge = info.horizontalEdge();
                _verticalEdge = info.verticalEdge();

                if (isLandscape()) {
                    int temp;

                    temp = _width;
                    _width = _height;
                    _height = temp;

                    temp = _horizontalEdge;
                    _horizontalEdge = _verticalEdge;
                    _verticalEdge = temp;
                }
                Log.i(TAG, String.format("size:%d, %d edge:%d, %d", _width, _height, _horizontalEdge, _verticalEdge));
                return;
            }
        }

        Display display = CSActivity.sharedActivity().getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        Point realSize = new Point();
        display.getRealSize(realSize);

        swapSizeIfRotated(view, size);
        swapSizeIfRotated(view, realSize);

        Log.i(TAG, String.format("cutout size:%d, %d full size:%d, %d", size.x, size.y, realSize.x, realSize.y));

        _horizontalEdge = 0;
        _verticalEdge = 0;

        boolean landscale = isLandscape();

        if (Build.VERSION.SDK_INT >= 28) {
            _width = realSize.x;
            _height = realSize.y;
            if (landscale) _horizontalEdge = (realSize.x - size.x) / 2;
            else _verticalEdge = (realSize.y - size.y) / 2;

            Log.i(TAG, String.format("full screen edge:%d, %d", _horizontalEdge, _verticalEdge));

            Resources resources = CSActivity.sharedActivity().getResources();
            int resourceId = resources.getIdentifier("status_bar_height", "dimen", "android");
            if (resourceId > 0) {
                int statusBarHeight = resources.getDimensionPixelSize(resourceId);
                if (landscale) {
                    _horizontalEdge = Math.min(statusBarHeight, _horizontalEdge * 2);
                }
                else {
                    _verticalEdge = Math.min(statusBarHeight, _verticalEdge * 2);
                }
                Log.i(TAG, String.format("full screen status bar height:%d", statusBarHeight));
            }
            else {
                Log.i(TAG, "full screen status bar height not available");
            }
        }
        else {
            _width = size.x;
            _height = size.y;

            boolean hasMenuKey = ViewConfiguration.get(CSActivity.sharedActivity().getApplicationContext()).hasPermanentMenuKey();

            if (!hasMenuKey) {
                Resources resources = CSActivity.sharedActivity().getResources();
                int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
                if (resourceId > 0) {
                    int softMenuHeight = resources.getDimensionPixelSize(resourceId);

                    Log.i(TAG, String.format("soft menu height:%d", softMenuHeight));

                    if (landscale) {
                        _width = Math.min(size.x + softMenuHeight, realSize.x);
                        _horizontalEdge = softMenuHeight / 2;
                    }
                    else {
                        _height = Math.min(size.y + softMenuHeight, realSize.y);
                        _verticalEdge = softMenuHeight / 2;
                    }
                }
                else {
                    Log.w(TAG, "soft menu height not available");
                }
            }
            else {
                Log.i(TAG, "no soft menu");
            }
        }

        Log.i(TAG, String.format("size:%d, %d edge:%d, %d", _width, _height, _horizontalEdge, _verticalEdge));
    }

    public boolean isLandscape() {
        int orientation = CSActivity.sharedActivity().getRequestedOrientation();

        switch (orientation) {
            case ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE:
            case ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE:
            case ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE:
                return true;
            default:
                return false;
        }
    }

    private void swapSizeIfRotated(View view, Point size) {
        int currentOrientation = view.getResources().getConfiguration().orientation;
        int requestedOrientation = CSActivity.sharedActivity().getRequestedOrientation();

        switch (currentOrientation) {
            case Configuration.ORIENTATION_LANDSCAPE:
                switch (requestedOrientation) {
                    // Portrait
                    case ActivityInfo.SCREEN_ORIENTATION_PORTRAIT:
                    case ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT:
                    case ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT:
                        {
                            int temp = size.x;
                            size.x = size.y;
                            size.y = temp;
                        }
                        break;
                }
                break;
            case Configuration.ORIENTATION_PORTRAIT:
                switch (requestedOrientation) {
                    // Landscape
                    case ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE:
                    case ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE:
                    case ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE:
                        {
                            int temp = size.x;
                            size.x = size.y;
                            size.y = temp;
                        }
                        break;
                }
                break;
        }
    }

    public int width() {
        return _width;
    }

    public int height() {
        return _height;
    }

    public int horizontalEdge() {
        return _horizontalEdge;
    }

    public int verticalEdge() {
        return _verticalEdge;
    }

    private static class FixedDeviceInfo {
        private final String _brand;
        private final String[] _models;
        private final int _width;
        private final int _height;
        private final int _horizontalEdge;
        private final int _verticalEdge;

        public FixedDeviceInfo(String brand, String[] models, int width, int height, int horizontalEdge, int verticalEdge) {
            _brand = brand;
            _models = models;
            _width = width;
            _height = height;
            _horizontalEdge = horizontalEdge;
            _verticalEdge = verticalEdge;
        }

        public boolean match() {
            if (_brand.equalsIgnoreCase(Build.BRAND)) {
                for (String model : _models) {
                    if (model.equalsIgnoreCase(Build.MODEL)) {
                        return true;
                    }
                }
            }
            return false;
        }

        public int width() {
            return _width;
        }

        public int height() {
            return _height;
        }

        public int horizontalEdge() {
            return _horizontalEdge;
        }

        public int verticalEdge() {
            return _verticalEdge;
        }
    }

    private static final FixedDeviceInfo[] fixedDeviceInfos = {     //padding을 잘 모를 경우 높이의 0.03
        new FixedDeviceInfo("OPPO", new String[]{"CPH1719", "R11s"}, 1080, 2160, 0, 64),
        new FixedDeviceInfo("OPPO", new String[]{"CPH1879", "PBEM00"}, 1080, 2340, 0, 70),
        new FixedDeviceInfo("OPPO", new String[]{"CPH1725"}, 1080, 2160, 0, 64),
        new FixedDeviceInfo("XIAOMI", new String[]{"M1803E1A"}, 1080, 2248, 0, 68),
        new FixedDeviceInfo("VIVO", new String[]{"1725","X21A","X21UD A","X21i A"}, 1080, 2280, 0, 68)
    };
}
