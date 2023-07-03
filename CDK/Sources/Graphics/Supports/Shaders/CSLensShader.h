//
//  CSLensShader.h
//  CDK
//
//  Created by 김찬 on 12. 4. 26..
//  Copyright (c) 2012 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSLensShader__
#define __CDK__CSLensShader__

#include "CSVector2.h"

#include "CSProgram.h"

enum CSLensShaderUniforms {
    CSLensUniformWorldViewProjection,
    CSLensUniformScreen,
    CSLensUniformResolution,
    CSLensUniformCenter,
    CSLensUniformRange,
    CSLensUniformConvex,
    CSLensUniformCount
};

enum CSLensShaderAttribs {
    CSLensAttribPosition
};

class CSGraphics;

class CSLensShader {
private:
    CSProgram* _program;
    int _uniforms[CSLensUniformCount];
	uint _vao;
	uint _vbo;
public:
    CSLensShader();
    ~CSLensShader();
    
    void draw(CSGraphics* graphics, const CSVector3& center, float range, bool convex, bool dirtyOffscreen);
};

#endif /* defined(__CDK__CSLensShader__) */

#endif
