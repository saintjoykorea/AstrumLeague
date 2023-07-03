//
//  CSAppSafer.m
//  TalesCraft2
//
//  Created by Kim Chan on 2021. 9. 28..
//  Copyright © 2021년 brgames. All rights reserved.
//


#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppSafer.h"

#ifndef DEBUG

#import "CSAppDelegate.h"

#import <AppSaferFramework/AppSaferFramework.h>

#define TamperingInterval	60

@interface CSAppSaferImpl : NSObject<AppSaferDelegate> {
	AppSafer* _appSafer;
	BOOL _alive;
}

-(id)init;
-(void)dealloc;
-(void)showAlert:(NSString*)message exit:(BOOL)exit;
-(void)setUserId:(NSString*)userId;
-(void)stopTampering;
-(void)checkTampering;

@end

@implementation CSAppSaferImpl

-(id)init {
	if (self = [super init]) {
		NSString* key = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"AppSaferKey"];
	
		if (!key) {
			NSLog(@"CSAppSafer key not ready");
			[self release];
			return nil;
		}

		_appSafer = [[AppSafer alloc] init];
		
		if (!_appSafer) {
			NSLog(@"AppSafer not available");
			[self release];
			return nil;
		}
		
		int res = [_appSafer initAppSafer:self serviceCode:@"ncloud" key:key];
		
		if (res != 0) {
			NSLog(@"CSAppSafer not init fail");			
			[self release];
			return nil;
		}
		NSLog(@"CSAppSafer initialized");
		
		_alive = YES;
	}
	return self;
}

-(void)dealloc {
	[_appSafer release];
	
	[super dealloc];
}


-(void)showAlert:(NSString*)message exit:(BOOL)exit {
	UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Alert" message:message preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        if (exit) ::exit(0);
    }]];
    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:alert animated:YES completion:nil];
}

-(void)setUserId:(NSString*)userId {
	NSLog(@"CSAppSafer userId:%@", userId);
	
	[_appSafer setUserId:userId];
}

-(void)stopTampering {
	_alive = NO;
}

-(void)checkTampering {
	int res = [_appSafer checkTampering];
        
	switch (res) {
		case -1:
			NSLog(@"CSAppSafer failed to check tampering");
			
			//[self showAlert:@"Failed to check tampering" exit:NO];		//TODO:CONFIRM MESSAGE
			break;
		case 0:
			NSLog(@"CSAppSafer check tampering success");
			
			//[self showAlert:@"Check tampering success" exit:NO];		//TODO:CONFIRM MESSAGE
			break;
		case 2:
			NSLog(@"CSAppSafer device is blocked!");
		
			//[self showAlert:@"terminated due to security issues" exit:YES];
			break;
		case 3:
			NSLog(@"CSAppSafer is not initialized");
			
			//[self showAlert:@"Not initialized" exit:NO];		//TODO:CONFIRM MESSAGE
			break;
	}
}

-(void)appSaferDidInitFinish:(int)result {
    NSLog(@"CSAppSafer appSaferDidInitFinish result: %d", result);
	
    if (result == 0) {
        if (_alive) [self checkTampering];
    }
    else if (result == 2) {
		[self showAlert:@"terminated due to security issues" exit:YES];
    }
}

-(void)appSaferDidCheckTamperingFinish:(int)result {
    NSLog(@"CSAppSafer appSaferDidCheckTamperingFinish result: %d", result);
    
    if (result == 2) {
        [self showAlert:@"terminated due to security issues" exit:YES];
    }
	else if (_alive) {
		[self performSelector:@selector(checkTampering) withObject:nil afterDelay:TamperingInterval];
	}
}

-(void)appSaferDetectedJailbreak {
    NSLog(@"CSAppSafer appSaferDetectedJailbreak");
}

-(void)appSaferDetectedSimulator {
    NSLog(@"CSAppSafer appSaferDetectedSimulator");
}

-(void)appSaferDetectedDebugging {
    NSLog(@"CSAppSafer appSaferDetectedDebugging");
}

-(void)appSaferDetectedMemoryTampered {
    NSLog(@"CSAppSafer appSaferDetectedMemoryTampered");
}

@end

static CSAppSaferImpl* __sharedImpl = nil;

void CSAppSafer::initialize() {
	if (!__sharedImpl) {
        __sharedImpl = [[CSAppSaferImpl alloc] init];
    }
}

void CSAppSafer::finalize() {
	[__sharedImpl stopTampering];
	[__sharedImpl release];
	__sharedImpl = nil;
}

void CSAppSafer::setUserId(const char* userId) {
    [__sharedImpl setUserId:[NSString stringWithUTF8String:userId]];
}

#else

#import <Foundation/Foundation.h>

void CSAppSafer::initialize() {
    NSLog(@"CSAppSafer is off on DEBUG");
}

void CSAppSafer::finalize() {
    
}

void CSAppSafer::setUserId(const char* userId) {
    
}

#endif

#endif

