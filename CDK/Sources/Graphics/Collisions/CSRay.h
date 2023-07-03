//
//  CSRay.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSRay__
#define __CDK__CSRay__

#include "CSCollision.h"
#include "CSVector3.h"
#include "CSMatrix.h"

struct CSRay {
public:
    CSVector3 position, direction;
    
    inline CSRay() {
    
    }
    
    inline CSRay(const CSVector3& pos, const CSVector3& dir) : position(pos), direction(dir) {
    
    }
    
    inline bool intersects(const CSPlane& plane) const {
        float distance;
        return CSCollision::rayIntersectsPlane(*this, plane, distance);
    }
    
    inline bool intersects(const CSPlane& plane, CSVector3& point) const {
        return CSCollision::rayIntersectsPlane(*this, plane, point);
    }
    
    inline bool intersects(const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3) const {
        float distance;
        return CSCollision::rayIntersectsTriangle(*this, vertex1, vertex2, vertex3, distance);
    }
    
    inline bool intersects(const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, CSVector3& point) const {
        return CSCollision::rayIntersectsTriangle(*this, vertex1, vertex2, vertex3, point);
    }
    
    inline bool intersects(const CSBoundingBox& box, float& distance) const {
        return CSCollision::rayIntersectsBox(*this, box, distance);
    }
    
    inline bool intersects(const CSBoundingBox& box, CSVector3& point) const {
        return CSCollision::rayIntersectsBox(*this, box, point);
    }
    
    inline bool intersects(const CSBoundingSphere& sphere, float& distance) const {
        return CSCollision::rayIntersectsSphere(*this, sphere, distance);
    }
    
    inline bool intersects(const CSBoundingSphere& sphere, CSVector3& point) const {
        return CSCollision::rayIntersectsSphere(*this, sphere, point);
    }
    
    static void pickRay(const CSVector2& pos, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection, CSRay& result);
    static inline CSRay pickRay(const CSVector2& pos, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection) {
        CSRay result;
        pickRay(pos, x, y, width, height, znear, zfar, worldViewProjection, result);
        return result;
    }
    
    inline bool operator ==(const CSRay& value) const {
        return position == value.position && direction == value.direction;
    }
    
    inline bool operator !=(const CSRay& value) const {
        return !(*this == value);
    }
};

#endif /* __CDK__CSRay__ */
