//
//  CSLineLoginImpl.h
//  TalesCraft2
//
//  Created by 엄미정 on 12/02/2019.
//  Copyright © 2019 brgames. All rights reserved.
//

#if defined(CDK_IOS) && defined(CDK_IMPL)

#ifndef CSLineLoginImpl_h
#define CSLineLoginImpl_h

#import <UIKit/UIKit.h>

NSObject<UIApplicationDelegate>* CSLineLogin_applicationDelegate();
bool CSLineLogin_initialize();
void CSLineLogin_finalize();
void CSLineLogin_login();
void CSLineLogin_logout();
bool CSLineLogin_isConnected();
const char* CSLineLogin_userId();
const char* CSLineLogin_pictureUrl();

extern void CSLineLogin_loginResultCB(int result);

#endif /* CSLineLoginImpl_h */

#endif

