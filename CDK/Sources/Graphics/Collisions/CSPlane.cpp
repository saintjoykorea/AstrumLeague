//
//  CSPlane.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 7..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSPlane.h"

#include "CSMatrix.h"

const CSPlane CSPlane::Ground(0, 0, 1, 0);

CSPlane::CSPlane(const CSVector3& point1, const CSVector3& point2, const CSVector3& point3) {
    float x1 = point2.x - point1.x;
    float y1 = point2.y - point1.y;
    float z1 = point2.z - point1.z;
    float x2 = point3.x - point1.x;
    float y2 = point3.y - point1.y;
    float z2 = point3.z - point1.z;
    float yz = (y1 * z2) - (z1 * y2);
    float xz = (z1 * x2) - (x1 * z2);
    float xy = (x1 * y2) - (y1 * x2);
    float invPyth = 1.0f / CSMath::sqrt((yz * yz) + (xz * xz) + (xy * xy));
    
    normal.x = yz * invPyth;
    normal.y = xz * invPyth;
    normal.z = xy * invPyth;
    d = -((normal.x * point1.x) + (normal.y * point1.y) + (normal.z * point1.z));
}

void CSPlane::normalize(const CSPlane& plane, CSPlane& result) {
    float magnitude = 1.0f / CSMath::sqrt((plane.normal.x * plane.normal.x) + (plane.normal.y * plane.normal.y) + (plane.normal.z * plane.normal.z));
    
    result.normal.x = plane.normal.x * magnitude;
    result.normal.y = plane.normal.y * magnitude;
    result.normal.z = plane.normal.z * magnitude;
    result.d = plane.d * magnitude;
}

void CSPlane::reflection(CSMatrix& result) const {
    float x = normal.x;
    float y = normal.y;
    float z = normal.z;
    float x2 = -2.0f * x;
    float y2 = -2.0f * y;
    float z2 = -2.0f * z;
    
    result.m11 = (x2 * x) + 1.0f;
    result.m12 = y2 * x;
    result.m13 = z2 * x;
    result.m14 = 0.0f;
    result.m21 = x2 * y;
    result.m22 = (y2 * y) + 1.0f;
    result.m23 = z2 * y;
    result.m24 = 0.0f;
    result.m31 = x2 * z;
    result.m32 = y2 * z;
    result.m33 = (z2 * z) + 1.0f;
    result.m34 = 0.0f;
    result.m41 = x2 * d;
    result.m42 = y2 * d;
    result.m43 = z2 * d;
    result.m44 = 1.0f;
}

void CSPlane::transform(const CSPlane& plane, const CSQuaternion& rotation, CSPlane& result) {
    float x2 = rotation.x + rotation.x;
    float y2 = rotation.y + rotation.y;
    float z2 = rotation.z + rotation.z;
    float wx = rotation.w * x2;
    float wy = rotation.w * y2;
    float wz = rotation.w * z2;
    float xx = rotation.x * x2;
    float xy = rotation.x * y2;
    float xz = rotation.x * z2;
    float yy = rotation.y * y2;
    float yz = rotation.y * z2;
    float zz = rotation.z * z2;
    
    float x = plane.normal.x;
    float y = plane.normal.y;
    float z = plane.normal.z;
    
    result.normal.x = ((x * ((1.0f - yy) - zz)) + (y * (xy - wz))) + (z * (xz + wy));
    result.normal.y = ((x * (xy + wz)) + (y * ((1.0f - xx) - zz))) + (z * (yz - wx));
    result.normal.z = ((x * (xz - wy)) + (y * (yz + wx))) + (z * ((1.0f - xx) - yy));
    result.d = plane.d;
}

void CSPlane::transform(const CSPlane& plane, const CSMatrix& trans, CSPlane& result) {
    float x = plane.normal.x;
    float y = plane.normal.y;
    float z = plane.normal.z;
    float pd = plane.d;
    
    CSMatrix inverse;
    CSMatrix::invert(trans, inverse);
    
    result.normal.x = (((x * inverse.m11) + (y * inverse.m12)) + (z * inverse.m13)) + (pd * inverse.m14);
    result.normal.y = (((x * inverse.m21) + (y * inverse.m22)) + (z * inverse.m23)) + (pd * inverse.m24);
    result.normal.z = (((x * inverse.m31) + (y * inverse.m32)) + (z * inverse.m33)) + (pd * inverse.m34);
    result.d = (((x * inverse.m41) + (y * inverse.m42)) + (z * inverse.m43)) + (pd * inverse.m44);
}

void CSPlane::shadow(const CSVector4& light, CSMatrix& result) const {
    float dot = (normal.x * light.x) + (normal.y * light.y) + (normal.z * light.z) + (d * light.w);
    float x = -normal.x;
    float y = -normal.y;
    float z = -normal.z;
    float d = -this->d;
    
    result.m11 = (x * light.x) + dot;
    result.m21 = y * light.x;
    result.m31 = z * light.x;
    result.m41 = d * light.x;
    result.m12 = x * light.y;
    result.m22 = (y * light.y) + dot;
    result.m32 = z * light.y;
    result.m42 = d * light.y;
    result.m13 = x * light.z;
    result.m23 = y * light.z;
    result.m33 = (z * light.z) + dot;
    result.m43 = d * light.z;
    result.m14 = x * light.w;
    result.m24 = y * light.w;
    result.m34 = z * light.w;
    result.m44 = (d * light.w) + dot;
}
