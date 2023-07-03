//
//  CSFirebase.mm
//  CDK
//
//  Created by WooyolJung on 2017. 11. 10.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL
#import "CSAppDelegate.h"
#import "Application.h"
#import "CSFirebase.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-property-no-attribute"
#import "Firebase.h"
#import "FirebaseFirestore/FirebaseFirestore.h"
#pragma clang diagnostic pop

#import <FirebaseMessaging/FirebaseMessaging.h>
#import <FirebaseAuth/FirebaseAuth.h>
#import <UserNotifications/UserNotifications.h>
#import <FirebaseCrashlytics/FirebaseCrashlytics.h>
#import <sys/utsname.h>

@interface CSFirebaseImpl : NSObject<UIApplicationDelegate, FIRMessagingDelegate,UNUserNotificationCenterDelegate> {
	CSFirebaseDelegate* _delegate;
    NSString* _token;
    BOOL _connected;
}

@property (readwrite, assign) CSFirebaseDelegate* delegate;
@property (readonly) NSString* token;

-(void)dealloc;

-(void)refreshToken;
-(NSString*) deviceName;
-(void)addTopic:(NSString*)topic;
-(void)removeTopic:(NSString*)topic;

@end

@implementation CSFirebaseImpl

@synthesize delegate = _delegate;
@synthesize token = _token;

-(void)dealloc {
	[_token release];
	
	[super dealloc];
}

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    
    [FIRApp configure];
    [FIRMessaging messaging].delegate = self;
    
    //[FIRApp.defaultApp setDataCollectionDefaultEnabled:YES];
#if Vendor != VendorSJVP
    [FIRAnalytics setAnalyticsCollectionEnabled:NO];
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:NO];
#endif
    /* You must set setCrashlyticsCollectionEnabled to false in order to use
    checkForUnsentReportsWithCompletion. */
    /*
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:false];
    [[FIRCrashlytics crashlytics] checkForUnsentReportsWithCompletion:^(BOOL hasUnsentReports) {
      if (hasUnsentReports) {
        [FIRCrashlytics.crashlytics log:@"Crashlytics log"];
        [FIRCrashlytics.crashlytics setCustomValue:@"CrashIos" forKey:@"str_key"];
        [[FIRCrashlytics crashlytics] sendUnsentReports];
      } else {
        [[FIRCrashlytics crashlytics] deleteUnsentReports];
      }
    }];
     */

    
    //[FIRCrashlytics.crashlytics log:@"Crashlytics log"];
    //[FIRCrashlytics.crashlytics setCustomValue:@(100) forKey:@"int_key"];
    //[FIRCrashlytics.crashlytics setCustomValue:@"Crash" forKey:@"str_key"];
    //[FIRCrashlytics.crashlytics sendUnsentReports];
    
    /*
    NSDictionary *userInfo = @{
      NSLocalizedDescriptionKey: NSLocalizedString(@"The request failed.", nil),
      NSLocalizedFailureReasonErrorKey: NSLocalizedString(@"The response returned a 404.", nil),
      NSLocalizedRecoverySuggestionErrorKey: NSLocalizedString(@"Does this page exist?", nil),
      @"ProductID": @"123456",
      @"View": @"MainView",
    };

    NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:-1001 userInfo:userInfo];
    [FIRCrashlytics.crashlytics recordError:error];
     */
  
    /*
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tokenRefreshNotification:)
                                                 name:kFIRInstanceIDTokenRefreshNotification object:nil];

    
	UNAuthorizationOptions authOptions = UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge;
	
	[[UNUserNotificationCenter currentNotificationCenter]
		requestAuthorizationWithOptions:authOptions
		completionHandler:^(BOOL granted, NSError * _Nullable error) {}];

    
	//[[FIRMessaging messaging] setRemoteMessageDelegate:self];
	
	[[UIApplication sharedApplication] registerForRemoteNotifications];
     */
    
    if ([UNUserNotificationCenter class] != nil) {
        // iOS 10 or later
        // For iOS 10 display notification (sent via APNS)
        [UNUserNotificationCenter currentNotificationCenter].delegate = self;
        UNAuthorizationOptions authOptions = UNAuthorizationOptionAlert |
            UNAuthorizationOptionSound | UNAuthorizationOptionBadge;
        [[UNUserNotificationCenter currentNotificationCenter]
            requestAuthorizationWithOptions:authOptions
            completionHandler:^(BOOL granted, NSError * _Nullable error) {
              // ...
            }];
      } else {
        // iOS 10 notifications aren't available; fall back to iOS 8-9 notifications.
        UIUserNotificationType allNotificationTypes =
        (UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge);
        UIUserNotificationSettings *settings =
        [UIUserNotificationSettings settingsForTypes:allNotificationTypes categories:nil];
        [application registerUserNotificationSettings:settings];
      }

      [application registerForRemoteNotifications];
    return YES;
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
  // If you are receiving a notification message while your app is in the background,
  // this callback will not be fired till the user taps on the notification launching the application.
  // TODO: Handle data of notification

  // With swizzling disabled you must let Messaging know about the message, for Analytics
  [[FIRMessaging messaging] appDidReceiveMessage:userInfo];
  NSLog(@"didReceiveuserInfo %@", userInfo);
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
    fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
  [[FIRMessaging messaging] appDidReceiveMessage:userInfo];

  NSLog(@"didReceiveRemoteNotification userInfo%@", userInfo);

  completionHandler(UIBackgroundFetchResultNewData);
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
didReceiveNotificationResponse:(UNNotificationResponse *)response
         withCompletionHandler:(void(^)(void))completionHandler {
  NSDictionary *userInfo = response.notification.request.content.userInfo;
  
  NSLog(@"didReceiveNotificationResponse %@", userInfo);

  completionHandler();
}

