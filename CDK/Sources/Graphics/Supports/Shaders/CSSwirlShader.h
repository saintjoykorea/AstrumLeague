//
//  CSSwirlShader.h
//  CDK
//
//  Created by 김찬 on 12. 4. 23..
//  Copyright (c) 2012 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSSwirlShader__
#define __CDK__CSSwirlShader__

#include "CSVector2.h"

#include "CSProgram.h"

enum CSSwirlShaderUniforms {
    CSSwirlUniformWorldViewProjection,
    CSSwirlUniformScreen,
    CSSwirlUniformResolution,
    CSSwirlUniformCenter,
    CSSwirlUniformRange,
    CSSwirlUniformAngle,
    CSSwirlUniformCount
};

enum CSSwirlShaderAttribs {
    CSSwirlAttribPosition
};

class CSGraphics;

class CSSwirlShader {
private:
    CSProgram* _program;
    int _uniforms[CSSwirlUniformCount];
	uint _vao;
	uint _vbo;
public:
    CSSwirlShader();
    ~CSSwirlShader();
    
    void draw(CSGraphics* graphics, const CSVector3& center, float range, float angle, bool dirtyOffscreen);
};

#endif /* defined(__CDK__CSSwirlShader__) */

#endif

