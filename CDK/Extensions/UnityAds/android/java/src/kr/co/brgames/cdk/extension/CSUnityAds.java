package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.unity3d.ads.IUnityAdsInitializationListener;
import com.unity3d.ads.IUnityAdsLoadListener;
import com.unity3d.ads.IUnityAdsShowListener;
import com.unity3d.ads.UnityAds;

import kr.co.brgames.cdk.CSActivity;

class CSUnityAdsImpl {
    private static final String TAG = "CSUnityAds";

    private final String _appId;
    private final String _unitId;

    private boolean _loading;
    private boolean _showing;

    public CSUnityAdsImpl(boolean testMode) {
        _appId = CSActivity.sharedActivity().resourceString("unity_ads_app_id");
        _unitId = CSActivity.sharedActivity().resourceString("unity_ads_unit_id");

        Log.i(TAG, "Start Init");

        assert _appId != null;
        assert _unitId != null;

        UnityAds.initialize(CSActivity.sharedActivity().getApplicationContext(), _appId, testMode, new IUnityAdsInitializationListener() {
            @Override
            public void onInitializationComplete() {
                Log.d(TAG, "initialize - onInitializationComplete");

                load();
            }

            @Override
            public void onInitializationFailed(UnityAds.UnityAdsInitializationError unityAdsInitializationError, String s) {
                Log.e(TAG, "Init Failed. Error Code : " + unityAdsInitializationError.name() + " " + unityAdsInitializationError.hashCode());
            }
        });
    }

    private void load() {
        _loading = true;

        Log.i(TAG, "Start Load");

        if (UnityAds.isInitialized()) {
            Log.i(TAG, "UnityAds is Initialized.");

            UnityAds.load(_appId, new IUnityAdsLoadListener() {
                @Override
                public void onUnityAdsAdLoaded(String s) {
                    Log.i(TAG, "onUnityAdsAdLoaded");

                    _loading = false;
                }

                @Override
                public void onUnityAdsFailedToLoad(String s, UnityAds.UnityAdsLoadError unityAdsLoadError, String s1) {
                    Log.e(TAG, "Load Failed. Error Code : " + unityAdsLoadError.hashCode());
                    Log.e(TAG, "Load Failed. Error Message : " + s1);

                    _loading = false;

                    if (_showing) {
                        _showing = false;

                        CSActivity.sharedActivity().view().queueEvent(() -> CSUnityAds.nativeFail(unityAdsLoadError.toString()));
                    }
                }
            });
        }
        else {
            Log.i(TAG, "UnityAds isn't Initialized");
            _loading = false;
        }
    }

    public void show() {
        Log.i(TAG, "Start ad showing");

        _showing = true;

        if (UnityAds.isInitialized()) {
            UnityAds.show(CSActivity.sharedActivity(), _unitId, new IUnityAdsShowListener() {
                @Override
                public void onUnityAdsShowFailure(String s, UnityAds.UnityAdsShowError unityAdsShowError, String s1) {
                    Log.i(TAG, "onUnityAdsShowFailure");
                    Log.i(TAG, "error : " + s1);

                    CSActivity.sharedActivity().view().queueEvent(() -> CSUnityAds.nativeFail(unityAdsShowError.toString()));
                }

                @Override
                public void onUnityAdsShowStart(String s) {
                    Log.i(TAG, "onUnityAdsShowStart");
                }

                @Override
                public void onUnityAdsShowClick(String s) {
                    Log.i(TAG, "onUnityAdsShowClick");
                }

                @Override
                public void onUnityAdsShowComplete(String s, UnityAds.UnityAdsShowCompletionState unityAdsShowCompletionState) {
                    Log.i(TAG, "onUnityAdsShowComplete");

                    _showing = false;

                    if (unityAdsShowCompletionState == UnityAds.UnityAdsShowCompletionState.COMPLETED) {
                        CSActivity.sharedActivity().view().queueEvent(CSUnityAds::nativeReward);
                    }
                    else {
                        CSActivity.sharedActivity().view().queueEvent(CSUnityAds::nativeClose);
                    }
                }
            });
        }
        else if (!_loading) {
            Log.i(TAG, "Ad Can't Show.");

            load();
        }
    }
}

public class CSUnityAds {
    private static final String TAG = "CSUnityAds";

    private static CSUnityAdsImpl _impl;

    public static void initialize(boolean testMode) {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSUnityAdsImpl(testMode));
    }

    public static void dispose() {
        _impl = null;
    }

    public static void show() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.show());
    }

    public static native void nativeReward();
    public static native void nativeClose();
    public static native void nativeFail(String msg);
}
