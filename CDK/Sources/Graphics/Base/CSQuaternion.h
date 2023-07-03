//
//  CSQuaternion.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 7..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSQuaternion__
#define __CDK__CSQuaternion__

#include "CSVector4.h"

struct CSMatrix;

class CSBuffer;
struct CSQuaternion {
public:
    static const CSQuaternion Identity;
    
    float x, y, z, w;
    
    inline CSQuaternion() {
    
    }
    inline CSQuaternion(float x, float y, float z, float w) : x(x) , y(y), z(z), w(w) {
    
    }
    inline CSQuaternion(const CSVector2& vector, float z, float w) : x(vector.x), y(vector.y), z(z), w(w) {
    
    }
    inline CSQuaternion(const CSVector3& vector, float w) : x(vector.x) , y(vector.y), z(vector.z), w(w) {
    
    }
    inline CSQuaternion(const CSVector4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {
    
    }
    
    CSQuaternion(CSBuffer* buffer);
    
    float angle() const;
    CSVector3 axis() const;
    
    inline void conjugate() {
        x = -x;
        y = -y;
        z = -z;
    }
    
    void invert();
    
    inline float length() const {
        return CSMath::sqrt((x * x) + (y * y) + (z * z) + (w * w));
    }
    
    inline float lengthSquared() const {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }
    
    static void normalize(const CSQuaternion& qt, CSQuaternion& result);
    static inline CSQuaternion normalize(const CSQuaternion& qt) {
        CSQuaternion result;
        normalize(qt, result);
        return result;
    }
    void normalize();
    
    static inline CSQuaternion conjugate(const CSQuaternion& qt) {
        return CSQuaternion(-qt.x, -qt.y, -qt.z, qt.w);
    }
    
    static inline float dot(const CSQuaternion& left, const CSQuaternion& right) {
        return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) + (left.w * right.w);
    }
    
    static void lerp(const CSQuaternion& start, const CSQuaternion& end, float amount, CSQuaternion& result);
    static inline CSQuaternion lerp(const CSQuaternion& start, const CSQuaternion& end, float amount) {
        CSQuaternion result;
        lerp(start, end, amount, result);
        return result;
    }
    
    static void rotationAxis(const CSVector3& axis, float angle, CSQuaternion& result);
    static inline CSQuaternion rotationAxis(const CSVector3& axis, float angle) {
        CSQuaternion result;
        rotationAxis(axis, angle, result);
        return result;
    }
    
    
    static void rotationMatrix(const CSMatrix& matrix, CSQuaternion& result);
    static inline CSQuaternion rotationMatrix(const CSMatrix& matrix) {
        CSQuaternion result;
        rotationMatrix(matrix, result);
        return result;
    }
    
    static void lookAtLH(const CSVector3& eye, const CSVector3& target, const CSVector3& up, CSQuaternion& result);
    static inline CSQuaternion lookAtLH(const CSVector3& eye, const CSVector3& target, const CSVector3& up) {
        CSQuaternion result;
        lookAtLH(eye, target, up, result);
        return result;
    }
    
    static inline void rotationLookAtLH(const CSVector3& forward, const CSVector3& up, CSQuaternion& result) {
        CSVector3 eye;
        lookAtLH(eye, forward, up, result);
    }
    static inline CSQuaternion rotationLookAtLH(const CSVector3& forward, const CSVector3& up) {
        CSQuaternion result;
        rotationLookAtLH(forward, up, result);
        return result;
    }
    
    static void lookAtRH(const CSVector3& eye, const CSVector3& target, const CSVector3& up, CSQuaternion& result);
    static inline CSQuaternion lookAtRH(const CSVector3& eye, const CSVector3& target, const CSVector3& up) {
        CSQuaternion result;
        lookAtRH(eye, target, up, result);
        return result;
    }
    
    static inline void rotationLookAtRH(const CSVector3& forward, const CSVector3& up, CSQuaternion& result) {
        CSVector3 eye;
        lookAtRH(eye, forward, up, result);
    }
    static inline CSQuaternion rotationLookAtRH(const CSVector3& forward, const CSVector3& up) {
        CSQuaternion result;
        rotationLookAtRH(forward, up, result);
        return result;
    }
    
    static void rotationYawPitchRoll(float yaw, float pitch, float roll, CSQuaternion& result);
    static inline CSQuaternion rotationYawPitchRoll(float yaw, float pitch, float roll) {
        CSQuaternion result;
        rotationYawPitchRoll(yaw, pitch, roll, result);
        return result;
    }
    
    static void slerp(const CSQuaternion& start, const CSQuaternion& end, float amount, CSQuaternion& result);
    static inline CSQuaternion slerp(const CSQuaternion& start, const CSQuaternion& end, float amount) {
        CSQuaternion result;
        slerp(start, end, amount, result);
        return result;
    }
    
    static inline void squad(const CSQuaternion& qt1, const CSQuaternion& qt2, const CSQuaternion& qt3, const CSQuaternion& qt4, float amount, CSQuaternion& result) {
        CSQuaternion start, end;
        slerp(qt1, qt4, amount, start);
        slerp(qt2, qt3, amount, end);
        slerp(start, end, 2.0f * amount * (1.0f - amount), result);
    }
    static inline CSQuaternion squad(const CSQuaternion& qt1, const CSQuaternion& qt2, const CSQuaternion& qt3, const CSQuaternion& qt4, float amount) {
        CSQuaternion result;
        squad(qt1, qt2, qt3, qt4, amount, result);
        return result;
    }
    
    inline CSQuaternion operator +(const CSQuaternion& qt) const {
        return CSQuaternion(x + qt.x, y + qt.y, z + qt.z, w + qt.w);
    }
    
    inline CSQuaternion operator -(const CSQuaternion& qt) const {
        return CSQuaternion(x - qt.x, y - qt.y, z - qt.z, w - qt.w);
    }
    
    inline CSQuaternion operator -() const {
        return CSQuaternion(-x, -y, -z, -w);
    }
    
    inline CSQuaternion operator *(float scale) const {
        return CSQuaternion(x * scale, y * scale, z * scale, w * scale);
    }
    
    CSQuaternion operator *(const CSQuaternion& qt) const;
    
    inline CSQuaternion& operator +=(const CSQuaternion& qt) {
        return *this = *this + qt;
    }
    
    inline CSQuaternion& operator -=(const CSQuaternion& qt) {
        return *this = *this - qt;
    }
    
    inline CSQuaternion& operator *=(float scale) {
        return *this = *this * scale;
    }
    
    inline CSQuaternion& operator *=(const CSQuaternion& qt) {
        return *this = *this * qt;
    }
    
    inline operator const float*() const {
        return &x;
    }
    
    inline explicit operator uint() const {          //hash
        return (((uint)(x * 255) & 0xff) << 24) | (((uint)(y * 255) & 0xff) << 16) | (((uint)(z * 255) & 0xff) << 8) | ((uint)(w * 255) & 0xff);
    }
    
    inline bool operator ==(const CSQuaternion& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    
    inline bool operator !=(const CSQuaternion& other) const {
        return !(*this == other);
    }
};

#endif /* __CDK__CSQuaternion__ */
