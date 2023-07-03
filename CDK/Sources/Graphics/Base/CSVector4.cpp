//
//  CSVector4.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSVector4.h"

#include "CSQuaternion.h"
#include "CSMatrix.h"
#include "CSBuffer.h"

const CSVector4 CSVector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);

CSVector4::CSVector4(CSBuffer* buffer) :
    x(buffer->readFloat()),
    y(buffer->readFloat()),
    z(buffer->readFloat()),
    w(buffer->readFloat())
{
}

CSVector4::CSVector4(const byte*& raw) :
    x(readFloat(raw)),
    y(readFloat(raw)),
    z(readFloat(raw)),
    w(readFloat(raw))
{
}

void CSVector4::hermite(const CSVector4& vector1, const CSVector4& tangent1, const CSVector4& vector2, const CSVector4& tangent2, float amount, CSVector4& result) {
    float squared = amount * amount;
    float cubed = amount * squared;
    float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
    float part2 = (-2.0f * cubed) + (3.0f * squared);
    float part3 = (cubed - (2.0f * squared)) + amount;
    float part4 = cubed - squared;
    
    result = CSVector4((vector1.x * part1) + (vector2.x * part2) + (tangent1.x * part3) + (tangent2.x * part4),
                       (vector1.y * part1) + (vector2.y * part2) + (tangent1.y * part3) + (tangent2.y * part4),
                       (vector1.z * part1) + (vector2.z * part2) + (tangent1.z * part3) + (tangent2.z * part4),
                       (vector1.w * part1) + (vector2.w * part2) + (tangent1.w * part3) + (tangent2.w * part4));
}

void CSVector4::transform(const CSVector4& vector, const CSQuaternion& rotation, CSVector4& result) {
    float rx = rotation.x + rotation.x;
    float ry = rotation.y + rotation.y;
    float rz = rotation.z + rotation.z;
    float wx = rotation.w * rx;
    float wy = rotation.w * ry;
    float wz = rotation.w * rz;
    float xx = rotation.x * rx;
    float xy = rotation.x * ry;
    float xz = rotation.x * rz;
    float yy = rotation.y * ry;
    float yz = rotation.y * rz;
    float zz = rotation.z * rz;
    
    result = CSVector4(((vector.x * ((1.0f - yy) - zz)) + (vector.y * (xy - wz))) + (vector.z * (xz + wy)),
                       ((vector.x * (xy + wz)) + (vector.y * ((1.0f - xx) - zz))) + (vector.z * (yz - wx)),
                       ((vector.x * (xz - wy)) + (vector.y * (yz + wx))) + (vector.z * ((1.0f - xx) - yy)),
                       vector.w);
}

void CSVector4::transform(const CSVector4& vector, const CSMatrix& trans, CSVector4& result) {
    result = CSVector4((vector.x * trans.m11) + (vector.y * trans.m21) + (vector.z * trans.m31) + (vector.w * trans.m41),
                       (vector.x * trans.m12) + (vector.y * trans.m22) + (vector.z * trans.m32) + (vector.w * trans.m42),
                       (vector.x * trans.m13) + (vector.y * trans.m23) + (vector.z * trans.m33) + (vector.w * trans.m43),
                       (vector.x * trans.m14) + (vector.y * trans.m24) + (vector.z * trans.m34) + (vector.w * trans.m44));
}
