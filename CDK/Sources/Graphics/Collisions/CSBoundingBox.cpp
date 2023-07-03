//
//  CSBoundingBox.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBoundingBox.h"

void CSBoundingBox::corners(CSVector3* points) const {
    points[0] = CSVector3(minimum.x, maximum.y, maximum.z);
    points[1] = CSVector3(maximum.x, maximum.y, maximum.z);
    points[2] = CSVector3(maximum.x, minimum.y, maximum.z);
    points[3] = CSVector3(minimum.x, minimum.y, maximum.z);
    points[4] = CSVector3(minimum.x, maximum.y, minimum.z);
    points[5] = CSVector3(maximum.x, maximum.y, minimum.z);
    points[6] = CSVector3(maximum.x, minimum.y, minimum.z);
    points[7] = CSVector3(minimum.x, minimum.y, minimum.z);
}
