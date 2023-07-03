package kr.co.brgames.cdk.extension;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.text.Html;
import android.util.Log;

import com.onestore.iap.api.IapEnum;
import com.onestore.iap.api.IapResult;
import com.onestore.iap.api.ProductDetail;
import com.onestore.iap.api.PurchaseClient;
import com.onestore.iap.api.PurchaseData;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import kr.co.brgames.cdk.CSActivity;

class CSOneStoreIAPImpl implements CSActivity.ActivityResultListener {
    private static final String TAG = "CSOneStoreIAP";
    
    private static final int IAP_API_VERSION = 5;

    private static final int RC_LOGIN = 10300;
    private static final int RC_PURCHASE = 10301;

    private static final int ERROR_DISCONNECTED = 50000;
    private static final int ERROR_NEED_UPDATE = 50001;
    private static final int ERROR_REMOTE = 50002;
    private static final int ERROR_SECURITY = 50003;
    private static final int ERROR_LOGIN_FAIL = 50004;
    private static final int ERROR_INVALID_PURCHASE = 50005;

    private PurchaseClient _client;
    private int _reservedPurchaseCount;
    
    public CSOneStoreIAPImpl() {
        CSActivity.sharedActivity().addActivityResultListener(this);

        _client = new PurchaseClient(CSActivity.sharedActivity(), AppSecurity.getPublicKey());
    }

    public void dispose() {
        if (_client != null) {
            _client.terminate();
            _client = null;

            CSActivity.sharedActivity().removeActivityResultListener(this);
        }
    }
    
    public void connect() {
        _client.connect(mServiceConnectionListener);
    }
    
    public void sync(String[] productIds) {
        _client.queryProductsAsync(IAP_API_VERSION, new ArrayList<String>(Arrays.asList(productIds)), IapEnum.ProductType.IN_APP.getType(), mQueryProductsListener);
    }

    public void purchase(String productId, String payload) {
        _client.launchPurchaseFlowAsync(IAP_API_VERSION, CSActivity.sharedActivity(), RC_PURCHASE, productId, "", IapEnum.ProductType.IN_APP.getType(), payload, "", false, mPurchaseFlowListener);
    }
    
