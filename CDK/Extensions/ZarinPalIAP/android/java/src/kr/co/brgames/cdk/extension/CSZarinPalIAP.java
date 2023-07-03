package kr.co.brgames.cdk.extension;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatDelegate;

import com.zarinpal.ZarinPalBillingClient;
import com.zarinpal.billing.purchase.Purchase;
import com.zarinpal.client.BillingClientStateListener;
import com.zarinpal.client.ClientState;
import com.zarinpal.provider.core.future.FutureCompletionListener;
import com.zarinpal.provider.core.future.TaskResult;
import com.zarinpal.provider.model.response.Receipt;

import kr.co.brgames.cdk.CSActivity;

class CSZarinPalIAPImpl implements BillingClientStateListener, FutureCompletionListener<Receipt> {
    private static final String TAG = "CSZarinPalIAP";

    private static final int PURCHASE_COMPLETE_ERROR = 501;
    private static final int SETUP_FINISHED_ERROR = 502;

    private ZarinPalBillingClient _billingClient;
    private String _productId;
    private String _payload;
    private boolean _purchased;

    public CSZarinPalIAPImpl() {
        Log.i(TAG, "Start Init");

        _billingClient = ZarinPalBillingClient.newBuilder(CSActivity.sharedActivity())
                .enableShowInvoice()
                .setListener(this)
                .setNightMode(AppCompatDelegate.MODE_NIGHT_YES)
                .build();
    }

    public void dispose() {
        if (_billingClient != null) {
            _billingClient = null;
        }
    }

    @Override
    public void onClientServiceDisconnected() {
        Log.e(TAG, "onClientServiceDisconnected");

        if (!_purchased) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) {
                    CSZarinPalIAP.nativePurchaseCancelled();
                }
            });
        }
    }

    @Override
    public void onClientSetupFinished(@NonNull ClientState clientState) {
        Log.i(TAG, "onClientSetupFinished - ClientState : " + clientState.name());

        if (clientState == ClientState.USER_CANCELED) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) {
                    CSZarinPalIAP.nativePurchaseCancelled();
                }
            });
        }
        else if (clientState == ClientState.ERROR) {
            Log.i(TAG, "onClientSetupFinished - nativeError");
            CSActivity.sharedActivity().view().queueEvent(() -> CSZarinPalIAP.nativeError(SETUP_FINISHED_ERROR));
        }
    }

    @Override
    public void onComplete(TaskResult<Receipt> task) {
        if (task.isSuccess()) {
            Log.i(TAG, "task.isSuccess - productId : " + _productId);

            if (task.getSuccess().isSuccess()) {
                _purchased = true;

                final String productId = _productId;
                final String receipt = task.getSuccess().getTransactionID();
                final String signature = task.getSuccess().getTransactionID();
                final String payload = _payload;

                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_billingClient != null)
                        CSZarinPalIAP.nativePurchaseComplete(productId, receipt, signature, payload);
                });
            }
            else {
                Log.i(TAG, "task.getSuccess().isSuccess");
            }
        }
        else {
            Log.e(TAG, "failed onComplete : " + _productId);
            CSActivity.sharedActivity().view().queueEvent(() -> CSZarinPalIAP.nativeError(PURCHASE_COMPLETE_ERROR));
        }
    }

    public void connect() {
        Log.i(TAG, "connecting");
        if (_billingClient != null) {
            CSActivity.sharedActivity().view().queueEvent(() -> CSZarinPalIAP.nativeConnected());
        }
    }

    public void purchase(String productId, String payload) {
        Log.i(TAG, "Start Purchase");

        _productId = productId;
        _payload = payload;

        Purchase purchase = Purchase.newBuilder()
                .asSku(_productId)
                .build();

        _billingClient.launchBillingFlow(purchase, this);
    }
}

class CSZarinPalIAP {
    private static CSZarinPalIAPImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSZarinPalIAPImpl());
    }

    public static void dispose() {
        if (_impl != null) {
			CSActivity.sharedActivity().queueEvent(() -> {
				_impl.dispose();
				_impl = null;
			});
        }
    }

    public static void connect() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.connect());
    }

    public static void purchase(final String productId, final String payload) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.purchase(productId, payload));
    }

    public static native void nativeConnected();
    public static native void nativePurchaseComplete(String productId, String chargeData1, String chargeData2, String payload);
    public static native void nativePurchaseCancelled();
    public static native void nativeError(int errorCode);
}
