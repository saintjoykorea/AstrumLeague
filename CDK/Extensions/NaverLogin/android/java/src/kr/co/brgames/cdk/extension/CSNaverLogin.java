package kr.co.brgames.cdk.extension;

import com.nhn.android.naverlogin.OAuthLogin;
import com.nhn.android.naverlogin.OAuthLoginHandler;
import com.nhn.android.naverlogin.data.OAuthErrorCode;

import android.util.Log;

import kr.co.brgames.cdk.CSActivity;

public class CSNaverLogin {
	private static final int LOGIN_RESULT_SUCCESS = 0;
    private static final int LOGIN_RESULT_CANCELLED = 1;
    private static final int LOGIN_RESULT_ERROR = 2;
	
	private static final String TAG = "CSNaverLogin";

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> {
			final String appName = CSActivity.sharedActivity().resourceString("app_name");
			final String clientId = CSActivity.sharedActivity().resourceString("naver_client_id");
			final String clientSecret = CSActivity.sharedActivity().resourceString("naver_client_secret");
			
			assert appName != null && clientId != null && clientSecret != null;
			
			OAuthLogin.getInstance().init(CSActivity.sharedActivity(), clientId, clientSecret, appName);
        });
    }
    
    public static void login() {
        CSActivity.sharedActivity().queueEvent(() -> {
			OAuthLogin.getInstance().startOauthLoginActivity(CSActivity.sharedActivity(), new OAuthLoginHandler() {
				@Override
				public void run(boolean success) {
					if (success) {
						CSActivity.sharedActivity().view().queueEvent(() -> CSNaverLogin.nativeLoginResult(LOGIN_RESULT_SUCCESS));
					} else {
						String errorCode = OAuthLogin.getInstance().getLastErrorCode(CSActivity.sharedActivity()).getCode();
						String errorDesc = OAuthLogin.getInstance().getLastErrorDesc(CSActivity.sharedActivity());
						
						Log.e(TAG, String.format("login error:%s(%s)", errorCode, errorDesc));
						
						final int result = errorCode.equals(OAuthErrorCode.CLIENT_USER_CANCEL) ? LOGIN_RESULT_CANCELLED : LOGIN_RESULT_ERROR;
						
						CSActivity.sharedActivity().view().queueEvent(() -> CSNaverLogin.nativeLoginResult(result));
					}
				}
			});
        });
    }

    public static void logout() {
        CSActivity.sharedActivity().queueEvent(() -> OAuthLogin.getInstance().logout(CSActivity.sharedActivity()));
    }

    public static boolean isConnected() {
        return accessToken() != null;
    }
	
	public static String accessToken() {
        return OAuthLogin.getInstance().getAccessToken(CSActivity.sharedActivity());
    }

    public static native void nativeLoginResult(int result);
}
