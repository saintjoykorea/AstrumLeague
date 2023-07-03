//
//  AppSafer.h
//  AppSaferFramework
//
//  Created by Moonriver on 2016. 4. 5..
//  Copyright © 2016년 Naver Business Platform. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#ifndef _APP_SAFER_H
#define _APP_SAFER_H

#ifdef DEBUG
#define BETA
#else
#define REAL
#define NSLog(...)
#endif

#ifndef APPSAFER_VERSION
#define APPSAFER_VERSION @"3.4.0"
#endif

#define APPSAFER_KEY_MID_LENGTH 16

#if defined(DEV)
#define CODE_LOGGER_PROJECT_KEY @"P8f89c1NB11995e8a38b_AppSaferCodeDev"
#define CODE_LOGGER_SERVER_HOST @"nelo2-col.navercorp.com"
#define POLICY_MANAGER_URL @"http://dev-api.appsafer.ncloud.com/log/v4/init"
#define POLICY_MANAGER_SERVER_HOST @"beta-elsa-col.ncloud.com"
#elif defined(BETA)
#define CODE_LOGGER_PROJECT_KEY @"P8f89c1NB11995e8a38b_AppSaferCodeBeta"
#define CODE_LOGGER_SERVER_HOST @"nelo2-col.navercorp.com"
#define POLICY_MANAGER_URL @"https://beta-api-appsafer.ncloud.com/log/v4/init"
#define DETECT_URL @"https://beta-api-appsafer.ncloud.com/log/v4/detect"
#define POLICY_MANAGER_SERVER_HOST @"beta-elsa-col.ncloud.com"
#elif defined(REAL)
#define CODE_LOGGER_PROJECT_KEY @"P8f89c1NB11995e8a38b_AppSaferCodeReal"
#define CODE_LOGGER_SERVER_HOST @"nelo2-col.navercorp.com"
#define POLICY_MANAGER_URL @"https://api-appsafer.ncloud.com/log/v4/init"
#define DETECT_URL @"https://api-appsafer.ncloud.com/log/v4/detect"
#define POLICY_MANAGER_SERVER_HOST @"elsa-col.ncloud.com"
#endif

#define APPSAFER_FLAG_CHECK_ROOTING                0x01u // 1,  1 << 0
#define APPSAFER_FLAG_CHECK_EMULATOR               0x02u // 2,  1 << 1
#define APPSAFER_FLAG_CHECK_DEBUGGING              0x04u // 4,  1 << 2
#define APPSAFER_FLAG_CHECK_MEMORY_TAMPERED        0x08u // 8,  1 << 3
#define APPSAFER_FLAG_CHECK_SPEEDHACK              0x10u // 16, 1 << 4
#define APPSAFER_FLAG_CHECK_APP_INTEGRITY_TAMPERED 0x20u // 32, 1 << 5
#define APPSAFER_FLAG_CHECK_UNAUTHORIZED_SIGNATURE 0x40u // 32, 1 << 5

// COMMON RETURN VALUE
#define FAIL       -1
#define SUCCESS    0
#define SAFE       0
#define DETECT     1
#define BLOCK      2
#define BEFOREINIT 3
#define BYPASS     4

// ERROR CODE  >= 100
#define LOCAL_ERROR_GET_INFO                100
#define LOCAL_ERROR_CHECK_TAMPERING         110
#define LOCAL_STOP_SCAN                     120
#define AUTH_ERROR                          200
#define AUTH_INVALID_LOCAL_INFO             210
#define SERVER_ERROR_INTERNAL               300
#define HTTP_ERROR                          400
#define _AUTH_ERROR                         800
#define _AUTH_INVALID_LOCAL_INFO            810

@protocol AppSaferDelegate<NSObject>
@optional
- (void)appSaferDetectedJailbreak;
- (void)appSaferDetectedSimulator;
- (void)appSaferDetectedDebugging;
- (void)appSaferDetectedMemoryTampered;
@required
- (void)appSaferDidInitFinish:(int)result;
- (void)appSaferDidCheckTamperingFinish:(int)result;
@end

@interface AppSafer : NSObject
- (int)initAppSafer:(id<AppSaferDelegate>)delegate serviceCode:(NSString *)serviceCode key:(NSString *)appsaferKey;
- (int)releaseAppSafer;
- (int)checkTampering;
- (int)setUserId:(NSString*)userId;
- (NSString*)getDetectMessage;

+ (void)registerScreenProtectionObserver:(UIWindow *)window observer:(id)observer selector:(SEL) selector;
+ (void)preventScreenshot:(UIWindow *)window isInvisible:(int) isInvisible;
@end


#endif
