package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.appsflyer.AppsFlyerConversionListener;
import com.appsflyer.AppsFlyerLib;

import java.util.HashMap;
import java.util.Map;

import kr.co.brgames.cdk.CSActivity;

public class CSAppsFlyer {
    private static final String TAG = "CSAppsFlyer";

    public static void initialize() {
        Log.i(TAG, "init start");
        CSActivity.sharedActivity().queueEvent(() -> {
            final String devKey = CSActivity.sharedActivity().resourceString("apps_flyer_dev_key");

            assert devKey != null;

            AppsFlyerConversionListener conversionDataListener =  new AppsFlyerConversionListener() {
                @Override
                public void onConversionDataSuccess(Map<String, Object> conversionData) {
                    for (String attrName : conversionData.keySet()) {
                        Log.d(TAG, "onConversionDataSuccess:" + attrName + " = " + conversionData.get(attrName));
                    }
                }

                @Override
                public void onConversionDataFail(String errorMessage) {
                    Log.e(TAG, "onConversionDataFail:" + errorMessage);
                }

                @Override
                public void onAppOpenAttribution(Map<String, String> conversionData) {
                    for (String attrName : conversionData.keySet()) {
                        Log.d(TAG, "onAppOpenAttribution:" + attrName + " = " + conversionData.get(attrName));
                    }
                }

                @Override
                public void onAttributionFailure(String errorMessage) {
                    Log.e(TAG, "onAttributionFailure:" + errorMessage);
                }
            };
            AppsFlyerLib.getInstance().init(devKey, conversionDataListener, CSActivity.sharedActivity().getApplicationContext());

            //AppsFlyerLib.getInstance().startTracking(CSActivity.sharedActivity().getApplication(), devKey);
            AppsFlyerLib.getInstance().startTracking(CSActivity.sharedActivity().getApplication());
        });
    }
    
    public static void log(final String name, final String[] params) {
        CSActivity.sharedActivity().queueEvent(() -> {
            Map<String, Object> ps = new HashMap<>();
            for (int i = 0; i < params.length; i += 2) {
                ps.put(params[i], params[i + 1]);
            }
            AppsFlyerLib.getInstance().trackEvent(CSActivity.sharedActivity().getApplication(), name, ps);
        });
    }
}
