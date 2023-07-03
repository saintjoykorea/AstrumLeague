//
//  CSColor.h
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSColor__
#define __CDK__CSColor__

#include "CSColor3.h"

class CSBuffer;

struct CSColor {
	static const CSColor White;
	static const CSColor Black;
	static const CSColor Transparent;
	static const CSColor Gray;
	static const CSColor DarkGray;
	static const CSColor LightGray;
	static const CSColor Red;
	static const CSColor DarkRed;
	static const CSColor LightRed;
	static const CSColor Green;
	static const CSColor DarkGreen;
	static const CSColor LightGreen;
	static const CSColor Blue;
	static const CSColor DarkBlue;
	static const CSColor LightBlue;
	static const CSColor Orange;
	static const CSColor Yellow;
	static const CSColor Magenta;
	static const CSColor Brown;
	static const CSColor TranslucentBlack;
	static const CSColor TranslucentWhite;
	
	float r;
    float g;
    float b;
    float a;

    inline CSColor() {
    
    }
    
    inline CSColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {
    }
    
    inline CSColor(int r, int g, int b, int a) : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f) {
    }
    
    inline CSColor(uint rgba) : r((rgba >> 24) / 255.0f), g((rgba >> 16 & 0xff) / 255.0f), b((rgba >> 8 & 0xff) / 255.0f), a((rgba & 0xff) / 255.0f) {
    }
    
    inline CSColor(const CSColor3& rgb, float a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {
    
    }
    CSColor(CSBuffer* buffer, bool normalized);
    CSColor(const byte*& raw, bool normalized);
    
    static inline void lerp(const CSColor& start, const CSColor& end, float amount, CSColor& result) {
        result.r = CSMath::lerp(start.r, end.r, amount);
        result.g = CSMath::lerp(start.g, end.g, amount);
        result.b = CSMath::lerp(start.b, end.b, amount);
        result.a = CSMath::lerp(start.a, end.a, amount);
    }
    static inline CSColor lerp(const CSColor& start, const CSColor& end, float amount) {
        CSColor result;
        lerp(start, end, amount, result);
        return result;
    }
    
    inline bool operator ==(const CSColor& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    
    inline bool operator !=(const CSColor& other) const {
        return !(*this == other);
    }
    
    inline CSColor& operator *= (const CSColor& color) {
        r *= color.r;
        g *= color.g;
        b *= color.b;
        a *= color.a;
        return *this;
    }
    
    inline CSColor operator *(const CSColor& color) const {
        return CSColor(*this) *= color;
    }
    
    inline CSColor& operator /= (const CSColor& color) {
        r /= color.r;
        g /= color.g;
        b /= color.b;
        a /= color.a;
        return *this;
    }
    
    inline CSColor operator /(const CSColor& color) const {
        return CSColor(*this) /= color;
    }
    
    inline CSColor& operator +=(const CSColor& color) {
        r += color.r;
        g += color.g;
        b += color.b;
        a += color.a;
        return *this;
    }
    
    inline CSColor operator +(const CSColor& color) const {
        return CSColor(*this) += color;
    }
    
    inline CSColor& operator -=(const CSColor& color) {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        a -= color.a;
        return *this;
    }
    
    inline CSColor operator -(const CSColor& color) const {
        return CSColor(*this) -= color;
    }
    
    inline CSColor& operator *= (float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }
    
    inline CSColor operator *(float scalar) const {
        return CSColor(*this) *= scalar;
    }
    
    inline CSColor& operator /= (float scalar) {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
        return *this;
    }
    
    inline CSColor operator /(float scalar) const {
        return CSColor(*this) /= scalar;
    }
    
    inline CSColor& operator +=(float scalar) {
        r += scalar;
        g += scalar;
        b += scalar;
        a += scalar;
        return *this;
    }
    
    inline CSColor operator +(float scalar) const {
        return CSColor(*this) += scalar;
    }
    
    inline CSColor& operator -=(float scalar) {
        r -= scalar;
        g -= scalar;
        b -= scalar;
        a -= scalar;
        return *this;
    }
    
    inline CSColor operator -(float scalar) const {
        return CSColor(*this) -= scalar;
    }
    
    inline operator uint() const {
        return (((uint)(r * 255) & 0xff) << 24) | (((uint)(g * 255) & 0xff) << 16) | (((uint)(b * 255) & 0xff) << 8) | (uint)(a * 255);
    }
    
    inline operator const float*() const {
        return &r;
    }
    
    inline explicit operator CSColor3() const {
        return CSColor3(r, g, b);
    }
};


#endif /* defined(__CDK__CSColor__) */
