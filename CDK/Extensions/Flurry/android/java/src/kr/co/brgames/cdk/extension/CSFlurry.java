package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.flurry.android.FlurryAgent;
import com.flurry.android.FlurryPerformance;
import com.flurry.android.FlurryEventRecordStatus;
import java.util.HashMap;
import java.util.Map;

import kr.co.brgames.cdk.CSActivity;

public class CSFlurry {
    private static final String TAG = "CSFlurry";

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> {
			final String devKey = CSActivity.sharedActivity().resourceString("flurry_dev_key");
			
			if (devKey == null) {
				Log.e(TAG, "not ready");
				System.exit(0);
			}

			new FlurryAgent.Builder()
					.withDataSaleOptOut(false) //CCPA - the default value is false
					.withCaptureUncaughtExceptions(true)
					.withIncludeBackgroundSessionsInMetrics(true)
					.withLogLevel(Log.VERBOSE)
					//.withLogEnabled(true)
					.withPerformanceMetrics(FlurryPerformance.ALL)
					.build(CSActivity.sharedActivity().getApplication(), devKey);
        });
    }

    public static void log(final String name, final String[] params) {
        CSActivity.sharedActivity().queueEvent(() -> {
			Map<String, String> ps = new HashMap<String, String>();
			for (int i = 0; i < params.length; i += 2) {
				ps.put(params[i], params[i + 1]);
			}
			FlurryEventRecordStatus result = FlurryAgent.logEvent(name, ps);
        });
    }
}
