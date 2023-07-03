//
//  CSAppleLogin.h
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSAppleLogin__
#define __CDK__CSAppleLogin__

#include "CSDelegate.h"

enum CSAppleLoginResult {
	CSAppleLoginResultSuccess,
	CSAppleLoginResultCancelled,
	CSAppleLoginResultError
};

class CSAppleLoginDelegate {
public:
	virtual void onAppleLogin(CSAppleLoginResult result) = 0;
};

class CSAppleLogin {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSAppleLoginDelegate* delegate);
    static void login();
	static bool isConnected();
	static const char* userId();
	static const char* email();
    static bool isEnabled();
};

#endif
