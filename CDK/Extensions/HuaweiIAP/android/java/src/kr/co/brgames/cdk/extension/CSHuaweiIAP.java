package kr.co.brgames.cdk.extension;

import android.content.Intent;
import android.content.IntentSender;
import android.util.Log;

import com.huawei.hmf.tasks.OnFailureListener;
import com.huawei.hmf.tasks.OnSuccessListener;
import com.huawei.hmf.tasks.Task;
import com.huawei.hms.iap.IapApiException;
import com.huawei.hms.iap.entity.ConsumeOwnedPurchaseReq;
import com.huawei.hms.iap.entity.ConsumeOwnedPurchaseResult;
import com.huawei.hms.iap.entity.IsEnvReadyResult;
import com.huawei.hms.iap.entity.IsSandboxActivatedReq;
import com.huawei.hms.iap.entity.IsSandboxActivatedResult;
import com.huawei.hms.iap.entity.OwnedPurchasesReq;
import com.huawei.hms.iap.entity.ProductInfoReq;
import com.huawei.hms.iap.entity.PurchaseIntentReq;
import com.huawei.hms.iap.Iap;
import com.huawei.hms.iap.IapClient;
import com.huawei.hms.iap.entity.InAppPurchaseData;
import com.huawei.hms.iap.entity.OrderStatusCode;
import com.huawei.hms.iap.entity.OwnedPurchasesResult;
import com.huawei.hms.iap.entity.ProductInfo;
import com.huawei.hms.iap.entity.ProductInfoResult;
import com.huawei.hms.iap.entity.PurchaseIntentResult;
import com.huawei.hms.iap.entity.PurchaseResultInfo;
import com.huawei.hms.iap.util.IapClientHelper;
import com.huawei.hms.support.api.client.Status;

import org.json.JSONException;

import java.util.ArrayList;
import java.util.List;

import kr.co.brgames.cdk.CSActivity;

class CSHuaweiIAPImpl implements CSActivity.ActivityResultListener {
    private static final String TAG = "CSHuaweiIAP";

    private static final int PURCHASE_COMPLETE = 500;
    private static final int PURCHASE_COMPLETE_ERROR = 501;
    private static final int SETUP_FINISHED_ERROR = 502;
    private static final int QUERY_FINISHED_ERROR = 502;

    private IapClient _iapClient;
    private ArrayList<String> _productIds;
    private String _productId;
    private String _payload;
    private String _pushToken;

