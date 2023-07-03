package kr.co.brgames.cdk.extension;

import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import ir.bazaar.billingclient.util.IabHelper;
import ir.bazaar.billingclient.util.IabResult;
import ir.bazaar.billingclient.util.Inventory;
import ir.bazaar.billingclient.util.Purchase;
import ir.bazaar.billingclient.util.SkuDetails;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

import kr.co.brgames.cdk.CSActivity;

class CSBazaarIAPImpl implements IabHelper.OnIabSetupFinishedListener, IabHelper.QueryInventoryFinishedListener, IabHelper.OnIabPurchaseFinishedListener, IabHelper.OnConsumeFinishedListener, IabHelper.OnConsumeMultiFinishedListener, CSActivity.ActivityResultListener {
    private IabHelper _helper;
    private boolean _connected;
    private ArrayList<String> _productIds;

    private static final int QUERY_PRODUCT_COUNT_AT_ONCE = 20;
    private static final int REQUEST_CODE_PURCHASE = 40000;
    private static final int BAZAAR_IAP_ERROR_PURCHASE_FAILED = 701;
    private static final int BAZAAR_IAP_ERROR_CONSUME_FAILED = 702;

    private static final int RC_SHOW_REVIEW = 1001;

    private static final String TAG = "CSBazaarIAP";

    public CSBazaarIAPImpl() {
        Log.i(TAG, "Start Init");

        String base64PublicKey = CSActivity.sharedActivity().resourceString("bazaar_public_key");
        _helper = new IabHelper(CSActivity.sharedActivity(), base64PublicKey);
        _helper.enableDebugLogging(true);

        _productIds = new ArrayList<String>();

        CSActivity.sharedActivity().addActivityResultListener(this);
    }

    public void dispose() {
        if (_helper != null) {
            _helper.dispose();
            _helper = null;

            CSActivity.sharedActivity().removeActivityResultListener(this);
        }
    }

