//
//  ThemeData.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 5. 23..
//  Copyright © 2019년 brgames. All rights reserved.
//
#include "ThemeData.h"

ThemeData::ThemeData(CSBuffer* buffer) :
    icon(buffer),
    name(CSObject::retain(buffer->readLocaleString()))
{
    int skinCount = buffer->readLength();
    CSDictionary<UnitIndex, ushort>* skins = new CSDictionary<UnitIndex, ushort>();
    for (int i = 0; i < skinCount; i++) {
        UnitIndex index(buffer);
        int skin = buffer->readShort();
        
        skins->setObject(index, skin);
    }
    this->skins = skins;
}

ThemeData::~ThemeData() {
    name->release();
    skins->release();
}
