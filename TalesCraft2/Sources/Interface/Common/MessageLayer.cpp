//
//  MessageLayer.cpp
//  TalesCraft2
//
//  Created by Jae Hong Kim on 2014. 11. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "MessageLayer.h"

#include "Asset.h"

#include "SoundButton.h"

#define SHORT_FRAME_HEIGHT 180

MessageLayer::MessageLayer(const CSString* message, const CSString* caption, MessageLayerType type) :
    _type(type),
    _message(retain(message)),
    _caption(retain(caption))
{
	float strHeight = CSGraphics::stringSize(message, Asset::sharedAsset()->boldMediumFont, 600).height;
	_shortHeight = strHeight <= SHORT_FRAME_HEIGHT;

    setFrame(CSRect((ProjectionWidth - 640) / 2, (ProjectionHeight - (_shortHeight ? 360 : 500)) / 2, 640, (_shortHeight ? 360 : 500)));
    setTransition(CSLayerTransitionPop | CSLayerTransitionFade);
    setOrder(2);
    
    if (_type == MessageLayerTypeConfirm){
        SoundButton* confirmButton = SoundButton::button(SoundFxClick);
        confirmButton->setFrame(CSRect(182, frame().size.height - 78, 276, 60));
        confirmButton->OnDraw.add(this, &MessageLayer::onDrawConfirm);
        confirmButton->OnTouchesEnded.add(this, &MessageLayer::onTouchesEndedConfirm);
        addLayer(confirmButton);
        
        SoundButton* closeButton = SoundButton::button(SoundFxClick);
        closeButton->setFrame(CSRect(559, -10, 83, 68));
        closeButton->OnDraw.add(this, &MessageLayer::onDrawClose);
        closeButton->OnTouchesEnded.add(this, &MessageLayer::onTouchesEndedCancel);
        addLayer(closeButton);
    }
    else {
        SoundButton* closeButton = SoundButton::button(SoundFxClick);
        closeButton->setFrame(CSRect(559, -10, 83, 68));
        closeButton->OnDraw.add(this, &MessageLayer::onDrawClose);
        closeButton->OnTouchesEnded.add(this, &MessageLayer::onTouchesEndedCancel);
        addLayer(closeButton);
        
        SoundButton* cancelButton = SoundButton::button(SoundFxClick);
        cancelButton->setFrame(CSRect(29, frame().size.height - 78, 276, 60));
        cancelButton->OnDraw.add(this, &MessageLayer::onDrawCancel);
        cancelButton->OnTouchesEnded.add(this, &MessageLayer::onTouchesEndedCancel);
        addLayer(cancelButton);
        
        SoundButton* confirmButton = SoundButton::button(SoundFxClick);
        confirmButton->setFrame(CSRect(335, frame().size.height - 78, 276, 60));
        switch (_type) {
            case MessageLayerTypeConfirmCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawConfirm);
                break;
            case MessageLayerTypeDenyCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawDeny);
                break;
            case MessageLayerTypeRemoveCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawRemove);
                break;
            case MessageLayerTypeSaveCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawSave);
                break;
            case MessageLayerTypeMoveCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawMove);
                break;
            case MessageLayerTypeReconnectCancel:
                confirmButton->OnDraw.add(this, &MessageLayer::onDrawReconnect);
                break;
        }
        confirmButton->OnTouchesEnded.add(this, &MessageLayer::onTouchesEndedConfirm);
        addLayer(confirmButton);
    }
}

MessageLayer::~MessageLayer() {
    release(_message);
    release(_caption);
}

void MessageLayer::onBackKey() {
    if (OnBackKey.exists()) {
        OnBackKey(this);
    }
    else if (!_shouldConfirm) {
        OnConfirm(this, false);
        removeFromParent(true);
    }
}

void MessageLayer::onDraw(CSGraphics *graphics) {
	float ly = frame().size.height;

    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonPopupTopFrame), CSRect(0, 0, 640, 50));
    
    graphics->setColor(CSColor(40, 60, 80, 255));
    graphics->drawRect(CSRect(0, 50, 640, ly - 140), true);
    graphics->setColor(CSColor(60, 90, 130, 255));
    graphics->drawLine(CSVector2(0, ly - 91), CSVector2(640, ly - 91));
    graphics->setColor(CSColor(10, 20, 30, 255));
    graphics->drawRect(CSRect(0, ly - 90, 640, 90), true);

    graphics->setColor(CSColor::White);
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonInnerLightFrame), CSRect(10, 60, 620, _shortHeight ? 200 : 334));
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonBlackGrayLine_3_6), CSRect(0, ly - 6, 640, 6), CSRect(1, 0, 0, 6));
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);    
    graphics->setStrokeWidth(2);
    if (_caption) {
        graphics->drawString(_caption, CSVector2(320, 24), CSAlignCenterMiddle);
    }
    
    graphics->drawStringScaled(_message, CSRect(20, 70, 600, _shortHeight ? 180 : 314), CSAlignCenterMiddle);
}

void MessageLayer::onTouchesCancelled() {
	if (!_shouldConfirm) {
		OnConfirm(this, false);
		removeFromParent(true);
	}
}

void MessageLayer::onDrawCancel(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonLightBrownRect_12_50), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonCancel), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onTouchesEndedCancel(CSLayer *layer) {
    OnConfirm(this, false);
    removeFromParent(true);
}

void MessageLayer::onDrawConfirm(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonOrangeRect_12_60), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonConfirm), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onTouchesEndedConfirm(CSLayer *layer) {
    OnConfirm(this, true);
    removeFromParent(true);
}

void MessageLayer::onDrawDeny(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonRedRect_276_60), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonDeny), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onDrawRemove(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonRedRect_276_60), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonRemove), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onDrawSave(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonLightBlueRect_276_60), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonSave), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onDrawMove(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonLightBlueRect_276_60), layer->bounds());
    
    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonMove), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onDrawReconnect(CSLayer* layer, CSGraphics* graphics) {
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonOrangeRect_12_60), layer->bounds());

    graphics->setFont(Asset::sharedAsset()->boldMediumFont);
    graphics->setStrokeColor(CSColor(0, 20, 60, 255));
    graphics->setStrokeWidth(2);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonReconnect), layer->centerMiddle(), CSAlignCenterMiddle);
}

void MessageLayer::onDrawClose(CSLayer* layer, CSGraphics* graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonClose), layer->centerMiddle(), CSAlignCenterMiddle);
}
