//
//  CSPlane.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 7..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSPlane__
#define __CDK__CSPlane__

#include "CSCollision.h"
#include "CSMath.h"
#include "CSVector4.h"
#include "CSQuaternion.h"
#include "CSMatrix.h"

struct CSPlane {
public:
    CSVector3 normal;
    float d;

	static const CSPlane Ground;
    
    inline CSPlane() {
    
    }
    
    inline CSPlane(float a, float b, float c, float d) : normal(a, b, c), d(d) {
    
    }
    
    inline CSPlane(const CSVector3& point, const CSVector3& normal) : normal(normal), d(-CSVector3::dot(normal, point)) {
    
    }
    
    inline CSPlane(const CSVector3& normal, float d) : normal(normal), d(d) {
    
    }
    
    CSPlane(const CSVector3& point1, const CSVector3& point2, const CSVector3& point3);
    
    static void normalize(const CSPlane& plane, CSPlane& result);
    static inline CSPlane normalize(const CSPlane& plane) {
        CSPlane result;
        normalize(plane, result);
        return result;
    }
    inline void normalize() {
        normalize(*this, *this);
    }
    
    inline CSPlaneIntersection intersects(const CSVector3& point) const {
        return CSCollision::planeIntersectsPoint(*this, point);
    }
    
    inline bool intersects(const CSRay& ray, float& distance) const {
        return CSCollision::rayIntersectsPlane(ray, *this, distance);
    }
    
    inline bool intersects(const CSRay& ray, CSVector3& point) const {
        return CSCollision::rayIntersectsPlane(ray, *this, point);
    }
    
    inline bool intersects(const CSPlane& plane, CSRay& line) const {
        return CSCollision::planeIntersectsPlane(*this, plane, line);
    }
    
    inline CSPlaneIntersection intersects(const CSBoundingBox& box) const {
        return CSCollision::planeIntersectsBox(*this, box);
    }
    
    inline CSPlaneIntersection intersects(const CSBoundingSphere& sphere) const {
        return CSCollision::planeIntersectsSphere(*this, sphere);
    }
    
    void reflection(CSMatrix& result) const;
    inline CSMatrix reflection() const {
        CSMatrix result;
        reflection(result);
        return result;
    }
    
    static inline float dot(const CSPlane& left, const CSVector4& right) {
        return (left.normal.x * right.x) + (left.normal.y * right.y) + (left.normal.z * right.z) + (left.d * right.w);
    }
    
    static inline float dotCoordinate(const CSPlane& left, const CSVector3& right) {
        return (left.normal.x * right.x) + (left.normal.y * right.y) + (left.normal.z * right.z) + left.d;
    }
    
    static inline float dotNormal(const CSPlane& left, const CSVector3& right) {
        return (left.normal.x * right.x) + (left.normal.y * right.y) + (left.normal.z * right.z);
    }
    
    static void transform(const CSPlane& plane, const CSQuaternion& rotation, CSPlane& result);
    static void transform(const CSPlane& plane, const CSMatrix& trans, CSPlane& result);
    
    static inline CSPlane transform(const CSPlane& plane, const CSQuaternion& rotation) {
        CSPlane result;
        transform(plane, rotation, result);
        return result;
    }
    static inline CSPlane transform(const CSPlane& plane, const CSMatrix& trans) {
        CSPlane result;
        transform(plane, trans, result);
        return result;
    }
    inline void transform(const CSQuaternion& rotation) {
        transform(*this, rotation, *this);
    }
    inline void transform(const CSMatrix& trans) {
        transform(*this, trans, *this);
    }
    
    void shadow(const CSVector4& light, CSMatrix& result) const;
    inline CSMatrix shadow(const CSVector4& light) const {
        CSMatrix result;
        shadow(light, result);
        return result;
    }
    
    inline CSPlane operator *(float scale) const {
        return CSPlane(normal.x * scale, normal.y * scale, normal.z * scale, d * scale);
    }
    inline CSPlane& operator *=(float scale) {
        normal.x *= scale;
        normal.y *= scale;
        normal.z *= scale;
        d *= scale;
        return *this;
    }
    
    inline bool operator ==(const CSPlane& other) const {
        return normal == other.normal && d == other.d;
    }
    
    inline bool operator !=(const CSPlane& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSPlane__ */
