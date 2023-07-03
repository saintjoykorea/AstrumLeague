//
//  CSRenderTarget.h
//  CDK
//
//  Created by 김찬 on 12. 8. 6..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#ifndef __CDK__CSRenderTarget__
#define __CDK__CSRenderTarget__

#include "CSRootImage.h"

class CSRenderTarget : public CSObject {
private:
    CSRootImage* _screens[2];
    uint _frameBuffers[2];
    uint _depthStencilTexture;
    uint _width;
    uint _height;
    bool _cursor;
    bool _origin;
    bool _depthStencil;
    CSGLContextIdentifier _identifier;
public:
    CSRenderTarget(uint width, uint height, bool depthStencil);
    CSRenderTarget(CSRootImage* origin, bool depthStencil);
private:
    ~CSRenderTarget();
public:
    static inline CSRenderTarget* target(uint width, uint height, bool depthStencil) {
        return autorelease(new CSRenderTarget(width, height, depthStencil));
    }
    static inline CSRenderTarget* target(CSRootImage* origin, bool depthStencil) {
        return autorelease(new CSRenderTarget(origin, depthStencil));
    }
public:
    inline bool isValid() const {
        return _identifier.isValid();
    }
    inline uint width() const {
        return _width;
    }
    inline uint height() const {
        return _height;
    }
    inline CSRootImage* screen(bool opposite) {
        return _screens[_cursor ^ opposite];
    }
    inline const CSRootImage* screen(bool opposite) const {
        return _screens[_cursor ^ opposite];
    }
    void resize(uint width, uint height);
    void validate();
    void swap(bool copy);
    void blit(uint framebuffer, bool opposite);
    
    inline uint depthStencilTexture() const {
        return _depthStencilTexture;
    }
    
    void useDepthStencil();
    
    inline bool usingDepthStencil() const {
        return _depthStencil;
    }
    
    void flush();
    
    bool screenshot(const char* path) const;
};

#endif
