//
//  CSAppleGameCenter.h
//  CDK
//
//  Created by WooyolJung on 2017. 11. 22.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSAppleGameCenter__
#define __CDK__CSAppleGameCenter__

#include "CSDelegate.h"

enum CSAppleGameCenterLoginResult {
	CSAppleGameCenterLoginResultSuccess,
	CSAppleGameCenterLoginResultCancelled,
	CSAppleGameCenterLoginResultError
};

class CSAppleGameCenterDelegate {
public:
	virtual void onAppleGameCenterLogin(CSAppleGameCenterLoginResult result) = 0;
};

class CSAppleGameCenter {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSAppleGameCenterDelegate* delegate);
    static void login();
    static bool isConnected();
    static const char* playerId();
    static void reportAchievements(const char* id, uint value, uint maxValue, bool replace);
    static void showAchievements();
    static void reportLeaderboards(uint score);
    static void showLeaderboards();
};

#endif
