//
//  CSShader.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSFillShader.h"

#include "CSOpenGL.h"
#include "CSVertex.h"
#include "CSString.h"
#include "CSFile.h"

#include "CSGraphics.h"

enum CSFillAttrib {
    CSFillAttribPosition,
	CSFillAttribScreenCoord,
    CSFillAttribCount
};

CSFillShader::CSFillShader() {
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/fill.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/fill.fsh"));
    
    CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    CSAssert(vertexShader, "shader compile fail");
    
    CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    CSAssert(fragmentShader, "shader compile fail");

    _programs[0] = new CSProgram();
    _programs[0]->attach(vertexShader);
    _programs[0]->attach(fragmentShader);
    glBindAttribLocationCS(_programs[0]->object(), CSFillAttribPosition, "attrPosition");
    
    bool link = _programs[0]->link();
    CSAssert(link, "CSFillShader:program link fail");
    
	vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER, "UsingScreen");
	CSAssert(vertexShader, "shader compile fail");

    fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER, "UsingScreen");
    CSAssert(fragmentShader, "shader compile fail");
    
    _programs[1] = new CSProgram();
    _programs[1]->attach(vertexShader);
    _programs[1]->attach(fragmentShader);
    glBindAttribLocationCS(_programs[1]->object(), CSFillAttribPosition, "attrPosition");
	glBindAttribLocationCS(_programs[1]->object(), CSFillAttribScreenCoord, "attrScreenCoord");
    
    link = _programs[1]->link();
    CSAssert(link, "program link fail");
    
    _screenUniform = glGetUniformLocationCS(_programs[1]->object(), "screen");

	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSFillAttribPosition, 2, GL_FLOAT, GL_FALSE, 16, (const GLvoid*)0);
	glVertexAttribPointerCS(CSFillAttribScreenCoord, 2, GL_FLOAT, GL_FALSE, 16, (const GLvoid*)8);
	glEnableVertexAttribArrayCS(CSFillAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CSFillShader::~CSFillShader() {
    _programs[0]->release();
    _programs[1]->release();

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSFillShader::draw(const CSImage* screen, const CSRenderBounds& renderBounds) {
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth | CSGraphicsValidationScissor);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDisableCS(GL_DEPTH_TEST);
    glDisableCS(GL_SCISSOR_TEST);
    glDepthMaskCS(GL_FALSE);
    
    glUseProgramCS(_programs[screen != NULL]->object());
    
	if (screen) {
		glActiveTextureCS(GL_TEXTURE0);
		glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
		glUniform1iCS(_screenUniform, 0);
	}
    
	const float vertices[] = {
		renderBounds.left, renderBounds.top, 0.0f, 0.0f,
		renderBounds.right, renderBounds.top, 1.0f, 0.0f,
		renderBounds.left, renderBounds.bottom, 0.0f, 1.0f,
		renderBounds.right, renderBounds.bottom, 1.0f, 1.0f
	};
	glBindVertexArrayCS(_vao);
	glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
	glBufferDataCS(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
	
	if (screen) {
		glEnableVertexAttribArrayCS(CSFillAttribScreenCoord);
	}
	else {
		glDisableVertexAttribArrayCS(CSFillAttribScreenCoord);
	}

    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);
    
	glBindVertexArrayCS(0);
	glBindBufferCS(GL_ARRAY_BUFFER, 0);
}

