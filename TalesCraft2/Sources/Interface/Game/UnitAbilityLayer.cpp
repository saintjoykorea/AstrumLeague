//
//  UnitAbilityLayer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 6. 13..
//  Copyright © 2018년 brgames. All rights reserved.
//
#include "UnitAbilityLayer.h"

UnitAbilityLayer::UnitAbilityLayer(const CSObject* tag, const UnitBase* unit, const CSVector2& position) : _tag(retain(tag)), _unit(unit), _position(position) {
	setTransition(CSLayerTransitionFade);
	setTransitionDuration(0.1f);
	setTimeoutInterval(1.0f);

	_pane = CSScrollPane::scrollPane();
	_pane->scroll.setBarColor(CSColor::White);
	_pane->scroll.setBarThickness(5);
	_pane->OnDrawContent.add(this, &UnitAbilityLayer::onDrawPaneContent);
	_pane->OnTouchesEnded.add(this, &UnitAbilityLayer::onTouchesEndedPane);
	addLayer(_pane);
	
	resetFrame();
}

UnitAbilityLayer::~UnitAbilityLayer() {
	_tag->release();
    release(_values);
    _descriptions->release();
}

void UnitAbilityLayer::resetFrame() {
    uint align = 0;
    if (_position.x > ProjectionWidth / 2) {
        align |= CSAlignRight;
    }
    if (_position.y > ProjectionHeight / 2) {
        align |= CSAlignBottom;
    }

	retain(_values, _unit->abilityValues());
	float h = ((_values->count() + 1) / 2) * 48 + 45;

	retain(_descriptions, _unit->abilityDescriptions());
	foreach (const AbilityDescription&, desc, _descriptions) {
        CSSize size = CSGraphics::stringSize(desc.value, Asset::sharedAsset()->boldExtraSmallFont, 420);
        h += size.height + 30;
    }

	_pane->setContentHeight(h);
	
	if (h > 490) h = 490;
	
	_pane->setFrame(CSRect(5, 5, 430, h));

	h += 10;

    CSVector2 p = _position;
    CSGraphics::applyAlign(p, CSSize(440, h), (CSAlign)align);
    setFrame(CSRect(p.x, p.y, 440, h));
}

void UnitAbilityLayer::onDraw(CSGraphics* graphics) {
	graphics->setColor(CSColor(0, 0, 0, 160));
	graphics->drawRoundRect(bounds(), 10, true);
}

void UnitAbilityLayer::onDrawPaneContent(CSLayer* layer, CSGraphics* graphics) {
	float from = _pane->scroll.position().y;
	float to = from + _pane->height();

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->setStrokeWidth(2);

    float x = 5;
    float y = 5;

	const UnitData& data = _unit->data();

	for (int i = 0; i < _values->count(); i++) {
		if (y < to && y + 48 > from) {
			const AbilityDescription& value = _values->objectAtIndex(i);

			graphics->setColor(CSColor::Orange);
			graphics->drawString(value.name, CSVector2(x, y + 12), CSAlignMiddle);
			graphics->setColor(CSColor::White);
			graphics->drawString(value.value, CSVector2(x, y + 36), CSAlignMiddle);
		}
		if (i & 1){
			x = 5;
			y += 48;
		}
		else x = 215;
	}
	if (_values->count() & 1) {
		y += 48;
	}
	if (y < to && y + 40 > from) {
		if (data.attackType >= 0) {
			const DamageTypeData& subdata = Asset::sharedAsset()->attackType(data.attackType);

			graphics->setStrokeWidth(0);

			graphics->drawImageScaled(Asset::sharedAsset()->image(subdata.icon), CSVector2(15, y + 20), 0.5f, CSAlignCenterMiddle);
			graphics->setStrokeWidth(2);
			graphics->drawString(subdata.name, CSVector2(35, y + 20), CSAlignMiddle);
			x = 210;
		}
		else x = 0;

		if (data.armorType >= 0) {
			const DamageTypeData& subdata = Asset::sharedAsset()->armorType(data.armorType);

			graphics->setStrokeWidth(0);
			graphics->drawImageScaled(Asset::sharedAsset()->image(subdata.icon), CSVector2(x + 15, y + 20), 0.5f, CSAlignCenterMiddle);
			graphics->setStrokeWidth(2);
			graphics->drawString(subdata.name, CSVector2(x + 35, y + 20), CSAlignMiddle);
		}
	}
	y += 40;
    
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

void UnitAbilityLayer::onTouchesEnded() {
	removeFromParent(true);
}

void UnitAbilityLayer::onTouchesCancelled() {
	removeFromParent(true);
}

void UnitAbilityLayer::onTimeout() {
	resetFrame();
}

void UnitAbilityLayer::onDrawAttackType(CSLayer* layer, CSGraphics* graphics) {
    graphics->setColor(CSColor(0, 0, 0, 160));
    graphics->drawRoundRect(layer->bounds(), 10, true);
    graphics->setColor(CSColor::White);
    
    int attackType = _unit->data().attackType;
    
    if (attackType >= 0) {
        for (int i = 0; i < Asset::sharedAsset()->armorTypes->count(); ++i) {
            graphics->drawImage(Asset::sharedAsset()->image(Asset::sharedAsset()->armorTypes->objectAtIndex(i).icon), CSVector2(35 + (i % 2) * 200, 75 + (i / 2) * 70), CSAlignCenterMiddle);
        }
        
		graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
        graphics->setStrokeWidth(2);
        graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameAttackPower), CSVector2(20, 20), CSAlignMiddle);
        
        for (int i = 0; i < Asset::sharedAsset()->armorTypes->count(); ++i) {
            graphics->drawString(Asset::sharedAsset()->armorTypes->objectAtIndex(i).name, CSVector2(70 + (i % 2) * 200, 55 + (i / 2) * 70), CSAlignMiddle);
            float demage = Asset::sharedAsset()->damage(attackType, i) * 100;
            if (demage > 100) {
                graphics->setColor(CSColor(0, 255, 0, 255));
            }
            else if (demage < 100) {
                graphics->setColor(CSColor(255, 70, 70, 255));
            }
            graphics->drawString(CSString::cstringWithFormat("%d%%", (int)CSMath::round(demage)), CSVector2(70 + (i % 2) * 200, 85 + (i / 2) * 70), CSAlignMiddle);
            graphics->setColor(CSColor::White);
        }
    }
}

