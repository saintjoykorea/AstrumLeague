//
//  CSScratch.cpp
//  CDK
//
//  Created by Kim Chan on 2016. 3. 15..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSScratch.h"

#include "CSOpenGL.h"

CSScratch* CSScratch::create(uint format, uint contentWidth, uint contentHeight) {
    CSRootImage* image = CSRootImage::imageWithFormat(format, contentWidth, contentHeight);
    
    return image ? create(image) : NULL;
}

CSScratch* CSScratch::create(CSRootImage* image) {
    uint textureFormat;
    uint textureType;
    int bpp;
    
    uint format = image->format();
    
    switch (format & 31) {
        case CSImageFormatRawRGBA8888:
            textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 4;
            break;
        case CSImageFormatRawRGB565:
            textureFormat = GL_RGB;
            textureType = GL_UNSIGNED_SHORT_5_6_5;
            bpp = 2;
            break;
        case CSImageFormatRawRGBA5551:
            textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_SHORT_5_5_5_1;
            bpp = 2;
            break;
        case CSImageFormatRawRGBA4444:
            textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_SHORT_4_4_4_4;
            bpp = 2;
            break;
        case CSImageFormatRawL8:
        case CSImageFormatRawA8:
        case CSImageFormatRawR8:
            textureFormat = GL_RED;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 1;
            break;
        case CSImageFormatRawL8A8:
        case CSImageFormatRawR8G8:
            textureFormat = GL_RG;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 2;
            break;
        default:
            CSAssert(false, "unsupported format:%d", format);
            return NULL;
    }
    void* raster = calloc(image->textureWidth() * image->textureHeight(), bpp);
    if (!raster) {
        return NULL;
    }
    CSScratch* scratch = new CSScratch();
    scratch->_image = retain(image);
    scratch->_textureFormat = textureFormat;
    scratch->_textureType = textureType;
    scratch->_raster = raster;
    scratch->_bpp = bpp;
    return scratch;
}

CSScratch::~CSScratch() {
    if (_image->isValid()) {
        transfer();
    }
    free(_raster);
    _image->release();
}

uint CSScratch::pixel(uint x, uint y) const {
	if (x < _image->textureWidth() && y < _image->textureHeight()) {
		uint i = y * _image->textureWidth() + x;
		switch (_bpp) {
			case 1:
				return ((byte*)_raster)[i];
			case 2:
				return ((ushort*)_raster)[i];
			case 4:
				return ((uint*)_raster)[i];
		}
	}
	return 0;
}

void CSScratch::update(uint x, uint y, uint data) {
    if (x < _image->textureWidth() && y < _image->textureHeight()) {
        uint i = y * _image->textureWidth() + x;
        switch (_bpp) {
            case 1:
                ((byte*)_raster)[i] = data;
                break;
            case 2:
                ((ushort*)_raster)[i] = data;
                break;
            case 4:
                ((uint*)_raster)[i] = data;
                break;
        }
        _dirty = true;
    }
}

void CSScratch::transfer() const {
    if (_dirty) {
        glBindTextureCS(GL_TEXTURE_2D, _image->texture0());
        glPixelStoreiCS(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2DCS(GL_TEXTURE_2D, 0, 0, 0, _image->textureWidth(), _image->textureHeight(), _textureFormat, _textureType, _raster);      //glTexImage2D 를 사용하면 데이터가 어긋나는 단말있음(ASUS P01MA)
        _dirty = false;
    }
}


