package kr.co.brgames.cdk.extension;

import android.util.Log;

import androidx.annotation.NonNull;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import java.util.List;
import java.util.ArrayList;
import java.util.Objects;

import kr.co.brgames.cdk.CSActivity;

class CSGoogleIAPImpl implements BillingClientStateListener, ProductDetailsResponseListener, PurchasesUpdatedListener, ConsumeResponseListener {
    private BillingClient _billingClient;
    private List<ProductDetails> _productDetails;
    private List<Purchase> _purchases;
    private BillingFlowParams _billingParams;
    private boolean _sync;

    private static final String TAG = "CSGoogleIAP";

    public CSGoogleIAPImpl() {
        _billingClient = BillingClient.newBuilder(CSActivity.sharedActivity().getApplication())
                .setListener(this)
                .enablePendingPurchases()
                .build();

        _productDetails = new ArrayList<>();
        _purchases = new ArrayList<>();
    }

    public void dispose() {
        if (_billingClient != null) {
            BillingClient billingClient = _billingClient;
            _billingClient = null;
            if (billingClient.isReady()) {
                billingClient.endConnection();
            }
            _productDetails = null;
            _purchases = null;
            _billingParams = null;
        }
    }

    private Purchase getPurchase(String token) {
        for (Purchase purchase : _purchases) {
            if (purchase.getPurchaseToken().equals(token)) {
                return purchase;
            }
        }
        return null;
    }

    private ProductDetails getProductDetail(String productId) {
        for (ProductDetails productDetails : _productDetails) {
            if (productDetails.getProductId().equals(productId)) {
                return productDetails;
            }
        }
        return null;
    }
    
