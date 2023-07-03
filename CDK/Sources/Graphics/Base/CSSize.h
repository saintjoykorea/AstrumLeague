//
//  CSSize.h
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSSize__
#define __CDK__CSSize__

#include "CSMath.h"

class CSBuffer;

struct CSSize {
	static const CSSize Zero;
	
	float width;
    float height;
    
    inline CSSize() {
    }
    
    inline CSSize(float width, float height) : width(width), height(height) {
    }
    
    CSSize(CSBuffer* buffer);
    CSSize(const byte*& raw);
    
    inline bool operator ==(const CSSize& other) const {
        return width == other.width && height == other.height;
    }
    
    inline bool operator !=(const CSSize& s) const {
        return !(*this == s);
    }
    
    inline CSSize operator +(const CSSize& other) const {
        return CSSize(width + other.width, height + other.height);
    }
    
    inline CSSize operator -(const CSSize& other) const {
        return CSSize(width - other.width, height - other.height);
    }
    
    inline CSSize operator +(float scalar) const {
        return CSSize(width + scalar, height + scalar);
    }
    
    inline CSSize operator -(float scalar) const {
        return CSSize(width - scalar, height - scalar);
    }
    
    inline CSSize operator *(float scale) const {
        return CSSize(width * scale, height * scale);
    }
    
    inline CSSize operator /(float scale) const {
        return CSSize(width / scale, height / scale);
    }
    
    inline CSSize& operator +=(const CSSize& other) {
        width += other.width;
        height += other.height;
        return *this;
    }
    
    inline CSSize& operator -=(const CSSize& other) {
        width -= other.width;
        height -= other.height;
        return *this;
    }
    
    inline CSSize& operator+=(float scalar) {
        width += scalar;
        height += scalar;
        return *this;
    }
    
    inline CSSize& operator-=(float scalar) {
        width -= scalar;
        height -= scalar;
        return *this;
    }
    
    inline CSSize& operator*=(float scale) {
        width *= scale;
        height *= scale;
        return *this;
    }
    
    inline CSSize& operator/=(float scale) {
        width /= scale;
        height /= scale;
        return *this;
    }
    
    inline CSSize& intersect(const CSSize& other) {
        width = CSMath::min(width, other.width);
        height = CSMath::min(height, other.height);
        return *this;
    }
    
    inline CSSize intersectedSize(const CSSize& size) const {
        return CSSize(*this).intersect(size);
    }
    
    inline float lengthSquared() const {
        return width * width + height * height;
    }
    
    inline float length() const {
        return CSMath::sqrt(lengthSquared());
    }
    
    inline operator const float*() const {
        return &width;
    }
    
    inline explicit operator uint() const {          //hash
        return ((int)height & 0xffff) << 16 | ((int)width & 0xffff);
    }
};

#endif /* defined(__CDK__CSSize__) */
