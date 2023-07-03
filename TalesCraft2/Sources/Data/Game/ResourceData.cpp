//
//  ResourceData.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 4. 2..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "ResourceData.h"

ResourceData::ResourceData(CSBuffer* buffer) :
    icon(buffer),
    name(CSObject::retain(buffer->readLocaleString())),
    collider(buffer->readFixed()),
    radius(buffer->readFloat()),
	body(buffer->readFloat()),
	top(buffer->readFloat()),
	gatherings(buffer->readByte()),
	color(buffer->readInt()),
    levels(CSObject::retain(buffer->readArray<ResourceLevelData>()))
{
    CSAssert(levels, "invalid data");
}

ResourceData::~ResourceData() {
    CSObject::release(name);
    levels->release();
}
