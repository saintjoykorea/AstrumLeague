//
//  CSRenderer.h
//  CDK
//
//  Created by 김찬 on 12. 8. 1..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSRenderer__
#define __CDK__CSRenderer__

#include "CSRenderProgram.h"

#include "CSDictionary.h"

class CSRenderer {
private:
    CSDictionary<uint, CSShader>* _vertexShaders;
    CSDictionary<uint, CSShader>* _fragmentShaders;
    CSDictionary<uint, CSRenderProgram>* _programs;
public:
    CSRenderer();
    ~CSRenderer();
    
public:
    CSRenderProgram* program(const CSRenderState& state);
};

#endif  /*  __CDK__CSRenderer__ */

#endif
