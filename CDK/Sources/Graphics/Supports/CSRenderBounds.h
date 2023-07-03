//
//  CSRenderBounds.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 2. 23..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef __CDK__CSRenderBounds__
#define __CDK__CSRenderBounds__

#include "CSMath.h"

struct CSRenderBounds {
    float left;
    float right;
    float top;
    float bottom;
    
    inline CSRenderBounds() {}
    CSRenderBounds(float left, float right, float top, float bottom);
    
    void clip();
    void append(float x, float y);
    void append(const CSRenderBounds& other);
    bool contains(const CSRenderBounds& other) const;
    bool onScreen() const;
    bool fullScreen() const;
    
    static CSRenderBounds None;
    static CSRenderBounds Full;
};

#endif /* __CDK__CSRenderBounds__ */
