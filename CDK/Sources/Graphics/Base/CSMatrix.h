//
//  CSMatrix.h
//  CDK
//
//  Created by ChangSun on 2016. 1. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMatrix__
#define __CDK__CSMatrix__

#include "CSMath.h"
#include "CSQuaternion.h"
#include "CSVector2.h"
#include "CSVector3.h"
#include "CSVector4.h"

class CSBuffer;

struct CSMatrix {
public:
    static const CSMatrix Identity;
    
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
    
    inline CSMatrix() {
    
    }
    
    CSMatrix(float mv11, float mv12, float mv13, float mv14,
             float mv21, float mv22, float mv23, float mv24,
             float mv31, float mv32, float mv33, float mv34,
             float mv41, float mv42, float mv43, float mv44);

    CSMatrix(CSBuffer* buffer);
    
    inline CSVector4 row1() const {
        return CSVector4(m11, m12, m13, m14);
    }
    
    inline void setRow1(const CSVector4& vec) {
        m11 = vec.x;
        m12 = vec.y;
        m13 = vec.z;
        m14 = vec.w;
    }
    
    inline CSVector4 row2() const {
        return CSVector4(m21, m22, m23, m24);
    }
    
    inline void setRow2(const CSVector4& vec) {
        m21 = vec.x;
        m22 = vec.y;
        m23 = vec.z;
        m24 = vec.w;
    }
    
    inline CSVector4 row3() const {
        return CSVector4(m31, m32, m33, m34);
    }
    
    inline void setRow3(const CSVector4& vec) {
        m31 = vec.x;
        m32 = vec.y;
        m33 = vec.z;
        m34 = vec.w;
    }
    
    inline CSVector4 row4() const {
        return CSVector4(m41, m42, m43, m44);
    }
    
    inline void setRow4(const CSVector4& vec) {
        m41 = vec.x;
        m42 = vec.y;
        m43 = vec.z;
        m44 = vec.w;
    }
    
    inline CSVector4 column1() const {
        return CSVector4(m11, m21, m31, m41);
    }
    
    inline void setColumn1(const CSVector4& vec) {
        m11 = vec.x;
        m21 = vec.y;
        m31 = vec.z;
        m41 = vec.w;
    }
    
    inline CSVector4 column2() const {
        return CSVector4(m12, m22, m32, m42);
    }
    
    inline void setColumn2(const CSVector4& vec) {
        m12 = vec.x;
        m22 = vec.y;
        m32 = vec.z;
        m42 = vec.w;
    }
    
    inline CSVector4 column3() const {
        return CSVector4(m13, m23, m33, m43);
    }
    
    inline void setColumn3(const CSVector4& vec) {
        m13 = vec.x;
        m23 = vec.y;
        m33 = vec.z;
        m43 = vec.w;
    }
    
    inline CSVector4 column4() const {
        return CSVector4(m14, m24, m34, m44);
    }
    
    inline void setColumn4(const CSVector4& vec) {
        m14 = vec.x;
        m24 = vec.y;
        m34 = vec.z;
        m44 = vec.w;
    }
    
    inline CSVector3 translationVector() const {
        return CSVector3(m41, m42, m43);
    }
    
    inline void setTranslationVector(const CSVector3& vec) {
        m41 = vec.x;
        m42 = vec.y;
        m43 = vec.z;
    }
    
    inline CSVector3 scaleVector() const {
        return CSVector3(m11, m22, m33);
    }
    
    inline void setScaleVector(const CSVector3& vec) {
        m11 = vec.x;
        m22 = vec.y;
        m33 = vec.z;
    }
    
    float determinant() const;
    
    inline void invert() {
        invert(*this, *this);
    }
    
    inline void transpose() {
        transpose(*this, *this);
    }
    
    bool decompose(CSVector3& scale, CSQuaternion& rotation, CSVector3& trans);
    bool decomposeUniformScale(float& scale, CSQuaternion& rotation, CSVector3& trans);
    
