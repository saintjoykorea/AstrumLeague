//
//  Area.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 29..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "Area.h"

Area::Area(CSBuffer* buffer) :
    point(buffer),
    range(buffer->readFixed()),
    frange2(range * range),
    irange2((range + fixed::One) * (range + fixed::One)),
    buildingPoints(CSObject::retain(buffer->readArray<FPoint>())),
    rallyPoints(CSObject::retain(buffer->readArray<FPoint>()))
{

}

Area::~Area() {
    CSObject::release(buildingPoints);
    CSObject::release(rallyPoints);
}

bool Area::contains(const FPoint& point) const {
    return this->point.distanceSquared(point) <= frange2;
}

bool Area::contains(const IPoint& point) const {
    return this->point.distanceSquared(point) < irange2;
}
