//
//  FogResource.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 27..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "FogResource.h"

#include "Map.h"

FogResource::FogResource(const Resource* resource, int alliance) :
    Fog(alliance),
    kind(resource->kind),
    _motion(resource->captureMotion())
{
	_type = (ObjectType)((int)ObjectTypeFogMineral + kind);
	_collider = data().collider;
	_radius = data().radius;

    locate(resource->point());

    Map::sharedMap()->registerObject(this);
}

FogResource::~FogResource() {
	_motion->release();
}

FogResource* FogResource::fog(const Resource* resource, int alliance) {
    FogResource* fog = new FogResource(resource, alliance);
    fog->release();
    return fog;
}

const CSColor& FogResource::color(bool visible, bool detecting) const {
    const Force* vision = Map::sharedMap()->vision();
    
    return visible || !vision || alliance != vision->alliance() ? CSColor::Transparent : data().color;
}

void FogResource::locate(const FPoint& point) {
    Object::locate(point);
    
	_motion->setClientPosition(worldPoint(ObjectLayerBottom));
}

ObjectDisplay FogResource::display() const {
	const Force* vision = Map::sharedMap()->vision();

	return ObjectDisplay(_located && vision && vision->alliance() == alliance ? ObjectVisibleNormal : ObjectVisibleNone, 1);
}

void FogResource::draw(CSGraphics* graphics, ObjectLayer layer) {
	switch (layer) {
		case ObjectLayerShadow:
		case ObjectLayerBottom:
		case ObjectLayerMiddle:
		case ObjectLayerTop:
			_motion->drawLayered(graphics, animationVisibleKey(0, layer), layer == ObjectLayerShadow);
			break;
	}
}
