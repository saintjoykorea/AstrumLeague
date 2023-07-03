//
//  CSAppleLogin.mm
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSVungleAd.h"
#import "CSAppDelegate.h"
#import <VungleSDK/VungleSDK.h>

//#import <GoogleMobileAds/GoogleMobileAds.h>

@interface CSVungleAdImpl :NSObject<VungleSDKDelegate>{
    NSString* _key;
    NSString* _placementID;
    CSVungleAdDelegate* _delegate;
    BOOL _loading;
    //NSError* error;
}

@property (atomic, readonly, getter=isInitialized) BOOL initialzed;

-(id)init;
-(void)doShow;
-(void)show:(CSVungleAdDelegate*)delegate;
-(void)load;

@end

@implementation CSVungleAdImpl

-(id)init {
    if (self = [super init]) {
        NSError *error = nil;
        _key = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"VungleUnitId"] retain];
        _placementID = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"VungleplacementID"] retain];
        
        [[VungleSDK sharedSDK] startWithAppId:_key error:&error];
        [[VungleSDK sharedSDK] setDelegate:self];
    }
    return self;
}

//init 성공
- (void)vungleSDKDidInitialize {
    NSLog(@"CSVungle: vungleSDKDidInitialize complete");
    _loading = YES;
    //[self load];
}
//init 실패
- (void)vungleSDKFailedToInitializeWithError:(NSError *)error {
    NSLog(@"CSVungle: Error vungleAd encountered playing ad: %@", error);
}

-(void)load {
    NSError *error = nil;
    if ([[VungleSDK sharedSDK] loadPlacementWithID:_placementID error:&error]) {
        NSLog(@"CSVungle: vungleAd load complete");
        _loading = YES;
        if (_delegate) [self doShow];
    }
    else {
        //로딩 실패
        NSLog(@"CSVungle: vungleAd load Fail");
        if (_delegate) {
            const char* msg = error.localizedDescription ? error.localizedDescription.UTF8String : "Unknown";
            _delegate->onVungleAdFail(msg);
            _delegate = NULL;
        }
        _loading = NO;
    }
}

- (void)vungleAdPlayabilityUpdate:(BOOL)isAdPlayable placementID:(NSString *)placementID error:(nullable NSError *)error {
    
    if(error) {
        NSLog(@"CSVungle: -->> vungleAdPlayabilityUpdate Error %@", error.localizedDescription);
    }
    if (isAdPlayable) {
            NSLog(@"CSVungle: -->> Delegate Callback: vungleAdPlayabilityUpdate: Ad is available for Placement ID: %@", placementID);
        } else {
            NSLog(@"CSVungle: -->> Delegate Callback: vungleAdPlayabilityUpdate: Ad is NOT available for Placement ID: %@", placementID);
        }
    
    if ([placementID isEqualToString:_placementID]) {
        NSLog(@"CSVungle: vungleAdPlayabilityUpdate placementID: %@  _placementID: %@", placementID , _placementID);
        }
}



-(void)show:(CSVungleAdDelegate*)delegate {
    NSLog(@"CSVungle: show beggin");
    _delegate = delegate;

    if (_loading) {
        NSLog(@"CSVungle: show doshow");
        [self doShow];
    }
    else {
        NSLog(@"CSVungle: show load");
        [self load];
    }
}

