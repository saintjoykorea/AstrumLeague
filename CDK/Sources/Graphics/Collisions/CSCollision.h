//
//  CSCollision.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 7..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSCollision__
#define __CDK__CSCollision__

#include "CSVector3.h"

struct CSRay;
struct CSPlane;
struct CSBoundingBox;
struct CSBoundingSphere;

enum CSBoundingContainment {
    CSBoundingContainmentDisjoint,
    CSBoundingContainmentContains,
    CSBoundingContainmentIntersects
};

enum CSPlaneIntersection {
    CSPlaneIntersectionBack,
    CSPlaneIntersectionFront,
    CSPlaneIntersectionIntersecting
};

struct CSCollision {
public:
    static float distancePlanePoint(const CSPlane& plane, const CSVector3& point);
    
    static void closestPointPointTriangle(const CSVector3& point, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, CSVector3& result);
    
    static bool rayIntersectsPlane(const CSRay& ray, const CSPlane& plane, float& distance);
    static bool rayIntersectsPlane(const CSRay& ray, const CSPlane& plane, CSVector3& point);
    static bool rayIntersectsTriangle(const CSRay& ray, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, float& distance);
    static bool rayIntersectsTriangle(const CSRay& ray, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, CSVector3& point);
    static bool rayIntersectsBox(const CSRay& ray, const CSBoundingBox& box, float& distance);
    static bool rayIntersectsBox(const CSRay& ray, const CSBoundingBox& box, CSVector3& point);
    static bool rayIntersectsSphere(const CSRay& ray, const CSBoundingSphere& sphere, float& distance);
    static bool rayIntersectsSphere(const CSRay& ray, const CSBoundingSphere& sphere, CSVector3& point);
    
    static CSPlaneIntersection planeIntersectsPoint(const CSPlane& plane, const CSVector3& point);
    static bool planeIntersectsPlane(const CSPlane& plane1, const CSPlane& plane2);
    static bool planeIntersectsPlane(const CSPlane& plane1, const CSPlane& plane2, CSRay& line);
    static CSPlaneIntersection planeIntersectsBox(const CSPlane& plane, const CSBoundingBox& box);
    static CSPlaneIntersection planeIntersectsSphere(const CSPlane& plane, const CSBoundingSphere& sphere);
    
    static bool boxIntersectsBox(const CSBoundingBox& box1, const CSBoundingBox& box2);
    static bool boxIntersectsSphere(const CSBoundingBox& box, const CSBoundingSphere& sphere);
    
    static bool sphereIntersectsTriangle(const CSBoundingSphere& sphere, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3);
    static bool sphereIntersectsSphere(const CSBoundingSphere& sphere1, const CSBoundingSphere& sphere2);
    
    static CSBoundingContainment boxContainsPoint(const CSBoundingBox& box, const CSVector3& point);
    static CSBoundingContainment boxContainsBox(const CSBoundingBox& box1, const CSBoundingBox& box2);
    static CSBoundingContainment boxContainsSphere(const CSBoundingBox& box, const CSBoundingSphere& sphere);
    
    static CSBoundingContainment sphereContainsPoint(const CSBoundingSphere& sphere, const CSVector3& point);
    static CSBoundingContainment sphereContainsTriangle(const CSBoundingSphere& sphere, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3);
    static CSBoundingContainment sphereContainsBox(const CSBoundingSphere& sphere, const CSBoundingBox& box);
    static CSBoundingContainment sphereContainsSphere(const CSBoundingSphere& sphere1, const CSBoundingSphere& sphere2);
};

#endif /* __CDK__CSCollision__ */
