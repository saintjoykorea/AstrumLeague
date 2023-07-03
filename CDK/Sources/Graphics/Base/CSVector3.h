//
//  CSVector3.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSVector3__
#define __CDK__CSVector3__

#include "CSVector2.h"

struct CSVector4;
struct CSMatrix;
struct CSQuaternion;

class CSBuffer;

struct CSVector3 {
public:
    static const CSVector3 Zero;
	static const CSVector3 One;
	static const CSVector3 UnitX;
	static const CSVector3 UnitY;
	static const CSVector3 UnitZ;
    
    float x;
    float y;
    float z;
    
    inline CSVector3() {
    
    }
    
    inline CSVector3(float x, float y, float z) : x(x), y(y), z(z) {
    
    }
    
    inline CSVector3(const CSVector2& other, float z) : x(other.x), y(other.y), z(z) {
    
    }
    
    CSVector3(CSBuffer* buffer);
    CSVector3(const byte*& raw);
    
    inline float lengthSquared() const {
        return (x * x) + (y * y) + (z * z);
    }
    
    inline float length() const {
        return CSMath::sqrt(lengthSquared());
    }
    
    static inline void normalize(const CSVector3& vector, CSVector3& result) {
        float length = vector.length();
        result = length ? vector / length : vector;
    }
    static inline CSVector3 normalize(const CSVector3& vector) {
        float length = vector.length();
        return length ? vector / length : vector;
    }
    inline void normalize() {
        float length = this->length();
        if (length) {
            operator /=(length);
        }
    }
    
    static inline void clamp(const CSVector3& vector, const CSVector3& min, const CSVector3& max, CSVector3& result) {
        result.x = CSMath::clamp(vector.x, min.x, max.x);
        result.y = CSMath::clamp(vector.y, min.y, max.y);
        result.z = CSMath::clamp(vector.z, min.z, max.z);
    }
    static inline CSVector3 clamp(const CSVector3& vector, const CSVector3& min, const CSVector3& max) {
        CSVector3 result;
        clamp(vector, min, max, result);
        return result;
    }
    inline void clamp(const CSVector3& min, const CSVector3& max) {
        clamp(*this, min, max, *this);
    }
    
    static inline void cross(const CSVector3& left, const CSVector3& right, CSVector3& result) {
        float x = (left.y * right.z) - (left.z * right.y);
        float y = (left.z * right.x) - (left.x * right.z);
        float z = (left.x * right.y) - (left.y * right.x);

		result.x = x;
		result.y = y;
		result.z = z;
    }
    static inline CSVector3 cross(const CSVector3& left, const CSVector3& right) {
        CSVector3 result;
        cross(left, right, result);
        return result;
    }
    
    static inline float distanceSquared(const CSVector3& vector1, const CSVector3& vector2) {
        float x = vector1.x - vector2.x;
        float y = vector1.y - vector2.y;
        float z = vector1.z - vector2.z;
        return (x * x) + (y * y) + (z * z);
    }
    static inline float distance(const CSVector3& left, const CSVector3& right) {
        return CSMath::sqrt(distanceSquared(left, right));
    }
    inline float distanceSquared(const CSVector3& other) const {
        return distanceSquared(*this, other);
    }
    inline float distance(const CSVector3& other) const {
        return distance(*this, other);
    }
    
    static inline float dot(const CSVector3& left, const CSVector3& right) {
        return (left.x * right.x) + (left.y * right.y) + (left.z * right.z);
    }
    
    static inline void lerp(const CSVector3& start, const CSVector3& end, float amount, CSVector3& result) {
        result.x = CSMath::lerp(start.x, end.x, amount);
        result.y = CSMath::lerp(start.y, end.y, amount);
        result.z = CSMath::lerp(start.z, end.z, amount);
    }
    static inline CSVector3 lerp(const CSVector3& start, const CSVector3& end, float amount) {
        CSVector3 result;
        lerp(start, end, amount, result);
        return result;
    }
    
    static inline void smoothStep(const CSVector3& start, const CSVector3& end, float amount, CSVector3& result) {
        amount = CSMath::smoothStep(amount);
        lerp(start, end, amount, result);
    }
    static inline CSVector3 smoothStep(const CSVector3& start, const CSVector3& end, float amount) {
        CSVector3 result;
        smoothStep(start, end, amount, result);
        return result;
    }
    
    static void hermite(const CSVector3& vector1, const CSVector3& tangent1, const CSVector3& vector2, const CSVector3& tangent2, float amount, CSVector3& result);
    static inline CSVector3 hermite(const CSVector3& vector1, const CSVector3& tangent1, const CSVector3& vector2, const CSVector3& tangent2, float amount) {
        CSVector3 result;
        hermite(vector1, tangent1, vector2, tangent2, amount, result);
        return result;
    }
    
    static inline void max(const CSVector3& left, const CSVector3& right, CSVector3& result) {
        result.x = CSMath::max(left.x, right.x);
        result.y = CSMath::max(left.y, right.y);
        result.z = CSMath::max(left.z, right.z);
    }
    static inline CSVector3 max(const CSVector3& left, const CSVector3 right) {
        CSVector3 result;
        max(left, right, result);
        return result;
    }
    
