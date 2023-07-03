//
//  CSRay.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRay.h"

void CSRay::pickRay(const CSVector2& pos, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection, CSRay& result) {
    CSVector3 nearPoint(pos, 0);
    CSVector3 farPoint(pos, 1);
    CSVector3::unproject(nearPoint, x, y, width, height, znear, zfar, worldViewProjection, nearPoint);
    CSVector3::unproject(farPoint, x, y, width, height, znear, zfar, worldViewProjection, farPoint);
    CSVector3 direction = farPoint - nearPoint;
    direction.normalize();
    
    result.position = nearPoint;
    result.direction = direction;
}
