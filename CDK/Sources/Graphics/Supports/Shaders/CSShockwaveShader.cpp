//
//  CSShockwaveShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 4. 26..
//  Copyright (c) 2012 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSShockwaveShader.h"

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
    "thickness"
};

CSShockwaveShader::CSShockwaveShader() {
    _program = new CSProgram();
    
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/shockwave.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/shockwave.fsh"));
    
    CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    
    _program->attach(vertexShader);
    _program->attach(fragmentShader);
    
    glBindAttribLocationCS(_program->object(), CSShockwaveAttribPosition, "attrPosition");
    
    bool link = _program->link();
    
    CSAssert(link, "program link fail");
    
    for (int i = 0; i < CSShockwaveUniformCount; i++) {
        _uniforms[i] = glGetUniformLocationCS(_program->object(), uniformNames[i]);
    }

	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSShockwaveAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArrayCS(CSShockwaveAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CSShockwaveShader::~CSShockwaveShader() {
    _program->release();

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSShockwaveShader::draw(CSGraphics* graphics, const CSVector3& center, float range, float thickness, float progress, bool dirtyOffscreen) {
    CSAssert(progress <= 1.0f, "invalid operation");
    
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);
    
    if (dirtyOffscreen) graphics->target()->swap(true);
    
    const CSRootImage* screen = graphics->target()->screen(true);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDepthMaskCS(GL_FALSE);
    
    glUseProgramCS(_program->object());
    
    CSMatrix worldViewProjection = graphics->world() * graphics->camera().viewProjection();
    
    glUniformMatrix4fvCS(_uniforms[CSShockwaveUniformWorldViewProjection], 1, 0, worldViewProjection);
    
    glActiveTextureCS(GL_TEXTURE0);
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(_uniforms[CSShockwaveUniformScreen], 0);
    glUniform2fCS(_uniforms[CSShockwaveUniformResolution], graphics->target()->width(), graphics->target()->height());
    glUniform3fCS(_uniforms[CSShockwaveUniformCenter], center.x, center.y, center.z);
    glUniform1fCS(_uniforms[CSShockwaveUniformRange], range * progress);
    glUniform1fCS(_uniforms[CSShockwaveUniformThickness], thickness * (1.0f - progress));
    
    const float vertices[] = {
        center.x - range - thickness, center.y - range - thickness, center.z,
        center.x + range + thickness, center.y - range - thickness, center.z,
        center.x - range - thickness, center.y + range + thickness, center.z,
        center.x + range + thickness, center.y + range + thickness, center.z
    };
    
	glBindVertexArrayCS(_vao);
	glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
	glBufferDataCS(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);
    
	glBindVertexArrayCS(0);
	glBindBufferCS(GL_ARRAY_BUFFER, 0);
}

