//
//  CSBoundingFrustum.cpp
//  CDK
//
//  Created by Kim Chan on 2016. 3. 17..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBoundingFrustum.h"

CSBoundingFrustum::CSBoundingFrustum(const CSMatrix& matrix) : matrix(matrix) {
    planesFromMatrix(matrix, planes);
}

void CSBoundingFrustum::planesFromMatrix(const CSMatrix& matrix, CSPlane* planes) {
    // Left plane
    planes[0].normal.x = matrix.m14 + matrix.m11;
    planes[0].normal.y = matrix.m24 + matrix.m21;
    planes[0].normal.z = matrix.m34 + matrix.m31;
    planes[0].d = matrix.m44 + matrix.m41;
    planes[0].normalize();
    
    // Right plane
    planes[1].normal.x = matrix.m14 - matrix.m11;
    planes[1].normal.y = matrix.m24 - matrix.m21;
    planes[1].normal.z = matrix.m34 - matrix.m31;
    planes[1].d = matrix.m44 - matrix.m41;
    planes[1].normalize();
    
    // Top plane
    planes[2].normal.x = matrix.m14 - matrix.m12;
    planes[2].normal.y = matrix.m24 - matrix.m22;
    planes[2].normal.z = matrix.m34 - matrix.m32;
    planes[2].d = matrix.m44 - matrix.m42;
    planes[2].normalize();
    
    // Bottom plane
    planes[3].normal.x = matrix.m14 + matrix.m12;
    planes[3].normal.y = matrix.m24 + matrix.m22;
    planes[3].normal.z = matrix.m34 + matrix.m32;
    planes[3].d = matrix.m44 + matrix.m42;
    planes[3].normalize();
    
    // Near plane
    planes[4].normal.x = matrix.m13;
    planes[4].normal.y = matrix.m23;
    planes[4].normal.z = matrix.m33;
    planes[4].d = matrix.m43;
    planes[4].normalize();
    
    // Far plane
    planes[5].normal.x = matrix.m14 - matrix.m13;
    planes[5].normal.y = matrix.m24 - matrix.m23;
    planes[5].normal.z = matrix.m34 - matrix.m33;
    planes[5].d = matrix.m44 - matrix.m43;
    planes[5].normalize();
}

CSVector3 CSBoundingFrustum::pointInter3Planes(const CSPlane& p1, const CSPlane& p2, const CSPlane& p3) {
    //P = -d1 * N2xN3 / N1.N2xN3 - d2 * N3xN1 / N2.N3xN1 - d3 * N1xN2 / N3.N1xN2
    CSVector3 v =
        CSVector3::cross(p2.normal, p3.normal) * -p1.d / CSVector3::dot(p1.normal, CSVector3::cross(p2.normal, p3.normal)) +
        CSVector3::cross(p3.normal, p1.normal) * -p2.d / CSVector3::dot(p2.normal, CSVector3::cross(p3.normal, p1.normal)) +
        CSVector3::cross(p1.normal, p2.normal) * -p3.d / CSVector3::dot(p3.normal, CSVector3::cross(p1.normal, p2.normal));
        
    return v;
}

void CSBoundingFrustum::corners(CSVector3* corners) const {
    corners[0] = pointInter3Planes(planes[4], planes[3], planes[1]);
    corners[1] = pointInter3Planes(planes[4], planes[2], planes[1]);
    corners[2] = pointInter3Planes(planes[4], planes[2], planes[0]);
    corners[3] = pointInter3Planes(planes[4], planes[3], planes[0]);
    corners[4] = pointInter3Planes(planes[5], planes[3], planes[1]);
    corners[5] = pointInter3Planes(planes[5], planes[2], planes[1]);
    corners[6] = pointInter3Planes(planes[5], planes[2], planes[0]);
    corners[7] = pointInter3Planes(planes[5], planes[3], planes[0]);
}

