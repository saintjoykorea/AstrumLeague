package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.huawei.hms.ads.AdParam;
import com.huawei.hms.ads.HwAds;
import com.huawei.hms.ads.reward.Reward;
import com.huawei.hms.ads.reward.RewardAd;
import com.huawei.hms.ads.reward.RewardAdLoadListener;
import com.huawei.hms.ads.reward.RewardAdStatusListener;

import kr.co.brgames.cdk.CSActivity;

class CSHuaweiAdsImpl {
    private static final String TAG = "CSHuaweiAds";

    private final String _unitId;
    private RewardAd _rewardAd;
    private boolean _loading;
    private boolean _showing;
    private boolean _rewarded;

    public CSHuaweiAdsImpl(boolean testMode) {
        Log.i(TAG, "Start Init");

        if (testMode) {
            _unitId = CSActivity.sharedActivity().resourceString("huawei_ad_test_id");
            Log.i(TAG, "Test Ad On");
        }
        else {
            _unitId = CSActivity.sharedActivity().resourceString("huawei_ad_unit_id");
        }

        assert _unitId != null;

        HwAds.init(CSActivity.sharedActivity());

        _rewardAd = new RewardAd(CSActivity.sharedActivity(), _unitId);
    }

    private void load() {
        _loading = true;

        Log.i(TAG, "Start Load");

        if (_rewardAd == null) {
            _loading = false;

            _rewardAd = new RewardAd(CSActivity.sharedActivity(), _unitId);
        }

        RewardAdLoadListener listener = new RewardAdLoadListener() {
            @Override
            public void onRewardedLoaded() {
                Log.i(TAG, "Load Complete");

                _loading = false;

                if (_showing) {
                    show();
                }
            }

            @Override
            public void onRewardAdFailedToLoad(int errorCode) {
                Log.i(TAG, "Load Failed. errorCode : " + errorCode);

                _loading = false;

                if (_showing) {
                    _showing = false;

                    CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiAds.nativeFail("errorCode : " + errorCode));
                }
            }
        };
        _rewardAd.loadAd(new AdParam.Builder().build(), listener);
    }

    public void show() {
        Log.i(TAG, "Start ad showing");

        _rewarded = false;
        _showing = true;

        if (_rewardAd.isLoaded()) {
            Log.i(TAG, "Ad Can Play in show().");

            _rewardAd.show(CSActivity.sharedActivity(), new RewardAdStatusListener() {
                @Override
                public void onRewardAdClosed() {
                    Log.i(TAG, "onRewardAdClosed()");

                    _showing = false;
                    if (!_rewarded) {
                        load();

                        CSActivity.sharedActivity().view().queueEvent(CSHuaweiAds::nativeClose);
                    }
                }

                @Override
                public void onRewardAdFailedToShow(int errorCode) {
                    Log.i(TAG, "onError in show()");
                    Log.e(TAG, "Play Failed. Error Code : " + errorCode);

                    _showing = false;

                    CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiAds.nativeFail("Ad show failed. errorCode : " + errorCode));
                }

                @Override
                public void onRewardAdOpened() {
                    Log.i(TAG, "onRewardAdOpened()");
                }

                @Override
                public void onRewarded(Reward reward) {
                    Log.i(TAG, "onRewarded()");

                    _rewarded = true;
                    _showing = false;

                    load();

                    CSActivity.sharedActivity().view().queueEvent(CSHuaweiAds::nativeReward);
                }
            });
        }
        else {
            Log.i(TAG, "Ad Can't Play in show().");

            load();
        }

//        if (Vungle.canPlayAd(_placementId)) {
//            Log.i(TAG, "Ad Can Play in show().");
//
//            AdConfig adConfig = new AdConfig();
//            adConfig.setMuted(false);
//            adConfig.setAdOrientation(AdConfig.AUTO_ROTATE);
//
//            Vungle.playAd(_placementId, adConfig, new PlayAdCallback() {
//                @Override
//                public void creativeId(String creativeId) {}
//
//                @Override
//                public void onAdStart(String placementId) {
//                    Log.i(TAG, "onAdStart in show()");
//                }
//
//                @Override
//                public void onAdEnd(String placementId, boolean completed, boolean isCTAClicked) {
//                    Log.i(TAG, "onAdEnd in rewarded close");
//
//                    _showing = false;
//
//                    if (completed) {
//                        CSActivity.sharedActivity().view().queueEvent(CSHuaweiAds::nativeReward);
//                    }
//                    else {
//                        CSActivity.sharedActivity().view().queueEvent(CSHuaweiAds::nativeClose);
//                    }
//                }
//
//                @Override
//                public void onAdEnd(String placementId) {
//                    Log.i(TAG, "onAdEnd in show()");
//                }
//
//                @Override
//                public void onAdClick(String placementId) {
//                    Log.i(TAG, "onAdClick in show()");
//                }
//
//                @Override
//                public void onAdRewarded(String placementId) {
//                    Log.i(TAG, "onAdRewarded in show()");
//                }
//
//                @Override
//                public void onAdLeftApplication(String placementId) {
//                    Log.i(TAG, "onAdLeftApplication in show()");
//                }
//
//                @Override
//                public void onError(String placementId, VungleException exception) {
//                    Log.i(TAG, "onError in show()");
//                    Log.e(TAG, "Play Failed. Error Code : " + exception.getExceptionCode());
//                    Log.e(TAG, "Play Failed. Error Message : " + exception.getLocalizedMessage());
//
//                    CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiAds.nativeFail(exception.getLocalizedMessage()));
//                }
//
//                @Override
//                public void onAdViewed(String placementId) {
//                    Log.i(TAG, "onAdViewed in show()");
//                }
//            });
//        }
//        else if (!_loading) {
//            Log.i(TAG, "Ad Can't Play in show().");
//
//            load();
//        }
    }

    public void dispose() {
        if (_rewardAd != null) {
            _rewardAd = null;
        }
    }
}

public class CSHuaweiAds {
    private static final String TAG = "CSHuaweiAds";

    private static CSHuaweiAdsImpl _impl;

    public static void initialize(boolean testMode) {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSHuaweiAdsImpl(testMode));
    }

    public static void dispose() {
        if (_impl != null) {
            CSActivity.sharedActivity().queueEvent(() -> {
                _impl.dispose();
                _impl = null;
            });
        }
    }

    public static void show() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.show());
    }

    public static native void nativeReward();
    public static native void nativeClose();
    public static native void nativeFail(String msg);
}
