//
//  CSPixellateShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 4. 23..
//  Copyright (c) 2012 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSPixellateShader.h"

#include "CSOpenGL.h"
#include "CSGraphics.h"
#include "CSString.h"
#include "CSFile.h"

static const char* uniformNames[] = {
    "worldViewProjection",
    "screen",
    "resolution",
    "fraction"
};

CSPixellateShader::CSPixellateShader() {
    _program = new CSProgram();
    
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/pixellate.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/pixellate.fsh"));
    
    CSShader* vertexShader = (CSShader*)CSShader::shader(vs, GL_VERTEX_SHADER, NULL);
    CSShader* fragmentShader = (CSShader*)CSShader::shader(fs, GL_FRAGMENT_SHADER, NULL);
    
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    
    _program->attach(vertexShader);
    _program->attach(fragmentShader);
    
    glBindAttribLocationCS(_program->object(), CSPixellateAttribPosition, "attrPosition");
    
    bool link = _program->link();
    CSAssert(link, "program link fail");
    for (int i = 0; i < CSPixellateUniformCount; i++) {
        _uniforms[i] = glGetUniformLocationCS(_program->object(), uniformNames[i]);
    }

	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSPixellateAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArrayCS(CSPixellateAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CSPixellateShader::~CSPixellateShader() {
    _program->release();

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSPixellateShader::draw(CSGraphics* graphics, const CSZRect& rect, float fraction) {
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);
    
    graphics->target()->swap(true);
    
    const CSRootImage* screen = graphics->target()->screen(true);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDepthMaskCS(GL_FALSE);
    
    glUseProgramCS(_program->object());
    
    glUniformMatrix4fvCS(_uniforms[CSPixellateUniformWorldViewProjection], 1, 0, graphics->world() * graphics->camera().viewProjection());
    
    glActiveTextureCS(GL_TEXTURE0);
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(_uniforms[CSPixellateUniformScreen], 0);
    glUniform2fCS(_uniforms[CSPixellateUniformResolution], graphics->target()->width(), graphics->target()->height());
    glUniform2fCS(_uniforms[CSPixellateUniformFraction], fraction / graphics->camera().width(), fraction / graphics->camera().height());
    
    const float vertices[] = {
        rect.origin.x, rect.origin.y, rect.origin.z,
        rect.origin.x + rect.size.width, rect.origin.y, rect.origin.z,
        rect.origin.x, rect.origin.y + rect.size.height, rect.origin.z,
        rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, rect.origin.z
    };
    
	glBindVertexArrayCS(_vao);
	glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    
    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);
    
	glBindVertexArrayCS(0);
	glBindBufferCS(GL_ARRAY_BUFFER, 0);
}

