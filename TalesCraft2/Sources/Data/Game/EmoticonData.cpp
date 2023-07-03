//
//  EmoticonData.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 9. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "EmoticonData.h"

EmoticonData::EmoticonData(CSBuffer* buffer) :
    icon(buffer),
    animation(buffer),
    message(buffer->readBoolean() ? CSObject::retain(buffer->readLocaleString()) : NULL),
	name(CSObject::retain(buffer->readLocaleString()))
{

}

EmoticonData::~EmoticonData() {
    CSObject::release(message);
	name->release();
}
