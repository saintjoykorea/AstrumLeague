//
//  CSGraphicsImpl.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphicsImpl.h"

#include "CSOpenGL.h"
#include "CSRootImage.h"
#include "CSFile.h"

CSGraphicsImpl* CSGraphicsImpl::__impl = NULL;

CSGraphicsImpl::CSGraphicsImpl() {
    _renderer = new CSRenderer();
    _characters = new CSCharacters();
    _fillShader = new CSFillShader();
    _defaultFont = new CSFont(14.0f, CSFontStyleNormal);
    
    _shockwaveShader = NULL;
    _lensShader = NULL;
    _swirlShader = NULL;
    _pixellateShader = NULL;
    _kuwaharaShader = NULL;
    _blurShader = NULL;
    _particleImage = NULL;
}

CSGraphicsImpl::~CSGraphicsImpl() {
    delete _renderer;
    delete _characters;
    delete _fillShader;
    if (_shockwaveShader) {
        delete _shockwaveShader;
    }
    if (_lensShader) {
        delete _lensShader;
    }
    if (_swirlShader) {
        delete _swirlShader;
    }
    if (_pixellateShader) {
        delete _pixellateShader;
    }
    if (_kuwaharaShader) {
        delete _kuwaharaShader;
    }
    if (_blurShader) {
        delete _blurShader;
    }
    CSObject::release(_particleImage);
    
    _defaultFont->release();
}

void CSGraphicsImpl::initialize() {
    if (!__impl) {
        __impl = new CSGraphicsImpl();
    }
}

void CSGraphicsImpl::finalize() {
    if (__impl) {
        delete __impl;
        __impl = NULL;
    }
}

void CSGraphicsImpl::reload() {
    delete _renderer;
    delete _fillShader;
    _characters->clear();
    if (_shockwaveShader) {
        delete _shockwaveShader;
        _shockwaveShader = NULL;
    }
    if (_lensShader) {
        delete _lensShader;
        _lensShader = NULL;
    }
    if (_swirlShader) {
        delete _swirlShader;
        _swirlShader = NULL;
    }
    if (_pixellateShader) {
        delete _pixellateShader;
        _pixellateShader = NULL;
    }
    if (_kuwaharaShader) {
        delete _kuwaharaShader;
        _kuwaharaShader = NULL;
    }
    if (_blurShader) {
        delete _blurShader;
        _blurShader = NULL;
    }
    CSObject::release(_particleImage);
    
    _renderer = new CSRenderer();
    _fillShader = new CSFillShader();
}

void CSGraphicsImpl::purge() {
    _characters->clear();
}

CSShockwaveShader* CSGraphicsImpl::shockwaveShader() {
    if (!_shockwaveShader) {
        _shockwaveShader = new CSShockwaveShader();
        CSLog("load shockwave filter");
    }
    return _shockwaveShader;
}

CSLensShader* CSGraphicsImpl::lensShader() {
    if (!_lensShader) {
        _lensShader = new CSLensShader();
        CSLog("load lens filter");
    }
    return _lensShader;
}

CSSwirlShader* CSGraphicsImpl::swirlShader() {
    if (!_swirlShader) {
        _swirlShader = new CSSwirlShader();
        CSLog("load swirl filter");
    }
    return _swirlShader;
}

CSPixellateShader* CSGraphicsImpl::pixellateShader() {
    if (!_pixellateShader) {
        _pixellateShader = new CSPixellateShader();
        CSLog("load pixellate filter");
    }
    return _pixellateShader;
}

CSKuwaharaShader* CSGraphicsImpl::kuwaharaShader() {
    if (!_kuwaharaShader) {
        _kuwaharaShader = new CSKuwaharaShader();
        CSLog("load kuwahara filter");
    }
    return _kuwaharaShader;
}

CSBlurShader* CSGraphicsImpl::blurShader() {
    if (!_blurShader) {
        _blurShader = new CSBlurShader();
        CSLog("load blur filter");
    }
    return _blurShader;
}

const CSRootImage* CSGraphicsImpl::particleImage() {
    if (!_particleImage) {
        _particleImage = CSRootImage::createWithContentOfFile(CSFile::bundlePath("graphics/particle.png"), CSImageFormatImageA8);
    }
    return _particleImage;
}
