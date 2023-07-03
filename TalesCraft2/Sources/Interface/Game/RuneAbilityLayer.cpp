//
//  RuneAbilityLayer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 6. 13..
//  Copyright © 2018년 brgames. All rights reserved.
//
#include "RuneAbilityLayer.h"

#include "Map.h"

RuneAbilityLayer::RuneAbilityLayer(const Rune* rune, const CSVector2& position, bool train) : _rune(retain(rune)), _position(position), _train(train) {
	setTransition(CSLayerTransitionFade);
	setTransitionDuration(0.1f);
	setTimeoutInterval(1.0f);

	_pane = CSScrollPane::scrollPane();
	_pane->scroll.setBarColor(CSColor::White);
	_pane->scroll.setBarThickness(5);
	_pane->OnDrawContent.add(this, &RuneAbilityLayer::onDrawPaneContent);
	addLayer(_pane);

	if (_train) {
		_trainConfirmButton = CSButton::button();
		_trainConfirmButton->OnDraw.add(this, &RuneAbilityLayer::onDrawTrainConfirm);
		_trainConfirmButton->OnTouchesEnded.add(this, &RuneAbilityLayer::onTouchesEndedTrainConfirm);
		addLayer(_trainConfirmButton);

		_trainCancelButton = CSButton::button();
		_trainCancelButton->OnDraw.add(this, &RuneAbilityLayer::onDrawTrainCancel);
		_trainCancelButton->OnTouchesEnded.add(this, &RuneAbilityLayer::onTouchesEndedTrainCancel);
		addLayer(_trainCancelButton);
	}

	resetFrame();
}

RuneAbilityLayer::~RuneAbilityLayer() {
	_rune->release();
	_values->release();
	_descriptions->release();
}

void RuneAbilityLayer::resetFrame() {
	uint align = 0;
	if (_position.x > ProjectionWidth / 2) {
		align |= CSAlignRight;
	}
	if (_position.y > ProjectionHeight / 2) {
		align |= CSAlignBottom;
	}
	retain(_values, _rune->abilityValues());
	retain(_descriptions, _rune->abilityDescriptions());
	float h = 5 + ((_values->count() + 1) / 2) * 48;

	foreach(const AbilityDescription&, desc, _descriptions) {
		CSSize size = CSGraphics::stringSize(desc.value, Asset::sharedAsset()->boldExtraSmallFont, 420);
		h += size.height + 30;
	}

	_pane->setContentHeight(h);

	if (h > 310) h = 310;

	_pane->setFrame(CSRect(5, 95, 430, h));

	h += 100;

	if (_train) {
		_trainConfirmButton->setFrame(CSRect(20, h, 190, 50));
		_trainCancelButton->setFrame(CSRect(230, h, 190, 50));
		h += 70;
	}

	CSVector2 p = _position;
	CSGraphics::applyAlign(p, CSSize(440, h), (CSAlign)align);
	setFrame(CSRect(p.x, p.y, 440, h));
}

void RuneAbilityLayer::onDraw(CSGraphics* graphics) {
	graphics->setColor(CSColor(0, 0, 0, 160));
	graphics->drawRoundRect(bounds(), 10, true);

	const RuneData& data = _rune->data();

	graphics->setColor(CSColor::White);
	graphics->drawImageScaled(Asset::sharedAsset()->image(data.smallIcon), CSVector2(50, 50), 0.8f, CSAlignCenterMiddle);
	
	float x = 375;
	if (data.resources[1]) {
		graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameGas), CSVector2(x, 30), CSAlignCenterMiddle);
		x -= 70;
	}
	if (data.resources[0]) {
		graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameMineral), CSVector2(x, 30), CSAlignCenterMiddle);
	}

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->setStrokeWidth(2);
	graphics->drawStringScaled(data.name, CSVector2(100, 30), CSAlignMiddle, 170);

	x = 410;
	if (data.resources[1]) {
		graphics->drawDigit(data.resources[1], false, CSVector2(x, 30), CSAlignCenterMiddle);
		x -= 70;
	}
	if (data.resources[0]) {
		graphics->drawDigit(data.resources[0], false, CSVector2(x, 30), CSAlignCenterMiddle);
	}

	if (_rune->force) {
		graphics->setStrokeWidth(0);

		x = 118;

		int i = 0;
		foreach(const Card*, card, _rune->force->cards()) {
			const SkinData* subdata = card->skinData();
			if (subdata) {
				graphics->setSaturation(_rune->match(card) ? 1 : 0);
				graphics->drawImageScaled(Asset::sharedAsset()->image(subdata->smallIcon), CSVector2(x, 72), 0.37f, CSAlignCenterMiddle);
				x += 42;
				if (++i > CardSlots) break;
			}
		}
	}
}

void RuneAbilityLayer::onDrawPaneContent(CSLayer* layer, CSGraphics* graphics) {
	float from = _pane->scroll.position().y;
	float to = from + _pane->height();

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->setStrokeWidth(2);

	float x = 5;
	float y = 5;
	for (int i = 0; i < _values->count(); i++) {
		if (y < to && y + 48 > from) {
			const AbilityDescription& value = _values->objectAtIndex(i);

			graphics->setColor(CSColor::Orange);
			graphics->drawString(value.name, CSVector2(x, y + 12), CSAlignMiddle);
			graphics->setColor(CSColor::White);
			graphics->drawString(value.value, CSVector2(x, y + 36), CSAlignMiddle);
		}
		if (i & 1) {
			x = 5;
			y += 48;
		}
		else x = 215;
	}
	if (_values->count() & 1) {
		y += 48;
	}

	for (int i = 0; i < _descriptions->count(); i++) {
		const AbilityDescription& desc = _descriptions->objectAtIndex(i);

		float h = graphics->stringSize(desc.value, 420).height;

		if (y < to && y + h + 24 > from) {
			graphics->setColor(CSColor::Orange);
			graphics->drawString(desc.name, CSVector2(5, y + 12), CSAlignMiddle);
			graphics->setColor(CSColor::White);
			graphics->drawString(desc.value, CSRect(5, y + 24, 420, h));
		}
		y += h + 30;
	}
}

void RuneAbilityLayer::onTouchesEnded() {
	OnConfirm(this, false);

	removeFromParent(true);
}

void RuneAbilityLayer::onTouchesCancelled() {
	OnConfirm(this, false);

	removeFromParent(true);
}

void RuneAbilityLayer::onTimeout() {
	resetFrame();
}

void RuneAbilityLayer::onDrawTrainConfirm(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonOrangeRect_12_60), layer->bounds());
	graphics->setFont(Asset::sharedAsset()->boldMediumFont);
	graphics->setStrokeWidth(2);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonConfirm), layer->centerMiddle(), CSAlignCenterMiddle);
}

void RuneAbilityLayer::onTouchesEndedTrainConfirm(CSLayer* layer) {
	OnConfirm(this, true);

	removeFromParent(true);
}

void RuneAbilityLayer::onDrawTrainCancel(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetCommon, ImageCommonLightBrownRect_12_50), layer->bounds());
	graphics->setFont(Asset::sharedAsset()->boldMediumFont);
	graphics->setStrokeWidth(2);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonCancel), layer->centerMiddle(), CSAlignCenterMiddle);
}

void RuneAbilityLayer::onTouchesEndedTrainCancel(CSLayer* layer) {
	OnConfirm(this, false);

	removeFromParent(true);
}