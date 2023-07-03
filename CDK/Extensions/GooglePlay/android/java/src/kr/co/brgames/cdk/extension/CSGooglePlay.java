package kr.co.brgames.cdk.extension;

import android.content.Intent;
import android.util.Log;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesActivityResultCodes;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.tasks.Task;
import com.google.android.play.core.review.ReviewException;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;
import com.google.firebase.auth.AuthCredential;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.auth.PlayGamesAuthProvider;

import kr.co.brgames.cdk.CSActivity;

class CSGooglePlayImpl implements CSActivity.ActivityResultListener {
    private static final int RC_SIGN_IN = 10100;
    private static final int RC_SHOW_ACHIEVEMENTS = 10101;
    private static final int RC_SHOW_LEADERBOARD = 10102;

    private static final int LOGIN_RESULT_SUCCESS = 0;
    private static final int LOGIN_RESULT_CANCELLED = 1;
    private static final int LOGIN_RESULT_ERROR = 2;

    private static final String TAG = "CSGooglePlay";

    private GoogleSignInClient _googleSignInClient;
    private AchievementsClient _achievementsClient;
    private LeaderboardsClient _leaderboardsClient;
	private String _leaderBoardId;
    private String _userId;
	private String _email;
	private String _imageUrl;
	
    public CSGooglePlayImpl() {
        CSActivity.sharedActivity().addActivityResultListener(this);

		String clientId = CSActivity.sharedActivity().resourceString("google_play_client_id");
		
		_leaderBoardId = CSActivity.sharedActivity().resourceString("google_play_leader_board_id");

		assert clientId != null && _leaderBoardId != null;

        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN)
				.requestEmail()
				.requestProfile()
                .requestServerAuthCode(clientId)
                .requestIdToken(clientId)
                .build();

