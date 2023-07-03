//
//  CSKuwaharaShader.h
//  CDK
//
//  Created by 김찬 on 12. 4. 27..
//  Copyright (c) 2012 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSKuwaharaShader__
#define __CDK__CSKuwaharaShader__

#include "CSRect.h"

#include "CSProgram.h"

enum CSKuwaharaShaderUniforms {
    CSKuwaharaUniformWorldViewProjection,
    CSKuwaharaUniformScreen,
    CSKuwaharaUniformResolution,
    CSKuwaharaUniformProjection,
    CSKuwaharaUniformCount
};

enum CSKuwaharaShaderAttribs {
    CSKuwaharaAttribPosition
};

class CSGraphics;

#define CSKuwaharaMaxRadius  3

class CSKuwaharaShader {
private:
    class Program : public CSProgram {
    public:
        int uniforms[CSKuwaharaUniformCount];
        bool link();
    };
    Program* _programs[CSKuwaharaMaxRadius];
	uint _vao;
	uint _vbo;
public:
    CSKuwaharaShader();
    ~CSKuwaharaShader();
    
    void draw(CSGraphics* graphics, const CSZRect& rect, int radius);
};

#endif /* defined(__CDK__CSKuwaharaShader__) */

#endif
