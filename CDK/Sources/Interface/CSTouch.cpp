//
//  CSTouch.cpp
//  CDK
//
//  Created by chan on 13. 4. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTouch.h"

#include "CSTime.h"

CSTouch::CSTouch(uint key, CSTouchButton button, const CSVector2& point) :
    _key(key),
	_button(button),
    _firstPoint(point),
    _prevPoint(point),
    _point(point),
    _timestamp(CSTime::currentTime())
{

}

void CSTouch::setPoint(const CSVector2& point) {
    _prevPoint = _point;
    _point = point;
    _state = CSTouchStateMoved;
}



float CSTouch::time() const {
    return CSTime::currentTime() - _timestamp;
}
