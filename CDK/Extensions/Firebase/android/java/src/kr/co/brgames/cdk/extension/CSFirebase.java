package kr.co.brgames.cdk.extension;

import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.firebase.FirebaseApp;
import com.google.firebase.FirebaseOptions;
import com.google.firebase.analytics.FirebaseAnalytics;
import com.google.firebase.messaging.FirebaseMessaging;
import com.google.firebase.crashlytics.FirebaseCrashlytics;

import kr.co.brgames.cdk.CSActivity;

public class CSFirebase {
    public static final String TAG = "CSFirebase";
    private static FirebaseAnalytics mFirebaseAnalytics;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> {
            GoogleApiAvailability googleApiAvailability = GoogleApiAvailability.getInstance();
            int result = googleApiAvailability.isGooglePlayServicesAvailable(CSActivity.sharedActivity());

            if (result != ConnectionResult.SUCCESS && googleApiAvailability.isUserResolvableError(result)) {
                Dialog errorDialog = googleApiAvailability.getErrorDialog(CSActivity.sharedActivity(), result, 1);
                assert errorDialog != null;
                errorDialog.setCancelable(false);
                errorDialog.show();
            }

            FirebaseOptions options = FirebaseOptions.fromResource(CSActivity.sharedActivity());
            assert options != null;
            FirebaseApp.initializeApp(CSActivity.sharedActivity(), options, TAG);

            mFirebaseAnalytics = FirebaseAnalytics.getInstance(CSActivity.sharedActivity());

            try {
                FirebaseMessaging.getInstance().getToken().addOnSuccessListener(token -> CSActivity.sharedActivity().view().queueEvent(() -> nativeTokenRefresh(token)));
            }
            catch (IllegalStateException e) {}      //TODO:CRASH ON SOME DEVICES
        });
    }

    public static void addTopic(final String topic) {
        CSActivity.sharedActivity().queueEvent(() -> FirebaseMessaging.getInstance().subscribeToTopic(topic));
    }

    public static void removeTopic(final String topic) {
        CSActivity.sharedActivity().queueEvent(() -> FirebaseMessaging.getInstance().unsubscribeFromTopic(topic));
    }

    public static void log(final String name, final String[] params) {
        CSActivity.sharedActivity().queueEvent(() -> {
            Log.i(TAG, "Log Start : " + name);
            Bundle param = new Bundle();
            for (int i = 0; i < params.length; i += 2) {
                param.putString(params[i], params[i + 1]);
                Log.i(TAG, "Log Param : (" + params[i] + ")(" + params[i + 1] + ")");
            }

            mFirebaseAnalytics.logEvent(name, param);
        });
    }

    public static void setUserId(final String userId) {
        CSActivity.sharedActivity().queueEvent(() -> {
            Log.i(TAG, "Set User Id : " + userId);
            FirebaseCrashlytics.getInstance().setUserId(userId);
        });
    }

    public static native void nativeTokenRefresh(String token);
}