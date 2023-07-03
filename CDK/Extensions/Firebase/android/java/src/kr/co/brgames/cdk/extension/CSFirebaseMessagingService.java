package kr.co.brgames.cdk.extension;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import kr.co.brgames.cdk.CSActivity;
import kr.co.brgames.cdk.CSNotification;

public class CSFirebaseMessagingService extends FirebaseMessagingService {
    private static final int NOTIFICATION_ID_START = 16 << 16;

    private int _count;

    @Override
    public void onNewToken(final String token) {
        if (CSActivity.sharedActivity() != null) {
            CSActivity.sharedActivity().view().queueEvent(() -> CSFirebase.nativeTokenRefresh(token));
        }
    }

    @Override
    public void onMessageReceived(final RemoteMessage remoteMessage) {
        if (remoteMessage.getNotification() != null) {
            CSActivity.sharedActivity().queueEvent(() -> {
                String title = remoteMessage.getNotification().getTitle();
                String message = remoteMessage.getNotification().getBody();
                int smallIconId = CSActivity.sharedActivity().resourceAsId("@drawable/ic_c2s_notification_small_icon");

                CSNotification.sendNotification(NOTIFICATION_ID_START + _count, title, message, smallIconId, 0);

                _count++;
            });
        }
    }
}
