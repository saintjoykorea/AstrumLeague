//
//  CSFlurry.m
//  TalesCraft2
//
//  Created by Kim Su hyun on 2020. 8. 19..
//  Copyright © 2018년 brgames. All rights reserved.
//


#ifdef CDK_IOS

#define CDK_IMPL

#import "CSFlurry.h"

#import "CSAppDelegate.h"

#import "Flurry.h"

void CSFlurry::initialize() {    
	NSString* devKey = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"FlurryDevKey"];		//J3K8YJC9Q6NC8H3ZCZ5X
	
	if (!devKey) {
		NSLog(@"CSFlurry not ready");
		abort();
	}

    NSString *appVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    FlurrySessionBuilder* builder = [[[[[FlurrySessionBuilder new] withLogLevel:FlurryLogLevelAll] withCrashReporting:YES] withSessionContinueSeconds:10] withAppVersion:appVersion];
    [Flurry startSession:devKey withSessionBuilder:builder];
}

void CSFlurry::finalize() {
    
}

void CSFlurry::log(const char* name, int paramCount, ...) {
    NSMutableDictionary* params = [[NSMutableDictionary alloc] initWithCapacity:paramCount];
	
	va_list ap;
    va_start(ap, paramCount);
    for (int i = 0; i < paramCount; i++) {
		const char* key = va_arg(ap, const char*);
		const char* value = va_arg(ap, const char*);
		[params setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
	}
    va_end(ap);
	
    [Flurry logEvent:[NSString stringWithUTF8String:name] withParameters:params];
   // [Flurry logEvent:name withParameters:params];
	
	[params release];
}

#endif




