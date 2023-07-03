//
//  CSLocalNotification.h
//  CDK
//
//  Created by 김수현 on 2017. 12. 13..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSLocalNotification_h
#define __CDK__CSLocalNotification_h

class CSLocalNotification {
public:
    static void addNotification(int notificationId, const char* title, const char* message, int delay);
    static void removeNotification(int notificationId);
    static void clearNotifications();
};

#endif /* __CDK__CSLocalNotification_h */
