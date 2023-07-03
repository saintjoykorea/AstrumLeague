//
//  GridRenderer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "GridRenderer.h"

enum GridVertexAttrib {
    GridVertexAttribPosition,
    GridVertexAttribColor
};
static const CSVertexLayout __GridVertexLayouts[] = {
    CSVertexLayout(GridVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0),
    CSVertexLayout(GridVertexAttribColor, 3, GL_FLOAT, GL_FALSE, offsetof(GridVertex, color)),
};
const CSArray<CSVertexLayout>* GridVertexLayouts = new CSArray<CSVertexLayout>(__GridVertexLayouts, 3);

static const char* uniformNames[] = {
    "world",
    "viewProjection",
    "alpha"
};

GridRenderer::GridRenderer() {
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/grid.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/grid.fsh"));
    
    CSShader* vertexShader;
    CSShader* fragmentShader;
    bool link;
    
    _program = new CSProgram();
    vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    _program->attach(vertexShader);
    _program->attach(fragmentShader);
    glBindAttribLocationCS(_program->object(), GridVertexAttribPosition, "attrPosition");
    glBindAttribLocationCS(_program->object(), GridVertexAttribColor, "attrColor");
    link = _program->link();
    CSAssert(link, "program link fail");
    
    for (int i = 0; i < GridUniformCount; i++) {
        _uniforms[i] = glGetUniformLocationCS(_program->object(), uniformNames[i]);
    }
}

GridRenderer::~GridRenderer() {
    _program->release();
}

void GridRenderer::draw(CSGraphics* graphics, const CSTVertexArray<GridVertex>* arr, float alpha) {
    graphics->push();
    graphics->setCullMode(CSCullBack);
    graphics->setBlendMode(CSBlendNormal);
    graphics->setDepthMode(CSDepthRead);
    graphics->commit();
    
    glUseProgramCS(_program->object());
    
    glUniformMatrix4fvCS(_uniforms[GridUniformWorld], 1, 0, graphics->world());
    glUniformMatrix4fvCS(_uniforms[GridUniformViewProjection], 1, 0, graphics->camera().viewProjection());
    glUniform1fCS(_uniforms[GridUniformAlpha], alpha);

    arr->render(GL_LINES);
    
    graphics->pop();
}
