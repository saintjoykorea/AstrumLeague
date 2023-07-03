//
//  CSAppsFlyer.m
//  TalesCraft2
//
//  Created by ChangSun on 2018. 8. 10..
//  Copyright © 2018년 brgames. All rights reserved.
//


#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppsFlyer.h"

#import "CSAppDelegate.h"

#import <AppsFlyerLib/AppsFlyerLib.h>

void CSAppsFlyer::initialize() {
    NSString* appId = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"AppsFlyerAppID"];
	NSString* devKey = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"AppsFlyerDevKey"];
	
	if (!appId || !devKey) {
		NSLog(@"CSAppsFlyer not ready");
		abort();
	}
	
    [AppsFlyerLib shared].appleAppID = appId;
    [AppsFlyerLib shared].appsFlyerDevKey  = devKey;
    //[AppsFlyerLib shared].delegate = self;
    [[AppsFlyerLib shared] start];
#ifdef DEBUG
    [AppsFlyerLib shared].isDebug = true;
    [AppsFlyerLib shared].useReceiptValidationSandbox = YES;
    [AppsFlyerLib shared].useUninstallSandbox = true;
#endif
}

void CSAppsFlyer::finalize() {

}

void CSAppsFlyer::log(const char* name, int paramCount, ...) {
    NSMutableDictionary* params = [[NSMutableDictionary alloc] initWithCapacity:paramCount];
	
	va_list ap;
    va_start(ap, paramCount);
    for (int i = 0; i < paramCount; i++) {
		const char* key = va_arg(ap, const char*);
		const char* value = va_arg(ap, const char*);
		[params setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
	}
    va_end(ap);
	
    [[AppsFlyerLib shared] logEvent:[NSString stringWithUTF8String:name] withValues:params];
	
	[params release];
}

#endif

