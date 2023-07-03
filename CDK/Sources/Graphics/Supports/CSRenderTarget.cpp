//
//  CSRenderTarget.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 23..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRenderTarget.h"

#include "CSGraphicsImpl.h"

#include "CSGraphics.h"

#include "CSRaster.h"

CSRenderTarget::CSRenderTarget(uint width, uint height, bool depthStencil) : _width(width), _height(height), _depthStencil(depthStencil) {
    _identifier.reset();
}

CSRenderTarget::CSRenderTarget(CSRootImage* image, bool depthStencil) : _width(image->width()), _height(image->height()), _depthStencil(depthStencil) {
    CSAssert(image->textureWidth() == image->width() && image->textureHeight() == image->height(), "invalid operation");
    
    _identifier.reset();
    
    _origin = true;
    _screens[0] = retain(image);
    glGenFramebuffersCS(1, &_frameBuffers[0]);
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[0]);
    glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _screens[0]->texture0(), 0);
    
    CSGraphics::invalidate(CSGraphicsValidationTarget);
}

CSRenderTarget::~CSRenderTarget() {
    if (_identifier.isValid()) {
        glDeleteTexturesCS(1, &_depthStencilTexture);
        glDeleteFramebuffersCS(2, _frameBuffers);
    }
    release(_screens[0]);
    release(_screens[1]);
}

void CSRenderTarget::resize(uint width, uint height) {
    CSAssert(!_origin, "invalid operation");
    if (_width != width || _height != height) {
        _width = width;
        _height = height;
        
        if (_identifier.isValid()) {
            glDeleteTexturesCS(1, &_depthStencilTexture);
            glDeleteFramebuffersCS(2, _frameBuffers);
        }
        _depthStencilTexture = 0;
        _frameBuffers[0] = 0;
        _frameBuffers[1] = 0;
        release(_screens[0]);
        release(_screens[1]);
        
        CSGraphics::invalidate(CSGraphicsValidationTarget);
    }
}

static void attachDepthStencilTexture(uint texture) {
    glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
}

void CSRenderTarget::validate() {
    _identifier.use();

	if (!_screens[_cursor]) {
        _screens[_cursor] = CSRootImage::createWithFormat(CSImageFormatRawRGBA8888, _width, _height);
        CSAssert(_screens[_cursor], "invalid operation");
        glGenFramebuffersCS(1, &_frameBuffers[_cursor]);
        glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
        glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _screens[_cursor]->texture0(), 0);
        if (_depthStencilTexture) {
            attachDepthStencilTexture(_depthStencilTexture);
        }
		CSAssert(glCheckFramebufferStatusCS(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "invalid state");
	}
    glViewportCS(0, 0, _width, _height);
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
    
    CSGraphics::invalidate(this, CSGraphicsValidationTarget);
}

void CSRenderTarget::swap(bool copy) {
    _identifier.use();
    
    CSGraphics::invalidate(this, CSGraphicsValidationTarget);
    
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
    
    if (_depthStencilTexture) {
        attachDepthStencilTexture(0);
    }
    
    _cursor = !_cursor;
    
    validate();
    
    if (copy) {
        blit(_frameBuffers[_cursor], true);
    }
    if (_depthStencilTexture) {
        attachDepthStencilTexture(_depthStencilTexture);
    }
}

void CSRenderTarget::blit(uint frameBuffer, bool opposite) {
    CSGraphics::invalidate(this, CSGraphicsValidationTarget);

	CSGraphics::invalidate(CSGraphicsValidationScissor);
    glDisableCS(GL_SCISSOR_TEST);
        
    glBindFramebufferCS(GL_READ_FRAMEBUFFER, _frameBuffers[_cursor ^ opposite]);
    glBindFramebufferCS(GL_DRAW_FRAMEBUFFER, frameBuffer);
    glBlitFramebufferCS(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
}

void CSRenderTarget::useDepthStencil() {
    _identifier.use();
    
    if (_depthStencil && !_depthStencilTexture) {
        glGenTexturesCS(1, &_depthStencilTexture);
        glBindTextureCS(GL_TEXTURE_2D, _depthStencilTexture);
        glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2DCS(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
        attachDepthStencilTexture(_depthStencilTexture);
        glDepthMaskCS(GL_TRUE);
        glStencilMaskCS(-1);
        glClearDepthfCS(1);
        glClearStencilCS(0);
        glClearCS(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        CSGraphics::invalidate(this, CSGraphicsValidationTarget);
        CSGraphics::invalidate(CSGraphicsValidationDepth);
    }
}

void CSRenderTarget::flush() {
    if (_origin && _cursor) {
        swap(true);
    }
}

bool CSRenderTarget::screenshot(const char* path) const {
    uint* originPixels = (uint*)malloc(_width * _height * 4);
    if (!originPixels) {
        return false;
    }
    uint* platformPixels = (uint*)malloc(_width * _height * 4);
    if (!platformPixels) {
        free(originPixels);
        return false;
    }
    CSGraphics::invalidate(this, CSGraphicsValidationTarget);
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffers[_cursor]);
    glReadPixelsCS(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, originPixels);
    
    uint offset1, offset2;
    for (int i = 0; i < _height; i++) {
        offset1 = i * _width;
        offset2 = (_height - i - 1) * _width;
        for (int j = 0; j < _width; j++) {
            uint pixel = originPixels[offset1 + j];
#ifdef CDK_ANDROID      //argb source for android
            uint blue = (pixel >> 16) & 0xff;
            uint red = (pixel << 16) & 0x00ff0000;
            pixel = (pixel & 0xff00ff00) | red | blue;
#endif
            platformPixels[offset2 + j] = pixel;
        }
    }
    free(originPixels);
    bool rtn = CSRaster::saveImageFromRaster(path, platformPixels, _width, _height);
    free(platformPixels);
    return rtn;
}

