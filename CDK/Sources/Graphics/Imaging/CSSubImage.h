//
//  CSSubImage.h
//  CDK
//
//  Created by 김찬 on 12. 8. 6..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSSubImage__
#define __CDK__CSSubImage__

#include "CSRootImage.h"

#include "CSDictionary.h"

class CSSubImage : public CSImage {
private:
    const CSImage* _parent;
    CSRect _frame;
    CSDictionary<CSString, CSRect>* _localeFrames;
public:
    CSSubImage(const CSImage* parent, const CSRect& frame);
    CSSubImage(CSBuffer* buffer, const CSImage* parent);
private:
    ~CSSubImage();
public:
    static inline CSSubImage* image(const CSImage* parent, const CSRect& frame) {
        return autorelease(new CSSubImage(parent, frame));
    }
    static CSSubImage* imageWithBuffer(CSBuffer* buffer, const CSImage* parent);
    
    const CSRect& frame() const override;
    
    inline const CSRootImage* root() const override {
        return _parent->root();
    }
    inline uint texture0() const override {
        return _parent->texture0();
    }
    inline uint texture1() const override {
        return _parent->texture1();
    }
    
    inline const CSSize& textureSize() const override {
        return _parent->textureSize();
    }
};

#endif /* defined(__CDK__CSSubImage__) */
