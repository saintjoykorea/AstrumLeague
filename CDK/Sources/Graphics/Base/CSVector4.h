//
//  CSVector4.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSVector4__
#define __CDK__CSVector4__

#include "CSMath.h"

#include "CSVector2.h"
#include "CSVector3.h"

struct CSQuaternion;
struct CSMatrix;

class CSBuffer;

struct CSVector4 {
public:
    static const CSVector4 Zero;
    
    float x, y, z, w;
    
    inline CSVector4() {
    
    }
    inline CSVector4(float x, float y, float z, float w)  : x(x), y(y), z(z), w(w) {
    
    }
    inline CSVector4(const CSVector2& other, float z, float w)  : x(other.x), y(other.y), z(z), w(w) {
    
    }
    inline CSVector4(const CSVector3& other, float w)  : x(other.x), y(other.y), z(other.z), w(w) {
    
    }
    CSVector4(CSBuffer* buffer);
    CSVector4(const byte*& raw);
    
    inline float lengthSquared() const {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }
    
    inline float length() const {
        return CSMath::sqrt(lengthSquared());
    }
    
    static inline void normalize(const CSVector4& vector, CSVector4& result) {
        float length = vector.length();
        result = length ? vector / length : vector;
    }
    static inline CSVector4 normalize(const CSVector4& vector) {
        float length = vector.length();
        return length ? vector / length : vector;
    }
    inline void normalize() {
        float length = this->length();
        if (length) {
            operator /=(length);
        }
    }
    
    static inline void clamp(const CSVector4& vector, const CSVector4& min, const CSVector4& max, CSVector4& result) {
        result.x = CSMath::clamp(vector.x, min.x, max.x);
        result.y = CSMath::clamp(vector.y, min.y, max.y);
        result.z = CSMath::clamp(vector.z, min.z, max.z);
        result.w = CSMath::clamp(vector.w, min.w, max.w);
    }
    static inline CSVector4 clamp(const CSVector4& vector, const CSVector4& min, const CSVector4& max) {
        CSVector4 result;
        clamp(vector, min, max, result);
        return result;
    }
    inline void clamp(const CSVector4& min, const CSVector4& max) {
        clamp(*this, min, max, *this);
    }
    
    static inline float distanceSquared(const CSVector4& left, const CSVector4& right) {
        float x = left.x - right.x;
        float y = left.y - right.y;
        float z = left.z - right.y;
        float w = left.w - right.w;
        return (x * x) + (y * y) + (z * z) + (w * w);
    }
    static inline float distance(const CSVector4& left, const CSVector4& right) {
        return CSMath::sqrt(distanceSquared(left, right));
    }
    inline float distanceSquared(const CSVector4& other) const {
        return distanceSquared(*this, other);
    }
    inline float distance(const CSVector4& other) const {
        return distance(*this, other);
    }
    
