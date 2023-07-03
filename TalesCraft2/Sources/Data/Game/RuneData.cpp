//
//  RuneData.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2017. 1. 18..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "RuneData.h"

#include "Asset.h"

#include "UnitBase.h"

RuneData::RuneData(CSBuffer* buffer) :
    smallIcon(buffer),
	largeIcon(buffer),
    name(CSObject::retain(buffer->readLocaleString())),
	prefix(CSObject::retain(buffer->readLocaleString())),
    resources{ (ushort)buffer->readShort(), (ushort)buffer->readShort() },
    progress(buffer->readShort()),
    grade(buffer->readByte()),
	tier(buffer->readByte()),
	valuation(buffer->readByte()),
    target(CSObject::retain(buffer->readArray<byte>())),
    ability(buffer)
{
}

RuneData::~RuneData() {
    name->release();
	prefix->release();
	if (target) target->release();
}

bool RuneData::match(const UnitBase* unit) const {
	return !target || unit->match(unit->force(), target->pointer());
}