//
//  CSScreenBuffer.h
//  CDK
//
//  Created by 김찬 on 14. 3. 25..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSScreenBuffer__
#define __CDK__CSScreenBuffer__

#include "CSObject.h"

class CSScreenBuffer : public CSObject {
protected:
    inline virtual ~CSScreenBuffer() {
    
    }
public:
    virtual uint renderBuffer() const = 0;
    virtual uint frameBuffer() const = 0;
    virtual uint width() const = 0;
    virtual uint height() const = 0;
};

#endif

#endif
