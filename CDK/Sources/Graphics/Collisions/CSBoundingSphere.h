//
//  CSBoundingSphere.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 11..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSBoundingSphere__
#define __CDK__CSBoundingSphere__

#include "CSCollision.h"
#include "CSVector3.h"
#include "CSArray.h"

struct CSBoundingSphere {
public:
    CSVector3 center;
    float radius;
    
    inline CSBoundingSphere() {
    
    }
    inline CSBoundingSphere(CSVector3 center, float radius) : center(center), radius(radius) {
    
    }
    
    inline bool intersects(const CSRay& ray, float& distance) const {
        return CSCollision::rayIntersectsSphere(ray, *this, distance);
    }
    
    inline bool intersects(const CSRay& ray, CSVector3& point) const {
        return CSCollision::rayIntersectsSphere(ray, *this, point);
    }
    
    inline CSPlaneIntersection intersects(const CSPlane& plane) const {
        return CSCollision::planeIntersectsSphere(plane, *this);
    }
    
    inline bool intersects(const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3) const {
        return CSCollision::sphereIntersectsTriangle(*this, vertex1, vertex2, vertex3);
    }
    
    inline bool intersects(const CSBoundingBox& box) const {
        return CSCollision::boxIntersectsSphere(box, *this);
    }
    
    inline bool intersects(const CSBoundingSphere& sphere) const {
        return CSCollision::sphereIntersectsSphere(*this, sphere);
    }
    
    CSBoundingContainment contains(const CSVector3& point) const {
        return CSCollision::sphereContainsPoint(*this, point);
    }
    
    CSBoundingContainment contains(const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3) const {
        return CSCollision::sphereContainsTriangle(*this, vertex1, vertex2, vertex3);
    }
    
    CSBoundingContainment contains(const CSBoundingBox& box) const {
        return CSCollision::sphereContainsBox(*this, box);
    }
    
    CSBoundingContainment contains(const CSBoundingSphere& sphere) const {
        return CSCollision::sphereContainsSphere(*this, sphere);
    }
    
    static void fromPoints(const CSArray<CSVector3>* points, int start, int count, CSBoundingSphere& result);
    static void fromBox(const CSBoundingBox& box, CSBoundingSphere& result);
    static void merge(const CSBoundingSphere& value1, const CSBoundingSphere& value2, CSBoundingSphere& result);
    
    static inline CSBoundingSphere fromPoints(const CSArray<CSVector3>* points, int start, int count) {
        CSBoundingSphere result;
        fromPoints(points, start, count, result);
        return result;
    }
    static inline CSBoundingSphere fromBox(const CSBoundingBox& box) {
        CSBoundingSphere result;
        fromBox(box, result);
        return result;
    }
    static inline CSBoundingSphere merge(const CSBoundingSphere& value1, const CSBoundingSphere& value2) {
        CSBoundingSphere result;
        merge(value1, value2, result);
        return result;
    }
    
    inline bool operator ==(const CSBoundingSphere& other) {
        return center == other.center && radius == other.radius;
    }
    
    inline bool operator !=(const CSBoundingSphere& other) {
        return !(*this == other);
    }
};
#endif /* __CDK__CSBoundingSphere__ */
