//
//  PVPConnectLayer.h
//  TalesCraft2
//
//  Created by Chan Kim on 2016. 7. 27..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef PVPConnectLayer_h
#define PVPConnectLayer_h

#include "Asset.h"

#include "GatewayDelegate.h"

enum PVPConnectResult {
    PVPConnectResultSuccess,
    PVPConnectResultFail,
    PVPConnectResultShutdown
};

class PVPConnectLayer : public CSLayer, public PVPGatewayDelegate {
public:
    enum ConnectParty_ { ConnectParty };
    enum ConnectMatch_ { ConnectMatch };
private:
    enum ActionCode {
        ActionNone,
        ActionParty,
        ActionMatch
    };
    struct {
        ActionCode code;
        union {
            struct {
                const CSString* partyId;
                bool entering;
            } party;
            struct {
                uint matchId;
                uint frame;
                bool asPlayer;
                bool entering;
            } match;
        } context;
    }_action;
    
    Animation* _animation;
public:
	CSHandler<CSLayer*, PVPConnectResult> OnComplete;
public:
    PVPConnectLayer();
    PVPConnectLayer(ConnectParty_ param, const CSString* partyId);
    PVPConnectLayer(ConnectMatch_ param, uint matchId, uint frame, bool asPlayer);
private:
    void init();
private:
    ~PVPConnectLayer();
public:
    static PVPConnectLayer* layer();
    static PVPConnectLayer* layerForParty(const CSString* partyId);
    static PVPConnectLayer* layerForMatch(uint matchId, uint frame, bool asPlayer);
private:
    void onLink() override;
    void onUnlink() override;
    void onTimeout() override;
    void onDraw(CSGraphics* graphics) override;
    
    void onPVPConnect() override;
    void onPVPShutdown() override;
    void onPVPLogin() override;
    void onPVPPartyEnter() override;
    void onPVPPartyEnterFail() override;
    void onPVPMatchEnter() override;
    void onPVPMatchEnterFail() override;
};

#endif /* PVPConnectLayer_h */

