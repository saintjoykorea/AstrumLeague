package kr.co.brgames.cdk.extension;

import android.app.Activity;
import android.app.Application;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.igaworks.IgawCommon;
import com.igaworks.adbrix.IgawAdbrix;
import com.igaworks.commerce.IgawCommerce;
import com.igaworks.commerce.IgawCommerceProductModel;
import com.igaworks.interfaces.DeferredLinkListener;

import kr.co.brgames.cdk.CSActivity;

class CSIGAWorksImpl implements Application.ActivityLifecycleCallbacks {
    private static final String TAG = "CSIGAWorks";

    public CSIGAWorksImpl() {
        CSActivity.sharedActivity().getApplication().registerActivityLifecycleCallbacks(this);

        IgawCommon.autoSessionTracking(CSActivity.sharedActivity().getApplication());

        //페이스북 트래킹을 위한 API 추가
        IgawCommon.setDeferredLinkListener(CSActivity.sharedActivity(), new DeferredLinkListener() {
            @Override
            public void onReceiveDeeplink(String s) {
                try {
                    Log.i(TAG, "Facebook Deeplink: " + s);
                    Intent intent = new Intent(Intent.ACTION_VIEW);
                    intent.addCategory(Intent.CATEGORY_BROWSABLE);
                    intent.setData(Uri.parse(s));
                    CSActivity.sharedActivity().startActivity(intent);
                } catch (Exception e) {
                    // TODO Auto-generated catch block
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        });
    }
    
    public void dispose() {
        CSActivity.sharedActivity().getApplication().unregisterActivityLifecycleCallbacks(this);
    }
    
    public void firstTimeExperience(String userActivity) {
        IgawAdbrix.firstTimeExperience(userActivity);
    }

    public void retention(String userActivity) {
        IgawAdbrix.retention(userActivity);
    }

    public void retention(String userActivity, String subActivity) {
        IgawAdbrix.retention(userActivity, subActivity);
    }

    public void purchase(String productId, String productName, double price, String currency) {
        IgawCommerceProductModel detail = new IgawCommerceProductModel()
                .setProductID(productId)
                .setProductName(productName)
                .setPrice(price)
                .setCurrency(currency);

        IgawAdbrix.purchase(CSActivity.sharedActivity(), "", detail, IgawCommerce.IgawPaymentMethod.MobilePayment);
    }

    // Application.ActivityLifecycleCallbacks

    @Override
    public void onActivityCreated(Activity activity, Bundle savedInstanceState) { }

    @Override
    public void onActivityStarted(Activity activity) { }

    @Override
    public void onActivityResumed(Activity activity) {
        IgawCommon.startSession(CSActivity.sharedActivity());
    }

    @Override
    public void onActivityPaused(Activity activity) {
        IgawCommon.endSession();
    }

    @Override
    public void onActivityStopped(Activity activity) { }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) { }

    @Override
    public void onActivityDestroyed(Activity activity) {
        
    }
}

public class CSIGAWorks {
    private static CSIGAWorksImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSIGAWorksImpl());
    }
    
    public static void dispose() {
		if (_impl != null) {
			CSActivity.sharedActivity().queueEvent(() -> {
				_impl.dispose();
				_impl = null;
			});
		}
    }

    public static void firstTimeExperience(final String userActivity) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.firstTimeExperience(userActivity));
    }

    public static void retention(final String userActivity) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.retention(userActivity));
    }

    public static void retention(final String userActivity, final String subActivity) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.retention(userActivity, subActivity));
    }

    public static void purchase(final String productId, final String productName, final double price, final String currency) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.purchase(productId, productName, price, currency));
    }
}