    PurchaseClient.ServiceConnectionListener mServiceConnectionListener = new PurchaseClient.ServiceConnectionListener() {
        public void onConnected() {
            Log.d(TAG, "ServiceConnectionListener onConnected");
    
            if (_client != null) _client.isBillingSupportedAsync(IAP_API_VERSION, mBillingSupportedListener);
        }

        public void onDisconnected() {
            Log.e(TAG, "ServiceConnectionListener onDisconnected");

            error(ERROR_DISCONNECTED);
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "ServiceConnectionListener onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }
    };

    PurchaseClient.BillingSupportedListener mBillingSupportedListener = new PurchaseClient.BillingSupportedListener() {
        public void onSuccess() {
            Log.d(TAG, "BillingSupportedListener onSuccess");

            if (_client != null) {
                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_client != null) CSOneStoreIAP.nativeConnected();
                });
            }
        }

        public void onError(IapResult result) {
            Log.e(TAG, "BillingSupportedListener onError:" + result.toString());

            if (_client != null) {
                if (IapResult.RESULT_NEED_LOGIN == result) {
                    alertDecision("원스토어 계정 정보를 확인 할 수 없습니다. 로그인 하시겠습니까?", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            if (_client != null)  _client.launchLoginFlowAsync(IAP_API_VERSION, CSActivity.sharedActivity(), RC_LOGIN, mLoginFlowListener);
                        }
                    });
                }
                else {
                    error(result.getCode());
                }
            }
        }

        public void onErrorRemoteException() {
            Log.e(TAG, "BillingSupportedListener onErrorRemoteException");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            Log.e(TAG, "BillingSupportedListener onErrorSecurityException");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "BillingSupportedListener onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }
    };

    PurchaseClient.QueryProductsListener mQueryProductsListener = new PurchaseClient.QueryProductsListener() {
        public void onSuccess(List<ProductDetail> productDetails) {
            Log.d(TAG, "QueryProductsListener onSuccess, " + productDetails.toString());

            if (_client != null) {
                for (int i = 0; i < productDetails.size(); i++) {
                    ProductDetail productDetail = productDetails.get(i);
                    final String productId = productDetail.getProductId();
                    final String price = productDetail.getPrice();

                    CSActivity.sharedActivity().view().queueEvent(() -> {
						if (_client != null) CSOneStoreIAP.nativeSyncProduct(productId, price);
                    });
                }

                _client.queryPurchasesAsync(IAP_API_VERSION, IapEnum.ProductType.IN_APP.getType(), mQueryPurchaseListener);
            }
        }

        public void onErrorRemoteException() {
            Log.e(TAG, "QueryProductsListener onErrorRemoteException");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            Log.e(TAG, "QueryProductsListener onErrorSecurityException");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "QueryProductsListener onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }

        public void onError(IapResult result) {
            Log.e(TAG, "QueryProductsListener onError:" + result.toString());

            error(result.getCode());

            alert(result.getDescription());
        }
    };

    PurchaseClient.QueryPurchaseListener mQueryPurchaseListener = new PurchaseClient.QueryPurchaseListener() {
        public void onSuccess(List<PurchaseData> purchaseDataList, String productType) {
            Log.d(TAG, "QueryPurchaseListener onSuccess");

            _reservedPurchaseCount = 0;

            if (IapEnum.ProductType.IN_APP.getType().equalsIgnoreCase(productType)) {
                if (_client != null) {
                    for (PurchaseData purchase : purchaseDataList) {
                        boolean result = AppSecurity.verifyPurchase(purchase.getPurchaseData(), purchase.getSignature());

                        if (result) {
                            _reservedPurchaseCount++;
                            _client.consumeAsync(IAP_API_VERSION, purchase, mConsumeListener);
                        }
                        else {
                            String productId = purchase.getProductId();
                            String purchaseId = purchase.getPurchaseId();
                            String signature = purchase.getSignature();
                            String payload = purchase.getDeveloperPayload();

                            Log.e(TAG, "QueryPurchaseListener invalid purchase:" + productId + ", " + purchaseId + ", " + signature + ", " + payload);
                        }
                    }
                }
            }
            else {
                Log.e(TAG, "QueryPurchaseListener onSuccess:" + productType + " not supported");
            }
            if (_client != null && _reservedPurchaseCount == 0) {
                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_client != null) CSOneStoreIAP.nativeSync();
                });
            }
        }

        public void onErrorRemoteException() {
            Log.e(TAG, "QueryPurchaseListener onErrorRemoteException");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            Log.e(TAG, "QueryPurchaseListener onErrorSecurityException");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "QueryPurchaseListener onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }

        public void onError(IapResult result) {
            Log.e(TAG, "QueryPurchaseListener onError:" + result.toString());

            error(result.getCode());

            alert(result.getDescription());
        }
    };

    PurchaseClient.ConsumeListener mConsumeListener = new PurchaseClient.ConsumeListener() {
        public void onSuccess(PurchaseData purchaseData) {
            if (_client != null) {
                final String productId = purchaseData.getProductId();
                final String purchaseId = purchaseData.getPurchaseId();
                final String signature = purchaseData.getSignature();
                final String payload = purchaseData.getDeveloperPayload();

                CSActivity.sharedActivity().view().queueEvent(() -> {
					if (_client != null) CSOneStoreIAP.nativePurchaseComplete(productId, purchaseId, signature, payload);
                });
                if (_reservedPurchaseCount > 0) {
                    _reservedPurchaseCount--;
                    if (_reservedPurchaseCount == 0) {
                        CSActivity.sharedActivity().view().queueEvent(() -> {
							if (_client != null) CSOneStoreIAP.nativeSync();
                        });
                    }
                }
            }
        }

        public void onErrorRemoteException() {
            _reservedPurchaseCount = 0;

            Log.e(TAG, "ConsumeListener onErrorRemoteException");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            _reservedPurchaseCount = 0;

            Log.e(TAG, "ConsumeListener onErrorSecurityException");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            _reservedPurchaseCount = 0;

            Log.e(TAG, "ConsumeListener onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }

        public void onError(IapResult result) {
            _reservedPurchaseCount = 0;

            Log.e(TAG, "ConsumeListener onError:" + result.toString());

            error(result.getCode());

            alert(result.getDescription());
        }
    };

    PurchaseClient.PurchaseFlowListener mPurchaseFlowListener = new PurchaseClient.PurchaseFlowListener() {
        public void onSuccess(PurchaseData purchaseData) {
            boolean validPurchase = AppSecurity.verifyPurchase(purchaseData.getPurchaseData(), purchaseData.getSignature());

            if (validPurchase) {
                if (_client != null) _client.consumeAsync(IAP_API_VERSION, purchaseData, mConsumeListener);
            }
            else {
                error(ERROR_INVALID_PURCHASE);

                alert("Signature 정보가 유효하지 않습니다.\n\nSignature information is invalid.");
            }
        }

        public void onError(IapResult result) {
            Log.e(TAG, "PurchaseFlowListener onError:" + result.toString());

            error(result.getCode());

            alert(result.getDescription());
        }

        public void onErrorRemoteException() {
            Log.e(TAG, "launchPurchaseFlowAsync onErrorRemoteException");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            Log.e(TAG, "launchPurchaseFlowAsync onErrorSecurityException");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "launchPurchaseFlowAsync onErrorNeedUpdateException");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }
    };

    PurchaseClient.LoginFlowListener mLoginFlowListener = new PurchaseClient.LoginFlowListener() {
        public void onSuccess() {
            if (_client != null) _client.isBillingSupportedAsync(IAP_API_VERSION, mBillingSupportedListener);
        }

        public void onError(IapResult result) {
            Log.e(TAG, "launchLoginFlowAsync onError:" + result.toString());

            error(result.getCode());

            if (IapResult.RESULT_USER_CANCELED == result) {
                alert("원스토어 로그인이 취소 되었습니다");
            }
        }

        public void onErrorRemoteException() {
            Log.e(TAG, "launchLoginFlowAsync onError, 원스토어 서비스와 연결을 할 수 없습니다");

            error(ERROR_REMOTE);

            alert("원스토어 서비스와 연결을 할 수 없습니다");
        }

        public void onErrorSecurityException() {
            Log.e(TAG, "launchLoginFlowAsync onError, 비정상 앱에서 결제가 요청되었습니다");

            error(ERROR_SECURITY);

            alert("원스토어 설치와 원스토어 약관 동의를 확인해 주시길 바랍니다");
        }

        public void onErrorNeedUpdateException() {
            Log.e(TAG, "launchLoginFlowAsync onError, 원스토어 서비스앱의 업데이트가 필요합니다");

            error(ERROR_NEED_UPDATE);

            PurchaseClient.launchUpdateOrInstallFlow(CSActivity.sharedActivity());
        }
    };

    public void alert(final String message) {
        alert(message, false);
    }

    public void alert(final String message, final boolean isHtml) {
        AlertDialog.Builder bld = new AlertDialog.Builder(CSActivity.sharedActivity());
        bld.setMessage(isHtml ? Html.fromHtml(message) : message)
                .setPositiveButton("확인", null)
                .create()
                .show();
    }

    public void alertDecision(final String message, final DialogInterface.OnClickListener posListener) {
        AlertDialog.Builder bld = new AlertDialog.Builder(CSActivity.sharedActivity());
        bld.setMessage(message)
                .setPositiveButton("예", posListener)
                .setNegativeButton("아니요", null)
                .create()
                .show();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (_client != null) {
            switch (requestCode) {
                case RC_LOGIN:
                    if (resultCode == Activity.RESULT_OK) {
                        _client.handleLoginData(data);
                    }
                    else {
                        Log.e(TAG, "onActivityResult login error:" + resultCode);
                        error(ERROR_LOGIN_FAIL);
                    }
                    break;

                case RC_PURCHASE:
                    if (resultCode == Activity.RESULT_OK) {
                        _client.handlePurchaseData(data);
                    }
                    else {
                        Log.e(TAG, "onActivityResult purchase error:" + resultCode);
                        CSActivity.sharedActivity().view().queueEvent(() -> {
							if (_client != null) CSOneStoreIAP.nativePurchaseCancelled();
                        });
                    }
                    break;
            }
        }
    }

    private void error(final int code) {
        if (_client != null) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
				if (_client != null) CSOneStoreIAP.nativeError(code);
            });
        }
    }
}

class CSOneStoreIAP {
    private static CSOneStoreIAPImpl _impl;
    
    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSOneStoreIAPImpl());
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
    public static native void nativeSyncProduct(String productId, String price);
    public static native void nativeSync();
    public static native void nativePurchaseComplete(String productId, String chargeData1, String chargeData2, String payload);
    public static native void nativePurchaseCancelled();
    public static native void nativeError(int errorCode);
}