    public void connect() {
        Log.i(TAG, "connect - Start");
        if (_connected) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
				if (_helper != null) CSBazaarIAP.nativeConnected();
            });
        }
        else {
            _helper.startSetup(this);
        }
    }

    public void sync(String[] productIds) {
        Log.i(TAG, "sync - Start");
        for (int i = 0; i < productIds.length; i++) {
            Log.i(TAG, "sync - [" + i + "]'s productId : " + productIds[i]);
            _productIds.add(productIds[i]);
        }
        _helper.queryInventoryAsync(true, _productIds.size() > QUERY_PRODUCT_COUNT_AT_ONCE ? _productIds.subList(0, QUERY_PRODUCT_COUNT_AT_ONCE) : _productIds, this);
    }

    public void purchase(final String productId, String payload) {
        Log.i(TAG, "purchase - Start");
        try {
            Log.i(TAG, "purchase - productId : " + productId);
            _helper.launchPurchaseFlow(CSActivity.sharedActivity(), productId, REQUEST_CODE_PURCHASE, this, payload);
        }
        catch (Exception e) {        //google code has error
            Log.e(TAG, e.getMessage(), e);

            CSActivity.sharedActivity().view().queueEvent(() -> {
				if (_helper != null) CSBazaarIAP.nativeError(BAZAAR_IAP_ERROR_PURCHASE_FAILED);
            });
        }
    }

    public void showReview() {
        Intent intent = new Intent(Intent.ACTION_EDIT);
        intent.setData(Uri.parse(CSActivity.sharedActivity().resourceString("bazaar_review_url")));
        intent.setPackage("com.farsitel.bazaar");
        CSActivity.sharedActivity().startActivityForResult(intent, RC_SHOW_REVIEW);
    }

    @Override
    public void onIabSetupFinished(IabResult result) {
        Log.i(TAG, "onIabSetupFinished - Entry");

        if (_helper != null) {
            if (result.isSuccess()) {
                Log.i(TAG, "onIabSetupFinished - Success");
                _connected = true;

                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) CSBazaarIAP.nativeConnected();
                });
            }
            else {
                final int responseCode = result.getResponse();
                Log.i(TAG, "onIabSetupFinished - Connect Failed : " + responseCode);
                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) CSBazaarIAP.nativeError(responseCode);
                });
            }
        }
    }

    @Override
    public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
        Log.d(TAG, "QueryInventoryFinished - start");

        if (_helper != null) {
            if (result.isSuccess()) {
                Log.d(TAG, "QueryInventoryFinished - success");

                int productCount = Math.min(_productIds.size(), QUERY_PRODUCT_COUNT_AT_ONCE);

                for (int i = 0; i < productCount; i++) {
                    final String productId = _productIds.remove(0);

                    SkuDetails sku = inventory.getSkuDetails(productId);
                    if (sku != null) {
                        final String price = sku.getPrice();
                        Log.i(TAG, "onQueryInventoryFinished - ProductId : " + productId);
                        Log.i(TAG, "onQueryInventoryFinished - Price : " + price);
                        CSActivity.sharedActivity().view().queueEvent(() -> {
							if (_helper != null) CSBazaarIAP.nativeSyncProduct(productId, price);
                        });
                    }
                    else {
                        Log.i(TAG, "onQueryInventoryFinished - ProductId : " + productId);
                        Log.i(TAG, "onQueryInventoryFinished - sku is null");

                        CSActivity.sharedActivity().view().queueEvent(() -> {
							if (_helper != null) CSBazaarIAP.nativeSyncProduct(productId, "");
                        });
                    }
                }
                if (!_productIds.isEmpty()) {
                    _helper.queryInventoryAsync(true, _productIds.size() > QUERY_PRODUCT_COUNT_AT_ONCE ? _productIds.subList(0, QUERY_PRODUCT_COUNT_AT_ONCE) : _productIds, this);
                }
                else {
                    List<Purchase> purchases = inventory.getAllPurchases();

                    if (purchases.isEmpty()) {
                        CSActivity.sharedActivity().view().queueEvent(() -> {
							if (_helper != null) CSBazaarIAP.nativeSync();
                        });
                    } else {
                        _helper.consumeAsync(purchases, this);
                    }
                }
            }
            else {
                final int responseCode = result.getResponse();
                Log.i(TAG, "onQueryInventoryFinished - Failed : " + responseCode);
                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) CSBazaarIAP.nativeError(responseCode);
                });
            }
        }
    }

    @Override
    public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
        Log.i(TAG, "onIabPurchaseFinished - Start");
        if (purchase != null) {
            Log.i(TAG, "onIabPurchaseFinished - Purchase" + purchase.getPurchaseState());
        }
        else {
            Log.i(TAG, "onIabPurchaseFinished - Purchase is null");
        }

        if (_helper != null) {
            if (result.isSuccess()) {
                Log.i(TAG, "onIabPurchaseFinished - Success");
                _helper.consumeAsync(purchase, this);
            }
            else {
                final int responseCode = result.getResponse();
                final boolean cancelled = responseCode == IabHelper.IABHELPER_USER_CANCELLED;

                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) {
						if (cancelled) {
							Log.i(TAG, "onIabPurchaseFinished - Canceled");
							CSBazaarIAP.nativePurchaseCancelled();
						}
						else {
							Log.i(TAG, "onIabPurchaseFinished - Failed. responseCode : " + responseCode);
							CSBazaarIAP.nativeError(BAZAAR_IAP_ERROR_PURCHASE_FAILED);
						}
					}
                });
            }
        }
    }

    @Override
    public void onConsumeFinished(Purchase purchase, IabResult result) {
        Log.i(TAG, "onConsumeFinished - Start");
        if (_helper != null) {
            if (result.isSuccess()) {
                Log.i(TAG, "onConsumeFinished - Success");
                final String productId = purchase.getSku();
                final String receipt = purchase.getOriginalJson();
                final String signature = purchase.getSignature();
                final String payload = purchase.getDeveloperPayload();

                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) CSBazaarIAP.nativePurchaseComplete(productId, receipt, signature, payload);
                });
            }
            else {
                final int responseCode = result.getResponse();
                Log.i(TAG, "onConsumeFinished - Failed : " + responseCode);

                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_helper != null) CSBazaarIAP.nativeError(BAZAAR_IAP_ERROR_CONSUME_FAILED);
                });
           }
        }
    }

    @Override
    public void onConsumeMultiFinished(List<Purchase> purchases, List<IabResult> results) {       //sync -> queryInventoryAsync -> consumeAsync(multi)
        if (_helper != null) {
            for (int i = 0; i < purchases.size(); i++) {
                onConsumeFinished(purchases.get(i), results.get(i));
            }
            CSActivity.sharedActivity().view().queueEvent(() -> {
				if (_helper != null) CSBazaarIAP.nativeSync();
            });
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (_helper != null) {
            _helper.handleActivityResult(requestCode, resultCode, data);

            if (requestCode == RC_SHOW_REVIEW) {
                Log.i(TAG, "Market Review");
            }
        }
    }
}

public class CSBazaarIAP {
    private static CSBazaarIAPImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSBazaarIAPImpl());
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

    public static void sync(final String[] productIds) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.sync(productIds));
    }

    public static void purchase(final String productId, final String payload) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.purchase(productId, payload));
    }

    public static void showReview() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.showReview());
    }

    public static native void nativeConnected();
    public static native void nativeSyncProduct(String productId, String price);
    public static native void nativeSync();
    public static native void nativePurchaseComplete(String productId, String chargeData1, String chargeData2, String payload);
    public static native void nativePurchaseCancelled();
    public static native void nativeError(int errorCode);
}
