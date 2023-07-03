//
//  CSRootImage+Texture.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"

#include "CSGraphicsSetting.h"

uint CSRootImage::pot(uint width) {
    uint i = 1;
    while (i < width)
        i *= 2;
    return i;
}

uint CSRootImage::genTexture(uint format) {
    GLint wraps;
    if (format & CSImageFormatWrapSRepeat) wraps = GL_REPEAT;
    else if (format & CSImageFormatWrapSMirror) wraps = GL_MIRRORED_REPEAT;
    else wraps = GL_CLAMP_TO_EDGE;
    GLint wrapt;
    if (format & CSImageFormatWrapSRepeat) wrapt = GL_REPEAT;
    else if (format & CSImageFormatWrapSMirror) wrapt = GL_MIRRORED_REPEAT;
    else wrapt = GL_CLAMP_TO_EDGE;
    
	GLint minFilter;
	if (format & CSImageFormatMipmapLinear) {
		if (CSGraphicsSetting::usingMipmapLinear()) {
			minFilter = (format & CSImageFormatMinFilterNearest) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
		}
		else {
			minFilter = (format & CSImageFormatMinFilterNearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST;
		}
	}
	else if (format & CSImageFormatMipmapNearest) {
		minFilter = (format & CSImageFormatMinFilterNearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST;
	}
	else {
		minFilter = (format & CSImageFormatMinFilterNearest) ? GL_NEAREST : GL_LINEAR;
	}

    GLint magFilter = (format & CSImageFormatMagFilterNearest) ? GL_NEAREST : GL_LINEAR;
    
    uint texture;
    glGenTexturesCS(1, &texture);
    glBindTextureCS(GL_TEXTURE_2D, texture);
    
    glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    
    glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    return texture;
}
