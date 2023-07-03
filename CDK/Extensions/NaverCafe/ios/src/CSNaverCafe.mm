//
//  CSNaverCafe.mm
//  CDK
//
//  Created by WooyolJung on 2017. 11. 21.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSNaverCafe.h"

#import "CSFile.h"
#import "CSIOSView.h"
#import "CSAppDelegate.h"

#import "NaverThirdPartyConstantsForApp.h"

#import <NaverCafeSDK/NCSDKManager.h>
#import <NaverCafeSDK/NCSDKLoginManager.h>

@interface CSNaverCafeImpl : NSObject<NCSDKManagerDelegate, UIApplicationDelegate, UIAlertViewDelegate> {
    BOOL _sliderToggle;
}

-(id)init;
-(void)startHome;
-(void)startArticle:(int)articleId;

@end

@implementation CSNaverCafeImpl

-(id)init {
    if (self = [super init]) {
		NSDictionary* dic = [[NSBundle mainBundle] infoDictionary];
		
		BOOL naver = NO;
		BOOL neo = NO;

		NSString* naverConsumerKey = [dic objectForKey:@"NaverCafeConsumerKey"];
		NSString* naverConsumerSecret = [dic objectForKey:@"NaverCafeConsumerSecret"];
		NSNumber* naverCafeId = [dic objectForKey:@"NaverCafeId"];
        NSString* neoConsumerKey = [dic objectForKey:@"NaverCafeNeoConsumerKey"];
		NSNumber* neoCommunityId = [dic objectForKey:@"NaverCafeNeoCommunityId"];
		
		if (naverConsumerKey && naverConsumerSecret && naverCafeId) {
			[[NCSDKManager getSharedInstance] setNaverLoginClientId:naverConsumerKey
											 naverLoginClientSecret:naverConsumerSecret
															 cafeId:cafeId.intValue];
															 
			naver = YES;
		}
		if (neoConsumerKey && neoCommunityId) {
			[[NCSDKManager getSharedInstance] setNeoIdConsumerKey:neoConsumerKey
													  communityId:neoCommunityId.intValue];
			
			neo = YES;
		}
		if (!naver && !neo) {
			NSLog(@"CSNaverCafe not ready");
			abort();
		}
												  
        [[NCSDKLoginManager getSharedInstance] setNaverLoginURLScheme:kNaverServiceAppUrlScheme];
        [[NCSDKManager getSharedInstance] setParentViewController:[[CSAppDelegate sharedDelegate] viewController]];
        [[NCSDKManager getSharedInstance] setNcSDKDelegate:self];
        [[NCSDKManager getSharedInstance] setOrientationIsLandscape:YES];
        [[NCSDKManager getSharedInstance] setUseWidgetVideoRecord:YES];
    }
    return self;
}

-(void)startHome {
    [[NCSDKManager getSharedInstance] presentMainViewController];
}

-(void)startArticle:(int)articleId {
    [[NCSDKManager getSharedInstance] presentMainViewControllerWithArticleId:articleId];
}

#pragma mark - UIApplicationDelegate
-(BOOL)application:(UIApplication*)application openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options {
    return [[NCSDKLoginManager getSharedInstance] finishNaverLoginWithURL:url];
}

#pragma mark - NCSDKManagerDelegate
-(void)ncSDKViewDidLoad {
    NSLog(@"NaverCafe ncSDKViewDidLoad");
}
-(void)ncSDKViewDidUnLoad {
    NSLog(@"NaverCafe ncSDKViewDidUnLoad");
}
-(void)ncSDKJoinedCafeMember {
    NSLog(@"NaverCafe ncSDKJoinedCafeMember");
}
-(void)ncSDKPostedArticleAtMenu:(NSInteger)menuId
                attachImageCount:(NSInteger)imageCount
                attachVideoCount:(NSInteger)videoCount {
	NSLog(@"NaverCafe ncSDKPostedArticleAtMenu:%d", menuId);
}
-(void)ncSDKPostedCommentAtArticle:(NSInteger)articleId {
	NSLog(@"NaverCafe ncSDKPostedCommentAtArticle:%d", articleId);
}

-(void)ncSDKRequestScreenShot {
    
}

-(void)ncSDKDidVoteAtArticle:(NSInteger)articleId {
	NSLog(@"NaverCafe ncSDKDidVoteAtArticle:%d", articleId);
}

-(void)ncSDKWidgetPostArticleWithImage {
    const char* filename = "screenshot.png";
    const char* filePath = CSFile::storagePath(filename);
    bool result = ((CSIOSView*)[[CSAppDelegate sharedDelegate] viewController]).graphics->target()->screenshot(filePath);
    
    if (result) {
        [[NCSDKManager getSharedInstance] presentArticlePostViewControllerWithType:kGLArticlePostTypeImage filePath:[NSString stringWithUTF8String:filePath]];
    }
}
-(void)ncSDKWidgetSuccessVideoRecord {
    [[NCSDKManager getSharedInstance] presentArticlePostViewController];
}

#pragma mark - UIAlertViewDelegate
-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    NSString *title = [alertView buttonTitleAtIndex:buttonIndex];
    [[NCSDKManager getSharedInstance] setParentViewController:[[CSAppDelegate sharedDelegate] viewController]];
    if ([title isEqualToString:@"Article"]) {
        [[NCSDKManager getSharedInstance] presentMainViewControllerWithArticleId:1];
    } else if ([title isEqualToString:@"Notice"]) {
        [[NCSDKManager getSharedInstance] presentMainViewControllerWithTabIndex:kGLTabTypeNotice];
    } else if ([title isEqualToString:@"Event"]) {
        [[NCSDKManager getSharedInstance] presentMainViewControllerWithTabIndex:kGLTabTypeEvent];
    } else if ([title isEqualToString:@"Menu List"]) {
        [[NCSDKManager getSharedInstance] presentMainViewControllerWithTabIndex:kGLTabTypeMenuList];
    } else if ([title isEqualToString:@"Profile"]) {
        [[NCSDKManager getSharedInstance] presentMainViewControllerWithTabIndex:kGLTabTypeProfile];
    } else if ([title hasPrefix:@"Toggle"]) {
        _sliderToggle = !_sliderToggle;
        [[NCSDKManager getSharedInstance] disableTransparentSlider:_sliderToggle];
    }
}

@end

static CSNaverCafeImpl* __sharedImpl = nil;

void CSNaverCafe::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSNaverCafeImpl alloc] init];
        [[CSAppDelegate sharedDelegate] addSubDelegate: __sharedImpl];
    }
}

void CSNaverCafe::finalize() {
    if (__sharedImpl) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate: __sharedImpl];
        
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSNaverCafe::startHome() {
    [__sharedImpl startHome];
}

void CSNaverCafe::startArticle(int articleId) {
    [__sharedImpl startArticle:articleId];
}

#endif
