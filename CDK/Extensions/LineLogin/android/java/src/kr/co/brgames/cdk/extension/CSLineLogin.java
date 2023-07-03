package kr.co.brgames.cdk.extension;

import android.app.Activity;
import android.app.Application;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.linecorp.linesdk.LineApiResponse;
import com.linecorp.linesdk.LineProfile;
import com.linecorp.linesdk.Scope;
import com.linecorp.linesdk.api.LineApiClient;
import com.linecorp.linesdk.api.LineApiClientBuilder;
import com.linecorp.linesdk.auth.LineenticationParams;
import com.linecorp.linesdk.auth.LineLoginApi;
import com.linecorp.linesdk.auth.LineLoginResult;

import java.util.Arrays;

import kr.co.brgames.cdk.CSActivity;

class CSLineLoginImpl implements CSActivity.ActivityResultListener {
    private static final int RC_LOGIN = 10200;

	private static final int LOGIN_RESULT_SUCCESS = 0;
    private static final int LOGIN_RESULT_CANCELLED = 1;
    private static final int LOGIN_RESULT_ERROR = 2;
	
    private static final String TAG = "CSLineLogin";
    
    private LineApiClient _lineApiClient;
    private LineProfile _linePofile;

    public CSLineLoginImpl() {
        String channelId = CSActivity.sharedActivity().resourceString("line_channel_id");
		
		if (channelId == null) {
			Log.e(TAG, "not ready");
			System.exit(0);
		}
		
        CSActivity.sharedActivity().addActivityResultListener(this);

        LineApiClientBuilder apiClientBuilder = new LineApiClientBuilder(CSActivity.sharedActivity(), channelId);
        _lineApiClient = apiClientBuilder.build();
    }

    public void dispose() {
        CSActivity.sharedActivity().removeActivityResultListener(this);
    }
    
    public void login() {
        String channelId = CSActivity.sharedActivity().resourceString("line_channel_id");
        Intent loginIntent = LineLoginApi.getLoginIntent(
                CSActivity.sharedActivity().view().getContext(),
                channelId,
                new LineenticationParams.Builder()
                        .scopes(Arrays.asList(Scope.PROFILE))
                        .build());
        CSActivity.sharedActivity().startActivityForResult(loginIntent, RC_LOGIN);
    }

    public void logout() {
		_linePofile = null;
		
		_lineApiClient.logout();
    }

    public boolean isConnected() {
        return _linePofile != null;
    }
	
	public String userId() {
		return _linePofile != null ? _linePofile.getUserId() : null;
	}

    public String pictureUrl() {
        return _linePofile != null ? _linePofile.getPictureUrl().toString() : null;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode != RC_LOGIN) {
            return;
        }

        LineLoginResult loginResult = LineLoginApi.getLoginResultFromIntent(data);

        switch (loginResult.getResponseCode()) {
            case SUCCESS:
                _linePofile = loginResult.getLineProfile();
                CSActivity.sharedActivity().view().queueEvent(() -> CSLineLogin.nativeLoginResult(LOGIN_RESULT_SUCCESS));
                break;
            case CANCEL:
                _linePofile = null;
                Log.e(TAG, "login cancelled");
                CSActivity.sharedActivity().view().queueEvent(() -> CSLineLogin.nativeLoginResult(LOGIN_RESULT_CANCELLED));
                break;
            default:
                _linePofile = null;
                Log.e(TAG, loginResult.getErrorData().toString());
                CSActivity.sharedActivity().view().queueEvent(() -> CSLineLogin.nativeLoginResult(LOGIN_RESULT_ERROR));
				break;
        }
    }
}

public class CSLineLogin {
    private static CSLineLoginImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSLineLoginImpl());
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

    public static String pictureUrl() {
        return _impl.pictureUrl();
    }

    public static native void nativeLoginResult(int result);
}
