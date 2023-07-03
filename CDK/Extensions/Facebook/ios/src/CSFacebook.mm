//
//  CSFacebook.mm
//  CDK
//
//  Created by ChangSun on 2017. 11. 22..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSFacebook.h"

#import "CSAppDelegate.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
//#import <FBSDKShareKit/FBSDKShareKit.h>
#import <MobileCoreServices/MobileCoreServices.h>

@interface CSFacebookImpl : NSObject<UIApplicationDelegate> {
    CSFacebookDelegate* _delegate;
	NSString* _userId;
}

@property (readwrite, assign) CSFacebookDelegate* delegate;
@property (readonly) NSString* userId;
@property (readonly) NSString* pictureUrl;
@property (readonly, getter = isConnected) BOOL connected;

-(void)dealloc;

-(void)login;
-(void)logout;

@end

@implementation CSFacebookImpl

@synthesize delegate = _delegate;
@synthesize userId = _userId;

-(void)dealloc {
	[_userId release];
    
    [super dealloc];
}

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    [FBSDKProfile enableUpdatesOnAccessTokenChange:YES]; //추가
    [[FBSDKApplicationDelegate sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
    //NSLog( @"### running FB sdk version: %@", [FBSDKSettings sdkVersion] );
    return YES;
}

- (BOOL)application:(UIApplication*)application
            openURL:(NSURL*)url
            options:(nonnull NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options
{
    BOOL handled = [[FBSDKApplicationDelegate sharedInstance] application:application
                                                                  openURL:url
                                                        sourceApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey]
                                                               annotation:options[UIApplicationOpenURLOptionsAnnotationKey]
                    ];
    return handled;
}

-(void)applicationDidBecomeActive:(UIApplication*)application {
    // Facebook 앱 광고 - 설치 추적 활성화
    [FBSDKAppEvents activateApp];
    //[FBSDKSettings setAdvertiserTrackingEnabled:YES];
    //NSString *accessToken = [FBSDKAccessToken currentAccessToken];
    //추가
    [FBSDKAppLinkUtility fetchDeferredAppLink:^(NSURL *url, NSError *error) {
        if (error) {
            NSLog(@"CSFacebook fetchDeferredAppLink error: %@", error);
        }
        if (url) {
            [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:nil];
        }
    }];
}

-(BOOL)isConnected {
    return _userId != nil;
}

-(void)login {
	if (_userId) {
		if (_delegate) {
			_delegate->onFacebookLogin(CSFacebookLoginResultSuccess);
		}
	}
	else {
		FBSDKLoginManager* manager = [[FBSDKLoginManager alloc] init];
		
		[manager logInWithPermissions:@[@"public_profile"] fromViewController:[[CSAppDelegate sharedDelegate] viewController] handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
			if (error) {
				NSLog(@"CSFacebook login error:%@", error);
				if (_delegate) {
					_delegate->onFacebookLogin(CSFacebookLoginResultError);
				}
			}
			else if (result.isCancelled) {
				NSLog(@"CSFacebook login cancelled");
				if (_delegate) {
					_delegate->onFacebookLogin(CSFacebookLoginResultCancelled);
				}
			}
			else {
				if (result.token) {
					_userId = [result.token.userID retain];
					
					if (_delegate) {
						_delegate->onFacebookLogin(CSFacebookLoginResultSuccess);
					}
				}
				else {
					NSLog(@"CSFacebook login token is empty");
					
					if (_delegate) {
						_delegate->onFacebookLogin(CSFacebookLoginResultError);
					}
				}
			}
		}];
		[manager release];
	}
}

- (void)logout {
    FBSDKLoginManager* manager = [[FBSDKLoginManager alloc] init];
    [manager logOut];
    [manager release];
	[_userId release];
	_userId = nil;
}

-(NSString*)pictureUrl {
    /*
    [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile *profile, NSError *error) {
        
    }];

    NSURL* pictureUrl =  [[FBSDKProfile currentProfile] imageURLForPictureMode:FBSDKProfilePictureModeSquare size: CGSizeMake(140, 140)];
    
    return [pictureUrl absoluteString];
     */
    return nil;
}

@end

static CSFacebookImpl* __sharedImpl = nil;

void CSFacebook::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSFacebookImpl alloc] init];
        [[CSAppDelegate sharedDelegate] addSubDelegate:__sharedImpl];
    }
}

void CSFacebook::finalize() {
    if (__sharedImpl) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate:__sharedImpl];
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSFacebook::setDelegate(CSFacebookDelegate* delegate) {
    [__sharedImpl setDelegate:delegate];
}

void CSFacebook::login() {
    [__sharedImpl login];
}

void CSFacebook::logout() {
    [__sharedImpl logout];
}

bool CSFacebook::isConnected() {
    return __sharedImpl.isConnected;
}

const char* CSFacebook::userId() {
	return __sharedImpl.userId.UTF8String;
}

const char* CSFacebook::pictureUrl(){
    return __sharedImpl.pictureUrl.UTF8String;
}

#endif

