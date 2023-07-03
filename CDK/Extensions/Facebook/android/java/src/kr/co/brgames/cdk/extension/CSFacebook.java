package kr.co.brgames.cdk.extension;

import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.Profile;
import com.facebook.AccessToken;
import com.facebook.LoginStatusCallback;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;

import java.util.Collections;

import kr.co.brgames.cdk.CSActivity;

class CSFacebookImpl implements CSActivity.ActivityResultListener {
	private static final String TAG = "CSFacebook";
	
	private static final int LOGIN_RESULT_SUCCESS = 0;
	private static final int LOGIN_RESULT_CANCELLED = 1;
	private static final int LOGIN_RESULT_ERROR = 2;
	
    private final CallbackManager _callbackManager;
    private String _userId;

    public CSFacebookImpl() {
        CSActivity.sharedActivity().addActivityResultListener(this);

        _callbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(_callbackManager, new FacebookCallback<>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                Log.i(TAG, "FacebookCallback success");

                _userId = loginResult.getAccessToken().getUserId();

                notifyLoginResult(LOGIN_RESULT_SUCCESS);
            }

            @Override
            public void onCancel() {
                Log.w(TAG, "FacebookCallback cancel");

                _userId = null;

                notifyLoginResult(LOGIN_RESULT_CANCELLED);
            }

            @Override
            public void onError(@NonNull FacebookException e) {
                Log.e(TAG, "FacebookCallback error:" + e.getMessage());

                _userId = null;

                notifyLoginResult(LOGIN_RESULT_ERROR);
            }
        });
    }

    public void dispose() {
        CSActivity.sharedActivity().removeActivityResultListener(this);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        _callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    private void notifyLoginResult(int result) {
        CSActivity.sharedActivity().view().queueEvent(() -> CSFacebook.nativeLoginResult(result));
    }

    public void login() {
        if (_userId != null) {
            Log.i(TAG, "already logged in");

            notifyLoginResult(LOGIN_RESULT_SUCCESS);
            return;
        }

        Log.i(TAG, "login");

        LoginManager.getInstance().retrieveLoginStatus(CSActivity.sharedActivity(), new LoginStatusCallback() {
            @Override
            public void onCompleted(AccessToken accessToken) {
                Log.i(TAG, "LoginStatusCallback completed");

                _userId = accessToken.getUserId();

                notifyLoginResult(LOGIN_RESULT_SUCCESS);
            }

            @Override
            public void onFailure() {
                Log.w(TAG, "LoginStatusCallback failure");

                LoginManager.getInstance().logInWithReadPermissions(CSActivity.sharedActivity(), Collections.singletonList("public_profile"));
            }

            @Override
            public void onError(Exception e) {
                Log.e(TAG, "LoginStatusCallback error:" + e.getMessage());

                LoginManager.getInstance().logInWithReadPermissions(CSActivity.sharedActivity(), Collections.singletonList("public_profile"));
            }
        });
    }

    public void logout() {
        Log.i(TAG, "logout");

        _userId = null;

        LoginManager.getInstance().logOut();
    }

    public boolean isConnected() {
        return _userId != null;
    }

    public String userId() {
        return _userId;
    }

    public String pictureUrl() {
        Profile profile = Profile.getCurrentProfile();
        if (profile != null) {
            return profile.getProfilePictureUri(140, 140).toString();
        }
        return null;
    }
}

public class CSFacebook {
    private static CSFacebookImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSFacebookImpl());
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
