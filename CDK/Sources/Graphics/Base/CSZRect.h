//
//  CSZRect.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef __CDK__CSZRect__
#define __CDK__CSZRect__

#include "CSRect.h"

#include "CSVector3.h"

class CSBuffer;

struct CSZRect {
	static const CSZRect Zero;
	
	CSVector3 origin;
    CSSize size;
    
    inline CSZRect() {
        
    }
    
    inline CSZRect(float x, float y, float z, float width, float height) : origin(x, y, z), size(width, height) {
        
    }
    
    inline CSZRect(const CSVector3& origin, const CSSize& size) : origin(origin), size(size) {
        
    }
    
    CSZRect(CSBuffer* buffer);
    CSZRect(const byte*& raw);
    
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
    
    inline CSVector3 centerMiddle() const {
        return CSVector3(center(), middle(), origin.z);
    }
    
    inline bool operator ==(const CSZRect& other) const {
        return origin == other.origin && size == other.size;
    }
    inline bool operator !=(const CSZRect& other) const {
        return !(*this == other);
    }
    
    CSZRect& adjust(float x, float y);
    inline CSZRect& adjust(float xy) {
        return adjust(xy, xy);
    }
    
    inline CSZRect adjustRect(float x, float y) const {
        return CSZRect(*this).adjust(x, y);
    }
    
    inline CSZRect adjustRect(float xy) const {
        return adjustRect(xy, xy);
    }
    
    inline operator const float*() const {
        return &origin.x;
    }
    
    inline explicit operator CSRect() const {
        return CSRect((CSVector2)origin, size);
    }
    
    inline explicit operator uint() const {          //hash
        return (((int)size.height & 0xff) << 24) | (((int)size.width & 0xff) << 16) | (((int)origin.y & 0xff) << 8) | ((int)origin.x & 0xff);
    }
};

#endif /* __CDK__CSZRect__ */
