//
//  CSBoundingBox.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSBoundingBox__
#define __CDK__CSBoundingBox__

#include "CSCollision.h"
#include "CSVector3.h"

struct CSBoundingBox {
public:
    CSVector3 minimum, maximum;
    
    inline CSBoundingBox(CSVector3 min, CSVector3 max) : minimum(min), maximum(max) {
    
    }
    
    void corners(CSVector3* points) const;
    
    inline bool intersects(const CSRay& ray, float distance) const {
        return CSCollision::rayIntersectsBox(ray, *this, distance);
    }
    
    inline bool intersects(const CSRay& ray, CSVector3& point) const {
        return CSCollision::rayIntersectsBox(ray, *this, point);
    }
    
    inline CSPlaneIntersection intersects(const CSPlane& plane) const {
        return CSCollision::planeIntersectsBox(plane, *this);
    }
    
    inline bool intersects(const CSBoundingBox& box) const {
        return CSCollision::boxIntersectsBox(*this, box);
    }
    
    inline bool intersects(const CSBoundingSphere& sphere) const {
        return CSCollision::boxIntersectsSphere(*this, sphere);
    }
    
    inline CSBoundingContainment contains(const CSVector3& point) const {
        return CSCollision::boxContainsPoint(*this, point);
    }
    
    inline CSBoundingContainment contains(const CSBoundingBox& box) const {
        return CSCollision::boxContainsBox(*this, box);
    }
    
    inline CSBoundingContainment contains(const CSBoundingSphere& sphere) const {
        return CSCollision::boxContainsSphere(*this, sphere);
    }
    
    inline bool operator ==(const CSBoundingBox& other) const {
        return minimum == other.minimum && maximum == other.maximum;
    }
    
    inline bool operator !=(const CSBoundingBox& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSBoundingBox__ */
