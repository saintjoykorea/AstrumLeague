//
//  CSImage.h
//  CDK
//
//  Created by 김찬 on 12. 8. 6..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSImage__
#define __CDK__CSImage__

#include "CSObject.h"

#include "CSRect.h"

#include "CSResource.h"

class CSRootImage;

class CSBuffer;

class CSImage : public CSResource {
public:
    static CSImage* rootImageWithBuffer(CSBuffer* buffer);
    static CSImage* subImageWithBuffer(CSBuffer* buffer, const CSImage* parent);
    
    inline CSResourceType resourceType() const override {
        return CSResourceTypeImage;
    }
    
    virtual const CSRect& frame() const = 0;
    
    inline const CSSize& size() const {
        return frame().size;
    }
    inline float width() const {
        return size().width;
    }
    inline float height() const {
        return size().height;
    }
    
    virtual uint texture0() const = 0;
    virtual uint texture1() const = 0;
    
    virtual const CSSize& textureSize() const = 0;
    inline float textureWidth() const {
        return textureSize().width;
    }
    inline float textureHeight() const {
        return textureSize().height;
    }
    virtual const CSRootImage* root() const = 0;
};

#endif /* defined(__CDK__CSImage__) */
