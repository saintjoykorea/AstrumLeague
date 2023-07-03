//
//  CSVector2.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 5..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSVector2__
#define __CDK__CSVector2__

#include "CSMath.h"

struct CSMatrix;
struct CSVector3;
struct CSVector4;
struct CSQuaternion;

class CSBuffer;

struct CSVector2 {
public:
    static const CSVector2 Zero;
    
    float x, y;
    
    inline CSVector2() {
    
    }
    
    inline CSVector2(float x, float y) : x(x), y(y) {
    
    }
    
    CSVector2(CSBuffer* buffer);
    CSVector2(const byte*& raw);
    
    inline float lengthSquared() const {
        return (x * x) + (y * y);
    }
    
    inline float length() const {
        return CSMath::sqrt(lengthSquared());
    }
    
    static inline void normalize(const CSVector2& vector, CSVector2& result) {
        float length = vector.length();
        result = length ? vector / length : vector;
    }
    static inline CSVector2 normalize(const CSVector2& vector) {
        float length = vector.length();
        return length ? vector / length : vector;
    }
    inline void normalize() {
        float length = this->length();
        if (length) {
            operator /=(length);
        }
    }
    
    static inline void clamp(const CSVector2& vector, const CSVector2& min, const CSVector2& max, CSVector2& result) {
        result.x = CSMath::clamp(vector.x, min.x, max.x);
        result.y = CSMath::clamp(vector.y, min.y, max.y);
    }
    static inline CSVector2 clamp(const CSVector2& vector, const CSVector2& min, const CSVector2& max) {
        CSVector2 result;
        clamp(vector, min, max, result);
        return result;
    }
    inline void clamp(const CSVector2& min, const CSVector2& max) {
        clamp(*this, min, max, *this);
    }
    
    static inline float distanceSquared(const CSVector2& left, const CSVector2& right) {
        float x = left.x - right.x;
        float y = left.y - right.y;
        return (x * x) + (y * y);
    }
    static inline float distance(const CSVector2& left, const CSVector2& right) {
        return CSMath::sqrt(distanceSquared(left, right));
    }
    inline float distanceSquared(const CSVector2& other) const {
        return distanceSquared(*this, other);
    }
    inline float distance(const CSVector2& other) const {
        return distance(*this, other);
    }
    
    static inline float dot(const CSVector2& left, const CSVector2& right) {
        return (left.x * right.x) + (left.y * right.y);
    }
    
    static inline void lerp(const CSVector2& start, const CSVector2& end, float amount, CSVector2& result) {
        result.x = CSMath::lerp(start.x, end.x, amount);
        result.y = CSMath::lerp(start.y, end.y, amount);
    }
    static inline CSVector2 lerp(const CSVector2& start, const CSVector2& end, float amount) {
        CSVector2 result;
        lerp(start, end, amount, result);
        return result;
    }
    static inline void smoothStep(const CSVector2& start, const CSVector2& end, float amount, CSVector2& result) {
        amount = CSMath::smoothStep(amount);
        lerp(start, end, amount, result);
    }
    static inline CSVector2 smoothStep(const CSVector2& start, const CSVector2& end, float amount) {
        CSVector2 result;
        smoothStep(start, end, amount, result);
        return result;
    }
    static void hermite(const CSVector2& vector1, const CSVector2& tangent1, const CSVector2& vector2, const CSVector2& tangent2, float amount, CSVector2& result);
    static inline CSVector2 hermite(const CSVector2& vector1, const CSVector2& tangent1, const CSVector2& vector2, const CSVector2& tangent2, float amount) {
        CSVector2 result;
        hermite(vector1, tangent1, vector2, tangent2, amount, result);
        return result;
    }
    
    static inline void max(const CSVector2& left, const CSVector2 right, CSVector2& result) {
        result.x = CSMath::max(left.x, right.x);
        result.y = CSMath::max(left.y, right.y);
    }
    static inline CSVector2 max(const CSVector2& left, const CSVector2 right) {
        CSVector2 result;
        max(left, right, result);
        return result;
    }
    
