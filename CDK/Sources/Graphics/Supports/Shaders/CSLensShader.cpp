//
//  CSLensShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 4. 26..
//  Copyright (c) 2012 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLensShader.h"

#include "CSOpenGL.h"
#include "CSGraphics.h"
#include "CSString.h"
#include "CSFile.h"

static const char* uniformNames[] = {
    "worldViewProjection",
    "screen",
    "resolution",
    "center",
    "range",
    "convex"
};

CSLensShader::CSLensShader() {
    _program = new CSProgram();
    
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/lens.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/lens.fsh"));
    
    CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    
    _program->attach(vertexShader);
    _program->attach(fragmentShader);
    
    glBindAttribLocationCS(_program->object(), CSLensAttribPosition, "attrPosition");
    
    bool link = _program->link();
    
    CSAssert(link, "program link fail");
    
    for (int i = 0; i < CSLensUniformCount; i++) {
        _uniforms[i] = glGetUniformLocationCS(_program->object(), uniformNames[i]);
    }

	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSLensAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArrayCS(CSLensAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CSLensShader::~CSLensShader() {
    _program->release();

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSLensShader::draw(CSGraphics* graphics, const CSVector3& center, float range, bool convex, bool dirtyOffscreen) {
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);
    
    if (dirtyOffscreen) graphics->target()->swap(true);
    
    const CSRootImage* screen = graphics->target()->screen(true);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDepthMaskCS(GL_FALSE);
    
    glUseProgramCS(_program->object());
    
    glUniformMatrix4fvCS(_uniforms[CSLensUniformWorldViewProjection], 1, 0, graphics->world() * graphics->camera().viewProjection());
    
    glActiveTextureCS(GL_TEXTURE0);
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(_uniforms[CSLensUniformScreen], 0);
    glUniform2fCS(_uniforms[CSLensUniformResolution], graphics->target()->width(), graphics->target()->height());
    glUniform3fCS(_uniforms[CSLensUniformCenter], center.x, center.y, center.z);
    glUniform1fCS(_uniforms[CSLensUniformRange], range);
    glUniform1fCS(_uniforms[CSLensUniformConvex], convex ? 1 : -1);
    
    const float vertices[] = {
        center.x - range, center.y - range, center.z,
        center.x + range, center.y - range, center.z,
        center.x - range, center.y + range, center.z,
        center.x + range, center.y + range, center.z
    };
    
	glBindVertexArrayCS(_vao);
	glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    
    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);
    
	glBindVertexArrayCS(0);
	glBindBufferCS(GL_ARRAY_BUFFER, 0);
}