    static inline float dot(const CSVector4& left, const CSVector4& right) {
        return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) + (left.w * right.w);
    }
    
    static inline void lerp(const CSVector4& start, const CSVector4& end, float amount, CSVector4& result) {
        result.x = CSMath::lerp(start.x, end.x, amount);
        result.y = CSMath::lerp(start.y, end.y, amount);
        result.z = CSMath::lerp(start.z, end.z, amount);
        result.w = CSMath::lerp(start.w, end.w, amount);
    }
    static inline CSVector4 lerp(const CSVector4& start, const CSVector4& end, float amount) {
        CSVector4 result;
        lerp(start, end, amount, result);
        return result;
    }
    
    static inline void smoothStep(const CSVector4& start, const CSVector4& end, float amount, CSVector4& result) {
        amount = CSMath::smoothStep(amount);
        lerp(start, end, amount, result);
    }
    static inline CSVector4 smoothStep(const CSVector4& start, const CSVector4& end, float amount) {
        CSVector4 result;
        smoothStep(start, end, amount, result);
        return result;
    }
    
    static void hermite(const CSVector4& vector1, const CSVector4& tangent1, const CSVector4& vector2, const CSVector4& tangent2, float amount, CSVector4& result);
    static inline CSVector4 hermite(const CSVector4& vector1, const CSVector4& tangent1, const CSVector4& vector2, const CSVector4& tangent2, float amount) {
        CSVector4 result;
        hermite(vector1, tangent1, vector2, tangent2, amount, result);
        return result;
    }
    
    static inline void max(const CSVector4& left, const CSVector4& right, CSVector4& result) {
        result.x = CSMath::max(left.x, right.x);
        result.y = CSMath::max(left.y, right.y);
        result.z = CSMath::max(left.z, right.z);
        result.w = CSMath::max(left.w, right.w);
    }
    static inline CSVector4 max(const CSVector4& left, const CSVector4 right) {
        CSVector4 result;
        max(left, right, result);
        return result;
    }
    
    static inline void min(const CSVector4& left, const CSVector4& right, CSVector4& result) {
        result.x = CSMath::min(left.x, right.x);
        result.y = CSMath::min(left.y, right.y);
        result.z = CSMath::min(left.z, right.z);
        result.w = CSMath::min(left.w, right.w);
    }
    static inline CSVector4 min(const CSVector4& left, const CSVector4 right) {
        CSVector4 result;
        min(left, right, result);
        return result;
    }
    
    static void transform(const CSVector4& vector, const CSQuaternion& rotation, CSVector4& result);
    static void transform(const CSVector4& vector, const CSMatrix& trans, CSVector4& result);
    
    static inline CSVector4 transform(const CSVector4& vector, const CSQuaternion& rotation) {
        CSVector4 result;
        transform(vector, rotation, result);
        return result;
    }
    static inline CSVector4 transform(const CSVector4& vector, const CSMatrix& trans) {
        CSVector4 result;
        transform(vector, trans, result);
        return result;
    }
    inline void transform(const CSQuaternion& rotation) {
        transform(*this, rotation, *this);
    }
    inline void transform(const CSMatrix& trans) {
        transform(*this, trans, *this);
    }
    
    inline CSVector4 operator +(const CSVector4& vector) const {
        return CSVector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
    }
    
    inline CSVector4 operator +(float scalar) const {
        return CSVector4(x + scalar, y + scalar, z + scalar, w + scalar);
    }
    
    inline CSVector4 operator -(const CSVector4& vector) const {
        return CSVector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
    }
    
    inline CSVector4 operator -(float scalar) const {
        return CSVector4(x - scalar, y - scalar, z - scalar, w - scalar);
    }
    
    inline CSVector4 operator *(const CSVector4& vector) const {
        return CSVector4(x * vector.x, y * vector.y, z * vector.z, w * vector.w);
    }
    
    inline CSVector4 operator *(float scale) const {
        return CSVector4(x * scale, y * scale, z * scale, w * scale);
    }
    
    inline CSVector4 operator /(const CSVector4& vector) const {
        return CSVector4(x / vector.x, y / vector.y, z / vector.z, w / vector.w);
    }
    
    inline CSVector4 operator /(float scale) const {
        float inv = 1.0f / scale;
        return CSVector4(x * inv, y * inv, z * inv, w * inv);
    }
    
    inline CSVector4 operator -() const {
        return CSVector4(-x, -y, -z, -w);
    }
    
    inline CSVector4& operator +=(const CSVector4& vector) {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        w += vector.w;
        return *this;
    }
    
    inline CSVector4& operator +=(float scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }
    
    inline CSVector4& operator -=(const CSVector4& vector) {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        w -= vector.w;
        return *this;
    }
    
    inline CSVector4& operator -=(float scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }
    
    inline CSVector4& operator *=(const CSVector4& vector) {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        w *= vector.w;
        return *this;
    }
    
    inline CSVector4& operator *=(float scale) {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;
        return *this;
    }
    
    inline CSVector4& operator /=(const CSVector4& vector) {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        w /= vector.w;
        return *this;
    }
    
    inline CSVector4& operator /=(float scale) {
        float inv = 1.0f / scale;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }
    
    inline operator const float*() const {
        return &x;
    }
    
    inline explicit operator CSVector2() const {
        return CSVector2(x, y);
    }
    
    inline explicit operator CSVector3() const {
        return CSVector3(x, y, z);
    }
    
    inline explicit operator uint() const {          //hash
        return (((int)x & 0xff) << 24) | (((int)y & 0xff) << 16) | (((int)z & 0xff) << 8) | ((int)w & 0xff);
    }
    
    inline bool operator ==(const CSVector4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    
    inline bool operator !=(const CSVector4& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSVector4__ */
