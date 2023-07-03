//
//  CSVector3.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSVector3.h"

#include "CSVector4.h"
#include "CSQuaternion.h"
#include "CSMatrix.h"
#include "CSBuffer.h"

const CSVector3 CSVector3::Zero(0.0f, 0.0f, 0.0f);
const CSVector3 CSVector3::One(1.0f, 1.0f, 1.0f);
const CSVector3 CSVector3::UnitX(1.0f, 0.0f, 0.0f);
const CSVector3 CSVector3::UnitY(0.0f, 1.0f, 0.0f);
const CSVector3 CSVector3::UnitZ(0.0f, 0.0f, 1.0f);

CSVector3::CSVector3(CSBuffer* buffer) :
    x(buffer->readFloat()),
    y(buffer->readFloat()),
    z(buffer->readFloat())
{
}

CSVector3::CSVector3(const byte*& raw) :
    x(readFloat(raw)),
    y(readFloat(raw)),
    z(readFloat(raw))
{
}

void CSVector3::hermite(const CSVector3& vector1, const CSVector3& tangent1, const CSVector3& vector2, const CSVector3& tangent2, float amount, CSVector3& result) {
    float squared = amount * amount;
    float cubed = amount * squared;
    float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
    float part2 = (-2.0f * cubed) + (3.0f * squared);
    float part3 = (cubed - (2.0f * squared)) + amount;
    float part4 = cubed - squared;
    
    result = CSVector3((vector1.x * part1) + (vector2.x * part2) + (tangent1.x * part3) + (tangent2.x * part4),
                       (vector1.y * part1) + (vector2.y * part2) + (tangent1.y * part3) + (tangent2.y * part4),
                       (vector1.z * part1) + (vector2.z * part2) + (tangent1.z * part3) + (tangent2.z * part4));
}

void CSVector3::project(const CSVector3& vector, float viewportX, float viewportY, float width, float height, float minZ, float maxZ, const CSMatrix& worldViewProjection, CSVector3& result) {
    CSVector3 v;
    transformCoordinate(vector, worldViewProjection, v);
    result.x = ((1.0f + v.x) * 0.5f * width) + viewportX;
    result.y = ((1.0f - v.y) * 0.5f * height) + viewportY;
    result.z = (v.z * (maxZ - minZ)) + minZ;
}

void CSVector3::unproject(const CSVector3& vector, float x, float y, float width, float height, float minZ, float maxZ, const CSMatrix& worldViewProjection, CSVector3& result) {
    CSVector3 v;
    CSMatrix matrix = worldViewProjection;
    matrix.invert();
    
    v.x = (((vector.x - x) / width) * 2.0f) - 1.0f;
    v.y = -((((vector.y - y) / height) * 2.0f) - 1.0f);
    v.z = (vector.z - minZ) / (maxZ - minZ);
    
    transformCoordinate(v, matrix, result);
}

void CSVector3::transform(const CSVector3& vector, const CSQuaternion& rotation, CSVector3& result) {
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
    
    result = CSVector3(((vector.x * ((1.0f - yy) - zz)) + (vector.y * (xy - wz))) + (vector.z * (xz + wy)),
                       ((vector.x * (xy + wz)) + (vector.y * ((1.0f - xx) - zz))) + (vector.z * (yz - wx)),
                       ((vector.x * (xz - wy)) + (vector.y * (yz + wx))) + (vector.z * ((1.0f - xx) - yy)));
}

void CSVector3::transform(const CSVector3& vector, const CSMatrix& trans, CSVector3& result) {
    CSVector4 vec4;
    transform(vector, trans, vec4);
    result = (CSVector3)vec4;
}

void CSVector3::transform(const CSVector3& vector, const CSMatrix& trans, CSVector4& result) {
    result = CSVector4((vector.x * trans.m11) + (vector.y * trans.m21) + (vector.z * trans.m31) + trans.m41,
                       (vector.x * trans.m12) + (vector.y * trans.m22) + (vector.z * trans.m32) + trans.m42,
                       (vector.x * trans.m13) + (vector.y * trans.m23) + (vector.z * trans.m33) + trans.m43,
                       (vector.x * trans.m14) + (vector.y * trans.m24) + (vector.z * trans.m34) + trans.m44);
}

CSVector4 CSVector3::transform(const CSVector3& vector, const CSMatrix& trans) {
    CSVector4 result;
    transform(vector, trans, result);
    return result;
}

void CSVector3::transformCoordinate(const CSVector3& coordinate, const CSMatrix& trans, CSVector3& result) {
    CSVector4 vector;
    vector.x = (coordinate.x * trans.m11) + (coordinate.y * trans.m21) + (coordinate.z * trans.m31) + trans.m41;
    vector.y = (coordinate.x * trans.m12) + (coordinate.y * trans.m22) + (coordinate.z * trans.m32) + trans.m42;
    vector.z = (coordinate.x * trans.m13) + (coordinate.y * trans.m23) + (coordinate.z * trans.m33) + trans.m43;
    vector.w = 1.0f / ((coordinate.x * trans.m14) + (coordinate.y * trans.m24) + (coordinate.z * trans.m34) + trans.m44);
    
    result.x = vector.x * vector.w;
    result.y = vector.y * vector.w;
    result.z = vector.z * vector.w;
}

void CSVector3::transformNormal(const CSVector3& normal, const CSMatrix& trans, CSVector3& result) {
    result = CSVector3((normal.x * trans.m11) + (normal.y * trans.m21) + (normal.z * trans.m31),
                       (normal.x * trans.m12) + (normal.y * trans.m22) + (normal.z * trans.m32),
                       (normal.x * trans.m13) + (normal.y * trans.m23) + (normal.z * trans.m33));
}
