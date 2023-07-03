//
//  CSPixellateShader.h
//  CDK
//
//  Created by 김찬 on 12. 4. 23..
//  Copyright (c) 2012 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSPixellateShader__
#define __CDK__CSPixellateShader__

#include "CSRect.h"

#include "CSProgram.h"

enum CSPixellateShaderUniforms {
    CSPixellateUniformWorldViewProjection,
    CSPixellateUniformScreen,
    CSPixellateUniformResolution,
    CSPixellateUniformFraction,
    CSPixellateUniformCount
};

enum CSPixellateShaderAttribs {
    CSPixellateAttribPosition
};

class CSGraphics;

class CSPixellateShader {
private:
    CSProgram* _program;
    int _uniforms[CSPixellateUniformCount];
	uint _vao;
	uint _vbo;
public:
    CSPixellateShader();
    ~CSPixellateShader();
    
    void draw(CSGraphics* graphics, const CSZRect& rect, float fraction);
};

#endif /* defined(__CDK__CSPixellateShader__) */

#endif
