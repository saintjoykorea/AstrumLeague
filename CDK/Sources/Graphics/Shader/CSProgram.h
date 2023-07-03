//
//  CSProgram.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSProgram__
#define __CDK__CSProgram__

#include "CSShader.h"

#include "CSGLContextIdentifier.h"

class CSProgram : public CSObject {
protected:
    uint _object;
    CSGLContextIdentifier _identifier;
public:
    CSProgram();
protected:
    virtual ~CSProgram();
public:
    static inline CSProgram* program() {
        return autorelease(new CSProgram());
    }
    
    inline uint object() const {
        _identifier.use();
        
        return _object;
    }
    
    virtual void attach(const CSShader* shader);
    virtual void detach(const CSShader* shader);
    
    virtual bool link();
    virtual int uniformIndex(const char* name) const;
    
    inline bool isValid() const {
        return _identifier.isValid();
    }
};

#endif /* defined(__CDK__CSProgram__) */
