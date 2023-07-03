//
//  VisionRenderer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 15..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef VisionRenderer_h
#define VisionRenderer_h

#include "Application.h"

enum VisionUniforms {
    VisionUniformWorld,
    VisionUniformViewProjection,
	VisionUniformPositionScale,
    VisionUniformVisionTexture,
	VisionUniformVisionScale,
    VisionUniformCount
};

extern const CSArray<CSVertexLayout>* VisionVertexLayouts;

class VisionRenderer {
private:
    CSProgram* _program;
    int _uniforms[VisionUniformCount];
public:
    VisionRenderer();
    ~VisionRenderer();
    
    void draw(CSGraphics* graphics, const CSTVertexArray<CSVector3>* arr, const CSRootImage* vision);
};

#endif /* VisionRenderer_h */
