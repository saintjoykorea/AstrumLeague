//
//  CSColor3.h
//  CDK
//
//  Created by 김찬 on 2016. 1. 29..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSColor3__
#define __CDK__CSColor3__

#include "CSMath.h"

struct CSColor;

class CSBuffer;

struct CSColor3 {
	static const CSColor3 White;
	static const CSColor3 Black;
	static const CSColor3 Gray;
	static const CSColor3 DarkGray;
	static const CSColor3 LightGray;
	static const CSColor3 Red;
	static const CSColor3 DarkRed;
	static const CSColor3 LightRed;
	static const CSColor3 Green;
	static const CSColor3 DarkGreen;
	static const CSColor3 LightGreen;
	static const CSColor3 Blue;
	static const CSColor3 DarkBlue;
	static const CSColor3 LightBlue;
	static const CSColor3 Orange;
	static const CSColor3 Yellow;
	static const CSColor3 Magenta;
	static const CSColor3 Brown;
	
	float r;
    float g;
    float b;

    inline CSColor3() {
    }
    
    inline CSColor3(float r, float g, float b) : r(r), g(g), b(b) {
    }
    
    inline CSColor3(int r, int g, int b) : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f) {
    }
    
    CSColor3(CSBuffer* buffer, bool normalized);
    CSColor3(const byte*& raw, bool normalized);
    
    static inline void lerp(const CSColor3& start, const CSColor3& end, float amount, CSColor3& result) {
        result.r = CSMath::lerp(start.r, end.r, amount);
        result.g = CSMath::lerp(start.g, end.g, amount);
        result.b = CSMath::lerp(start.b, end.b, amount);
    }
    static inline CSColor3 lerp(const CSColor3& start, const CSColor3& end, float amount) {
        CSColor3 result;
        lerp(start, end, amount, result);
        return result;
    }
    
    inline bool operator ==(const CSColor3& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
    
    inline bool operator !=(const CSColor3& other) const {
        return !(*this == other);
    }
    
    inline CSColor3& operator *=(const CSColor3& color) {
        r *= color.r;
        g *= color.g;
        b *= color.b;
        return *this;
    }
    
    inline CSColor3 operator *(const CSColor3& color) const {
        return CSColor3(*this) *= color;
    }
    
    inline CSColor3& operator /=(const CSColor3& color) {
        r /= color.r;
        g /= color.g;
        b /= color.b;
        return *this;
    }
    
    inline CSColor3 operator /(const CSColor3& color) const {
        return CSColor3(*this) /= color;
    }
    
    inline CSColor3& operator +=(const CSColor3& color) {
        r += color.r;
        g += color.g;
        b += color.b;
        return *this;
    }
    
    inline CSColor3 operator +(const CSColor3& color) const {
        return CSColor3(*this) += color;
    }
    
    inline CSColor3& operator -=(const CSColor3& color) {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        return *this;
    }
    
    inline CSColor3 operator-(const CSColor3& color) const {
        return CSColor3(*this) -= color;
    }
    
    inline CSColor3& operator *=(float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }
    
    inline CSColor3 operator *(float scalar) const {
        return CSColor3(*this) *= scalar;
    }
    
    inline CSColor3& operator /=(float scalar) {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        return *this;
    }
    
    inline CSColor3 operator /(float scalar) const {
        return CSColor3(*this) /= scalar;
    }
    
    inline CSColor3& operator +=(float scalar) {
        r += scalar;
        g += scalar;
        b += scalar;
        return *this;
    }
    
    inline CSColor3 operator +(float scalar) const {
        return CSColor3(*this) += scalar;
    }
    
    inline CSColor3& operator -=(float scalar) {
        r -= scalar;
        g -= scalar;
        b -= scalar;
        return *this;
    }
    
    inline CSColor3 operator-(float scalar) const {
        return CSColor3(*this) -= scalar;
    }
    
    inline operator uint() const {
        return (((uint)(r * 255) & 0xff) << 24) | (((uint)(g * 255) & 0xff) << 16) | (((uint)(b * 255) & 0xff) << 8) | 255;
    }
    
    inline operator const float*() const {
        return &r;
    }
    
    operator CSColor() const;
    
};

#endif /* __CDK__CSColor3__ */
