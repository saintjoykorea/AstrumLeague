//
//  IPoint.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 12..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#include "IPoint.h"

#include "FPoint.h"

const IPoint IPoint::Zero(0, 0);

IPoint::IPoint(const FPoint& p) : x(p.x), y(p.y) {

}