-(void)doShow {
    NSDictionary *options;
    switch([[UIApplication sharedApplication] statusBarOrientation]){
        case UIInterfaceOrientationLandscapeLeft:
            NSLog(@"CSVungle: UIInterfaceOrientationMaskLandscapeLeft");
             options = @{VunglePlayAdOptionKeyOrientations: @(UIInterfaceOrientationLandscapeLeft),
                                      VunglePlayAdOptionKeyStartMuted:@"false",
                                      VunglePlayAdOptionKeyIncentivizedAlertBodyText : @"If the video isn't completed you won't get your reward! Are you sure you want to close early?",
                                      VunglePlayAdOptionKeyIncentivizedAlertCloseButtonText : @"Close",
                                      VunglePlayAdOptionKeyIncentivizedAlertContinueButtonText : @"Keep Watching",
                                      VunglePlayAdOptionKeyIncentivizedAlertTitleText : @"Careful!"};
            break;
        case UIInterfaceOrientationLandscapeRight:
            NSLog(@"CSVungle: UIInterfaceOrientationMaskLandscapeRight");
            options = @{VunglePlayAdOptionKeyOrientations: @(UIInterfaceOrientationMaskLandscapeRight),
                                     VunglePlayAdOptionKeyStartMuted:@"false",
                                     VunglePlayAdOptionKeyIncentivizedAlertBodyText : @"If the video isn't completed you won't get your reward! Are you sure you want to close early?",
                                     VunglePlayAdOptionKeyIncentivizedAlertCloseButtonText : @"Close",
                                     VunglePlayAdOptionKeyIncentivizedAlertContinueButtonText : @"Keep Watching",
                                     VunglePlayAdOptionKeyIncentivizedAlertTitleText : @"Careful!"};
            break;
        default:
            NSLog(@"CSVungle: UIInterfaceOrientationMaskAll");
            options = @{VunglePlayAdOptionKeyOrientations: @(UIInterfaceOrientationMaskAll),
                                     VunglePlayAdOptionKeyStartMuted:@"false",
                                     VunglePlayAdOptionKeyIncentivizedAlertBodyText : @"If the video isn't completed you won't get your reward! Are you sure you want to close early?",
                                     VunglePlayAdOptionKeyIncentivizedAlertCloseButtonText : @"Close",
                                     VunglePlayAdOptionKeyIncentivizedAlertContinueButtonText : @"Keep Watching",
                                     VunglePlayAdOptionKeyIncentivizedAlertTitleText : @"Careful!"};
            break;
    }
    NSError *error = nil;
    
    NSLog(@"CSVungle: doShow");
    
    BOOL isAdReady = [[VungleSDK sharedSDK] isAdCachedForPlacementID:_placementID];
    if(isAdReady){
        NSLog(@"CSVungle: isAdCachedForPlacementID   YES");
    }
    else{
        NSLog(@"CSVungle: isAdCachedForPlacementID    NO");
    }
    [[VungleSDK sharedSDK] playAd:[CSAppDelegate sharedDelegate].viewController options:options placementID:_placementID error:&error];
    
    
    if (error) {
        NSLog(@"CSVungle: Error vungleAd encountered playing ad: %@", error);
        if (_delegate) {
            _delegate->onVungleAdFail([NSString stringWithFormat:@"%@", error.localizedDescription].UTF8String);
            _delegate = NULL;
        }
    }
}



//동영상 광고를 재생하려고 할 때 호출
//구현하면 SDK가 광고를 표시하려고 할 때 호출됩니다. 이 점
//게임을 일시 중지하고 재생 중인 모든 소리를 끌 좋은 시간이 될 수 있습니다.
- (void)vungleWillShowAdForPlacementID:(nullable NSString *)placementID{
    NSLog(@"CSVungle: vungleWillShowAdForPlacementID");
}

//구현된 경우 SDK가 뷰 컨트롤러 또는
//* 광고가 있는 보기.
//* @param placeID 표시될 게재위치입니다.
- (void)vungleDidShowAdForPlacementID:(nullable NSString *)placementID{
    NSLog(@"CSVungle: vungleDidShowAdForPlacementID");
}

//* 구현된 경우 지정된 게재위치에 대해 광고가 처음 렌더링될 때 호출됩니다.
//* @참고: 조회수를 추적하려면 이 콜백을 사용하세요.
//* @param PlacementID 표시된 광고의 게재위치 ID
- (void)vungleAdViewedForPlacement:(NSString *)placementID{
    NSLog(@"CSVungle: vungleAdViewedForPlacement");
}

//구현된 경우 이 메서드는 Vungle 광고 단위가 완전히 닫히려고 할 때 호출됩니다.
//* 이 시점에서 게임 또는 앱을 다시 시작하는 것이 좋습니다.
- (void)vungleWillCloseAdForPlacementID:(nonnull NSString *)placementID{
    NSLog(@"CSVungle: vungleWillCloseAdForPlacementID");
}

//* 구현하면 Vungle 광고 단위가 완전히 닫힐 때 이 메서드가 호출됩니다.
//* 이 시점에서 필요한 경우 자동 캐시되지 않은 게재위치에 대해 다른 광고를 로드할 수 있습니다.
- (void)vungleDidCloseAdForPlacementID:(nonnull NSString *)placementID{
    NSLog(@"CSVungle: vungleDidCloseAdForPlacementID  onVungleAdsReward");
    if (_delegate) {
        _delegate->onVungleAdReward();
        _delegate = NULL;
    }
    [self load];
}

-(void)dealloc {
    [_key release];
    [_placementID release];
    [super dealloc];
}

@end

static CSVungleAdImpl* __sharedImpl = nil;

void CSVungleAd::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSVungleAdImpl alloc] init];
    }
}

void CSVungleAd::finalize() {
   [__sharedImpl release];
    __sharedImpl = nil;
}

void CSVungleAd::show(CSVungleAdDelegate* delegate)  {
    [__sharedImpl show:delegate];
}

#endif
