//
//  CSGLContextIdentifier.h
//  CDK
//
//  Created by chan on 13. 4. 30..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSGLContextIdentifier__
#define __CDK__CSGLContextIdentifier__

#include "CSLog.h"

#include "CSApplication.h"

class CSGLContextIdentifier {
private:
    unsigned int _identifier;
public:
    inline CSGLContextIdentifier() : _identifier(0) {
    
    }
public:
    inline bool isValid() const {
        return _identifier == CSApplication::sharedApplication()->glContextIdentifier();
    }
    
    inline void use() const {
        CSAssert(isValid(), "invalid context object");
    }
    
    inline void reset() {
        _identifier = CSApplication::sharedApplication()->glContextIdentifier();
    }
};

#endif /* defined(__CDK__CSGLContextIdentifier__) */
