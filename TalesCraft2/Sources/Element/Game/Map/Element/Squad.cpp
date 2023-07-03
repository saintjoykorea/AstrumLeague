//
//  Squad.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Squad.h"

#include "Map.h"

static Force* squadForce(int index) {
    return index >= 0 ? Map::sharedMap()->force(index) : NULL;
}

Squad::Squad(CSBuffer* buffer) :
    index(buffer),
    point(buffer->readBoolean() ? new FPoint(buffer) : NULL),
	range(buffer->readFixed()),
    force(squadForce(buffer->readByte())),
    level(buffer->readShort()),
    skin(buffer->readShort()),
    units(new CSArray<Unit, 1, false>()),
    growups(NULL),
    name(NULL),
    abilities(NULL),
	scaleBase(fixed::One),
	scaleUp(fixed::Zero)
{
    
}

Squad::~Squad() {
    if (point) {
        delete point;
    }
    units->release();
    CSObject::release(growups);
    CSObject::release(name);
    CSObject::release(abilities);
}
