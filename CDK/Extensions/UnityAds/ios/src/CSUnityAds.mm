//
//  CSAppleLogin.mm
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSUnityAds.h"
#import "CSAppDelegate.h"

#import <UnityAds/UnityAds.h>

//#import <GoogleMobileAds/GoogleMobileAds.h>

@interface CSUnityAdsImpl : NSObject<UnityAdsInitializationDelegate,UnityAdsLoadDelegate,UnityAdsShowDelegate> {
    NSString* _key;
    CSUnityAdsDelegate* _delegate;
    BOOL _loading;
}

-(id)init:(bool)testMode;
-(void)doShow;
-(void)show:(CSUnityAdsDelegate*)delegate;
-(void)load;

@end

@implementation CSUnityAdsImpl

-(id)init:(bool) testMode {
    _key = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"UnityAdsUnitId"] retain];
    if (self = [super init]) {
        [UnityAds initialize:_key testMode:testMode initializationDelegate:self];
    }
    return self;
}

-(void)initializationComplete {
    NSLog(@"CSUnityAds: -UnityAds initializationComplete");
    [self load];
}

-(void)initializationFailed:(UnityAdsInitializationError)error withMessage:(NSString *)message{
    NSLog(@"CSUnityAds: -UnityAds initializationFailed");
}
      
-(void)load {
   // [UnityAds load:@"Interstitial_iOS" loadDelegate:self];
    [UnityAds load:@"Rewarded_iOS" loadDelegate:self];

}
//로드 성공시 콜백
-(void)unityAdsAdLoaded:(NSString *)placementId {
    NSLog(@"CSUnityAds: -unityAdsAdLoaded Complete");
    _loading = YES;
    if (_delegate) [self doShow];
}

//로드 실패시 콜백
-(void)unityAdsAdFailedToLoad:(NSString *)placementId withError:(UnityAdsLoadError)error withMessage:(NSString *)message{
    NSLog(@"CSUnityAds: unityAdsAdFailedToLoad: %@", message);
    if (_delegate) {
        _delegate->onUnityAdsFail(message.UTF8String);
        _delegate = NULL;
    }
    _loading = NO;
}

-(void)show:(CSUnityAdsDelegate*)delegate {
    NSLog(@"CSUnityAds: show begin");
    _delegate = delegate;
    if(_loading) {
        NSLog(@"CSUnityAds: show doshow");
        [self doShow];
    }
    else {
        NSLog(@"CSUnityAds: show reload");
        [self load];
    }
}

-(void)doShow {
    NSLog(@"CSUnityAds: doshow begin");
    [UnityAds show:[CSAppDelegate sharedDelegate].viewController placementId:@"Rewarded_iOS" showDelegate:self];
}

-(void)unityAdsShowComplete:(NSString *)placementId withFinishState:(UnityAdsShowCompletionState)state{
    BOOL isRewardGet;
    if([placementId isEqualToString:@"Rewarded_iOS"] && state == kUnityShowCompletionStateCompleted) {
        NSLog(@"CSUnityAds: onUnityAdsReward");
        isRewardGet = YES;
        //_delegate->onUnityAdsReward();
    }
    else {   //kUnityShowCompletionStateSkipped
        NSLog(@"CSUnityAds: onUnityAdsClose");
        isRewardGet = NO;
        //_delegate->onUnityAdsClose();
    }
    
    if(_delegate) {
        if(isRewardGet) {
            _delegate->onUnityAdsReward();
        }
        else {
            _delegate->onUnityAdsClose();
        }
        _delegate = NULL;
    }
    [self load];
}

//Called when UnityAds has started to show ad with a specific placement.
- (void)unityAdsShowFailed: (NSString *)placementId withError: (UnityAdsShowError)error withMessage: (NSString *)message{
    NSLog(@"CSUnityAds: unityAdsShowFailed");
    if(_delegate) {
        _delegate->onUnityAdsFail(message.UTF8String);
        _delegate = NULL;
    }
}

//Called when UnityAds has received a click while showing ad with a specific placement.
- (void)unityAdsShowStart: (NSString *)placementId{
    
}

- (void)unityAdsShowClick: (NSString *)placementId{
    
}

-(void)dealloc {
    [_key release];
    [super dealloc];
}

@end

static CSUnityAdsImpl* __sharedImpl = nil;

void CSUnityAds::initialize(bool test) {
    if (!__sharedImpl) {
        __sharedImpl = [[CSUnityAdsImpl alloc] init:test];
    }
}

void CSUnityAds::finalize() {
    [__sharedImpl release];
    __sharedImpl = nil;
}

void CSUnityAds::show(CSUnityAdsDelegate* delegate)  {
    [__sharedImpl show:delegate];
}

#endif
