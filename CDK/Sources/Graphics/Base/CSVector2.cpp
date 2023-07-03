//
//  CSVector2.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 5..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSVector2.h"

#include "CSVector4.h"
#include "CSQuaternion.h"
#include "CSMatrix.h"

#include "CSBuffer.h"

const CSVector2 CSVector2::Zero(0.0f, 0.0f);

CSVector2::CSVector2(CSBuffer* buffer) :
    x(buffer->readFloat()),
    y(buffer->readFloat())
{
}

CSVector2::CSVector2(const byte*& raw) :
    x(readFloat(raw)),
    y(readFloat(raw))
{
}

void CSVector2::hermite(const CSVector2& vector1, const CSVector2& tangent1, const CSVector2& vector2, const CSVector2& tangent2, float amount, CSVector2& result) {
    float squared = amount * amount;
    float cubed = amount * squared;
    float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
    float part2 = (-2.0f * cubed) + (3.0f * squared);
    float part3 = (cubed - (2.0f * squared)) + amount;
    float part4 = cubed - squared;
    
    result = CSVector2((vector1.x * part1) + (vector2.x * part2) + (tangent1.x * part3) + (tangent2.x * part4),
                       (vector1.y * part1) + (vector2.y * part2) + (tangent1.y * part3) + (tangent2.y * part4));
}

void CSVector2::transform(const CSVector2& vector, const CSQuaternion& rotation, CSVector2& result) {
    float x = rotation.x + rotation.x;
    float y = rotation.y + rotation.y;
    float z = rotation.z + rotation.z;
    float wz = rotation.w * z;
    float xx = rotation.x * x;
    float xy = rotation.x * y;
    float yy = rotation.y * y;
    float zz = rotation.z * z;
    
    result = CSVector2((vector.x * (1.0f - yy - zz)) + (vector.y * (xy - wz)),
                       (vector.x * (xy + wz)) + (vector.y * (1.0f - xx - zz)));
}

void CSVector2::transform(const CSVector2& vector, const CSMatrix& trans, CSVector4& result) {
    result = CSVector4((vector.x * trans.m11) + (vector.y * trans.m21) + trans.m41,
                       (vector.x * trans.m12) + (vector.y * trans.m22) + trans.m42,
                       (vector.x * trans.m13) + (vector.y * trans.m23) + trans.m43,
                       (vector.x * trans.m14) + (vector.y * trans.m24) + trans.m44);
}

CSVector4 CSVector2::transform(const CSVector2& vector, const CSMatrix& trans) {
    CSVector4 result;
    transform(vector, trans, result);
    return result;
}

void CSVector2::transformCoordinate(const CSVector2& coordinate, const CSMatrix& trans, CSVector2& result) {
    CSVector4 vector;
    vector.x = (coordinate.x * trans.m11) + (coordinate.y * trans.m21) + trans.m41;
    vector.y = (coordinate.x * trans.m12) + (coordinate.y * trans.m22) + trans.m42;
    vector.z = (coordinate.x * trans.m13) + (coordinate.y * trans.m23) + trans.m43;
    vector.w = 1.0f / ((coordinate.x * trans.m14) + (coordinate.y * trans.m24) + trans.m44);
    
    result.x = vector.x * vector.w;
    result.y = vector.y * vector.w;
}

void CSVector2::transformNormal(const CSVector2& normal, const CSMatrix& trans, CSVector2& result) {
    result = CSVector2((normal.x * trans.m11) + (normal.y * trans.m21),
                       (normal.x * trans.m12) + (normal.y * trans.m22));
}

CSVector2::operator CSVector3() const {
    return CSVector3(x, y, 0);
}
