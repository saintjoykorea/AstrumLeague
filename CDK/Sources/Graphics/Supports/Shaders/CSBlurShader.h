//
//  CSBlurShader.h
//  CDK
//
//  Created by 김찬 on 2015. 8. 24..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSBlurShader__
#define __CDK__CSBlurShader__

#include "CSProgram.h"

#include "CSRect.h"

enum CSBlurShaderUniforms {
    CSBlurUniformWorldViewProjection,
    CSBlurUniformScreen,
    CSBlurUniformResolution,
    CSBlurUniformDirection,
	CSBlurUniformCenterCoord,
	CSBlurUniformRange,
    CSBlurUniformCount
};

enum CSBlurShaderAttribs {
    CSBlurAttribPosition
};

class CSGraphics;

class CSBlurShader {
private:
	struct {
		CSProgram* object;
		int uniforms[CSBlurUniformCount];
	}_programs[3];
	uint _vao;
	uint _vbo;
public:
    CSBlurShader();
    ~CSBlurShader();
    
	void draw(CSGraphics* graphics, const CSZRect& rect, float weight);
    void draw(CSGraphics* graphics, const CSZRect& rect, const CSVector2& dir);
	void draw(CSGraphics* graphics, const CSZRect& rect, const CSVector3& center, float range);
};


#endif /* defined(__CDK__CSBlurShader__) */

#endif
