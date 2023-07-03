//
//  CSAppleGameCenter.mm
//  CDK
//
//  Created by WooyolJung on 2017. 11. 22.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppleGameCenter.h"

#import "CSAppDelegate.h"
#import "CSFile.h"
#import "CSBuffer.h"

#import <GameKit/GameKit.h>

@interface CSAppleGameCenterImpl : NSObject<GKGameCenterControllerDelegate> {
    CSAppleGameCenterDelegate* _delegate;
}
@property (readwrite, assign) CSAppleGameCenterDelegate* delegate;
@property (readonly) NSString* playerId;
@property (readonly, getter = isConnected) BOOL connected;

-(id)init;

-(void)login;
-(void)reportAchievements:(NSString*)identifier value:(double)value;
-(void)showAchievements;
-(void)reportLeaderboards:(int64_t)value;
-(void)showLeaderboards;

@end

@implementation CSAppleGameCenterImpl

@synthesize delegate = _delegate;

-(id)init {
    if (self = [super init]) {
        GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
        
        localPlayer.authenticateHandler = ^(UIViewController* viewController, NSError* error) {
			if (viewController) {
                [[[CSAppDelegate sharedDelegate] viewController] presentViewController:viewController animated:YES completion:nil];
            }
			else if (localPlayer.isAuthenticated) {
				//if (_delegate) {
					//_delegate->onAppleGameCenterLogin(CSAppleGameCenterLoginResultSuccess);
				//}
			}
			else {
				NSLog(@"CSAppleGameCenter authenticate error: %@", error);
				if (_delegate) {
					_delegate->onAppleGameCenterLogin(error.code == GKErrorCancelled ? CSAppleGameCenterLoginResultCancelled : CSAppleGameCenterLoginResultError);
				}
			}
        };
    }
    return self;
}

-(void)login {
    if (self.isConnected) {
        if (_delegate) {
            _delegate->onAppleGameCenterLogin(CSAppleGameCenterLoginResultSuccess);
        }
    }
    else {
        GKGameCenterViewController*  gameCenterViewController = [[GKGameCenterViewController alloc] init];
        gameCenterViewController.gameCenterDelegate = self;
        [[[CSAppDelegate sharedDelegate] viewController] presentViewController:gameCenterViewController animated:YES completion:nil];
        [gameCenterViewController autorelease];
    }
}

-(BOOL)isConnected {
    GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
    
    return localPlayer.isAuthenticated;
}

-(NSString*)playerId {
	GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
	
    return localPlayer.playerID;
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [gameCenterViewController.presentingViewController dismissViewControllerAnimated:YES completion:^(void){}];
    [gameCenterViewController.presentedViewController dismissViewControllerAnimated:YES completion:^(void){}];
    [gameCenterViewController dismissViewControllerAnimated:YES completion:^(void){}];
}

-(void)reportAchievements:(NSString*)identifier value:(double)value {
	if (!self.isConnected) {
		NSLog(@"CSAppleGameCenter reportAchievements error: not connected");
		return;
	}
	
    GKAchievement* achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
    
    achievement.percentComplete = value;
    achievement.showsCompletionBanner = YES;
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError* error) {
        if (error) {
            NSLog(@"CSAppleGameCenter reportAchievements error: %@", error);
        }
    }];
    
    [achievement release];
}

-(void)showAchievements {
	if (!self.isConnected) {
		NSLog(@"CSAppleGameCenter showAchievements error: not connected");
		return;
	}
	
    GKGameCenterViewController* gameCenterViewController = [[GKGameCenterViewController alloc] init];
    gameCenterViewController.gameCenterDelegate = self;
    
    gameCenterViewController.viewState = GKGameCenterViewControllerStateAchievements;
    
    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:gameCenterViewController animated:YES completion:nil];
    [gameCenterViewController autorelease];
}

-(void)reportLeaderboards:(int64_t)value {
	if (!self.isConnected) {
		NSLog(@"CSAppleGameCenter reportLeaderboards error: not connected");
		return;
	}
	
	NSString* leaderboardId = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"GameCenterLeaderboardId"];
	
	if (!leaderboardId) {
		NSLog(@"CSAppleGameCenter reportLeaderboards error: leader board identifier not ready");
		return;
	}
	
    GKScore* score = [[GKScore alloc] initWithLeaderboardIdentifier:leaderboardId];
    
    score.value = value;
    
    [GKScore reportScores:@[score] withCompletionHandler:^(NSError* error) {
        if (error) {
            NSLog(@"reportScores ERROR: %@", error);
        }
    }];
    
    [score release];
}

-(void)showLeaderboards {
	if (!self.isConnected) {
		NSLog(@"CSAppleGameCenter showLeaderboards error: not connected");
		return;
	}

	NSString* leaderboardId = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"GameCenterLeaderboardId"];
	
	if (!leaderboardId) {
		NSLog(@"CSAppleGameCenter showLeaderboards error: leader board identifier not specified");
		return;
	}
	
    GKGameCenterViewController* gameCenterViewController = [[GKGameCenterViewController alloc] init];
    gameCenterViewController.gameCenterDelegate = self;
    
    gameCenterViewController.viewState = GKGameCenterViewControllerStateLeaderboards;
    gameCenterViewController.leaderboardIdentifier = leaderboardId;
    
    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:gameCenterViewController animated:YES completion:nil];
    [gameCenterViewController autorelease];
}

@end

static CSAppleGameCenterImpl* __sharedImpl = nil;

void CSAppleGameCenter::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSAppleGameCenterImpl alloc] init];
    }
}

void CSAppleGameCenter::finalize() {
    if (__sharedImpl) {
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSAppleGameCenter::setDelegate(CSAppleGameCenterDelegate* delegate) {
	[__sharedImpl setDelegate:delegate];
}

void CSAppleGameCenter::login() {
    [__sharedImpl login];
}

bool CSAppleGameCenter::isConnected() {
    return __sharedImpl.isConnected;
}

const char* CSAppleGameCenter::playerId() {
	return __sharedImpl.playerId.UTF8String;
}

void CSAppleGameCenter::reportAchievements(const char* id, uint value, uint maxValue, bool replace) {
    if (!replace) {
        const char* path = CSFile::storagePath(CSString::cstringWithFormat("gc_achv_%s_%s", __sharedImpl.playerId.UTF8String, id));
        
        CSBuffer* buffer = CSBuffer::createWithContentOfFile(path);
        if (buffer) {
            value += (uint)buffer->readInt();
            buffer->clear();
        }
        else {
            buffer = new CSBuffer(4);
        }
        buffer->writeInt(value);
        buffer->writeToFile(path);
        
        buffer->release();
    }
    
    [__sharedImpl reportAchievements:[NSString stringWithUTF8String:id] value:CSMath::min(value * 100.0 / maxValue, 100.0)];
}

void CSAppleGameCenter::showAchievements() {
    [__sharedImpl showAchievements];
}

void CSAppleGameCenter::reportLeaderboards(uint score) {
    [__sharedImpl reportLeaderboards:score];
}

void CSAppleGameCenter::showLeaderboards() {
    [__sharedImpl showLeaderboards];
}

#endif
