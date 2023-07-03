package kr.co.brgames.cdk.extension;

import com.nbp.appsafer.AppSafer;

import android.util.Log;

import kr.co.brgames.cdk.CSActivity;

public class CSAppSafer {
	public static String TAG = "CSAppSafer";

	public static void initialize() {
		CSActivity.sharedActivity().queueEvent(() -> {
			AppSafer.INSTANCE.start(CSActivity.sharedActivity().getApplicationContext());
		});
	}

	public static void setUserId(final String userId) {
		CSActivity.sharedActivity().queueEvent(() -> {
			AppSafer.INSTANCE.setUserId(userId);
		});
	}
}