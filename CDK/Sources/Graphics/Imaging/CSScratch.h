//
//  CSScratch.h
//  CDK
//
//  Created by Kim Chan on 2016. 3. 15..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSScratch__
#define __CDK__CSScratch__

#include "CSRootImage.h"

#include "CSOpenGL.h"

class CSScratch : public CSObject {
private:
    CSRootImage* _image;
    uint _textureType;
    uint _textureFormat;
    void* _raster;
    byte _bpp;
    mutable bool _dirty;
private:
    inline CSScratch() {}
    ~CSScratch();
public:
    static CSScratch* create(uint format, uint contentWidth, uint contentHeight);
    static CSScratch* create(CSRootImage* image);

    static inline CSScratch* scratch(uint format, uint contentWidth, uint contentHeight) {
        return autorelease(create(format, contentWidth, contentHeight));
    }
    static inline CSScratch* scratch(CSRootImage* image) {
        return autorelease(create(image));
    }
    
	uint pixel(uint x, uint y) const;

    void update(uint x, uint y, uint data);
    
    void transfer() const;
    
    inline const CSRootImage* image() const {
        transfer();
        return _image;
    }
};

#endif /* __CDK__CSScratch__ */
