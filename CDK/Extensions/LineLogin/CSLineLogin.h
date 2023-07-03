//
//  CSLineLogin.h
//  CDK
//
//  Created by 김수현 on 2019. 02. 08.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSLineLogin__
#define __CDK__CSLineLogin__

#include "CSDelegate.h"

enum CSLineLoginResult {
	CSLineLoginResultSuccess,
	CSLineLoginResultCancelled,
	CSLineLoginResultError
};

class CSLineLoginDelegate {
public:
	virtual void onLineLogin(CSLineLoginResult result) = 0;
};

class CSLineLogin {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSLineLoginDelegate* delegate);
    static void login();
    static void logout();
    static bool isConnected();
	static const char* userId();
    static const char* pictureUrl();
};

#endif

