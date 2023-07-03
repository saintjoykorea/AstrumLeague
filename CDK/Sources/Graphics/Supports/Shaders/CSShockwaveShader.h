//
//  CSShockwaveShader.h
//  CDK
//
//  Created by 김찬 on 12. 4. 26..
//  Copyright (c) 2012 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSShockwaveShader__
#define __CDK__CSShockwaveShader__

#include "CSVector2.h"

#include "CSProgram.h"

enum CSShockwaveShaderUniforms {
    CSShockwaveUniformWorldViewProjection,
    CSShockwaveUniformScreen,
    CSShockwaveUniformResolution,
    CSShockwaveUniformCenter,
    CSShockwaveUniformRange,
    CSShockwaveUniformThickness,
    CSShockwaveUniformCount
};

enum CSShockwaveShaderAttribs {
    CSShockwaveAttribPosition
};

class CSGraphics;

class CSShockwaveShader {
private:
    CSProgram* _program;
    int _uniforms[CSShockwaveUniformCount];
	uint _vao;
	uint _vbo;
public:
    CSShockwaveShader();
    ~CSShockwaveShader();
    
    void draw(CSGraphics* graphics, const CSVector3& center, float range, float thickness, float progress, bool dirtyOffscreen);
};

#endif /* defined(__CDK__CSShockwaveShader__) */

#endif

