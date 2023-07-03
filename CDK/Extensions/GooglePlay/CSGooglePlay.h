//
//  CSGooglePlay.h
//  CDK
//
//  Created by WooyolJung on 2017. 12. 07.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSGooglePlay__
#define __CDK__CSGooglePlay__

#include "CSDelegate.h"

enum CSGooglePlayLoginResult {
	CSGooglePlayLoginResultSuccess,
	CSGooglePlayLoginResultCancelled,
	CSGooglePlayLoginResultError
};

class CSGooglePlayDelegate {
public:
	virtual void onGooglePlayLogin(CSGooglePlayLoginResult result) = 0;
};

class CSGooglePlay {
public:
    static void initialize();
    static void finalize();

	static void setDelegate(CSGooglePlayDelegate* delegate);
	static void login();
	static void logout();
	static bool isConnected();
	static const char* userId();
	static const char* email();
	static const char* pictureUrl();
    static void reportAchievementsSteps(const char* id, int value);
    static void reportAchievementsIncrement(const char* id, int value);
    static void showAchievements();
    static void reportLeaderboards(int score);
    static void showLeaderboards();
	static void showReview();
};

#endif
