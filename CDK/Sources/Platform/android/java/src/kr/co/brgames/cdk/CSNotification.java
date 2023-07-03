package kr.co.brgames.cdk;

import android.app.IntentService;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.work.ExistingWorkPolicy;
import androidx.work.OneTimeWorkRequest;
import androidx.work.Worker;
import androidx.work.Data;
import androidx.work.WorkManager;
import androidx.work.WorkerParameters;

import java.util.concurrent.TimeUnit;

import static androidx.work.ListenableWorker.Result.success;

public class CSNotification extends IntentService {
    public static final String ACTION_NOTIFICATION = "cdk.notification.NOTIFICATION";

    public static final String EXTRA_ID = "cdk.notification.ID";
    public static final String EXTRA_TITLE = "cdk.notification.TITLE";
    public static final String EXTRA_MESSAGE = "cdk.notification.MESSAGE";
    public static final String EXTRA_SMALLICONRESID = "cdk.notification.SMALLICONRESID";
    public static final String EXTRA_LARGEICONRESID = "cdk.notification.LARGEICONRESID";

    public static final String CHANNEL_ID = "channel_id";

    private static final String TAG = "CSNotification";
    
    public CSNotification() {
        super("CSNotification");
    }

    public static void createChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = (NotificationManager) CSActivity.sharedActivity().getSystemService(Context.NOTIFICATION_SERVICE);

            if (notificationManager.getNotificationChannel(CHANNEL_ID) == null) {
                NotificationChannel notificationChannel = new NotificationChannel(CHANNEL_ID, "Announcement", NotificationManager.IMPORTANCE_DEFAULT);
                notificationChannel.enableLights(true);
                notificationChannel.setLightColor(Color.GREEN);
                notificationChannel.enableVibration(true);
                notificationChannel.setVibrationPattern(new long[]{100, 200, 100, 200});
                notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
                notificationManager.createNotificationChannel(notificationChannel);
            }
        }
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        if (intent != null && intent.getAction().equals(ACTION_NOTIFICATION)) {
            int id = intent.getIntExtra(EXTRA_ID, 0);
            String title = intent.getStringExtra(EXTRA_TITLE);
            String message = intent.getStringExtra(EXTRA_MESSAGE);
            int smallIconResId = intent.getIntExtra(EXTRA_SMALLICONRESID, 0);
            int largeIconResId = intent.getIntExtra(EXTRA_LARGEICONRESID, 0);
            sendNotification(getApplicationContext(), id, title, message, smallIconResId, largeIconResId);
        }
    }

    public static class NotificationWorker extends Worker {
        public NotificationWorker(
                @NonNull Context appContext,
                @NonNull WorkerParameters workerParams) {
            super(appContext, workerParams);
        }

        @Override
        @NonNull
        public Result doWork() {
            int id = (int) getInputData().getInt(EXTRA_ID, 0);
            String title = getInputData().getString(EXTRA_TITLE);
            String message = getInputData().getString(EXTRA_MESSAGE);
            int smallIconResId = (int) getInputData().getInt(EXTRA_SMALLICONRESID, 0);
            int largeIconResId = (int) getInputData().getInt(EXTRA_LARGEICONRESID, 0);

            sendNotification(getApplicationContext(), id, title, message, smallIconResId, largeIconResId);

            return success();
        }
    }

    public static void addNotification(int id, String title, String message, int smallIconResId, int largeIconResId, int delay) {
        if (delay > 0) {
            Data data = new Data.Builder()
                    .putString(EXTRA_TITLE, title)
                    .putString(EXTRA_MESSAGE, message)
                    .putInt(EXTRA_ID, id)
                    .putInt(EXTRA_SMALLICONRESID, smallIconResId)
                    .putInt(EXTRA_LARGEICONRESID, largeIconResId)
                    .build();

            OneTimeWorkRequest notificationWork = new OneTimeWorkRequest.Builder(NotificationWorker.class)
                    .setInitialDelay(delay, TimeUnit.SECONDS)
                    .setInputData(data).build();

            WorkManager instance = WorkManager.getInstance(CSActivity.sharedActivity());
            instance.enqueueUniqueWork("notification_" + id, ExistingWorkPolicy.REPLACE, notificationWork);
        }
        else {
            sendNotification(id, title, message, smallIconResId, largeIconResId);
        }
    }

    public static void removeNotification(int id) {
        WorkManager instance = WorkManager.getInstance(CSActivity.sharedActivity());
        instance.cancelUniqueWork("notification_" + id);
    }

    public static void clearNotifications() {
        NotificationManager notificationManager = (NotificationManager)CSActivity.sharedActivity().getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancelAll();
    }

    public static void sendNotification(int id, String title, String message, int smallIconResId, int largeIconResId) {
        sendNotificationImpl(CSActivity.sharedActivity().getApplicationContext(), id, title, message, smallIconResId, largeIconResId);
    }

    public static void sendNotification(Context appContext, int id, String title, String message, int smallIconResId, int largeIconResId) {
        if (CSActivity.sharedActivity().isVisible()) {
            return;
        }

        sendNotificationImpl(appContext, id, title, message, smallIconResId, largeIconResId);
    }

    private static void sendNotificationImpl(Context appContext, int id, String title, String message, int smallIconResId, int largeIconResId) {
        Intent intent = appContext.getPackageManager().getLaunchIntentForPackage(appContext.getPackageName());
        int flags;
        if (Build.VERSION.SDK_INT >= 23) flags = PendingIntent.FLAG_MUTABLE | PendingIntent.FLAG_UPDATE_CURRENT;
        else flags = PendingIntent.FLAG_UPDATE_CURRENT;
        PendingIntent pendingIntent = PendingIntent.getActivity(appContext, 0, intent, flags);

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(appContext, CHANNEL_ID);

        NotificationCompat.BigTextStyle bigTextStyle = new NotificationCompat.BigTextStyle();
        bigTextStyle.setBigContentTitle(title);
        bigTextStyle.bigText(message);

        notificationBuilder.setContentTitle(title)
                .setContentText(message)
                .setStyle(bigTextStyle)
                .setContentIntent(pendingIntent)
                .setCategory(NotificationCompat.CATEGORY_RECOMMENDATION)
                .setAutoCancel(true);

        if (largeIconResId > 0) {
            notificationBuilder.setLargeIcon(BitmapFactory.decodeResource(appContext.getResources(), largeIconResId));
        }
        if (smallIconResId > 0) {
            notificationBuilder.setSmallIcon(smallIconResId);
        }
        if (Build.VERSION.SDK_INT < 26) {
            notificationBuilder.setPriority(Notification.PRIORITY_MAX);
        }
        NotificationManager notificationManager = (NotificationManager)appContext.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(id, notificationBuilder.build());
    }
}
