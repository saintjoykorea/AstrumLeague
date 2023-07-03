package kr.co.brgames.cdk.extension;

import android.util.Log;
import android.os.Bundle;

import com.huawei.hms.analytics.HiAnalytics;
import com.huawei.hms.analytics.HiAnalyticsInstance;
import com.huawei.hms.analytics.HiAnalyticsTools;

import kr.co.brgames.cdk.CSActivity;

public class CSHuaweiAnalytics {
    private static final String TAG = "CSHuaweiAnalytics";

    private static HiAnalyticsInstance _instance;

    public static void initialize() {
        Log.i(TAG, "Init Start");

        CSActivity.sharedActivity().queueEvent(() -> {
            HiAnalyticsTools.enableLog();
            _instance = HiAnalytics.getInstance(CSActivity.sharedActivity());
        });
    }
    
    public static void log(final String name, final String[] params) {
        CSActivity.sharedActivity().queueEvent(() -> {
            Log.i(TAG, "Log Start : " + name);
            Bundle param = new Bundle();
            for (int i = 0; i < params.length; i += 2) {
                param.putString(params[i], params[i + 1]);
                Log.i(TAG, "Log Param : (" + params[i] + ")(" + params[i + 1] + ")");
            }
            _instance.onEvent(name, param);
        });
    }
}
