//
//  CSPopupCoverLayer.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 13..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSPopupCoverLayer.h"

#include "CSView.h"

CSPopupCoverLayer::CSPopupCoverLayer(CSLayer* contentLayer, float darkness, float blur) {
    CSAssert(darkness >= 0 && darkness <= 1 && blur >= 0, "invalid operation");
    _contentLayer = retain(contentLayer);
    _darkness = darkness;
    _blur = blur;
    setOrder(contentLayer->order());
    
    _contentLayer->OnStateChanged.add(this, &CSPopupCoverLayer::onStateChangedContent);
}

CSPopupCoverLayer::~CSPopupCoverLayer() {
    _contentLayer->OnStateChanged.remove(this);
    _contentLayer->release();
}

void CSPopupCoverLayer::onStateChanged() {
    if (_state == CSLayerStateAttach) {
        CSView* view = this->view();
        setFrame(CSRect(0, 0, view->projectionWidth(), view->projectionHeight()));
        view->insertLayer(view->layers()->indexOfObjectIdenticalTo(this) + 1, _contentLayer);
    }
    else if (_state == CSLayerStateDetach) {
        _contentLayer->removeFromParent(true);
    }
    else if (_state == CSLayerStateRemoved) {
        _contentLayer->removeFromParent(false);
    }
}

void CSPopupCoverLayer::onProjectionChanged() {
    setFrame(CSRect(0, 0, view()->projectionWidth(), view()->projectionHeight()));
}

void CSPopupCoverLayer::onStateChangedContent(CSLayer* layer) {
    if (layer->state() == CSLayerStateRemoved) {
        removeFromParent(false);
    }
}

void CSPopupCoverLayer::onDraw(CSGraphics* graphics) {
    if (_blur) {
        graphics->blur(bounds(), _blur * _contentLayer->transitionProgress());
    }
    if (_darkness) {
        graphics->setColor(CSColor(0.0f, 0.0f, 0.0f, _darkness * _contentLayer->transitionProgress()));
        graphics->drawRect(bounds(), true);
    }
}
