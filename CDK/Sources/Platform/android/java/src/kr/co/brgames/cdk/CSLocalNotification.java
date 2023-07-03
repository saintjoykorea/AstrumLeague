package kr.co.brgames.cdk;

public class CSLocalNotification {
    public static void addNotification(final int id, final String title, final String message, final int delay) {
        CSActivity.sharedActivity().queueEvent(() -> {
            String titleOrName = title != null ? title : CSActivity.sharedActivity().resourceString("app_name");
            int smallIconId = CSActivity.sharedActivity().resourceAsId("@drawable/ic_c2s_notification_small_icon");

            CSNotification.addNotification(id, titleOrName, message, smallIconId, 0, delay);
        });
    }

    public static void removeNotification(final int id) {
        CSActivity.sharedActivity().queueEvent(() -> CSNotification.removeNotification(id));
    }

    public static void clearNotifications() {
        CSActivity.sharedActivity().queueEvent(CSNotification::clearNotifications);
    }
}