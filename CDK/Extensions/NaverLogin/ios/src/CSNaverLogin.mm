//
//  CSNaverLogin.mm
//  CDK
//
//  Created by WooyolJung on 2017. 11. 21.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSNaverLogin.h"

#import "CSAppDelegate.h"

#import "NaverThirdPartyConstantsForApp.h"
#import "NLoginThirdPartyOAuth20InAppBrowserViewController.h"

@interface CSNaverLoginImpl : NSObject<NaverThirdPartyLoginConnectionDelegate, UIApplicationDelegate> {
@public
    CSNaverLoginDelegate* _delegate;
    BOOL _login;
}

@property (readwrite, assign) CSNaverLoginDelegate* delegate;
@property (readonly, getter = isConnected) BOOL connected;
@property (readonly) NSString* accessToken;

-(id)init;

-(void)login;
-(void)logout;

@end

@implementation CSNaverLoginImpl

@synthesize delegate = _delegate;

-(id)init {
    if (self = [super init]) {
        NSString* appName = [[NSBundle mainBundle]objectForInfoDictionaryKey:@"CFBundleDisplayName"];
        
        NaverThirdPartyLoginConnection* naver3rdPartyConnection = [NaverThirdPartyLoginConnection getSharedInstance];
        [naver3rdPartyConnection setServiceUrlScheme:kNaverServiceAppUrlScheme];
        [naver3rdPartyConnection setConsumerKey:kNaverConsumerKey];
        [naver3rdPartyConnection setConsumerSecret:kNaverConsumerSecret];
        [naver3rdPartyConnection setAppName:appName];
        
        [naver3rdPartyConnection setIsNaverAppOauthEnable:NO];
        [naver3rdPartyConnection setIsInAppOauthEnable:YES];
        
        _login = NO;
    }
    return self;
}

-(void)login {
    NaverThirdPartyLoginConnection* naver3rdPartyConnection = [NaverThirdPartyLoginConnection getSharedInstance];
    naver3rdPartyConnection.delegate = self;
    [naver3rdPartyConnection requestThirdPartyLogin];
    
    _login = YES;
}

-(void)logout {
    [[NaverThirdPartyLoginConnection getSharedInstance] resetToken];
}

-(BOOL)isConnected {
    return self.accessToken != nil;
}

-(NSString*)accessToken {
	return [[NaverThirdPartyLoginConnection getSharedInstance] accessToken];
}

-(BOOL)application:(UIApplication*)application openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options {
    [self handleWithUrl:url];
    
    return YES;
}

-(BOOL)handleWithUrl:(NSURL*)url {
    NSLog(@"naverAuthLogin - url : %@", url);
    NSLog(@"naverAuthLogin - received url scheme : %@", url.scheme);
    NSLog(@"naverAuthLogin - target url scheme : %@", kNaverServiceAppUrlScheme);
    NSLog(@"naverAuthLogin - result - %d", [url.scheme isEqualToString:kNaverServiceAppUrlScheme]);
    
    if ([[url scheme] isEqualToString:kNaverServiceAppUrlScheme]) {
        if ([[url host] isEqualToString:kCheckResultPage]) {
            NaverThirdPartyLoginConnection* naver3rdPartyConnection = [NaverThirdPartyLoginConnection getSharedInstance];
            THIRDPARTYLOGIN_RECEIVE_TYPE resultType = [naver3rdPartyConnection receiveAccessToken:url];
            if (resultType == SUCCESS) {
                // 결과값은 아래 델리게이트로 전달된다
            }
            else if (_login) {
				if (_delegate) {
					_delegate->onNaverLogin(resultType == CANCELBYUSER ? CSNaverLoginResultCancelled : CSNaverLoginResultError);
				}
				_login = NO;
            }
        }
        return YES;
    }
    return NO;
}

-(void)oauth20ConnectionDidOpenInAppBrowserForOAuth:(NSURLRequest*)request {
    NLoginThirdPartyOAuth20InAppBrowserViewController *inappAuthBrowser = [[NLoginThirdPartyOAuth20InAppBrowserViewController alloc] initWithRequest:request];
    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:inappAuthBrowser animated:YES completion:nil];
    [inappAuthBrowser release];
}

-(void)oauth20ConnectionDidFinishRequestACTokenWithAuthCode {
    if (_login) {
        if (_delegate) {
            (*_delegate)(CSNaverLoginResultSuccess);
        }
		_login = NO;
    }
}

-(void)oauth20ConnectionDidFinishRequestACTokenWithRefreshToken {
    
}

-(void)oauth20ConnectionDidFinishDeleteToken {
    
}

-(void)oauth20Connection:(NaverThirdPartyLoginConnection*)oauthConnection didFailWithError:(NSError*)error {
	if (_login) {
		if (_delegate) {
			(*_delegate)(CSNaverLoginResultError);
		}
		_login = NO;
	}
}

@end

static CSNaverLoginImpl* __sharedImpl = nil;

void CSNaverLogin::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSNaverLoginImpl alloc] init];
        [[CSAppDelegate sharedDelegate] addSubDelegate: __sharedImpl];
    }
}

void CSNaverLogin::finalize() {
    if (__sharedImpl) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate: __sharedImpl];
        
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSNaverLogin::setDelegate(CSNaverLoginDelegate *delegate) {
	[__sharedImpl setDelegate:delegate];
}

void CSNaverLogin::login() {
    [__sharedImpl login];
}

void CSNaverLogin::logout() {
    [__sharedImpl logout];
}

bool CSNaverLogin::isConnected() {
    return __sharedImpl.isConnected;
}

const char* CSNaverLogin::accessToken() {
	return __sharedImpl.accessToken.UTF8String;
}

#endif
