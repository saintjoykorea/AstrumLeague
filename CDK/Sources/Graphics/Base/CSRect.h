//
//  CSRect.h
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSRect__
#define __CDK__CSRect__

#include "CSMath.h"

#include "CSVector2.h"
#include "CSSize.h"

class CSBuffer;

struct CSZRect;

struct CSRect {
	static const CSRect Zero;
	
	CSVector2 origin;
    CSSize size;
    
    inline CSRect() {
        
    }
    
    inline CSRect(const CSVector2& origin, const CSSize& size) : origin(origin), size(size) {
        
    }
    
    inline CSRect(float x, float y, float width, float height) : origin(x, y), size(width, height) {
        
    }
    
    CSRect(CSBuffer* buffer);
    CSRect(const byte*& raw);
    
    inline float left() const {
        return origin.x;
    }
    
    inline float right() const {
        return origin.x + size.width;
    }
    
    inline float top() const {
        return origin.y;
    }
    
    inline float bottom() const {
        return origin.y + size.height;
    }
    
    inline float center() const {
        return origin.x + size.width * 0.5f;
    }
    
    inline float middle() const {
        return origin.y + size.height * 0.5f;
    }
    
    inline CSVector2 centerMiddle() const {
        return CSVector2(center(), middle());
    }
    
    inline bool operator ==(const CSRect& other) const {
        return origin == other.origin && size == other.size;
    }
    inline bool operator !=(const CSRect& other) const {
        return !(*this == other);
    }
    
    inline bool contains(const CSVector2& point) const {
        return point.x >= left() && point.x <= right() && point.y >= top() && point.y <= bottom();
    }
    
    inline bool contains(const CSRect& other) const {
        return !(right() < other.left() || other.right() < left() || bottom() < other.top() || other.bottom() < top());
    }
    
    CSRect& intersect(const CSRect& rect);
    
    inline CSRect intersectedRect(const CSRect& rect) const {
        return CSRect(*this).intersect(rect);
    }
    
    CSRect& adjust(float x, float y);
    
    inline CSRect& adjust(float xy) {
        return adjust(xy, xy);
    }
    
    inline CSRect adjustRect(float x, float y) const {
        return CSRect(*this).adjust(x, y);
    }
    
    inline CSRect adjustRect(float xy) const {
        return adjustRect(xy, xy);
    }
    
    inline operator const float*() const {
        return &origin.x;
    }
    
    operator CSZRect() const;
    
    inline explicit operator uint() const {          //hash
        return (((int)size.height & 0xff) << 24) | (((int)size.width & 0xff) << 16) | (((int)origin.y & 0xff) << 8) | ((int)origin.x & 0xff);
    }
};

#endif /* defined(__CDK__CSRect__) */
