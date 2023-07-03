//
//  Card.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 27..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Card.h"

#include "VirtualUnit.h"

Card::Card(const ForceBase* force, const UnitIndex& index, int level, int skin) :
    UnitBase(index, level, skin),
    _force(force)
{
}

Card::Card(CSBuffer* buffer, const ForceBase* force) :
    UnitBase(buffer),
    _force(force)
{
}

Card::Card(const CSJSONObject* json, const ForceBase* force) :
    UnitBase(json),
    _force(force)
{
}

void Card::writeTo(CSBuffer* buffer) const {
    _index.writeTo(buffer);
    buffer->writeShort(level);
    buffer->writeShort(skin);
}

