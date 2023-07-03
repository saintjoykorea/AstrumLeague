//
//  CSKuwaharaShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 4. 23..
//  Copyright (c) 2012 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSKuwaharaShader.h"

#include "CSOpenGL.h"
#include "CSGraphics.h"
#include "CSString.h"
#include "CSFile.h"

static const char* uniformNames[] = {
    "worldViewProjection",
    "screen",
    "resolution",
    "projection"
};

bool CSKuwaharaShader::Program::link() {
    glBindAttribLocationCS(_object, CSKuwaharaAttribPosition, "attrPosition");
    
    if (CSProgram::link()) {
        for (int i = 0; i < CSKuwaharaUniformCount; i++) {
            uniforms[i] = glGetUniformLocationCS(_object, uniformNames[i]);
        }
        return true;
    }
    return false;
}

CSKuwaharaShader::CSKuwaharaShader() {
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/kuwahara.vsh"));
    CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    CSAssert(vertexShader, "shader compile fail");
    
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/kuwahara.fsh"));
    
    for (int i = 0; i < CSKuwaharaMaxRadius; i++) {
        _programs[i] = new Program();
        
        const char* options = *CSString::stringWithFormat("Radius    %d", i + 1);
        CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER, options);
        CSAssert(fragmentShader, "shader compile fail");
        
        _programs[i]->attach(vertexShader);
        _programs[i]->attach(fragmentShader);
        
        bool link = _programs[i]->link();
        
        CSAssert(link, "program link fail");
    }

	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSKuwaharaAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArrayCS(CSKuwaharaAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CSKuwaharaShader::~CSKuwaharaShader() {
    for (int i = 0; i < CSKuwaharaMaxRadius; i++) {
        _programs[i]->release();
    }

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSKuwaharaShader::draw(CSGraphics* graphics, const CSZRect& rect, int radius) {
    CSAssert(radius >0 && radius <= CSKuwaharaMaxRadius, "invalid operation");
    
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);
    
    graphics->target()->swap(true);
    
    const CSRootImage* screen = graphics->target()->screen(true);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDepthMaskCS(GL_FALSE);
    
    Program* program = _programs[radius - 1];
    
    glUseProgramCS(program->object());
    
    glUniformMatrix4fvCS(program->uniforms[CSKuwaharaUniformWorldViewProjection], 1, 0, graphics->world() * graphics->camera().viewProjection());
    
    glActiveTextureCS(GL_TEXTURE0);
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(program->uniforms[CSKuwaharaUniformScreen], 0);
    glUniform2fCS(program->uniforms[CSKuwaharaUniformResolution], graphics->target()->width(), graphics->target()->height());
    glUniform2fCS(program->uniforms[CSKuwaharaUniformProjection], graphics->camera().width(), graphics->camera().height());
    
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

