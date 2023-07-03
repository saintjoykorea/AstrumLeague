//
//  CSLineLoginImpl.m
//  TalesCraft2
//
//  Created by kim su hyun on 11/02/2019.
//  Copyright © 2019 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSLineLoginImpl.h"

#import <LineSDK/LineSDK.h>

#define CSLineLoginResultSuccess	0
#define CSLineLoginResultCancelled	1
#define CSLineLoginResultError		2

@interface CSLineLoginImpl : NSObject<LineSDKLoginDelegate, UIApplicationDelegate> {
    LineSDKAPI* _lineApi;
    LineSDKProfile* _profile;
}

@property (readonly, getter = isConnected) BOOL connected;
@property (readonly) NSString* userId;
@property (readonly) NSString* pictureUrl;

-(id)init;
-(void)dealloc;

-(void)login;
-(void)logout;
    
@end

@implementation CSLineLoginImpl

-(id)init {
    if (self = [super init]) {
        _lineApi =  [[LineSDKAPI alloc] initWithConfiguration:[LineSDKConfiguration defaultConfig]];
        [LineSDKLogin sharedInstance].delegate = self;
    }
    return self;
}

-(void)dealloc {
    [LineSDKLogin sharedInstance].delegate = nil;
    [_profile release];
    [_lineApi release];
    [super dealloc];
}

- (void)login {
    [[LineSDKLogin sharedInstance] startLoginWithPermissions:@[@"profile"]];//, @"friends", @"groups"
}

- (void)didLogin:(LineSDKLogin*)login
      credential:(LineSDKCredential*)credential
         profile:(LineSDKProfile*)profile
           error:(NSError*)error
{
    if (!error) {
        [_profile release];
        _profile = [profile retain];
        CSLineLogin_loginResultCB(CSLineLoginResultSuccess);
    }
    else {
        NSLog(@"line login failed:%@", error);
        CSLineLogin_loginResultCB(error.code == LineSDKInternalErrorCodeAuthenticationCanceled ? CSLineLoginResultCancelled : CSLineLoginResultError);
    }
}

- (void)logout {
	[_profile release];
	_profile = nil;

    [_lineApi logoutWithCompletion:^(BOOL success, NSError* error) {

    }];
}

-(BOOL)isConnected {
    return _profile != nil;
}

-(NSString*)userId {
	return _profile.userID.UTF8String;
}

-(NSString*)pictureUrl {
    return _profile.pictureURL.absoluteString;
}

-(BOOL)application:(UIApplication*)app openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options {
    return [[LineSDKLogin sharedInstance] handleOpenURL:url];
}

-(BOOL)application:(UIApplication*)application continueUserActivity:(NSUserActivity*)userActivity restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> * _Nullable))restorationHandler {
    return [[LineSDKLogin sharedInstance] handleOpenURL:userActivity.webpageURL];
}

@end

static CSLineLoginImpl* __sharedImpl = nil;

NSObject<UIApplicationDelegate>* CSLineLogin_applicationDelegate() {
    return __sharedImpl;
}

bool CSLineLogin_initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSLineLoginImpl alloc] init];
        return true;
    }
    return false;
}

void CSLineLogin_finalize() {
    if (__sharedImpl) {
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSLineLogin_login() {
    [__sharedImpl login];
}

void CSLineLogin_logout() {
    [__sharedImpl logout];
}

bool CSLineLogin_isConnected() {
    return __sharedImpl.isConnected;
}

const char* CSLineLogin_userId() {
    return __sharedImpl.userId.UTF8String;
}

const char* CSLineLogin_pictureUrl() {
    return __sharedImpl.pictureUrl.UTF8String;
}

#endif

