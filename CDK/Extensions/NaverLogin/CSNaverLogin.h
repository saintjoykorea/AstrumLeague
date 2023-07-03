//
//  CSNaverLogin.h
//  CDK
//
//  Created by WooyolJung on 2017. 12. 13.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSNaverLogin__
#define __CDK__CSNaverLogin__

#include "CSDelegate.h"

enum CSNaverLoginResult {
	CSNaverLoginResultSuccess,
	CSNaverLoginResultCancelled,
	CSNaverLoginResultError
};

class CSNaverLoginDelegate {
public:
	virtual void onNaverLogin(CSNaverLoginResult result) = 0;
};

class CSNaverLogin {
public:
    static void initialize();
    static void finalize();
    static void setDelegate(CSNaverLoginDelegate* delegate);

    static void login();
    static void logout();
    static bool isConnected();
	static const char* accessToken();
};

#endif
