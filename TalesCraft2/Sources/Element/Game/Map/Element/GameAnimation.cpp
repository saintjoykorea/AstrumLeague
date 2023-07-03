//
//  GameAnimation.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 10. 18..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "GameAnimation.h"

#include "Map.h"

GameAnimation::GameAnimation(const byte*& cursor) {
    animation = new Animation(AnimationIndex(cursor), SoundControlEffect, false);
    positions[0].layerIndex = GameLayerIndex(cursor);
    positions[0].offset = CSVector2(cursor);
    positions[1].layerIndex = GameLayerIndex(cursor);
    positions[1].offset = CSVector2(cursor);

	for (int i = 0; i < 2; i++) {
		switch (positions[i].layerIndex.indices[0]) {
			case GameLayerKeyNavigation:
			case GameLayerKeyNumbering:
			case GameLayerKeySetNumbering:
			case GameLayerKeyMainCommand:
			case GameLayerKeySubCommand:
			case GameLayerKeyGroupCommand:
			case GameLayerKeySpawn:
			case GameLayerKeyMenu:
			case GameLayerKeyWarning:
				positions[i].width = 100;
				break;
			case GameLayerKeySelectAttackers:
				positions[i].width = 180;
				break;
			case GameLayerKeySelectBarracks:
			case GameLayerKeySelectLabors:
				positions[i].width = 140;
				break;
		}
	}
	scale = readFloat(cursor);
    animation->setClientScale(scale);
    animation->setClientRotation(CSQuaternion::rotationAxis(CSVector3::UnitZ, CSMath::degreesToRadians(readFloat(cursor))));
    visible = false;
}

GameAnimation::GameAnimation(const AnimationIndex& index, const GameLayerIndex& layer0, const CSVector2& offset0, const GameLayerIndex& layer1, const CSVector2& offset1) {
    animation = new Animation(index, SoundControlEffect, false);
    positions[0].layerIndex = layer0;
    positions[0].offset = offset0;
    positions[1].layerIndex = layer1;
    positions[1].offset = offset1;
    visible = false;
}

GameAnimation::~GameAnimation() {
    animation->release();
}
