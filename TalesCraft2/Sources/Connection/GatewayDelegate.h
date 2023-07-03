//
//  GatewayDelegate.h
//  TalesCraft2
//
//  Created by ChangSun on 2015. 5. 18..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef GatewayDelegate_h
#define GatewayDelegate_h

#include "Account.h"

class PVPGatewayDelegate {
protected:
    inline virtual ~PVPGatewayDelegate() {
    }
public:
    inline virtual void onPVPConnect() {}
    inline virtual void onPVPShutdown() {}
    inline virtual void onPVPLogin() {}
    inline virtual void onPVPPartyCreate() {}
    inline virtual void onPVPPartyEnter() {}
    inline virtual void onPVPPartyEnterNotify(const User* user) {}
    inline virtual void onPVPPartyEnterFail() {}
    inline virtual void onPVPPartyExit() {}
    inline virtual void onPVPPartyExitNotify(const User* user) {}
    inline virtual void onPVPPartyChangeHost() {}
    inline virtual void onPVPPartyMap() {}
    inline virtual void onPVPPartySlot(int index) {}
    inline virtual void onPVPPartyBan() {}
    inline virtual void onPVPPartyStart() {}
    inline virtual void onPVPPartyStopNotify(const User* user) {}
    inline virtual void onPVPPartyChat(const User* user, const CSString* msg) {}
    inline virtual void onPVPMatchCreate() {}
    inline virtual void onPVPMatchEnter() {}
    inline virtual void onPVPMatchEnterPlayerNotify(const Player* user) {}
    inline virtual void onPVPMatchEnterObserverNotify(const User* user) {}
    inline virtual void onPVPMatchEnterFail() {}
    inline virtual void onPVPMatchExit() {}
    inline virtual void onPVPMatchExitPlayerNotify(const Player* user) {}
    inline virtual void onPVPMatchExitObserverNotify(const User* user) {}
    inline virtual void onPVPMatchChangeHost() {}
    inline virtual void onPVPMatchLevelCorrection() {}
    inline virtual void onPVPMatchObserverDelay() {}
    inline virtual void onPVPMatchSlot(int index) {}
    inline virtual void onPVPMatchQuit(const User* user) {}
    inline virtual void onPVPMatchBan() {}
    inline virtual void onPVPMatchSwap(int index) {}
    inline virtual void onPVPMatchSwapToObserver(const User* user) {}
    inline virtual void onPVPMatchSwapFromObserver(const User* user) {}
    inline virtual void onPVPMatchReady() {}
    inline virtual void onPVPMatchStart() {}
    inline virtual void onPVPMatchSync() {}
    inline virtual void onPVPMatchChat(const User* user, const CSString* msg) {}
    inline virtual void onPVPMatchEmoticon(const User* user, const EmoticonIndex& index) {}
    inline virtual void onPVPMatchCommand(int framePerStep, uint frame, float serverDelay, const void* data, uint length) {}
	inline virtual void onPVPMatchPause(const User* user, bool on) {}
};

#endif /* GatewayDelegate_h */

