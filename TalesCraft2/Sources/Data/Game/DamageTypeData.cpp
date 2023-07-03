//
//  UnitDamageData.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 5. 4..
//  Copyright © 2018년 brgames. All rights reserved.
//
#include "DamageTypeData.h"

DamageTypeData::DamageTypeData(CSBuffer* buffer) :
    icon(buffer),
    name(CSObject::retain(buffer->readLocaleString()))
{
    
}

DamageTypeData::~DamageTypeData() {
    name->release();
}