void UnitAbilityLayer::onDrawArmorType(CSLayer* layer, CSGraphics* graphics) {
    graphics->setColor(CSColor(0, 0, 0, 160));
    graphics->drawRoundRect(layer->bounds(), 10, true);
    graphics->setColor(CSColor::White);
    
    int armorType = _unit->data().armorType;
    
    if (armorType >= 0) {
        for (int i = 0; i < Asset::sharedAsset()->attackTypes->count(); ++i) {
            graphics->drawImage(Asset::sharedAsset()->image(Asset::sharedAsset()->attackTypes->objectAtIndex(i).icon), CSVector2(35 + (i % 2) * 200, 75 + (i / 2) * 70), CSAlignCenterMiddle);
        }
        
        graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
        graphics->setStrokeWidth(2);
        graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameDefensePower), CSVector2(20, 20), CSAlignMiddle);
        
        for (int i = 0; i < Asset::sharedAsset()->attackTypes->count(); ++i) {
            graphics->drawString(Asset::sharedAsset()->attackTypes->objectAtIndex(i).name, CSVector2(70 + (i % 2) * 200, 55 + (i / 2) * 70), CSAlignMiddle);
            float demage = Asset::sharedAsset()->damage(i, armorType) * 100;
            if (demage < 100) {
                graphics->setColor(CSColor(0, 255, 0, 255));
            }
            else if (demage > 100) {
                graphics->setColor(CSColor(255, 70, 70, 255));
            }
            graphics->drawString(CSString::cstringWithFormat("%d%%", (int)CSMath::round(demage)), CSVector2(70 + (i % 2) * 200, 85 + (i / 2) * 70), CSAlignMiddle);
            graphics->setColor(CSColor::White);
        }
    }
}

static void onTouchesDamageType(CSLayer* layer) {
    layer->removeFromParent(true);
}

void UnitAbilityLayer::onTouchesEndedPane(CSLayer* layer) {
	const CSTouch* touch = layer->touch();

	if (!touch->isMoved()) {
		CSVector2 pos = touch->point(layer);

		float y = 5 + ((_values->count() + 1) / 2) * 48;

		if (pos.y >= y && pos.y <= y + 40) {
			bool attack = pos.x <= 245;

			if ((attack ? _unit->data().attackType : _unit->data().armorType) >= 0) {
				convertToViewSpace(&pos);

				CSLayer* descLayer = CSLayer::layer();
				descLayer->setOwner(this);
				descLayer->setTransition(CSLayerTransitionFade);
				descLayer->setFrame(CSRect(pos.x - 200, pos.y - 260, 400, 260));
				descLayer->OnDraw.add(this, attack ? &UnitAbilityLayer::onDrawAttackType : &UnitAbilityLayer::onDrawArmorType);
				descLayer->OnTouchesEnded.add(&onTouchesDamageType);
				descLayer->OnTouchesCancelled.add(&onTouchesDamageType);
				view()->addLayer(descLayer);
			}
		}
		else {
			removeFromParent(true);
		}
	}
}
