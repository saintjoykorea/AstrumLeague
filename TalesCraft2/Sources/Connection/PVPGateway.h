//
//  PVPGateway.h
//  TalesCraft2
//
//  Created by ChangSun on 2015. 5. 18..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef PVPGateway_h
#define PVPGateway_h

#include "GatewayDelegate.h"

#include "MapThumb.h"

#define PVPProtocolVersion      1

#define PVPPacketCapacity       (4 * 1024)

enum PVPConnState : byte {
    PVPConnStateShutdown,
    PVPConnStateConnecting,
    PVPConnStateConnected,
    PVPConnStateAuthoring,
    PVPConnStateLogin
};

class PVPGateway {
private:
    CSSocket* _socket;
    CSArray<PVPGatewayDelegate*>* _delegates;
    uint _connIndex;
    uint _rPacketIndex;
    uint _sPacketIndex;
    CSBuffer* _inpkt;
    CSBuffer* _outpkt;
    uint _outpktPosition;
    PVPConnState _connState;
    
    static PVPGateway* __gateway;
    
    PVPGateway();
    ~PVPGateway();
public:
    static inline PVPGateway* sharedGateway() {
        return __gateway;
    }
    
    static void load();
    static void destroy();
    
    void addDelegate(PVPGatewayDelegate* delegate);
    void removeDelegate(PVPGatewayDelegate* delegate);

    inline PVPConnState connState() const {
        return _connState;
    }
    inline bool isConnected() const {
        return _connState == PVPConnStateLogin;
    }
    void requestConnect();
    void requestShutdown(bool clear);
    void requestLogin(bool channelEnter);
    void requestUpdateProfile();
    void requestPartyCreate(GameOnlineMode mode, int playerCount, int teamPlayerCount, int totalPlayerCount, const char* msg);
    void requestPartyEnter(const char* partyId);
    void requestPartyExit();
    void requestPartyMap(const MapIndex* map);
    void requestPartySlot(int index);
    void requestPartyInvite(const User* user);
    void requestPartyRefuse(int64 userId, bool intend);
    void requestPartyStart(bool on);
    void requestPartySync();
    void requestPartyChat(const char* msg);
	void requestPartyBack();
    void requestMatchCreate(GameOnlineMode mode, GameLevelCorrection levelCorrection, int maxCardLevel, int maxRuneLevel, int observerDelay, const MapThumb* map, const char* msg);
    void requestMatchEnter(uint matchId, uint frame, bool asPlayer);
    void requestMatchExit();
    void requestMatchLevelCorrection(GameLevelCorrection levelCorrection);
    void requestMatchObserverDelay(int observerDelay);
    void requestMatchSlot(int index, GameControl control);
    void requestMatchQuit(int64 userId);
    void requestMatchInvite(const User* user);
    void requestMatchRefuse(int64 userId, bool intend);
    void requestMatchSwap(int64 userId, int toIndex);
    void requestMatchReady();
    void requestMatchStart();
    void requestMatchSync();
    void requestMatchChat(const char* msg, bool alliance);
    void requestMatchCommand(const void* data, uint length);
    void requestMatchLatency(int framePerSecond, int bottleneck, float totalLatency, uint totalLatencyCount);
    void requestMatchAuto(bool on);
	void requestMatchPause(bool on);
	void requestMatchResult(GameResult result);
    void requestPing();
private:
    void receiveShutdown();
    void receiveLogin();
    void receivePartyCreate();
    void receivePartyEnter();
    void receivePartyExit();
    void receivePartyMap();
    void receivePartySlot();
    void receivePartyStart();
	void receivePartyMatch();
    void receivePartySync();
    void receivePartyChat();
	void receivePartyBack();
    void receiveMatchCreate();
    void receiveMatchEnter();
    void receiveMatchExit();
    void receiveMatchLevelCorrection();
    void receiveMatchObserverDelay();
    void receiveMatchSlot();
    void receiveMatchQuit();
    void receiveMatchSwap();
    void receiveMatchReady();
    void receiveMatchStart();
    void receiveMatchSync();
    void receiveMatchChat();
    void receiveMatchCommand();
	void receiveMatchPause();
    
    bool beginSendPacket();
    void endSendPacket();
    void receivePacket();
    void receiveSocketEvent(CSSocket* socket, CSSocketEvent event);
    
public:
    inline void occurShutdown() {
        receiveShutdown();
    }
};

#endif /* PVPGateway_h */