    static void lerp(const CSMatrix& start, const CSMatrix& end, float amount, CSMatrix& result);
    static inline CSMatrix lerp(const CSMatrix& start, const CSMatrix& end, float amount) {
        CSMatrix result;
        lerp(start, end, amount, result);
        return result;
    }
    
    
    static inline void smoothStep(const CSMatrix& start, const CSMatrix& end, float amount, CSMatrix& result) {
        amount = CSMath::smoothStep(amount);
        lerp(start, end, amount, result);
    }
    static inline CSMatrix smoothStep(const CSMatrix& start, const CSMatrix& end, float amount) {
        CSMatrix result;
        smoothStep(start, end, amount, result);
        return result;
    }
    
    static void transpose(const CSMatrix& matrix, CSMatrix& result);
    static CSMatrix transpose(const CSMatrix& matrix) {
        CSMatrix result;
        transpose(matrix, result);
        return result;
    }
    
    static void invert(const CSMatrix& matrix, CSMatrix& result);
    static CSMatrix invert(const CSMatrix& matrix) {
        CSMatrix result;
        invert(matrix, result);
        return result;
    }
    
    static void lookAtLH(const CSVector3& eye, const CSVector3& target, const CSVector3& up, CSMatrix& result);
    static inline CSMatrix lookAtLH(const CSVector3& eye, const CSVector3& target, const CSVector3& up) {
        CSMatrix result;
        lookAtLH(eye, target, up, result);
        return result;
    }
    static void lookAtRH(const CSVector3& eye, const CSVector3& target, const CSVector3& up, CSMatrix& result);
    static inline CSMatrix lookAtRH(const CSVector3& eye, const CSVector3& target, const CSVector3& up) {
        CSMatrix result;
        lookAtRH(eye, target, up, result);
        return result;
    }
    
    static void orthoOffCenterLH(float left, float right, float bottom, float top, float znear, float zfar, CSMatrix& result);
    