    public CSHuaweiIAPImpl() {
        CSActivity.sharedActivity().addActivityResultListener(this);

        _iapClient = Iap.getIapClient(CSActivity.sharedActivity());
        _productIds = new ArrayList<>();

        Task<IsSandboxActivatedResult> sandBoxTask = _iapClient.isSandboxActivated((new IsSandboxActivatedReq()));
        sandBoxTask.addOnSuccessListener(new OnSuccessListener<IsSandboxActivatedResult>() {
            @Override
            public void onSuccess(IsSandboxActivatedResult isSandboxActivatedResult) {
                Log.i(TAG, "CSHuaweiIAPImpl - sandbox onSuccess");
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(Exception e) {
                if (e instanceof IapApiException) {
                    IapApiException apiException = (IapApiException) e;
                    int returnCode = apiException.getStatusCode();
                } else {
                    // Other external errors.
                }
            }
        });

        Task<IsEnvReadyResult> task = _iapClient.isEnvReady();
        task.addOnSuccessListener(new OnSuccessListener<IsEnvReadyResult>() {
            @Override
            public void onSuccess(IsEnvReadyResult result) {
                String carrierId = result.getCarrierId();
                Log.i(TAG, "CSHuaweiIAPImpl - onSuccess, CarrierId : " + carrierId);
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(Exception e) {
                if (e instanceof IapApiException) {
                    IapApiException apiException = (IapApiException) e;
                    Status status = apiException.getStatus();
                    if (status.getStatusCode() == OrderStatusCode.ORDER_HWID_NOT_LOGIN) {
                        if (status.hasResolution()) {
                            try {
                                status.startResolutionForResult(CSActivity.sharedActivity(), SETUP_FINISHED_ERROR);
                            } catch (IntentSender.SendIntentException exp) {
                                CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiIAP.nativeError(SETUP_FINISHED_ERROR));
                            }
                        }
                    }
                    else if (status.getStatusCode() == OrderStatusCode.ORDER_ACCOUNT_AREA_NOT_SUPPORTED) {
                        // The current country/region does not support HUAWEI IAP.
                        CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiIAP.nativeError(SETUP_FINISHED_ERROR));
                    }
                } else {
                    // Other external errors.
                    CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiIAP.nativeError(SETUP_FINISHED_ERROR));
                }
            }
        });
    }

    public void dispose() {
        if (_iapClient != null) {
            _iapClient = null;
        }
        _productIds = null;

        CSActivity.sharedActivity().removeActivityResultListener(this);
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == SETUP_FINISHED_ERROR) {
            if (data != null) {
                // Call the parseRespCodeFromIntent method to obtain the result of the API request.
                int returnCode = IapClientHelper.parseRespCodeFromIntent(data);
                // Use the parseCarrierIdFromIntent method to obtain the carrier ID returned by the API.
                String carrierId = IapClientHelper.parseCarrierIdFromIntent(data);
            }
        }
        else if (requestCode == PURCHASE_COMPLETE) {
            if (data == null) {
                Log.e("onActivityResult", "data is null");
                return;
            }
            // Call the parsePurchaseResultInfoFromIntent method to parse the payment result.
            PurchaseResultInfo purchaseResultInfo = _iapClient.parsePurchaseResultInfoFromIntent(data);
            switch(purchaseResultInfo.getReturnCode()) {
                case OrderStatusCode.ORDER_STATE_CANCEL:
                    CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiIAP.nativePurchaseCancelled());
                    break;
                case OrderStatusCode.ORDER_STATE_FAILED:
                case OrderStatusCode.ORDER_STATE_DEFAULT_CODE:
                case OrderStatusCode.ORDER_PRODUCT_OWNED:
                    Log.i(TAG, "onActivityResult(PURCHASE_COMPLETE) - orderStateCode : " + purchaseResultInfo.getReturnCode());
                    // Check whether the delivery is successful.
                    checkOwnedPurchaseProduct();
                    break;
                case OrderStatusCode.ORDER_STATE_SUCCESS:
                    // The payment is successful.
                    String inAppPurchaseData = purchaseResultInfo.getInAppPurchaseData();
                    String inAppPurchaseDataSignature = purchaseResultInfo.getInAppDataSignature();
                    // Verify the signature using your app's IAP public key.
                    // Deliver the product if the verification is successful.
                    // Call the consumeOwnedPurchase API to consume the product after delivery if the product is a consumable.

                    ConsumeOwnedPurchaseReq req = new ConsumeOwnedPurchaseReq();
                    try {
                        InAppPurchaseData inAppPuchaseDataBean = new InAppPurchaseData(inAppPurchaseData);
                        _pushToken = inAppPuchaseDataBean.getPurchaseToken();
                    } catch (JSONException e) {
                        CSActivity.sharedActivity().view().queueEvent(() -> {
                            if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                        });
                    }
                    req.setPurchaseToken(_pushToken);

                    Task<ConsumeOwnedPurchaseResult> task = _iapClient.consumeOwnedPurchase(req);
                    task.addOnSuccessListener(new OnSuccessListener<ConsumeOwnedPurchaseResult>() {
                        @Override
                        public void onSuccess(ConsumeOwnedPurchaseResult consumeOwnedPurchaseResult) {
                            final String receipt = consumeOwnedPurchaseResult.getConsumePurchaseData();
                            final String signature = consumeOwnedPurchaseResult.getDataSignature();
                            Log.i(TAG, "onActivityResult(PURCHASE_COMPLETE) - consume success, receipt : " + receipt);

                            CSActivity.sharedActivity().view().queueEvent(() -> {
                                if (_iapClient != null) CSHuaweiIAP.nativePurchaseComplete(_productId, _pushToken, _pushToken, _payload);
                            });
                        }
                    }).addOnFailureListener(new OnFailureListener() {
                        @Override
                        public void onFailure(Exception e) {
                            Log.i(TAG, "onActivityResult(PURCHASE_COMPLETE) - consume failed");
                            if (e instanceof IapApiException) {
                                IapApiException apiException = (IapApiException) e;
                                Status status = apiException.getStatus();
                                int returnCode = apiException.getStatusCode();

                                CSActivity.sharedActivity().view().queueEvent(() -> {
                                    if (_iapClient != null) CSHuaweiIAP.nativeError(returnCode);
                                });
                            } else {
                                // Other external errors.
                                CSActivity.sharedActivity().view().queueEvent(() -> {
                                    if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                                });
                            }
                        }
                    });
                    break;
                default:
                    break;
            }
        }
        else {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_iapClient != null) CSHuaweiIAP.nativeError(requestCode);
            });
        }
    }

    public void connect() {
        if (_iapClient != null) {
            CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiIAP.nativeConnected());
        }
    }

    public void sync(String[] productIds) {
        for (int i = 0; i < productIds.length; i++) {
            _productIds.add(productIds[i]);
        }

        ProductInfoReq req = new ProductInfoReq();
        req.setPriceType(0);
        req.setProductIds(_productIds);

        Task<ProductInfoResult> task = _iapClient.obtainProductInfo(req);
        task.addOnSuccessListener(new OnSuccessListener<ProductInfoResult>() {
            @Override
            public void onSuccess(ProductInfoResult result) {
                // Obtain the product details returned upon a successful API call.
                List<ProductInfo> productList = result.getProductInfoList();
                for (int i = 0; i < productList.size(); i++) {
                    ProductInfo productInfo = productList.get(i);
                    final String productId = productInfo.getProductId();
                    final String price = productInfo.getPrice();
                    final String originPrice = Double.toString((double) productInfo.getOriginalMicroPrice() / 1000000);
                    final String currencyCode = productInfo.getCurrency();

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeSyncProduct(productId, price, originPrice, currencyCode);
                    });
                }

                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_iapClient != null) CSHuaweiIAP.nativeSync();
                });
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(Exception e) {
                if (e instanceof IapApiException) {
                    IapApiException apiException = (IapApiException) e;
                    int returnCode = apiException.getStatusCode();

                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(returnCode);
                    });
                } else {
                    // Other external errors.
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(QUERY_FINISHED_ERROR);
                    });
                }
            }
        });

        checkOwnedPurchaseProduct();
    }

    public void purchase(String productId, String payload) {
        _productId = productId;
        _payload = payload;

        PurchaseIntentReq req = new PurchaseIntentReq();
        req.setProductId(_productId);
        req.setPriceType(0);
        req.setDeveloperPayload(_payload);

        Task<PurchaseIntentResult> task = _iapClient.createPurchaseIntent(req);
        task.addOnSuccessListener(new OnSuccessListener<PurchaseIntentResult>() {
            @Override
            public void onSuccess(PurchaseIntentResult result) {
                // Obtain the order creation result.
                Status status = result.getStatus();
                if (status.hasResolution()) {
                    try {
                        // Open the checkout screen returned.
                        status.startResolutionForResult(CSActivity.sharedActivity(), PURCHASE_COMPLETE);
                    } catch (IntentSender.SendIntentException exp) {
                        CSActivity.sharedActivity().view().queueEvent(() -> {
                            if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                        });
                    }
                }
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(Exception e) {
                if (e instanceof IapApiException) {
                    IapApiException apiException = (IapApiException) e;
                    Status status = apiException.getStatus();
                    int returnCode = apiException.getStatusCode();
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(returnCode);
                    });
                } else {
                    // Other external errors.
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                    });
                }
            }
        });
    }

    private void checkOwnedPurchaseProduct() {
        OwnedPurchasesReq ownedPurchasesReq = new OwnedPurchasesReq();
        ownedPurchasesReq.setPriceType(0);
        Task<OwnedPurchasesResult> task = _iapClient.obtainOwnedPurchases(ownedPurchasesReq);
        task.addOnSuccessListener(new OnSuccessListener<OwnedPurchasesResult>() {
            @Override
            public void onSuccess(OwnedPurchasesResult result) {
                // Obtain the execution result if the request is successful.
                if (result != null && result.getInAppPurchaseDataList() != null) {
                    for (int i = 0; i < result.getInAppPurchaseDataList().size(); i++) {
                        String inAppPurchaseData = result.getInAppPurchaseDataList().get(i);
                        String inAppSignature = result.getInAppSignature().get(i);
                        // Use the IAP public key to verify the signature of inAppPurchaseData.
                        // Check the purchase status of each product if the verification is successful. When the payment has been made, deliver the required product. After a successful delivery, consume the product.
                        try {
                            InAppPurchaseData inAppPurchaseDataBean = new InAppPurchaseData(inAppPurchaseData);
                            int purchaseState = inAppPurchaseDataBean.getPurchaseState();
                            if (purchaseState == 0) {
                                ConsumeOwnedPurchaseReq req = new ConsumeOwnedPurchaseReq();
                                try {
                                    InAppPurchaseData inAppPuchaseDataBean = new InAppPurchaseData(inAppPurchaseData);
                                    _pushToken = inAppPuchaseDataBean.getPurchaseToken();
                                } catch (JSONException e) {
                                    CSActivity.sharedActivity().view().queueEvent(() -> {
                                        if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                                    });
                                }
                                req.setPurchaseToken(_pushToken);

                                Task<ConsumeOwnedPurchaseResult> task = _iapClient.consumeOwnedPurchase(req);
                                task.addOnSuccessListener(new OnSuccessListener<ConsumeOwnedPurchaseResult>() {
                                    @Override
                                    public void onSuccess(ConsumeOwnedPurchaseResult consumeOwnedPurchaseResult) {
                                        final String receipt = consumeOwnedPurchaseResult.getConsumePurchaseData();
                                        final String signature = consumeOwnedPurchaseResult.getDataSignature();
                                        CSActivity.sharedActivity().view().queueEvent(() -> {
                                            if (_iapClient != null) CSHuaweiIAP.nativePurchaseComplete(_productId, _pushToken, _pushToken, _payload);
                                        });
                                    }
                                }).addOnFailureListener(new OnFailureListener() {
                                    @Override
                                    public void onFailure(Exception e) {
                                        if (e instanceof IapApiException) {
                                            IapApiException apiException = (IapApiException) e;
                                            Status status = apiException.getStatus();
                                            int returnCode = apiException.getStatusCode();

                                            CSActivity.sharedActivity().view().queueEvent(() -> {
                                                if (_iapClient != null) CSHuaweiIAP.nativeError(returnCode);
                                            });
                                        } else {
                                            // Other external errors.
                                            CSActivity.sharedActivity().view().queueEvent(() -> {
                                                if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                                            });
                                        }
                                    }
                                });
                            }
                        } catch (JSONException e) {
                            CSActivity.sharedActivity().view().queueEvent(() -> {
                                if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                            });
                        }
                    }
                }
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(Exception e) {
                if (e instanceof IapApiException) {
                    IapApiException apiException = (IapApiException) e;
                    Status status = apiException.getStatus();
                    int returnCode = apiException.getStatusCode();
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(returnCode);
                    });
                } else {
                    // Other external errors.
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_iapClient != null) CSHuaweiIAP.nativeError(PURCHASE_COMPLETE_ERROR);
                    });
                }
            }
        });
    }
}

class CSHuaweiIAP {
    private static CSHuaweiIAPImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSHuaweiIAPImpl());
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

    public static native void nativeConnected();
    public static native void nativeSync();
    public static native void nativeSyncProduct(String productId, String price, String originPrice, String currencyCode);
    public static native void nativePurchaseComplete(String productId, String chargeData1, String chargeData2, String payload);
    public static native void nativePurchaseCancelled();
    public static native void nativeError(int errorCode);
}
