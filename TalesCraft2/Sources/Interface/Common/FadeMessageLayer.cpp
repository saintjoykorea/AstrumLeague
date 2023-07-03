//
//  FadeMessageLayer.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2016. 6. 21..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "FadeMessageLayer.h"

#include "Asset.h"

#define DisplayDuration		1.5f
#define TransitionDuration	0.25f

FadeMessageLayer* FadeMessageLayer::__layer = NULL;

FadeMessageLayer::FadeMessageLayer(const CSString* message) : _message(retain(message)) {
	setFrame(CSRect(40, 40, ProjectionWidth - 80, ProjectionHeight - 80));
	setTransition(CSLayerTransitionFade);
	setTransitionDuration(TransitionDuration);
	setTimeoutInterval(DisplayDuration);
	setEnabled(false);
	setOrder(4);
}

FadeMessageLayer::~FadeMessageLayer() {
	_message->release();

	if (__layer == this) __layer = NULL;
}

void FadeMessageLayer::onTimeout() {
	removeFromParent(true);
}

void FadeMessageLayer::onDraw(CSGraphics* graphics) {
	graphics->setFont(Asset::sharedAsset()->boldLargeFont);
	graphics->setStrokeWidth(2);
	graphics->drawString(_message, bounds(), CSAlignCenterMiddle);
}

void FadeMessageLayer::show(CSView* view, const CSString* message) {
	if (__layer) __layer->removeFromParent(false);
	__layer = new FadeMessageLayer(message);
	view->addLayer(__layer);
	__layer->release();
}