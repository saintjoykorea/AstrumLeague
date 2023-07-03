//
//  CSAppleLogin.mm
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSGoogleAdMob.h"
#import "CSAppDelegate.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface CSGoogleAdMobImpl : NSObject<GADFullScreenContentDelegate> {
    NSString* _key;
    CSGoogleAdMobDelegate* _delegate;
    GADRewardedInterstitialAd* _ad;
    GADRequest* _request;
    BOOL _earned;
    BOOL _loading;
}

-(id)init;
-(void)doShow;
-(void)show:(CSGoogleAdMobDelegate*)delegate;
-(void)load;

@end

@implementation CSGoogleAdMobImpl

-(id)init {
    if (self = [super init]) {
        //보상형 광고    ca-app-pub-3940256099942544/1712485313
        //보상형 전면 광고    ca-app-pub-3940256099942544/6978759866
#ifdef DEBUG
        _key = [@"ca-app-pub-3940256099942544/5354046379" retain];
#else
        _key = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"GoogleAdMobUnitId"] retain];
 
        NSAssert(_key, @"invalid key");
#endif
        //_key = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"GoogleAdMobUnitId"] retain];
        [[GADMobileAds sharedInstance] startWithCompletionHandler:^(GADInitializationStatus* status) {
            [self load];
        }];
    }
    return self;
}

-(void)dealloc {
    [_key release];
    [_ad release];
    
    [super dealloc];
}

-(void)load {
    _loading = YES;

    [GADRewardedInterstitialAd
         loadWithAdUnitID:_key
                  request:_request
        completionHandler:^(GADRewardedInterstitialAd *ad, NSError *error)
     {
        _loading = NO;
        
        if (error) {
            NSLog(@"CSGoogleAdMob: fail to load: %@", error);
            if (_delegate) {
                //todo
                //_delegate->onGoogleAdMobFail(error.code);
                _delegate->onGoogleAdMobFail([NSString stringWithFormat:@"%@", error.localizedDescription].UTF8String);
                _delegate = NULL;
            }
        }
        else {
            _ad = [ad retain];
            _ad.fullScreenContentDelegate = self;
            if (_delegate) [self doShow];
        }
    }];
}

-(void)doShow {
    [_ad presentFromRootViewController:[CSAppDelegate sharedDelegate].viewController userDidEarnRewardHandler:^{
        NSLog(@"CSGoogleAdMob: earned");
        _earned = YES;
    }];
}

-(void)show:(CSGoogleAdMobDelegate*)delegate {
    _delegate = delegate;
    _earned = NO;

    if (_ad) {
        [self doShow];
    }
    else if(!_loading){
        [self load];
    }
}

- (void)ad:(nonnull id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(nonnull NSError *)error {
    NSLog(@"CSGoogleAdMob: fail to show: %@", error);

    if (_delegate) {
        //_delegate->onGoogleAdMobFail(error.description.UTF8String());
        _delegate->onGoogleAdMobFail([NSString stringWithFormat:@"%@", error.localizedDescription].UTF8String);
        _delegate = NULL;
    }
}

/// Tells the delegate that the ad presented full screen content.
- (void)adDidPresentFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
    NSLog(@"CSGoogleAdMob: show");
}

/// Tells the delegate that the ad dismissed full screen content.
- (void)adDidDismissFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
    NSLog(@"CSGoogleAdMob: close");
    
    if (_delegate) {
        if (_earned) {
            //GADAdReward *reward = _ad.adReward;
            //const char* type = reward.type.UTF8String;
            //int amount = reward.amount.integerValue;
            //_delegate->onGoogleAdMobReward(type, amount);
            _delegate->onGoogleAdMobReward();
        }
        else {
            _delegate->onGoogleAdMobClose();
        }
        _delegate = NULL;
    }
    [_ad release];
    _ad = nil;
    
    [self load];
}

@end

static CSGoogleAdMobImpl* __sharedImpl = nil;

void CSGoogleAdMob::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSGoogleAdMobImpl alloc] init];
    }
}

void CSGoogleAdMob::finalize() {
    [__sharedImpl release];
    __sharedImpl = nil;
}

void CSGoogleAdMob::show(CSGoogleAdMobDelegate* delegate)  {
    [__sharedImpl show:delegate];
}

#endif