    static inline void min(const CSVector3& left, const CSVector3& right, CSVector3& result) {
        result.x = CSMath::min(left.x, right.x);
        result.y = CSMath::min(left.y, right.y);
        result.z = CSMath::min(left.z, right.z);
    }
    static inline CSVector3 min(const CSVector3& left, const CSVector3 right) {
        CSVector3 result;
        min(left, right, result);
        return result;
    }
    
    static void project(const CSVector3& vector, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection, CSVector3& result);
    static inline CSVector3 project(const CSVector3& vector, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection) {
        CSVector3 result;
        project(vector, x, y, width, height, znear, zfar, worldViewProjection, result);
        return result;
    }
    static void unproject(const CSVector3& vector, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection, CSVector3& result);
    static inline CSVector3 unproject(const CSVector3& vector, float x, float y, float width, float height, float znear, float zfar, const CSMatrix& worldViewProjection) {
        CSVector3 result;
        unproject(vector, x, y, width, height, znear, zfar, worldViewProjection, result);
        return result;
    }
    
    static inline void reflect(const CSVector3& vector, const CSVector3& normal, CSVector3& result) {
        float dot = CSVector3::dot(vector, normal);
        result.x = vector.x - ((2.0f * dot) * normal.x);
        result.y = vector.y - ((2.0f * dot) * normal.y);
        result.z = vector.z - ((2.0f * dot) * normal.z);
    }
    static inline CSVector3 reflect(const CSVector3& vector, const CSVector3& normal) {
        CSVector3 result;
        reflect(vector, normal, result);
        return result;
    }
    
    static void transform(const CSVector3& vector, const CSQuaternion& rotation, CSVector3& result);
    static void transform(const CSVector3& vector, const CSMatrix& trans, CSVector4& result);
    static void transform(const CSVector3& vector, const CSMatrix& trans, CSVector3& result);
    static void transformCoordinate(const CSVector3& coordinate, const CSMatrix& trans, CSVector3& result);
    static void transformNormal(const CSVector3& normal, const CSMatrix& trans, CSVector3& result);
    
    static inline CSVector3 transform(const CSVector3& vector, const CSQuaternion& rotation) {
        CSVector3 result;
        transform(vector, rotation, result);
        return result;
    }
    static CSVector4 transform(const CSVector3& vector, const CSMatrix& trans);
    static inline CSVector3 transformCoordinate(const CSVector3& coordinate, const CSMatrix& trans) {
        CSVector3 result;
        transformCoordinate(coordinate, trans, result);
        return result;
    }
    static inline CSVector3 transformNormal(const CSVector3& normal, const CSMatrix& trans) {
        CSVector3 result;
        transformNormal(normal, trans, result);
        return result;
    }
    
    inline void transform(const CSQuaternion& rotation) {
        transform(*this, rotation, *this);
    }
    inline void transformCoordinate(const CSMatrix& trans) {
        transformCoordinate(*this, trans, *this);
    }
    inline void transformNormal(const CSMatrix& trans) {
        transformNormal(*this, trans, *this);
    }
    
    inline CSVector3 operator +(const CSVector3& vector) const {
        return CSVector3(x + vector.x, y + vector.y, z + vector.z);
    }
    
    inline CSVector3 operator +(float scalar) const {
        return CSVector3(x + scalar, y + scalar, z + scalar);
    }
    
    inline CSVector3 operator -(const CSVector3& vector) const {
        return CSVector3(x - vector.x, y - vector.y, z - vector.z);
    }
    
    inline CSVector3 operator -(float scalar) const {
        return CSVector3(x - scalar, y - scalar, z - scalar);
    }
    
    inline CSVector3 operator *(const CSVector3& vector) const {
        return CSVector3(x * vector.x, y * vector.y, z * vector.z);
    }
    
    inline CSVector3 operator *(float scale) const {
        return CSVector3(x * scale, y * scale, z * scale);
    }
    
    inline CSVector3 operator /(const CSVector3& vector) const {
        return CSVector3(x / vector.x, y / vector.y,  z / vector.z);
    }
    
    inline CSVector3 operator /(float scale) const {
        float inv = 1.0f / scale;
        return CSVector3(x * inv, y * inv, z * inv);
    }
    
    inline CSVector3 operator -() const {
        return CSVector3(-x, -y, -z);
    }
    
    inline CSVector3& operator +=(const CSVector3& vector) {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }
    
    inline CSVector3& operator +=(float scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }
    
    inline CSVector3& operator -=(const CSVector3& vector) {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }
    
    inline CSVector3& operator -=(float scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }
    
    inline CSVector3& operator *=(const CSVector3& vector) {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        return *this;
    }
    
    inline CSVector3& operator *=(float scale) {
        x *= scale;
        y *= scale;
        z *= scale;
        return *this;
    }
    
    inline CSVector3& operator /=(const CSVector3& vector) {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        return *this;
    }
    
    inline CSVector3& operator /=(float scale) {
        float inv = 1.0f / scale;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    
    inline operator const float*() const {
        return &x;
    }
    
    inline explicit operator CSVector2() const {
        return CSVector2(x, y);
    }
    
    inline explicit operator uint() const {          //hash
        return (((int)x & 0xff) << 16) | (((int)y & 0xff) << 8) | ((int)z & 0xff);
    }
    
    inline bool operator ==(const CSVector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    inline bool operator !=(const CSVector3& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSVector3__ */
