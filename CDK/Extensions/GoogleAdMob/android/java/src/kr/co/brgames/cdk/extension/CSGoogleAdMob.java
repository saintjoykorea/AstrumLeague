package kr.co.brgames.cdk.extension;

import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewardedinterstitial.RewardedInterstitialAd;
import com.google.android.gms.ads.rewardedinterstitial.RewardedInterstitialAdLoadCallback;

import kr.co.brgames.cdk.CSActivity;

class CSGoogleAdMobImpl {
    private static final String TAG = "CSGoogleAdMob";

    private RewardedInterstitialAd _ad;
    private final String _unitId;
    private RewardItem _earnedItem;
    private boolean _loading;
    private boolean _showing;

    public CSGoogleAdMobImpl() {
        _unitId = CSActivity.sharedActivity().resourceString("google_admob_unit_code_id");

        assert _unitId != null;

        load();
    }

    private void load() {
        _loading = true;

        RewardedInterstitialAd.load(
                CSActivity.sharedActivity(),
                _unitId,
                new AdRequest.Builder().build(),
                new RewardedInterstitialAdLoadCallback() {
                    @Override
                    public void onAdLoaded(@NonNull RewardedInterstitialAd ad) {
                        Log.i(TAG, "onAdLoaded");

                        _loading = false;

                        _ad = ad;
                        _ad.setImmersiveMode(true);
                        _ad.setFullScreenContentCallback(new FullScreenContentCallback() {
                            @Override
                            public void onAdFailedToShowFullScreenContent(@NonNull AdError adError) {
                                Log.e(TAG, "Ad Fail to load at onAdFailedToShowFullScreenContent - Error : " + adError.getCode());

                                _showing = false;

                                CSActivity.sharedActivity().view().queueEvent(() -> CSGoogleAdMob.nativeFail(adError.getMessage()));
                            }

                            @Override
                            public void onAdDismissedFullScreenContent() {
                                Log.i(TAG, "Ad dismissed");

                                _showing = false;

                                if (_earnedItem != null) {
                                    CSActivity.sharedActivity().view().queueEvent(() -> {
                                        CSGoogleAdMob.nativeReward();

                                        _earnedItem = null;
                                    });
                                }
                                else {
                                    CSActivity.sharedActivity().view().queueEvent(CSGoogleAdMob::nativeClose);
                                }

                                _ad = null;

                                load();
                            }
                        });

                        if (_showing) show();
                    }

                    @Override
                    public void onAdFailedToLoad(@NonNull LoadAdError adError) {
                        Log.i(TAG, "onAdFailedToLoad - Error Code : " + adError.getCode());
                        _loading = false;

                        if (_showing) {
                            _showing = false;

                            CSActivity.sharedActivity().view().queueEvent(() -> CSGoogleAdMob.nativeFail(adError.getMessage()));
                        }
                    }
                }
        );
    }

    public void show() {
        Log.i(TAG, "Start ad showing");

        _earnedItem = null;

        _showing = true;

        if (_ad != null) {
            Log.i(TAG, "Ad showing : _ad isn't null");
            _ad.show(CSActivity.sharedActivity(), rewardItem -> {
                Log.i(TAG, "Ad Earned : " + rewardItem.getType() + rewardItem.getAmount());
                _earnedItem = rewardItem;
            });
        }
        else if (!_loading) {
            load();
        }
    }
}

public class CSGoogleAdMob {
    private static final String TAG = "CSGoogleAdMob";

    private static CSGoogleAdMobImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSGoogleAdMobImpl());
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
