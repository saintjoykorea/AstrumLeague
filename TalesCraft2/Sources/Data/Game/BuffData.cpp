//
//  BuffData.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 2..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#include "BuffData.h"

BuffData::BuffData(CSBuffer* buffer) :
    maxStack(buffer->readFixed()),
    ability(buffer),
    trigger(CSObject::retain(buffer->readArray<byte, 2>()))
{
}

BuffData::~BuffData() {
    CSObject::release(trigger);
}