    static inline void min(const CSVector2& left, const CSVector2& right, CSVector2& result) {
        result.x = CSMath::min(left.x, right.x);
        result.y = CSMath::min(left.y, right.y);
    }
    static inline CSVector2 min(const CSVector2& left, const CSVector2 right) {
        CSVector2 result;
        min(left, right, result);
        return result;
    }
    
    static void transform(const CSVector2& vector, const CSQuaternion& rotation, CSVector2& result);
    static void transform(const CSVector2& vector, const CSMatrix& trans, CSVector4& result);
    static void transformCoordinate(const CSVector2& coordinate, const CSMatrix& trans, CSVector2& result);
    static void transformNormal(const CSVector2& normal, const CSMatrix& trans, CSVector2& result);
    
    static inline CSVector2 transform(const CSVector2& vector, const CSQuaternion& rotation) {
        CSVector2 result;
        transform(vector, rotation, result);
        return result;
    }
    static CSVector4 transform(const CSVector2& vector, const CSMatrix& trans);
    
    static inline CSVector2 transformCoordinate(const CSVector2& coordinate, const CSMatrix& trans) {
        CSVector2 result;
        transformCoordinate(coordinate, trans, result);
        return result;
    }
    static inline CSVector2 transformNormal(const CSVector2& normal, const CSMatrix& trans) {
        CSVector2 result;
        transformNormal(normal, trans, result);
        return result;
    }
    inline void transformCoordinate(const CSMatrix& trans) {
        transformCoordinate(*this, trans, *this);
    }
    inline void transformNormal(const CSMatrix& trans) {
        transformNormal(*this, trans, *this);
    }
    inline void transform(const CSQuaternion& rotation) {
        transform(*this, rotation, *this);
    }
    
    inline CSVector2 operator +(const CSVector2& vector) const {
        return CSVector2(x + vector.x, y + vector.y);
    }
    
    inline CSVector2 operator +(float scalar) const {
        return CSVector2(x + scalar, y + scalar);
    }
    
    inline CSVector2 operator -(const CSVector2& vector) const {
        return CSVector2(x - vector.x, y - vector.y);
    }
    
    inline CSVector2 operator -(float scalar) const {
        return CSVector2(x - scalar, y - scalar);
    }
    
    inline CSVector2 operator *(const CSVector2& vector) const {
        return CSVector2(x * vector.x, y * vector.y);
    }
    
    inline CSVector2 operator *(float scale) const {
        return CSVector2(x * scale, y * scale);
    }
    
    inline CSVector2 operator /(const CSVector2& vector) const {
        return CSVector2(x / vector.x, y / vector.y);
    }
    
    inline CSVector2 operator /(float scale) const {
        float inv = 1.0f / scale;
        return CSVector2(x * inv, y * inv);
    }
    
    inline CSVector2 operator -() const {
        return CSVector2(-x, -y);
    }
    
    inline CSVector2& operator +=(const CSVector2& vector) {
        x += vector.x;
        y += vector.y;
        return *this;
    }
    
    inline CSVector2& operator +=(float scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    
    inline CSVector2& operator -=(const CSVector2& vector) {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }
    
    inline CSVector2& operator -=(float scalar) {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    
    inline CSVector2& operator *=(const CSVector2& vector) {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    
    inline CSVector2& operator *=(float scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    
    inline CSVector2& operator /=(const CSVector2& vector) {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    
    inline CSVector2& operator /=(float scale) {
        float inv = 1.0f / scale;
        x *= inv;
        y *= inv;
        return *this;
    }
    
    inline operator const float*() const {
        return &x;
    }
    
    operator CSVector3() const;
    
    inline explicit operator uint() const {          //hash
        return (((int)x & 0xffff) << 16) | ((int)y & 0xffff);
    }
    
    inline bool operator ==(const CSVector2& other) const {
        return x == other.x && y == other.y;
    }
    
    inline bool operator !=(const CSVector2& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSVector2__ */
