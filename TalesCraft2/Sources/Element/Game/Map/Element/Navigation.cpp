//
//  Navigation.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 29..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "Navigation.h"

Navigation::Navigation(const UnitIndex& index) :
    index(index),
    _preparedUnits(new CSArray<Unit>()),
    _registeredUnits(new CSArray<Unit>())
{

}
Navigation::~Navigation() {
    _preparedUnits->release();
    _registeredUnits->release();
}

