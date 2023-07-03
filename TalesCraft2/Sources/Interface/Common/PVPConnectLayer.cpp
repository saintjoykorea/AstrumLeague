//
//  PVPConnectLayer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 8. 2..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "PVPConnectLayer.h"

#include "PVPGateway.h"

PVPConnectLayer::PVPConnectLayer() {
    _action.code = ActionNone;
    
    init();
}
PVPConnectLayer::PVPConnectLayer(ConnectParty_ param, const CSString* partyId) {
    _action.code = ActionParty;
    _action.context.party.partyId = retain(partyId);
    
    init();
}
PVPConnectLayer::PVPConnectLayer(ConnectMatch_ param, uint matchId, uint frame, bool asPlayer) {
    _action.code = ActionMatch;
    _action.context.match.matchId = matchId;
    _action.context.match.frame = frame;
    _action.context.match.asPlayer = asPlayer;
    
    init();
}

void PVPConnectLayer::init() {
    setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
    setTransition(CSLayerTransitionFade);
    
    _animation = new Animation(AnimationSetCommon, AnimationCommonLoading);
    _animation->setClientPosition(CSVector2(centerMiddle()));
}

PVPConnectLayer::~PVPConnectLayer() {
    switch (_action.code) {
        case ActionParty:
            _action.context.party.partyId->release();
            break;
    }
    _animation->release();
}

PVPConnectLayer* PVPConnectLayer::layer() {
    return autorelease(new PVPConnectLayer());
}

PVPConnectLayer* PVPConnectLayer::layerForParty(const CSString* partyId) {
    return autorelease(new PVPConnectLayer(ConnectParty, partyId));
}

PVPConnectLayer* PVPConnectLayer::layerForMatch(uint matchId, uint frame, bool asPlayer) {
    return autorelease(new PVPConnectLayer(ConnectMatch, matchId, frame, asPlayer));
}

void PVPConnectLayer::onLink() {
    PVPGateway::sharedGateway()->addDelegate(this);

    switch (_action.code) {
        case ActionNone:
            if (PVPGateway::sharedGateway()->connState() == PVPConnStateLogin) {
                OnComplete(this, PVPConnectResultSuccess);
                
                removeFromParent(true);
            }
            else if (PVPGateway::sharedGateway()->connState() == PVPConnStateShutdown) {
                PVPGateway::sharedGateway()->requestConnect();
            }
            break;
        case ActionParty:
            if (PVPGateway::sharedGateway()->connState() == PVPConnStateLogin) {
                _action.context.party.entering = true;
                PVPGateway::sharedGateway()->requestPartyEnter(*_action.context.party.partyId);
            }
            else if (PVPGateway::sharedGateway()->connState() == PVPConnStateShutdown) {
                PVPGateway::sharedGateway()->requestConnect();
            }
            break;
        case ActionMatch:
            if (PVPGateway::sharedGateway()->connState() == PVPConnStateLogin) {
                _action.context.match.entering = true;
                PVPGateway::sharedGateway()->requestMatchEnter(_action.context.match.matchId, _action.context.match.frame, _action.context.match.asPlayer);
            }
            else if (PVPGateway::sharedGateway()->connState() == PVPConnStateShutdown) {
                PVPGateway::sharedGateway()->requestConnect();
            }
            break;
    }
}

void PVPConnectLayer::onUnlink() {
    PVPGateway::sharedGateway()->removeDelegate(this);
    
    if (PVPGateway::sharedGateway()->isConnected()) {
        switch (_action.code) {
            case ActionParty:
                if (_action.context.party.entering) {
                    PVPGateway::sharedGateway()->requestPartyExit();
                }
                break;
            case ActionMatch:
                if (_action.context.match.entering) {
                    PVPGateway::sharedGateway()->requestMatchExit();
                }
                break;
        }
    }
}

void PVPConnectLayer::onTimeout() {
    _animation->update(timeoutInterval());
    
    refresh();
}

void PVPConnectLayer::onDraw(CSGraphics* graphics) {
    graphics->setColor(CSColor::TranslucentBlack);
    graphics->drawRect(bounds(), true);
    graphics->setColor(CSColor::White);
    _animation->draw(graphics); 
}


void PVPConnectLayer::onPVPConnect() {
    bool channelEnter = false;
    
    switch (_action.code) {
        case ActionNone:
		case ActionParty:
            channelEnter = true;
            break;
    }
    PVPGateway::sharedGateway()->requestLogin(channelEnter);
}

void PVPConnectLayer::onPVPShutdown() {
    OnComplete(this, PVPConnectResultShutdown);
    
    removeFromParent(true);
}

void PVPConnectLayer::onPVPLogin() {
    switch (_action.code) {
        case ActionNone:
            OnComplete(this, PVPConnectResultSuccess);
            
            removeFromParent(true);
            break;
        case ActionParty:
            _action.context.party.entering = true;
            PVPGateway::sharedGateway()->requestPartyEnter(*_action.context.party.partyId);
            break;
        case ActionMatch:
            _action.context.match.entering = true;
            PVPGateway::sharedGateway()->requestMatchEnter(_action.context.match.matchId, _action.context.match.frame, _action.context.match.asPlayer);
            break;
    }
}

void PVPConnectLayer::onPVPPartyEnter() {
    if (_action.code == ActionParty) {
        _action.context.party.entering = false;
        
        OnComplete(this, PVPConnectResultSuccess);
        
        removeFromParent(true);
    }
}

void PVPConnectLayer::onPVPPartyEnterFail() {
    if (_action.code == ActionParty) {
        _action.context.party.entering = false;
        
        OnComplete(this, PVPConnectResultFail);
        
        removeFromParent(true);
    }
}

void PVPConnectLayer::onPVPMatchEnter() {
    if (_action.code == ActionMatch) {
        _action.context.match.entering = false;
        
        OnComplete(this, PVPConnectResultSuccess);
        
        removeFromParent(true);
    }
}

void PVPConnectLayer::onPVPMatchEnterFail() {
    if (_action.code == ActionMatch) {
        _action.context.match.entering = false;
        
        OnComplete(this, PVPConnectResultFail);
        
        removeFromParent(true);
    }
}

