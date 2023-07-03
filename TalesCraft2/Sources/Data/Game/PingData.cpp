//
//  PingData.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 9. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "PingData.h"

PingData::PingData(CSBuffer* buffer) :
    icon(buffer),
    animation(buffer),
    color(buffer->readInt()),
    message(CSObject::retain(buffer->readLocaleString()))
{
    
}

PingData::~PingData() {
    message->release();
}
