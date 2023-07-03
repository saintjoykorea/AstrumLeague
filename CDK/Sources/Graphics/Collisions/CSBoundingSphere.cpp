//
//  CSBoundingSphere.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 11..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBoundingSphere.h"

#include "CSMath.h"

#include "CSBoundingBox.h"

void CSBoundingSphere::fromPoints(const CSArray<CSVector3>* points, int start, int count, CSBoundingSphere& result) {
    if (start < 0 || start >= points->count())     {
        CSAssert(false, "start invalid Data");
        return;
    }
    
    // Check that count is in the correct range
    if (count < 0 || (start + count) > points->count()) {
        CSAssert(false, "count invalid Data");
        return;
    }
    
    int upperEnd = start + count;
    
    //Find the center of all points.
    CSVector3 center;
    for (int i = start; i < upperEnd; ++i) {
        center = points->objectAtIndex(i) + center;
    }
    
    //This is the center of our sphere.
    center = center / (float)count;
    
    //Find the radius of the sphere
    float radius = 0;
    for (int i = start; i < upperEnd; ++i) {
        //We are doing a relative distance comparison to find the maximum distance
        //from the center of our sphere.
        float distance;
        distance = CSVector3::distanceSquared(center, points->objectAtIndex(i));
        
        if (distance > radius)
            radius = distance;
    }
    
    //Find the real distance from the DistanceSquared.
    radius = CSMath::sqrt(radius);
    
    //Construct the sphere.
    result.center = center;
    result.radius = radius;
}

void CSBoundingSphere::fromBox(const CSBoundingBox& box, CSBoundingSphere& result) {
    CSVector3::lerp(box.minimum, box.maximum, 0.5f, result.center);
    
    float x = box.minimum.x - box.maximum.x;
    float y = box.minimum.y - box.maximum.y;
    float z = box.minimum.z - box.maximum.z;
    
    float distance = (float)(CSMath::sqrt((x * x) + (y * y) + (z * z)));
    result.radius = distance * 0.5f;
}

void merge(const CSBoundingSphere& value1, const CSBoundingSphere& value2, CSBoundingSphere& result) {
    CSVector3 difference = value2.center - value1.center;
    
    float length = difference.length();
    float radius = value1.radius;
    float radius2 = value2.radius;
    
    if (radius + radius2 >= length) {
        if (radius - radius2 >= length) {
            result = value1;
            return;
        }
        
        if (radius2 - radius >= length) {
            result = value2;
            return;
        }
    }
    
    CSVector3 vector = difference * (1.0f / length);
    float min = CSMath::min(-radius, length - radius2);
    float max = (CSMath::max(radius, length + radius2) - min) * 0.5f;
    
    result.center = value1.center + vector * (max + min);
    result.radius = max;
}