CSBoundingContainment CSBoundingFrustum::contains(const CSVector3& point) const {
    CSPlaneIntersection result = CSPlaneIntersectionFront;
    CSPlaneIntersection planeResult = CSPlaneIntersectionFront;
    for (int i = 0; i < 6; i++) {
        planeResult = planes[i].intersects(point);
        
        switch (planeResult) {
            case CSPlaneIntersectionBack:
                return CSBoundingContainmentDisjoint;
            case CSPlaneIntersectionIntersecting:
                result = CSPlaneIntersectionIntersecting;
                break;
        }
    }
    switch (result) {
        case CSPlaneIntersectionIntersecting:
            return CSBoundingContainmentIntersects;
        default:
            return CSBoundingContainmentContains;
    }
}

void boxToPlanePVertexNVertex(const CSBoundingBox& box, const CSVector3& planeNormal, CSVector3& p, CSVector3& n) {
    p = box.minimum;
    if (planeNormal.x >= 0)
        p.x = box.maximum.x;
    if (planeNormal.y >= 0)
        p.y = box.maximum.y;
    if (planeNormal.z >= 0)
        p.z = box.maximum.z;
        
    n = box.maximum;
    if (planeNormal.x >= 0)
        n.x = box.minimum.x;
    if (planeNormal.y >= 0)
        n.y = box.minimum.y;
    if (planeNormal.z >= 0)
        n.z = box.minimum.z;
}

CSBoundingContainment CSBoundingFrustum::contains(const CSBoundingBox& box) const {
    CSVector3 p, n;
    CSPlane plane;
    CSBoundingContainment result = CSBoundingContainmentContains;
    for (int i = 0; i < 6; i++) {
        boxToPlanePVertexNVertex(box, planes[i].normal, p, n);
        if (CSCollision::planeIntersectsPoint(planes[i], p) == CSPlaneIntersectionBack)
            return CSBoundingContainmentDisjoint;
            
        if (CSCollision::planeIntersectsPoint(planes[i], n) == CSPlaneIntersectionBack)
            result = CSBoundingContainmentIntersects;
    }
    return result;
}

CSBoundingContainment CSBoundingFrustum::contains(const CSBoundingSphere& sphere) const {
    CSPlaneIntersection result = CSPlaneIntersectionFront;
    CSPlaneIntersection planeResult = CSPlaneIntersectionFront;
    for (int i = 0; i < 6; i++) {
        planeResult = planes[i].intersects(sphere);
        
        switch (planeResult) {
            case CSPlaneIntersectionBack:
                return CSBoundingContainmentDisjoint;
            case CSPlaneIntersectionIntersecting:
                result = CSPlaneIntersectionIntersecting;
                break;
        }
    }
    switch (result) {
        case CSPlaneIntersectionIntersecting:
            return CSBoundingContainmentIntersects;
        default:
            return CSBoundingContainmentContains;
    }
}

bool CSBoundingFrustum::intersects(const CSBoundingSphere& sphere) const {
    return contains(sphere) != CSBoundingContainmentDisjoint;
}

bool CSBoundingFrustum::intersects(const CSBoundingBox& box) const {
    return contains(box) != CSBoundingContainmentDisjoint;
}

CSPlaneIntersection planeIntersectsPoints(const CSPlane& plane, const CSVector3* points, uint count) {
    CSPlaneIntersection result = CSCollision::planeIntersectsPoint(plane, points[0]);
    for (int i = 1; i < count; i++)
        if (CSCollision::planeIntersectsPoint(plane, points[i]) != result)
            return CSPlaneIntersectionIntersecting;
    return result;
}

CSPlaneIntersection CSBoundingFrustum::intersects(const CSPlane& plane) const {
    CSVector3 corners[8];
    this->corners(corners);
    return planeIntersectsPoints(plane, corners, 8);
}

float CSBoundingFrustum::widthAtDepth(float depth) const {
    float hAngle = (float)((FloatPiOverTwo - CSMath::acos(CSVector3::dot(planes[4].normal, planes[0].normal))));
    return (float)(CSMath::tan(hAngle) * depth * 2);
}

float CSBoundingFrustum::heightAtDepth(float depth) const {
    float vAngle = (float)((FloatPiOverTwo - CSMath::acos(CSVector3::dot(planes[4].normal, planes[2].normal))));
    return (float)(CSMath::tan(vAngle) * depth * 2);
}

