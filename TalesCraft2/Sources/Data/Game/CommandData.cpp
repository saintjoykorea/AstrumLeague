//
//  CommandData.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 26..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "CommandData.h"

CommandData::CommandData(CSBuffer* buffer) :
    icon(buffer),
    name(CSObject::retain(buffer->readLocaleString())),
    description(CSObject::retain(buffer->readLocaleString()))
{

}
CommandData::~CommandData() {
    CSObject::release(name);
    CSObject::release(description);
}

CommandUnitData::CommandUnitData(CSBuffer* buffer) :
    commitTrigger(CSObject::retain(buffer->readArray<byte, 2>())),
    actionTrigger(CSObject::retain(buffer->readArray<byte, 2>()))
{

}
CommandUnitData::~CommandUnitData() {
    CSObject::release(commitTrigger);
    CSObject::release(actionTrigger);
}