    public void connect() {
        if (_billingClient.isReady()) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) CSGoogleIAP.nativeConnected();
            });
        }
        else {
            _billingClient.startConnection(this);
        }
    }

    @Override
    public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
        if (_billingClient != null) {
            final int responseCode = billingResult.getResponseCode();

			if (responseCode != BillingClient.BillingResponseCode.OK) {
				Log.e(TAG, "onBillingSetupFinished error:" + responseCode);
			}
			if (_billingParams != null) {
                if (responseCode == BillingClient.BillingResponseCode.OK) doPurchase();
                else {
                    _billingParams = null;

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_billingClient != null) {
                            CSGoogleIAP.nativeError(responseCode);
                        }
                    });
                }
            }
			else {
                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_billingClient != null) {
                        if (responseCode == BillingClient.BillingResponseCode.OK) CSGoogleIAP.nativeConnected();
                        else CSGoogleIAP.nativeError(responseCode);
                    }
                });
            }
        }
    }

    @Override
    public void onBillingServiceDisconnected() {
		Log.e(TAG, "onBillingServiceDisconnected");

		if (_billingClient != null && _billingParams != null) {
            _billingParams = null;

            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) CSGoogleIAP.nativeError(BillingClient.BillingResponseCode.SERVICE_DISCONNECTED);
            });
        }
    }

    public void sync(String[] productIds) {
        if (_billingClient.isReady()) {
            List<QueryProductDetailsParams.Product> products = new ArrayList<>(productIds.length);
            QueryProductDetailsParams.Product.Builder productBuilder = QueryProductDetailsParams.Product.newBuilder();
            for (String productId : productIds) {
                QueryProductDetailsParams.Product product = productBuilder.setProductId(productId).setProductType(BillingClient.ProductType.INAPP).build();
                products.add(product);
            }
            QueryProductDetailsParams queryProductDetailsParams = QueryProductDetailsParams.newBuilder().setProductList(products).build();

            _billingClient.queryProductDetailsAsync(queryProductDetailsParams, this);
        }
        else {
			Log.e(TAG, "sync fail:not connected");
			
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) CSGoogleIAP.nativeError(BillingClient.BillingResponseCode.SERVICE_DISCONNECTED);
            });
        }
    }

    @Override
    public void onProductDetailsResponse(@NonNull BillingResult billingResult, @NonNull List<ProductDetails> productDetailsList) {
        if (_billingClient != null) {
            final int responseCode0 = billingResult.getResponseCode();

            _purchases.clear();

            if (responseCode0 == BillingClient.BillingResponseCode.OK) {
                _productDetails.addAll(productDetailsList);

                for (ProductDetails productDetails : productDetailsList) {
                    ProductDetails.OneTimePurchaseOfferDetails subdetail = Objects.requireNonNull(productDetails.getOneTimePurchaseOfferDetails());

                    final String productId = productDetails.getProductId();
                    final String price = subdetail.getFormattedPrice();
                    final String originPrice = Double.toString((double) subdetail.getPriceAmountMicros() / 1000000);
                    final String currencyCode = subdetail.getPriceCurrencyCode();

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_billingClient != null) CSGoogleIAP.nativeSyncProduct(productId, price, originPrice, currencyCode);
                    });
                }

                //TODO:예제에서는 onResume에서 consume 되지 않은 구매이력을 조회하라고 되어 있음
                _billingClient.queryPurchasesAsync(
                        QueryPurchasesParams.newBuilder()
                                .setProductType(BillingClient.ProductType.INAPP)
                                .build(),
                        (billingResult1, list) -> {
                            final int responseCode1 = billingResult1.getResponseCode();
                            if (responseCode1 == BillingClient.BillingResponseCode.OK) {
                                for (Purchase purchase : list) {
                                    if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
                                        _purchases.add(purchase);
                                    }
                                }
                                if (_purchases.isEmpty()) {
                                    _sync = false;

                                    CSActivity.sharedActivity().view().queueEvent(() -> {
                                        if (_billingClient != null) CSGoogleIAP.nativeSync();
                                    });
                                }
                                else {
                                    _sync = true;

                                    ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(_purchases.get(0).getPurchaseToken()).build();

                                    _billingClient.consumeAsync(params, CSGoogleIAPImpl.this);
                                }
                            }
                            else {
                                Log.e(TAG, "queryPurchases error:" + responseCode1);

                                CSActivity.sharedActivity().view().queueEvent(() -> {
                                    if (_billingClient != null) CSGoogleIAP.nativeError(responseCode1);
                                });
                            }
                        });

            }
            else {
				Log.e(TAG, "onProductDetailsResponse error:" + responseCode0);
				
                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_billingClient != null) CSGoogleIAP.nativeError(responseCode0);
                });
            }
        }
    }

    private void doPurchase() {
        BillingResult result = _billingClient.launchBillingFlow(CSActivity.sharedActivity(), _billingParams);

        _billingParams = null;

        final int responseCode = result.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            Log.e(TAG, "purchase error:" + responseCode);

            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) CSGoogleIAP.nativeError(responseCode);
            });
        }
    }

    public void purchase(String productId, String payload) {
        ProductDetails productDetails = getProductDetail(productId);

        if (productDetails != null) {
            _billingParams = BillingFlowParams.newBuilder()
                    .setProductDetailsParamsList(List.of(
                            BillingFlowParams.ProductDetailsParams.newBuilder()
                                    .setProductDetails(productDetails)
                                    .build()))
                    .setObfuscatedAccountId(payload)
                    .build();

            if (_billingClient.isReady()) {
                doPurchase();
            }
            else {
                _billingClient.startConnection(this);
            }
        }
        else {
			Log.e(TAG, "invalid product:" + productId);
			
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_billingClient != null) CSGoogleIAP.nativeError(BillingClient.BillingResponseCode.ITEM_UNAVAILABLE);
            });
        }
    }

    @Override
    public void onPurchasesUpdated(@NonNull BillingResult billingResult, List<Purchase> purchases) {
        if (_billingClient != null) {
            final int responseCode = billingResult.getResponseCode();

            if (responseCode == BillingClient.BillingResponseCode.OK) {
                if (purchases != null) {
                    for (Purchase purchase : purchases) {
                        if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED && getPurchase(purchase.getPurchaseToken()) == null) {
                            _purchases.add(purchase);
                        }
                    }
                }
                if (!_purchases.isEmpty()) {
                    ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(_purchases.get(0).getPurchaseToken()).build();

                    _billingClient.consumeAsync(params, this);
                }
            }
            else {
				Log.e(TAG, "onPurchasesUpdated error:" + responseCode);
				
                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_billingClient != null) {
                        if (responseCode == BillingClient.BillingResponseCode.USER_CANCELED) CSGoogleIAP.nativePurchaseCancelled();
                        else CSGoogleIAP.nativeError(responseCode);
                    }
                });
            }
        }
    }

    @Override
    public void onConsumeResponse(@NonNull BillingResult result, @NonNull String token) {
        if (_billingClient != null) {
            Purchase purchase = getPurchase(token);

            if (purchase != null) {
                final int responseCode = result.getResponseCode();

                if (responseCode == BillingClient.BillingResponseCode.OK) {
                    final String productId = purchase.getProducts().get(0);
                    final String receipt = purchase.getOriginalJson();
                    final String signature = purchase.getSignature();
                    final String payload = Objects.requireNonNull(purchase.getAccountIdentifiers()).getObfuscatedAccountId();

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_billingClient != null) CSGoogleIAP.nativePurchaseComplete(productId, receipt, signature, payload);
                    });
                }
                else {
					Log.e(TAG, "onConsumeResponse error:" + responseCode);
					
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_billingClient != null) CSGoogleIAP.nativeError(responseCode);
                    });
                }
                _purchases.remove(purchase);
            }
            if (_purchases.isEmpty()) {
                if (_sync) {
                    _sync = false;

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_billingClient != null) CSGoogleIAP.nativeSync();
                    });
                }
            }
            else {
                ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(_purchases.get(0).getPurchaseToken()).build();

                _billingClient.consumeAsync(params, this);
            }
        }
    }
}

public class CSGoogleIAP {
    private static CSGoogleIAPImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSGoogleIAPImpl());
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

    //  marketProductId   :  원스토어 / 구글 플레이 / 앱스토어 Product ID
    //
    //  charge_data1 : 원스토어 txid(payload)
    //                 구글 플레이 purchaseData
    //                 앱스토어 receipt_data
    //
    //
    //  charge_data2 : 원스토어  signdata
    //                 구글 플레이 signagture
    //                 앱스토어 null
    //  payload     : tid(transaction id)

    public static native void nativeConnected();
    public static native void nativeSyncProduct(String productId, String price, String originPrice, String currencyCode);
    public static native void nativeSync();
    public static native void nativePurchaseComplete(String productId, String chargeData1, String chargeData2, String payload);
    public static native void nativePurchaseCancelled();
    public static native void nativeError(int errorCode);
}
