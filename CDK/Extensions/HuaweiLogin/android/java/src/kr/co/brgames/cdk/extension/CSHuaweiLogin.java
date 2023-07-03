package kr.co.brgames.cdk.extension;

import com.huawei.hmf.tasks.OnFailureListener;
import com.huawei.hmf.tasks.OnSuccessListener;
import com.huawei.hmf.tasks.Task;
import com.huawei.hms.common.ApiException;
import com.huawei.hms.support.account.AccountAuthManager;
import com.huawei.hms.support.account.request.AccountAuthParams;
import com.huawei.hms.support.account.request.AccountAuthParamsHelper;
import com.huawei.hms.support.account.result.AuthAccount;
import com.huawei.hms.support.account.service.AccountAuthService;

import android.content.Intent;
import android.util.Log;

import kr.co.brgames.cdk.CSActivity;

class CSHuaweiLoginImpl implements CSActivity.ActivityResultListener {
	private static final int REQUEST_CODE_SIGN_IN = 1000;

	private static final int LOGIN_RESULT_SUCCESS = 0;
	private static final int LOGIN_RESULT_CANCELLED = 1;
    private static final int LOGIN_RESULT_ERROR = 2;
	
	private static final String TAG = "CSHuaweiLogin";

	private AccountAuthService _authService;
	private AccountAuthParams _authParam;
	private String _userId;

	public CSHuaweiLoginImpl() {
		CSActivity.sharedActivity().addActivityResultListener(this);
		Log.i(TAG, "CSHuaweiLoginImpl()");
		_authParam = new AccountAuthParamsHelper(AccountAuthParams.DEFAULT_AUTH_REQUEST_PARAM)
				.setEmail()
				.setIdToken()
				.createParams();

		_authService = AccountAuthManager.getService(CSActivity.sharedActivity(), _authParam);
	}
    
    public void login() {
		Log.i(TAG, "login() - start");
		Task<AuthAccount> task = _authService.silentSignIn();
		task.addOnSuccessListener(new OnSuccessListener<AuthAccount>() {
			@Override
			public void onSuccess(AuthAccount authAccount) {
				Log.i(TAG, "login() - onSuccess : code " + authAccount.getAuthorizationCode());
				_userId = authAccount.getOpenId();
				Log.i(TAG, "login() - onSuccess : userId = " + authAccount.getAccessToken());
				CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiLogin.nativeLoginResult(LOGIN_RESULT_SUCCESS));
			}
		});
		task.addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				if (e instanceof ApiException) {
					Log.i(TAG, "login() - onFailure : ApiException message = " + e.getMessage() + ", code = " + ((ApiException) e).getStatusCode());
					ApiException apiException = (ApiException) e;
					Intent signInIntent = _authService.getSignInIntent();
					CSActivity.sharedActivity().startActivityForResult(signInIntent, REQUEST_CODE_SIGN_IN);
				}
			}
		});
    }

	public void dispose() {
		CSActivity.sharedActivity().removeActivityResultListener(this);
	}

    public void logout() {
		if (_authService == null) {
			return;
		}
		Task<Void> signOutTask = _authService.signOut();
		signOutTask.addOnSuccessListener(new OnSuccessListener<Void>() {
			@Override
			public void onSuccess(Void aVoid) {
				Log.i(TAG, "signOut Success");
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				Log.i(TAG, "signOut fail");
			}
		});
    }

    public boolean isConnected() {
        return _userId != null;
    }

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.i(TAG, "onActivityResult() - start : requestCode(" + requestCode + ")");
		if (requestCode == REQUEST_CODE_SIGN_IN) {
			Log.i(TAG, "onActivitResult of sigInInIntent, request code: " + REQUEST_CODE_SIGN_IN);
			Task<AuthAccount> authAccountTask = AccountAuthManager.parseAuthResultFromIntent(data);
			if (authAccountTask.isSuccessful()) {
				AuthAccount authAccount = authAccountTask.getResult();
				Log.i(TAG, "onActivitResult - authAccountTask.getAccessToken() = " + authAccount.getAccessToken());
				Log.i(TAG, "onActivitResult - authAccountTask.getIdToken() = " + authAccount.getIdToken());
				Log.i(TAG, "onActivitResult - authAccountTask.getUid() = " + authAccount.getUid());
				Log.i(TAG, "onActivitResult - authAccountTask.getAuthorizationCode() = " + authAccount.getAuthorizationCode());
				Log.i(TAG, "onActivitResult - authAccountTask.getOpenId() = " + authAccount.getOpenId());
				_userId = authAccount.getOpenId();
				CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiLogin.nativeLoginResult(LOGIN_RESULT_SUCCESS));
			}
			else {
				Log.e(TAG, "sign in failed : " + ((ApiException) authAccountTask.getException()).getStatusCode());
				CSActivity.sharedActivity().view().queueEvent(() -> CSHuaweiLogin.nativeLoginResult(((ApiException) authAccountTask.getException()).getStatusCode()));
			}
		}
	}

	public String userId() {
		return _userId;
	}
}


public class CSHuaweiLogin {
	private static CSHuaweiLoginImpl _impl;

	public static void initialize() {
		CSActivity.sharedActivity().queueEvent(() -> _impl = new CSHuaweiLoginImpl());
	}

	public static void dispose() {
		if (_impl != null) {
			CSActivity.sharedActivity().queueEvent(() -> {
				_impl.dispose();
				_impl = null;
			});
		}
	}

	public static void login() {
		CSActivity.sharedActivity().queueEvent(() -> _impl.login());
	}

	public static void logout() {
		CSActivity.sharedActivity().queueEvent(() -> _impl.logout());
	}

	public static boolean isConnected() {
		return _impl.isConnected();
	}

	public static String userId() {
		return _impl.userId();
	}

	public static native void nativeLoginResult(int result);
}