    static inline void orthoLH(float width, float height, float znear, float zfar, CSMatrix& result) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        
        orthoOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, znear, zfar, result);
    }
    static inline CSMatrix orthoLH(float width, float height, float znear, float zfar) {
        CSMatrix result;
        orthoLH(width, height, znear, zfar, result);
        return result;
    }
    
    static inline void orthoRH(float width, float height, float znear, float zfar, CSMatrix& result) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        
        orthoOffCenterRH(-halfWidth, halfWidth, -halfHeight, halfHeight, znear, zfar, result);
    }
    static inline CSMatrix orthoRH(float width, float height, float znear, float zfar) {
        CSMatrix result;
        orthoRH(width, height, znear, zfar, result);
        return result;
    }
    
    static inline CSMatrix orthoOffCenterLH(float left, float right, float bottom, float top, float znear, float zfar) {
        CSMatrix result;
        orthoOffCenterLH(left, right, bottom, top, znear, zfar, result);
        return result;
    }
    static inline void orthoOffCenterRH(float left, float right, float bottom, float top, float znear, float zfar, CSMatrix& result) {
        orthoOffCenterLH(left, right, bottom, top, znear, zfar, result);
        result.m33 *= -1.0f;
    }
    static inline CSMatrix orthoOffCenterRH(float left, float right, float bottom, float top, float znear, float zfar) {
        CSMatrix result;
        orthoOffCenterRH(left, right, bottom, top, znear, zfar, result);
        return result;
    }
    
    static void perspectiveFovLH(float fov, float aspect, float znear, float zfar, CSMatrix& result);
    static void perspectiveFovRH(float fov, float aspect, float znear, float zfar, CSMatrix& result);
    static void perspectiveOffCenterLH(float left, float right, float bottom, float top, float znear, float zfar, CSMatrix& result);
    static inline void perspectiveOffCenterRH(float left, float right, float bottom, float top, float znear, float zfar, CSMatrix& result) {
        perspectiveOffCenterLH(left, right, bottom, top, znear, zfar, result);
        result.m31 *= -1.0f;
        result.m32 *= -1.0f;
        result.m33 *= -1.0f;
        result.m34 *= -1.0f;
    }
    static inline CSMatrix perspectiveFovLH(float fov, float aspect, float znear, float zfar) {
        CSMatrix result;
        perspectiveFovLH(fov, aspect, znear, zfar, result);
        return result;
        
    }
    static inline CSMatrix perspectiveFovRH(float fov, float aspect, float znear, float zfar) {
        CSMatrix result;
        perspectiveFovRH(fov, aspect, znear, zfar, result);
        return result;
        
    }
    static inline CSMatrix perspectiveOffCenterLH(float left, float right, float bottom, float top, float znear, float zfar) {
        CSMatrix result;
        perspectiveOffCenterLH(left, right, bottom, top, znear, zfar, result);
        return result;
        
    }
    static inline CSMatrix perspectiveOffCenterRH(float left, float right, float bottom, float top, float znear, float zfar) {
        CSMatrix result;
        perspectiveOffCenterRH(left, right, bottom, top, znear, zfar, result);
        return result;
    }
    static inline void scaling(float x, float y, float z, CSMatrix& result) {
        result = CSMatrix::Identity;
        result.m11 = x;
        result.m22 = y;
        result.m33 = z;
    }
    static inline CSMatrix scaling(float x, float y, float z) {
        CSMatrix result;
        scaling(x, y, z, result);
        return result;
    }
    
    static inline void scaling(const CSVector3 vector, CSMatrix& result) {
        result = CSMatrix::Identity;
        result.m11 = vector.x;
        result.m22 = vector.y;
        result.m33 = vector.z;
    }
    static inline CSMatrix scaling(const CSVector3 vector) {
        CSMatrix result;
        scaling(vector, result);
        return result;
    }
    
    static inline void scaling(float scale, CSMatrix& result) {
        result = CSMatrix::Identity;
        result.m11 = result.m22 = result.m33 = scale;
    }
    
    static inline CSMatrix scaling(float scale) {
        CSMatrix result;
        scaling(scale, result);
        return result;
    }
    
    static void rotationX(float angle, CSMatrix& result);
    static inline CSMatrix rotationX(float angle) {
        CSMatrix result;
        rotationX(angle, result);
        return result;
    }
    
    static void rotationY(float angle, CSMatrix& result);
    static inline CSMatrix rotationY(float angle) {
        CSMatrix result;
        rotationY(angle, result);
        return result;
    }
    
    static void rotationZ(float angle, CSMatrix& result);
    static inline CSMatrix rotationZ(float angle) {
        CSMatrix result;
        rotationZ(angle, result);
        return result;
    }
    
    static void rotationAxis(const CSVector3& axis, float angle, CSMatrix& result);
    static inline CSMatrix rotationAxis(const CSVector3& axis, float angle) {
        CSMatrix result;
        rotationAxis(axis, angle, result);
        return result;
    }
    
    static void rotationQuaternion(const CSQuaternion& rotation, CSMatrix& result);
    static inline CSMatrix rotationQuaternion(const CSQuaternion& rotation) {
        CSMatrix result;
        rotationQuaternion(rotation, result);
        return result;
    }
    
    static inline void rotationYawPitchRoll(float yaw, float pitch, float roll, CSMatrix& result) {
        CSQuaternion quaternion;
        CSQuaternion::rotationYawPitchRoll(yaw, pitch, roll, quaternion);
        rotationQuaternion(quaternion, result);
    }
    static inline CSMatrix rotationYawPitchRoll(float yaw, float pitch, float roll) {
        CSMatrix result;
        rotationYawPitchRoll(yaw, pitch, roll, result);
        return result;
    }
    
    static inline void translation(const CSVector3& vec, CSMatrix& result) {
        result = CSMatrix::Identity;
        result.m41 = vec.x;
        result.m42 = vec.y;
        result.m43 = vec.z;
    }
    
    static inline CSMatrix translation(const CSVector3& vector) {
        CSMatrix result = CSMatrix::Identity;
        translation(vector, result);
        return result;
    }
    
    static void affineTransformation(float scale, const CSQuaternion& rotation, const CSVector3& trans, CSMatrix& result);
    static inline CSMatrix affineTransformation(float scale, const CSQuaternion& rotation, const CSVector3& trans) {
        CSMatrix result;
        affineTransformation(scale, rotation, trans, result);
        return result;
    }
    static void affineTransformation(float scale, const CSVector3& rotationCenter, const CSQuaternion& rotation, const CSVector3& trans, CSMatrix& result);
    static inline CSMatrix affineTransformation(float scale, const CSVector3& rotationCenter, const CSQuaternion& rotation, const CSVector3& trans) {
        CSMatrix result;
        affineTransformation(scale, rotationCenter, rotation, trans, result);
        return result;
    }
    static void affineTransformation2D(float scale, const CSVector2& rotationCenter, float rotation, const CSVector2& trans2d, CSMatrix& result);
    static inline CSMatrix affineTransformation2D(float scale, const CSVector2& rotationCenter, float rotation, const CSVector2& trans2d) {
        CSMatrix result;
        affineTransformation2D(scale, rotationCenter, rotation, trans2d, result);
        return result;
    }
    
    static void transformation(const CSVector3& scalingCenter, const CSQuaternion& scalingRotation, const CSVector3& scale, const CSVector3& rotationCenter, const CSQuaternion& rotation, const CSVector3& trans, CSMatrix& result);
    static inline CSMatrix transformation(const CSVector3& scalingCenter, const CSQuaternion& scalingRotation, const CSVector3& scale, const CSVector3& rotationCenter, const CSQuaternion& rotation, const CSVector3& trans) {
        CSMatrix result;
        transformation(scalingCenter, scalingRotation, scale, rotationCenter, rotation, trans, result);
        return result;
    }
    static void transformation2D(const CSVector2& scalingCenter, float scalingRotation, const CSVector2& scaling, const CSVector2& rotationCenter, float rotation, const CSVector2& trans, CSMatrix& result);
    static inline CSMatrix transformation2D(const CSVector2& scalingCenter, float scalingRotation, const CSVector2& scaling, const CSVector2& rotationCenter, float rotation, const CSVector2& trans) {
        CSMatrix result;
        transformation2D(scalingCenter, scalingRotation, scaling, rotationCenter, rotation, trans, result);
        return result;
    }
    
    inline CSVector3 right() const {
        return CSVector3(m11, m12, m13);
    }
    
    inline CSVector3 up() const {
        return CSVector3(m21, m22, m23);
    }
    
    inline CSVector3 forward() const {
        return CSVector3(-m31, -m32, -m33);
    }
    
    CSMatrix operator +(const CSMatrix& matrix) const;
    inline CSMatrix& operator +=(const CSMatrix& matrix) {
        return *this = *this + matrix;
    }
    CSMatrix operator -(const CSMatrix& matrix) const;
    inline CSMatrix& operator -=(const CSMatrix& matrix) {
        return *this = *this - matrix;
    }
    CSMatrix operator -() const;
    
    CSMatrix operator *(const CSMatrix& matrix) const;
    inline CSMatrix& operator *=(const CSMatrix& matrix) {
        return *this = *this * matrix;
    }
    CSMatrix operator *(float value) const;
    inline CSMatrix& operator *=(float value) {
        return *this = *this * value;
    }
    
    CSMatrix operator /(const CSMatrix& matrix) const;
    inline CSMatrix& operator /=(const CSMatrix& matrix) {
        return *this = *this / matrix;
    }
    CSMatrix operator /(float value) const;
    inline CSMatrix& operator /=(float value) {
        return *this = *this / value;
    }
    
    inline operator const float*() const {
        return &m11;
    }
    
    inline explicit operator uint() const {          //hash
        return m11 + m12 + m13 + m14 + m21 + m22 + m23 + m24 + m31 + m32 + m33 + m34 + m41 + m42 + m43 + m44;
    }
    
    inline bool operator ==(const CSMatrix& matrix) const {
        return memcmp(this, &matrix, sizeof(CSMatrix)) == 0;
    }
    inline bool operator !=(const CSMatrix& other) const {
        return !(*this == other);
    }
    
    void billboard(const CSMatrix* world, CSMatrix& result) const;
    inline CSMatrix billboard(const CSMatrix* world) const {
        CSMatrix result;
        billboard(world, result);
        return result;
    }
    
    void horizontalBillboard(const CSMatrix* world, CSMatrix& result) const;
    inline CSMatrix horizontalBillboard(const CSMatrix* world) const {
        CSMatrix result;
        horizontalBillboard(world, result);
        return result;
    }
    
    void verticalBillboard(const CSMatrix* world, CSMatrix& result) const;
    inline CSMatrix verticalBillboard(const CSMatrix* world) const {
        CSMatrix result;
        verticalBillboard(world, result);
        return result;
    }
    
    bool stretchBillboard(const CSMatrix* world, const CSVector3& dir, float rate, CSMatrix& result) const;
};


#endif /* __CDK__CSMatrix__ */