        _googleSignInClient =  GoogleSignIn.getClient(CSActivity.sharedActivity(), gso);
    }

    public void dispose() {
        CSActivity.sharedActivity().removeActivityResultListener(this);
    }

    public void login() {
        Task<GoogleSignInAccount> task = _googleSignInClient.silentSignIn();
        if (task.isSuccessful()) {
            GoogleSignInAccount account = task.getResult();
            onConnected(account);
        } else {
            task.addOnCompleteListener(CSActivity.sharedActivity(), task1 -> {
                if (task1.isSuccessful()) {
                    GoogleSignInAccount account = task1.getResult();
                    onConnected(account);
                } else {
                    Intent signInIntent = _googleSignInClient.getSignInIntent();
                    CSActivity.sharedActivity().startActivityForResult(signInIntent, RC_SIGN_IN);
                }
            });
        }
    }
	
	public void logout() {
		_googleSignInClient.signOut();
		
		onDisconnected();
	}
	
	public boolean isConnected() {
        return _userId != null;
    }
	
	public String userId() {
        return _userId;
    }
	
	public String email() {
        return _email;
    }
	
	public String imageUrl() {
        return _imageUrl;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == GamesActivityResultCodes.RESULT_RECONNECT_REQUIRED) {
            onDisconnected();

            login();
        }
        else if (requestCode == RC_SIGN_IN) {
            Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);

            try {
                GoogleSignInAccount account = task.getResult(ApiException.class);
                onConnected(account);
            }
            catch (ApiException e) {
				Log.e(TAG, "google sign in error", e);

                onDisconnected();
				
				final int result = e.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_CANCELLED ? LOGIN_RESULT_CANCELLED : LOGIN_RESULT_ERROR;

                CSActivity.sharedActivity().view().queueEvent(() -> CSGooglePlay.nativeLoginResult(result));
            }
        }
    }

    private void onConnected(final GoogleSignInAccount account) {
        AuthCredential credential = PlayGamesAuthProvider.getCredential(account.getServerAuthCode());
		
        FirebaseAuth.getInstance().signInWithCredential(credential).addOnCompleteListener(CSActivity.sharedActivity(), task -> {
            if (task.isSuccessful()) {
                FirebaseUser user = FirebaseAuth.getInstance().getCurrentUser();

                _userId = user.getUid();
                _email = account.getEmail();
                _imageUrl = account.getPhotoUrl() != null ? account.getPhotoUrl().toString() : null;

                _achievementsClient = Games.getAchievementsClient(CSActivity.sharedActivity(), account);
                _leaderboardsClient = Games.getLeaderboardsClient(CSActivity.sharedActivity(), account);

                CSActivity.sharedActivity().view().queueEvent(() -> CSGooglePlay.nativeLoginResult(LOGIN_RESULT_SUCCESS));
            }
            else {
                Log.e(TAG, "firebase sign in error", task.getException());

                onDisconnected();

                CSActivity.sharedActivity().view().queueEvent(() -> CSGooglePlay.nativeLoginResult(LOGIN_RESULT_ERROR));
            }
        });
    }

    private void onDisconnected() {
		_userId = null;
		_email = null;
		_imageUrl = null;
        _achievementsClient = null;
        _leaderboardsClient = null;
    }

    public void reportAchievementSteps(String id, int value) {
        if (_achievementsClient == null || value <= 0) {
            return;
        }
        _achievementsClient.setSteps(id, value);
    }

    public void reportAchievementIncrement(String id, int value) {
        if (_achievementsClient == null || value <= 0) {
            return;
        }
        _achievementsClient.increment(id, value);
    }

    public void showAchievements() {
        if (_achievementsClient == null) {
            return;
        }
        _achievementsClient.getAchievementsIntent().addOnSuccessListener(intent -> CSActivity.sharedActivity().startActivityForResult(intent, RC_SHOW_ACHIEVEMENTS));
    }

    public void reportLeaderboards(int score) {
        if (_leaderboardsClient == null || score <= 0) {
            return;
        }
        _leaderboardsClient.submitScore(_leaderBoardId, score);
    }

    public void showLeaderboards() {
        if (_leaderboardsClient == null) {
            return;
        }
        _leaderboardsClient.getLeaderboardIntent(_leaderBoardId).addOnSuccessListener(intent -> CSActivity.sharedActivity().startActivityForResult(intent, RC_SHOW_LEADERBOARD));
    }

    public void showReview() {
        ReviewManager manager = ReviewManagerFactory.create(CSActivity.sharedActivity());
        Task<ReviewInfo> request = manager.requestReviewFlow();
        request.addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                Log.i(TAG, "review init success");
                ReviewInfo reviewInfo = task.getResult();
                Task<Void> flow = manager.launchReviewFlow(CSActivity.sharedActivity(), reviewInfo);
                flow.addOnCompleteListener(flowtask -> {
                    Log.i(TAG, "review flow success");
                    // The flow has finished. The API does not indicate whether the user
                    // reviewed or not, or even whether the review dialog was shown. Thus, no
                    // matter the result, we continue our app flow.
                });
            }
            else {
                int reviewErrorCode = ((ReviewException)task.getException()).getErrorCode();
                Log.e(TAG, "review init failed, errorCode : " + reviewErrorCode);
            }
        });
    }
}

public class CSGooglePlay {
    private static CSGooglePlayImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSGooglePlayImpl());
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
	
	public static String email() {
        return _impl.email();
    }
	
	public static String imageUrl() {
        return _impl.imageUrl();
    }

    public static void reportAchievementSteps(final String id, final int value) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.reportAchievementSteps(id, value));
    }

    public static void reportAchievementIncrement(final String id, final int value) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.reportAchievementIncrement(id, value));
    }

    public static void showAchievements() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.showAchievements());
    }

    public static void reportLeaderboards(final int score) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.reportLeaderboards(score));
    }

    public static void showLeaderboards() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.showLeaderboards());
    }

    public static void showReview() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.showReview());
    }

    public static native void nativeLoginResult(int result);
}