bool CSBoundingFrustum::intersects(const CSRay& ray, float& inDistance, float& outDistance) const {
    if (contains(ray.position) != CSBoundingContainmentDisjoint) {
        float nearstPlaneDistance = FloatMax;
        for (int i = 0; i < 6; i++) {
            float distance;
            if (CSCollision::rayIntersectsPlane(ray, planes[i], distance) && distance < nearstPlaneDistance) {
                nearstPlaneDistance = distance;
            }
        }
        inDistance = nearstPlaneDistance;
        outDistance = FloatMax;
        return true;
    }
    else {
        float minDist = FloatMax;
        float maxDist = FloatMin;
        for (int i = 0; i < 6; i++) {
            float distance;
            if (CSCollision::rayIntersectsPlane(ray, planes[i], distance)) {
                minDist = CSMath::min(minDist, distance);
                maxDist = CSMath::max(maxDist, distance);
            }
        }
        
        CSVector3 minPoint = ray.position + ray.direction * minDist;
        CSVector3 maxPoint = ray.position + ray.direction * maxDist;
        CSVector3 center = (minPoint + maxPoint) * 0.5f;
        if (contains(center) != CSBoundingContainmentDisjoint) {
            inDistance = minDist;
            outDistance = maxDist;
            return true;
        }
        else {
            return false;
        }
    }
}

float CSBoundingFrustum::zoomToExtentsShiftDistance(CSVector3* points, uint count) const {
    float vAngle = FloatPiOverTwo - CSMath::acos(CSVector3::dot(planes[4].normal, planes[2].normal));
    float vSin = CSMath::sin(vAngle);
    float hAngle = FloatPiOverTwo - CSMath::acos(CSVector3::dot(planes[4].normal, planes[0].normal));
    float hSin = CSMath::sin(hAngle);
    float horizontalToVerticalMapping = vSin / hSin;
    
    CSBoundingFrustum ioFrustum = *this;
    ioFrustum.planes[0].normal = -ioFrustum.planes[0].normal;
    ioFrustum.planes[1].normal = -ioFrustum.planes[1].normal;
    ioFrustum.planes[2].normal = -ioFrustum.planes[2].normal;
    ioFrustum.planes[3].normal = -ioFrustum.planes[3].normal;
    ioFrustum.planes[4].normal = -ioFrustum.planes[4].normal;
    ioFrustum.planes[5].normal = -ioFrustum.planes[5].normal;
    
    float maxPointDist = FloatMax;
    for (int i = 0; i < count; i++) {
        float pointDist = CSCollision::distancePlanePoint(ioFrustum.planes[2], points[i]);
        pointDist = CSMath::max(pointDist, CSCollision::distancePlanePoint(ioFrustum.planes[3], points[i]));
        pointDist = CSMath::max(pointDist, CSCollision::distancePlanePoint(ioFrustum.planes[0], points[i]) * horizontalToVerticalMapping);
        pointDist = CSMath::max(pointDist, CSCollision::distancePlanePoint(ioFrustum.planes[1], points[i]) * horizontalToVerticalMapping);
        
        maxPointDist = CSMath::max(maxPointDist, pointDist);
    }
    return -maxPointDist / vSin;
}

float CSBoundingFrustum::zoomToExtentsShiftDistance(const CSBoundingBox& boundingBox) const {
    CSVector3 corners[8];
    boundingBox.corners(corners);
    return zoomToExtentsShiftDistance(corners, 8);
}

CSVector3 CSBoundingFrustum::zoomToExtentsShiftVector(CSVector3* points, uint count) const {
    return planes[4].normal * zoomToExtentsShiftDistance(points, count);
}

CSVector3 CSBoundingFrustum::zoomToExtentsShiftVector(const CSBoundingBox& boundingBox) const {
    CSVector3 corners[8];
    boundingBox.corners(corners);
    return planes[4].normal * zoomToExtentsShiftDistance(corners, 8);
}
