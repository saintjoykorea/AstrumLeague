//
//  CSShader.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSShader__
#define __CDK__CSShader__

#include "CSObject.h"

#include "CSGLContextIdentifier.h"

class CSShader : public CSObject {
private:
    uint _object;
    CSGLContextIdentifier _identifier;
private:
    CSShader(uint object);
    ~CSShader();
public:
    static CSShader* create(const char* source, uint type, const char* options = NULL);
    static inline CSShader* shader(const char* source, uint type, const char* options = NULL) {
        return autorelease(create(source, type, options));
    }
    
    inline uint object() const {
        _identifier.use();
        
        return _object;
    }
    
    inline bool isValid() const {
        return _identifier.isValid();
    }
};

#endif /* defined(__CDK__CSShader__) */
