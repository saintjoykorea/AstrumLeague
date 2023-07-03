//
//  FogBuilding.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 27..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "FogBuilding.h"

#include "Map.h"

#define DisplayingDuration              0.5f

FogBuilding::FogBuilding(const Unit* unit, int alliance) :
    Fog(alliance),
    force(unit->force()),
    index(unit->index()),
    _motion(unit->captureMotion()),
    _dir(unit->motionDirection())
{
	_type = ObjectTypeFogBuilding;
	_collider = data().collider;
	_radius = data().radius;

    if (_motion) {
        _motion->setSpriteDelegate0(this, &FogBuilding::spriteElement);
    }
    locate(unit->point());
    
    Map::sharedMap()->registerFogBuilding(this);
    Map::sharedMap()->registerObject(this);
}

FogBuilding::~FogBuilding() {
    release(_motion);
}

FogBuilding* FogBuilding::fog(const Unit* unit, int alliance) {
    FogBuilding* fog = new FogBuilding(unit, alliance);
    fog->release();
    return fog;
}

const CSColor& FogBuilding::color(bool visible, bool detecting) const {
    const Force* vision = Map::sharedMap()->vision();
    
    return !force || visible || !vision || alliance != vision->alliance() ? CSColor::Transparent : force->allianceColor();
}

void FogBuilding::locate(const FPoint& point) {
    Object::locate(point);
    /*
    if (_motion) {
        _motion->setClientPosition(worldPoint(ObjectLayerBottom));
    }
	*/
	_alpha = 1.0f;
}

void FogBuilding::dispose() {
    Object::dispose();
    
    Map::sharedMap()->unregisterFogBuilding(this);
}

ObjectDisplay FogBuilding::display() const {
	const Force* vision = Map::sharedMap()->vision();

	return ObjectDisplay(_alpha && vision && vision->alliance() == alliance ? ObjectVisibleNormal : ObjectVisibleNone, 1);
}

bool FogBuilding::update(bool running) {
	Object::update(running);

	if (!_located && _alpha) {
		_alpha = CSMath::max(_alpha - Map::sharedMap()->frameDelayFloat() / DisplayingDuration, 0.0f);
	}

	return _alpha != 0.0f;
}

void FogBuilding::spriteElement(CSGraphics* graphics, const byte* data) {
    switch (data[0]) {
        case SpriteOpUnitColorOff:
            graphics->popColor();
            break;
        case SpriteOpUnitColorOn:
            {
                CSColor color = force ? force->originColor() : CSColor::White;
                color.r *= 2;
                color.g *= 2;
                color.b *= 2;
                graphics->pushColor();
                graphics->setColor(color);
            }
            break;
        default:
            CSWarnLog("unknown sprite extern code:%d", data[0]);
            break;
    }
}

void FogBuilding::draw(CSGraphics* graphics, ObjectLayer layer) {
	if (_motion) {
		switch (layer) {
			case ObjectLayerShadow:
			case ObjectLayerBottom:
			case ObjectLayerMiddle:
			case ObjectLayerTop:
				graphics->pushColor();
				graphics->setColor(CSColor(1.0f, 1.0f, 1.0f, _alpha));
				_motion->drawLayered(graphics, animationVisibleKey(_dir, layer), layer == ObjectLayerShadow);
				graphics->popColor();
				break;
		}
	}
}
