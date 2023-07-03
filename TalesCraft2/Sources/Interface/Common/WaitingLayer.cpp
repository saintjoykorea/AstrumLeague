//
//  WaitingLayer.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 22..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "WaitingLayer.h"

#include "Asset.h"

#include "PVPGateway.h"

#include "MessageLayer.h"
#include "SoundButton.h"
#include "LobbyLayer.h"

#define ExpireInterval		20

#define AppearLatency		0.5f
#define TransitionLatency	0.25f

WaitingLayer* WaitingLayer::__layer = NULL;

WaitingLayer::WaitingLayer() : _owners(new CSArray<CSLayer>(1)), _alpha(-AppearLatency / TransitionLatency) {
    setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
    setOrder(4);
    
    _animation = new Animation(AnimationSetCommon, AnimationCommonLoading);
    _animation->setClientPosition(centerMiddle());
}

WaitingLayer::~WaitingLayer() {
    _animation->release();
    _owners->release();
    
    __layer = NULL;
}

void WaitingLayer::show(CSLayer* owner, bool expire) {
    if (!__layer) __layer = autorelease(new WaitingLayer());
	__layer->_owners->addObject(owner);
	if (expire) __layer->_expire = true;
	owner->view()->addLayer(__layer);
}

bool WaitingLayer::hide(CSLayer* owner) {
	return __layer && __layer->_owners->removeObjectIdenticalTo(owner);
}

void WaitingLayer::hideAll() {
    if (__layer) __layer->_owners->removeAllObjects();
}

bool WaitingLayer::contains(CSLayer* owner) {
    return __layer && __layer->_owners->containsObjectIdenticalTo(owner);
}

void WaitingLayer::onTimeout() {
    int i = 0;
    while (i < _owners->count()) {
        CSLayer* owner = _owners->objectAtIndex(i);
        if (owner->view()) i++;
        else _owners->removeObjectAtIndex(i);
    }

	float delta = timeoutInterval();

    if (_owners->count()) {
		if (_alpha < 1) _alpha = CSMath::min(_alpha + delta / TransitionLatency, 1.0f);
		if (_alpha > 0) {
			_animation->update(delta);
			refresh();

			if (_expire && !_expireButton && timeoutSequence() * delta >= ExpireInterval) {
				_expireButton = SoundButton::button(SoundFxClick);
				_expireButton->setFrame(CSRect(ProjectionWidth / 2 - 150, ProjectionHeight - 220, 300, 60));
				_expireButton->OnDraw.add(this, &WaitingLayer::onDrawExpire);
				_expireButton->OnTouchesEnded.add(this, &WaitingLayer::onTouchesEndedExpire);
				addLayer(_expireButton);
			}
		}
    }
	else if (_alpha > 0) _alpha -= delta / TransitionLatency;
	else removeFromParent(false);
}

void WaitingLayer::onDraw(CSGraphics* graphics) {
	if (_alpha <= 0) return;

    graphics->setColor(CSColor(0.0f, 0.0f, 0.0f, _alpha * 0.5f));
    graphics->drawRect(bounds(), true);
    graphics->setColor(CSColor::White);    
    _animation->draw(graphics);
}

void WaitingLayer::onDrawExpire(CSLayer* layer, CSGraphics* graphics) {
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonRedRoundRect_19_50), layer->bounds(), 6, 6);

    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonCancel), layer->centerMiddle(), CSAlignCenterMiddle);
}

void WaitingLayer::onTouchesEndedExpire(CSLayer* layer) {
    //============================================================
    //TODO
    PVPGateway::sharedGateway()->requestShutdown(true);

    view()->clearLayers(false);
    view()->addLayer(LobbyLayer::layer());
    //============================================================

    /*
    MessageLayer* popupLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetError, MessageErrorNetworkUnstableMoveTitle), NULL, MessageLayerTypeConfirmCancel);
    popupLayer->setOwner(this);
    popupLayer->OnConfirm.add([this](CSLayer* layer, bool yes) {
        if (yes) {
            PVPGateway::sharedGateway()->requestShutdown(true);

            view()->clearLayers(false);
            view()->addLayer(DeckLayer::layer());
        }
    });
    popupLayer->setOrder(4);
    view()->addLayerAsPopup(popupLayer);
    */
}

