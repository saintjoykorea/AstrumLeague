//
//  VisionRenderer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "VisionRenderer.h"

#include "Terrain.h"

enum VisionVertexAttrib {
    VisionVertexAttribPosition
};
static const CSVertexLayout __VisionVertexLayouts[] = {
    CSVertexLayout(VisionVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0)
};
const CSArray<CSVertexLayout>* VisionVertexLayouts = new CSArray<CSVertexLayout>(__VisionVertexLayouts, 1);

static const char* uniformNames[] = {
    "world",
    "viewProjection",
	"positionScale",
    "visionTexture",
	"visionScale"
};

VisionRenderer::VisionRenderer() {
    _program = new CSProgram();
    
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/vision.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/vision.fsh"));
    
    CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    
    _program->attach(vertexShader);
    _program->attach(fragmentShader);
    
    glBindAttribLocationCS(_program->object(), VisionVertexAttribPosition, "attrPosition");
    
    bool link = _program->link();
    
    CSAssert(link, "program link fail");
    
    for (int i = 0; i < VisionUniformCount; i++) {
        _uniforms[i] = glGetUniformLocationCS(_program->object(), uniformNames[i]);
    }
}

VisionRenderer::~VisionRenderer() {
    _program->release();
}

void VisionRenderer::draw(CSGraphics* graphics, const CSTVertexArray<CSVector3>* arr, const CSRootImage* vision) {
    graphics->push();
    graphics->setCullMode(CSCullBack);
    graphics->setBlendMode(CSBlendMultiply);
    graphics->setDepthMode(CSDepthRead);
    graphics->commit();
    
    glUseProgramCS(_program->object());
    glUniformMatrix4fvCS(_uniforms[VisionUniformWorld], 1, 0, graphics->world());
    glUniformMatrix4fvCS(_uniforms[VisionUniformViewProjection], 1, 0, graphics->camera().viewProjection());
	glUniform1fCS(_uniforms[VisionUniformPositionScale], TileSize);

    glActiveTextureCS(GL_TEXTURE0);
    glBindTextureCS(GL_TEXTURE_2D, vision->texture0());
    glUniform1iCS(_uniforms[VisionUniformVisionTexture], 0);
	glUniform2fCS(_uniforms[VisionUniformVisionScale], 1.0f / vision->width(), 1.0f / vision->height());

    arr->render(GL_TRIANGLES);
    
    graphics->pop();
}

