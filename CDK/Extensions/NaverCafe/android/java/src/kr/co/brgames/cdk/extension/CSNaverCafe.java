package kr.co.brgames.cdk.extension;

import android.util.Log;

import com.naver.glink.android.sdk.Glink;

import kr.co.brgames.cdk.CSActivity;

class CSNaverCafeImpl {
    private static final String TAG = "CSNaverCafe";

    public CSNaverCafeImpl() {
		boolean naver = false;
		boolean neo = false;
		
        String naverClientId = CSActivity.sharedActivity().resourceString("naver_client_id");
        String naverClientSecret = CSActivity.sharedActivity().resourceString("naver_client_secret");
        String naverCafeId = CSActivity.sharedActivity().resourceString("naver_cafe_id");
        String neoConsumerKey = CSActivity.sharedActivity().resourceString("naver_neo_consumer_key");
        String neoCommunityId = CSActivity.sharedActivity().resourceString("naver_neo_community_id");

		//TODO:기존 코드는 view thread에서 동작하게 되어 있었다. 그래야 제대로 동작하는지 체크 필요

		if (naverClientId != null && naverClientSecret != null && naverCafeId != null) {
			Glink.init(CSActivity.sharedActivity(), naverClientId, naverClientSecret, Integer.parseInt(naverCafeId));
			naver = true;
		}
		if (neoConsumerKey != null && neoCommunityId != null) {
			Glink.initGlobal(CSActivity.sharedActivity(), neoConsumerKey, Integer.parseInt(neoCommunityId));
			neo = true;
		}
		
		assert naver || neo;
		
		// 동영상 녹화 기능을 사용합니다.
		Glink.setUseVideoRecord(CSActivity.sharedActivity(), true);

		// SDK 시작 리스너 설정.
		Glink.setOnSdkStartedListener(new Glink.OnSdkStartedListener() {
			@Override public void onSdkStarted() {
				Log.d(TAG, "onSdkStarted");
			}
		});

		// SDK 종료 리스너 설정.
		Glink.setOnSdkStoppedListener(new Glink.OnSdkStoppedListener() {
			@Override public void onSdkStopped() {
				Log.d(TAG, "onSdkStopped");
			}
		});

		// 앱스킴 터치 리스너 설정.
		Glink.setOnClickAppSchemeBannerListener(new Glink.OnClickAppSchemeBannerListener() {
			@Override public void onClickAppSchemeBanner(String appScheme) {
				// 카페 관리에서 설정한 appScheme 문자열을 SDK에서 넘겨줍니다.
				// 각 appScheme 처리를 이곳에서 하시면 됩니다.
				Log.d(TAG, appScheme);

				CSActivity.sharedActivity().openURL(appScheme);
			}
		});

		// 카페 가입 리스너를 설정.
		Glink.setOnJoinedListener(new Glink.OnJoinedListener() {
			@Override public void onJoined() {
				Log.d(TAG, "onJoined");
			}
		});

		/** 게시글 등록 리스너를 설정.
		 * @param menuId 게시글이 등록된 menuId
		 * @param imageCount 첨부한 image 갯수
		 * @param videoCount 첨부한 video 갯수
		 **/
		Glink.setOnPostedArticleListener(new Glink.OnPostedArticleListener() {
			@Override public void onPostedArticle(int menuId, int imageCount, int videoCount) {
				Log.d(TAG, String.format("onPostedArticle:%d", menuId));
			}
		});

		// 댓글 등록 리스너를 설정.
		Glink.setOnPostedCommentListener(new Glink.OnPostedCommentListener() {
			@Override public void onPostedComment(int articleId) {
				Log.d(TAG, String.format("onPostedComment:%d", articleId));
			}
		});

		// 투표 완료 리스너를 설정.
		Glink.setOnVotedListener(new Glink.OnVotedListener() {
			@Override public void onVoted(int articleId) {
				Log.d(TAG, String.format("onVoted:%d", articleId));
			}
		});

		//위젯 스크린샷 버튼 클릭 리스너 설정.
		Glink.setOnWidgetScreenshotClickListener(new Glink.OnWidgetScreenshotClickListener() {
			@Override public void onScreenshotClick() {
				String filePath = CSActivity.sharedActivity().getFilesDir() + "/screenshot_" + System.currentTimeMillis() + ".png";
				boolean result = CSActivity.sharedActivity().view().screenshot(filePath);

				if (result) {
					Glink.startImageWrite(CSActivity.sharedActivity(), "file:" + filePath);
				}
			}
		});

		//동영상 녹화 완료 리스너 설정.
		Glink.setOnRecordFinishListener(new Glink.OnRecordFinishListener() {
			@Override public void onRecordFinished(String uri) {
				Log.d(TAG, String.format("onRecordFinished:%s", uri));
			}
		});
    }

    public void startHome() {
        Glink.startHome(CSActivity.sharedActivity());
    }

    public void startArticle(int articleId) {
        Glink.startArticle(CSActivity.sharedActivity(), articleId);
    }
}

public class CSNaverCafe {
    private static CSNaverCafeImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSNaverCafeImpl());
    }

    public static void startHome() {
        CSActivity.sharedActivity().queueEvent(() -> _impl.startHome());
    }

    public static void startArticle(final int articleId) {
        CSActivity.sharedActivity().queueEvent(() -> _impl.startArticle(articleId));
    }
}