- (void)messaging:(FIRMessaging *)messaging didReceiveRegistrationToken:(NSString *)fcmToken {
    NSLog(@"FCM registration token: %@", fcmToken);
    // Notify about received token.
    NSDictionary *dataDict = [NSDictionary dictionaryWithObject:fcmToken forKey:@"token"];
    [[NSNotificationCenter defaultCenter] postNotificationName:
     @"FCMToken" object:nil userInfo:dataDict];
}

-(void)applicationDidBecomeActive:(UIApplication*)application {
    [self refreshToken];
}

-(void)refreshToken {
    NSString* token = [[FIRMessaging messaging] FCMToken];
    if (token && ![token isEqualToString:_token]) {
        [_token release];
        _token = [token retain];
        
        if (_delegate) {
            _delegate->onFirebaseTokenRefresh(_token.UTF8String);
        }
    }
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
  NSLog(@"Unable to register for remote notifications: %@", error);
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
  NSLog(@"APNs device token retrieved: %@", deviceToken);

}


-(void)Crashlytics{
    [FIRApp.defaultApp setDataCollectionDefaultEnabled:YES];
        
        [FIRAnalytics setAnalyticsCollectionEnabled:YES];
    /*
        [FIRAnalytics logEventWithName:kFIREventSelectContent
        parameters:@{
                     kFIRParameterItemID:@"some-id",
                     kFIRParameterItemName:@"some-name",
                     kFIRParameterContentType:@"image"
                     }];
        
        [FIRCrashlytics.crashlytics log:@"Crashlytics log"];
        [FIRCrashlytics.crashlytics setCustomValue:@(100) forKey:@"int_key"];
        [FIRCrashlytics.crashlytics setCustomValue:@"Crash" forKey:@"str_key"];
        [FIRCrashlytics.crashlytics sendUnsentReports];
        
        NSDictionary *userInfo = @{
          NSLocalizedDescriptionKey: NSLocalizedString(@"The request failed.", nil),
          NSLocalizedFailureReasonErrorKey: NSLocalizedString(@"The response returned a 404.", nil),
          NSLocalizedRecoverySuggestionErrorKey: NSLocalizedString(@"Does this page exist?", nil),
          @"ProductID": @"123456",
          @"View": @"MainView",
        };

        NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:-1001 userInfo:userInfo];
        [FIRCrashlytics.crashlytics recordError:error];
     */
    
}



