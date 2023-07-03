//
//  Growup.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 12..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "Growup.h"

#include "UnitBase.h"

Growup::Growup(const byte*& cursor) :
    maxLevel(readShort(cursor)),
    levelupExp(readShort(cursor)),
	dropExpOp((DropExpOp)readByte(cursor)),
    dropExpBase(readShort(cursor)),
	dropExpUp(readShort(cursor)),
	rewardResources{ (ushort)readShort(cursor), (ushort)readShort(cursor) }
{
    int length = readLength(cursor);
    if (length) {
        CSArray<DropUnit>* dropUnits = new CSArray<DropUnit>(length);
        for (int i = 0; i < length; i++) {
            new (&dropUnits->addObject()) DropUnit(cursor);
        }
        this->dropUnits = dropUnits;
    }
    else {
        this->dropUnits = NULL;
    }
    levelupEffect = readBoolean(cursor) ? new AnimationIndex(cursor) : NULL;
    dropEffect = readBoolean(cursor) ? new AnimationIndex(cursor) : NULL;
}

Growup::~Growup() {
    CSObject::release(dropUnits);
    if (levelupEffect) {
        delete levelupEffect;
    }
    if (dropEffect) {
        delete dropEffect;
    }
}
