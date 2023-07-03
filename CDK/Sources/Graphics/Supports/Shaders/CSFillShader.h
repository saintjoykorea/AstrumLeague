//
//  CSFillShader.h
//  CDK
//
//  Created by 김찬 on 13. 2. 8..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSFillShader__
#define __CDK__CSFillShader__

#include "CSProgram.h"

#include "CSRenderBounds.h"

#include "CSImage.h"

class CSFillShader {
private:
    CSProgram* _programs[2];
    int _screenUniform;
	uint _vao;
	uint _vbo;
public:
    CSFillShader();
    ~CSFillShader();
    
    void draw(const CSImage* screen, const CSRenderBounds& renderBounds);
};

#endif /* defined(__CDK__CSFillShader__) */

#endif
