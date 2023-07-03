//
//  CSFacebook.h
//  CDK
//
//  Created by ChangSun on 2017. 11. 22..
//Copyright © 2017년 brgames. All rights reserved.
//

#ifndef CSFacebook_h
#define CSFacebook_h

#include "CSDelegate.h"

enum CSFacebookLoginResult {
	CSFacebookLoginResultSuccess,
	CSFacebookLoginResultCancelled,
	CSFacebookLoginResultError
};

class CSFacebookDelegate {
public:
	virtual void onFacebookLogin(CSFacebookLoginResult result) = 0;
};

class CSFacebook {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSFacebookDelegate* delegate);
    static void login();
    static void logout();
    static bool isConnected();
	static const char* userId();
    static const char* pictureUrl();
};

#endif /* CSFacebook_h */
