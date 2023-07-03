//
//  CSLineLogin.cpp
//  TalesCraft2
//
//  Created by 엄미정 on 12/02/2019.
//  Copyright © 2019 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppDelegate.h"
#import "CSLineLogin.h"
extern "C" {
#import "CSLineLoginImpl.h"
}

static CSLineLoginDelegate* __delegate = NULL;

void CSLineLogin::initialize() {
    if (CSLineLogin_initialize()) {
        [[CSAppDelegate sharedDelegate] addSubDelegate:CSLineLogin_applicationDelegate()];
    }
}

void CSLineLogin::finalize() {
    NSObject<UIApplicationDelegate>* applicationDelegate = CSLineLogin_applicationDelegate();
    
    if (applicationDelegate) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate:applicationDelegate];
    }
    
    CSLineLogin_finalize();
}

void CSLineLogin::setDelegate(LoginResultDelegate *delegate) {
    __delegate = delegate;
}

void CSLineLogin::login() {
    CSLineLogin_login();
}

void CSLineLogin::logout() {
    CSLineLogin_logout();
}

bool CSLineLogin::isConnected() {
    return CSLineLogin_isConnected();
}

const char* CSLineLogin::userId() {
    return CSLineLogin_userId();
}

const char* CSLineLogin::pictureUrl() {
    return CSLineLogin_pictureUrl();
}

void CSLineLogin_loginResultCB(int result) {
    if (__delegate) {
        __delegate->onLineLogin(result);
    }
}

#endif

