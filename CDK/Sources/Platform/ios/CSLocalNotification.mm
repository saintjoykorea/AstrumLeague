//
//  CSLocalNotification.mm
//  CDK
//
//  Created by 김수현 on 2017. 12. 13..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSLocalNotification.h"

#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

void CSLocalNotification::addNotification(int notificationId, const char* title, const char* message, int delay) {
    NSString* identfier = [NSString stringWithFormat:@"%d", notificationId];
    
    [[UNUserNotificationCenter currentNotificationCenter] removePendingNotificationRequestsWithIdentifiers:@[identfier]];
	
	UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
	if (title) [NSString localizedUserNotificationStringForKey:[NSString stringWithUTF8String:title] arguments:nil];
	content.body = [NSString localizedUserNotificationStringForKey:[NSString stringWithUTF8String:message] arguments:nil];
	content.sound = [UNNotificationSound defaultSound];
	content.badge = @([[UIApplication sharedApplication] applicationIconBadgeNumber] + 0);
	UNTimeIntervalNotificationTrigger *trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:delay repeats:NO];
	UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:identfier content:content trigger:trigger];

	UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
	[center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
		if (!error) {
			NSLog(@"addNotificationRequest succeeded!");
		}
	}];
	[content release];	
}

void CSLocalNotification::removeNotification(int notificationId) {
    NSString* identfier = [NSString stringWithFormat:@"%d", notificationId];
    
	[[UNUserNotificationCenter currentNotificationCenter] removePendingNotificationRequestsWithIdentifiers:@[identfier]];
}

void CSLocalNotification::clearNotifications() {
    [[UNUserNotificationCenter currentNotificationCenter] removeAllDeliveredNotifications];
}

#endif
