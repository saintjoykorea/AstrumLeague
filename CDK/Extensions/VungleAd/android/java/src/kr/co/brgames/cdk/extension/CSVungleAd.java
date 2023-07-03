package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.vungle.warren.AdConfig;
import com.vungle.warren.Vungle;
import com.vungle.warren.InitCallback;          // Initialization callback
import com.vungle.warren.LoadAdCallback;        // Load ad callback
import com.vungle.warren.PlayAdCallback;        // Play ad callback
import com.vungle.warren.error.VungleException; // onError message
import static com.vungle.warren.Vungle.getValidPlacements;

import kr.co.brgames.cdk.CSActivity;

class CSVungleAdImpl {
    private static final String TAG = "CSVungleAd";

    private final String _placementId;
    private boolean _loading;
    private boolean _showing;

    public CSVungleAdImpl() {
        String appId = CSActivity.sharedActivity().resourceString("vungle_ad_app_id");
        _placementId = CSActivity.sharedActivity().resourceString("vungle_ad_placement_id");

        Log.i(TAG, "Start Init");

        assert appId != null;
        assert _placementId != null;

        Vungle.init(appId, CSActivity.sharedActivity().getApplicationContext(), new InitCallback() {
            @Override
            public void onSuccess() {
                Log.i(TAG, "InitCallback - onSuccess");
                Log.i(TAG, "Vungle SDK Version - " + com.vungle.warren.BuildConfig.VERSION_NAME);
                Log.i(TAG, "Valid placement list:");

                for (String validPlacementReferenceIdId : getValidPlacements()) {
                    Log.i(TAG, validPlacementReferenceIdId);
                }

                if (Vungle.canPlayAd(_placementId)) {
                    Log.i(TAG, "Ad Can Play");
                    load();
                }
                else {
                    Log.i(TAG, "Ad Can't Play");
                }
            }

            @Override
            public void onError(VungleException exception) {
                Log.e(TAG, "Init Failed. Error Code : " + exception.getExceptionCode());
                Log.e(TAG, "Init Failed. Error Message : " + exception.getLocalizedMessage());
            }

            @Override
            public void onAutoCacheAdAvailable(String placementId) {
                Log.i(TAG, "onAutoCacheAdAvailable");
            }
        });
    }

    private void load() {
        _loading = true;

        Log.i(TAG, "Start Load");

        if (Vungle.isInitialized()) {
            Log.i(TAG, "Vungle is Initialized.");

            Vungle.loadAd(_placementId, new LoadAdCallback() {
                @Override
                public void onAdLoad(String placementId) {
                    Log.i(TAG, "onAdLoaded");

                    _loading = false;
                }

                @Override
                public void onError(String placementId, VungleException exception) {
                    Log.e(TAG, "Load Failed. PlacementId : " + placementId);
                    Log.e(TAG, "Load Failed. Error Code : " + exception.getExceptionCode());
                    Log.e(TAG, "Load Failed. Error Message : " + exception.getLocalizedMessage());

                    _loading = false;

                    if (_showing) {
                        _showing = false;

                        CSActivity.sharedActivity().view().queueEvent(() -> CSVungleAd.nativeFail(exception.getLocalizedMessage()));
                    }
                }
            });
        }
        else {
            Log.i(TAG, "Vungle isn't Initialized");
            _loading = false;
        }
    }

    public void show() {
        Log.i(TAG, "Start ad showing");

        _showing = true;

        if (Vungle.canPlayAd(_placementId)) {
            Log.i(TAG, "Ad Can Play in show().");

            AdConfig adConfig = new AdConfig();
            adConfig.setMuted(false);
            adConfig.setAdOrientation(AdConfig.AUTO_ROTATE);

            Vungle.playAd(_placementId, adConfig, new PlayAdCallback() {
                @Override
                public void creativeId(String creativeId) {}

                @Override
                public void onAdStart(String placementId) {
                    Log.i(TAG, "onAdStart in show()");
                }

                @Override
                public void onAdEnd(String placementId, boolean completed, boolean isCTAClicked) {
                    Log.i(TAG, "onAdEnd in rewarded close");

                    _showing = false;

                    if (completed) {
                        CSActivity.sharedActivity().view().queueEvent(CSVungleAd::nativeReward);
                    }
                    else {
                        CSActivity.sharedActivity().view().queueEvent(CSVungleAd::nativeClose);
                    }
                }

                @Override
                public void onAdEnd(String placementId) {
                    Log.i(TAG, "onAdEnd in show()");
                }

                @Override
                public void onAdClick(String placementId) {
                    Log.i(TAG, "onAdClick in show()");
                }

                @Override
                public void onAdRewarded(String placementId) {
                    Log.i(TAG, "onAdRewarded in show()");
                }

                @Override
                public void onAdLeftApplication(String placementId) {
                    Log.i(TAG, "onAdLeftApplication in show()");
                }

                @Override
                public void onError(String placementId, VungleException exception) {
                    Log.i(TAG, "onError in show()");
                    Log.e(TAG, "Play Failed. Error Code : " + exception.getExceptionCode());
                    Log.e(TAG, "Play Failed. Error Message : " + exception.getLocalizedMessage());

                    CSActivity.sharedActivity().view().queueEvent(() -> CSVungleAd.nativeFail(exception.getLocalizedMessage()));
                }

                @Override
                public void onAdViewed(String placementId) {
                    Log.i(TAG, "onAdViewed in show()");
                }
            });
        }
        else if (!_loading) {
            Log.i(TAG, "Ad Can't Play in show().");

            load();
        }
    }
}

public class CSVungleAd {
    private static final String TAG = "CSVungleAd";

    private static CSVungleAdImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSVungleAdImpl());
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
