//
//  CSBlurShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 4. 23..
//  Copyright (c) 2012 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBlurShader.h"

#include "CSOpenGL.h"
#include "CSGraphics.h"
#include "CSString.h"
#include "CSFile.h"

static const char* uniformNames[] = {
    "worldViewProjection",
    "screen",
    "resolution",
    "direction",
	"centerCoord",
	"range"
};

CSBlurShader::CSBlurShader() {
	const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/blur.vsh"));
	const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/blur.fsh"));

	CSShader* vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
	CSAssert(vertexShader, "shader compile fail");

	for (int i = 0; i < 3; i++) {
		const char* option = CSString::cstringWithFormat("BlurType=%d", i);

		CSShader* fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER, option);
		CSAssert(fragmentShader, "shader compile fail");

		_programs[i].object = new CSProgram();
		_programs[i].object->attach(vertexShader);
		_programs[i].object->attach(fragmentShader);

		bool link = _programs[i].object->link();

		CSAssert(link, "program link fail");

		for (int j = 0; j < CSBlurUniformCount; j++) {
			_programs[i].uniforms[j] = glGetUniformLocationCS(_programs[i].object->object(), uniformNames[j]);
		}
	}
	glGenVertexArraysCS(1, &_vao);
	glGenBuffersCS(1, &_vbo);
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointerCS(CSBlurAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArrayCS(CSBlurAttribPosition);
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

CSBlurShader::~CSBlurShader() {
    _programs[0].object->release();
	_programs[1].object->release();
	_programs[2].object->release();

	glDeleteBuffersCS(1, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);
}

void CSBlurShader::draw(CSGraphics* graphics, const CSZRect& rect, float weight) {
    CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);
    
    graphics->target()->swap(false);
    
    const CSRootImage* screen = graphics->target()->screen(true);
    
    glDisableCS(GL_BLEND);
    glDisableCS(GL_CULL_FACE);
    glDepthMaskCS(GL_FALSE);
    
    glUseProgramCS(_programs[0].object->object());
    
    glUniformMatrix4fvCS(_programs[0].uniforms[CSBlurUniformWorldViewProjection], 1, 0, graphics->world() * graphics->camera().viewProjection());
    glUniform2fCS(_programs[0].uniforms[CSBlurUniformResolution], graphics->target()->width(), graphics->target()->height());
    
    glActiveTextureCS(GL_TEXTURE0);
    
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(_programs[0].uniforms[CSBlurUniformScreen], 0);
    
    glUniform2fCS(_programs[0].uniforms[CSBlurUniformDirection], weight * graphics->target()->width() / graphics->camera().width(), 0.0f);

	const float vertices[] = {
		rect.origin.x, rect.origin.y, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y, rect.origin.z,
		rect.origin.x, rect.origin.y + rect.size.height, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, rect.origin.z
	};
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    
    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);

    graphics->target()->swap(false);
    
    screen = graphics->target()->screen(true);
    
    glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
    glUniform1iCS(_programs[0].uniforms[CSBlurUniformScreen], 0);
    
    glUniform2fCS(_programs[0].uniforms[CSBlurUniformDirection], 0.0f, weight * graphics->target()->height() / graphics->camera().height());

    glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);
    
	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CSBlurShader::draw(CSGraphics* graphics, const CSZRect& rect, const CSVector2& dir) {
	CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);

	graphics->target()->swap(true);

	const CSRootImage* screen = graphics->target()->screen(true);

	glDisableCS(GL_BLEND);
	glDisableCS(GL_CULL_FACE);
	glDepthMaskCS(GL_FALSE);

	glUseProgramCS(_programs[1].object->object());

	glUniformMatrix4fvCS(_programs[1].uniforms[CSBlurUniformWorldViewProjection], 1, 0, graphics->world() * graphics->camera().viewProjection());
	glUniform2fCS(_programs[1].uniforms[CSBlurUniformResolution], graphics->target()->width(), graphics->target()->height());

	glActiveTextureCS(GL_TEXTURE0);

	glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
	glUniform1iCS(_programs[1].uniforms[CSBlurUniformScreen], 0);

	glUniform2fCS(_programs[1].uniforms[CSBlurUniformDirection], dir.x * graphics->target()->width() / graphics->camera().width(), dir.y * graphics->target()->height() / graphics->camera().height());

	const float vertices[] = {
		rect.origin.x, rect.origin.y, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y, rect.origin.z,
		rect.origin.x, rect.origin.y + rect.size.height, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, rect.origin.z
	};
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

	glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CSBlurShader::draw(CSGraphics* graphics, const CSZRect& rect, const CSVector3& center, float range) {
	CSGraphics::invalidate(CSGraphicsValidationBlend | CSGraphicsValidationCull | CSGraphicsValidationDepth);

	graphics->target()->swap(true);

	const CSRootImage* screen = graphics->target()->screen(true);

	glDisableCS(GL_BLEND);
	glDisableCS(GL_CULL_FACE);
	glDepthMaskCS(GL_FALSE);

	glUseProgramCS(_programs[2].object->object());

	CSMatrix worldViewProjection = graphics->world() * graphics->camera().viewProjection();

	glUniformMatrix4fvCS(_programs[2].uniforms[CSBlurUniformWorldViewProjection], 1, 0, worldViewProjection);
	glUniform2fCS(_programs[2].uniforms[CSBlurUniformResolution], graphics->target()->width(), graphics->target()->height());

	glActiveTextureCS(GL_TEXTURE0);

	glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
	glUniform1iCS(_programs[2].uniforms[CSBlurUniformScreen], 0);

	CSVector4 c = CSVector3::transform(center, worldViewProjection);
	glUniform2fCS(_programs[2].uniforms[CSBlurUniformCenterCoord], c.x / c.w * 0.5f + 0.5f, c.y / c.w * 0.5f + 0.5f);
	glUniform1fCS(_programs[2].uniforms[CSBlurUniformRange], range);

	const float vertices[] = {
		rect.origin.x, rect.origin.y, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y, rect.origin.z,
		rect.origin.x, rect.origin.y + rect.size.height, rect.origin.z,
		rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, rect.origin.z
	};
	glBindVertexArrayCS(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

	glDrawArraysCS(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArrayCS(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