/*
-(void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken {
    [FIRMessaging messaging].APNSToken = deviceToken;
    //[[FIRInstanceID instanceID] setAPNSToken:deviceToken type:FIRInstanceIDAPNSTokenTypeProd];
    NSLog(@"CSFirebase token retrieved:%@", deviceToken);
}

- (void)applicationDidEnterBackground:(UIApplication*)application {
    //[[FIRMessaging messaging] disconnect];
    _connected = NO;
}

-(void)applicationDidBecomeActive:(UIApplication*)application {
    [self refreshToken];
}

-(void)applicationWillTerminate:(UIApplication*)application {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [FIRMessaging messaging].delegate = nil;

    //[[FIRMessaging messaging] setRemoteMessageDelegate:nil];
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions))completionHandler {

    //NSDictionary *userInfo = notification.request.content.userInfo;
    
    completionHandler(UNNotificationPresentationOptionNone);
    //completionHandler(UNNotificationPresentationOptionAlert); //푸시 배너를 띄운다.
     
}

//new version
- (void)messaging:(FIRMessaging *)messaging didReceiveRegistrationToken:(NSString *)fcmToken {
    NSLog(@"FCM registration token: %@", fcmToken);
    // Notify about received token.    NSDictionary *dataDict = [NSDictionary dictionaryWithObject:fcmToken forKey:@"token"];    [[NSNotificationCenter defaultCenter] postNotificationName:     @"FCMToken" object:nil userInfo:dataDict];    // TODO: If necessary send token to application server.    // Note: This callback is fired at each app startup and whenever a new token is generated.
}

-(void)tokenRefreshNotification:(NSNotification *)notification {
    [self refreshToken];
}

-(void)doRefreshToken {
    
	//NSString* token = [[FIRInstanceID instanceID] token];
				
	//if (token && ![token isEqualToString:_token]) {
	//	[_token release];
	//	_token = [token retain];
		
	//	if (_delegate) _delegate->onFirebaseTokenRefresh(_token.UTF8String);
	//}
}

-(void)refreshToken {
   // if (!_connected) {
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdeprecated"
 //       [[FIRMessaging messaging] connectWithCompletion:^(NSError * _Nullable error) {
//#pragma clang diagnostic pop
  //          if (error) {
  //              NSLog(@"CSFirebase unable to connect to FCM. %@", error);
   //         }
	//		else {
   //             _connected = YES;
   //             [self doRefreshToken];
     //       }
    //    }];
 //  }
 //   else {
//        [self doRefreshToken];
 //   }
}
 */

-(NSString*) deviceName {
    struct utsname systemInfo;
    uname(&systemInfo);
    return [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
}

-(void)addTopic:(NSString*)topic {
    [[FIRMessaging messaging] subscribeToTopic:[NSString stringWithFormat: @"/topics/%@", topic]];
}

-(void)removeTopic:(NSString*)topic {
    [[FIRMessaging messaging] unsubscribeFromTopic:[NSString stringWithFormat: @"/topics/%@", topic]];
}

@end

static CSFirebaseImpl* __sharedImpl = nil;

void CSFirebase::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSFirebaseImpl alloc] init];
        [[CSAppDelegate sharedDelegate] addSubDelegate:__sharedImpl];
    }
}

void CSFirebase::finalize() {
    if (__sharedImpl) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate:__sharedImpl];
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

const char* CSFirebase::token() {
	return __sharedImpl.token.UTF8String;
}

void CSFirebase::setDelegate(CSFirebaseDelegate* delegate) {
	[__sharedImpl setDelegate:delegate];
}

void CSFirebase::addTopic(const char* topic) {
    [__sharedImpl addTopic:[NSString stringWithUTF8String:topic]];
}

void CSFirebase::removeTopic(const char* topic) {
    [__sharedImpl removeTopic:[NSString stringWithUTF8String:topic]];
}

void CSFirebase::log(const char* name, int paramCount, ...) {
    NSMutableDictionary* params = [[NSMutableDictionary alloc] initWithCapacity:paramCount];
    
    va_list ap;
    va_start(ap, paramCount);
    for (int i = 0; i < paramCount; i++) {
        const char* key = va_arg(ap, const char*);
        const char* value = va_arg(ap, const char*);
        [params setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
    }
    va_end(ap);
    
    [FIRAnalytics logEventWithName:[NSString stringWithUTF8String:name] parameters:params];
    
    [params release];
}

void CSFirebase::setUserId(const char* userId) {
    [[FIRCrashlytics crashlytics] setUserID:[NSString stringWithUTF8String:userId]];
}

#endif
