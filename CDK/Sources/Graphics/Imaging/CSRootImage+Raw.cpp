//
//  CSRootImage+Raw.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

bool CSRootImage::loadRawData(const byte *data, uint format, Texture &texture) {
    GLenum internalTextureFormat;
    GLenum textureFormat;
    GLenum textureType;
    int bpp;
    
    glGetErrorCS();     //consume
    
    switch (format & 0xff) {
        case CSImageFormatRawRGBA8888:
            internalTextureFormat = textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 4;
            break;
        case CSImageFormatRawRGB565:
            internalTextureFormat = textureFormat = GL_RGB;
            textureType = GL_UNSIGNED_SHORT_5_6_5;
            bpp = 2;
            break;
        case CSImageFormatRawRGBA5551:
            internalTextureFormat = textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_SHORT_5_5_5_1;
            bpp = 2;
            break;
        case CSImageFormatRawRGBA4444:
            internalTextureFormat = textureFormat = GL_RGBA;
            textureType = GL_UNSIGNED_SHORT_4_4_4_4;
            bpp = 2;
            break;
		case CSImageFormatRawL8:
		case CSImageFormatRawA8:
        case CSImageFormatRawR8:
            internalTextureFormat = GL_R8;
            textureFormat = GL_RED;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 1;
            break;
		case CSImageFormatRawL8A8:
		case CSImageFormatRawR8G8:
            internalTextureFormat = GL_RG8;
            textureFormat = GL_RG;
            textureType = GL_UNSIGNED_BYTE;
            bpp = 2;
            break;
        default:
            CSErrorLog("raw unknown format:%d", format);
            return false;
    }
    
    if (format & CSImageFormatMipmap) {
        texture.width = pot(texture.contentWidth);
        texture.height = pot(texture.contentHeight);
    }
    else {
        texture.width = texture.contentWidth;
        texture.height = texture.contentHeight;
    }
    texture.object[0] = genTexture(format);
    texture.object[1] = 0;
    
    glPixelStoreiCS(GL_UNPACK_ALIGNMENT, 1);
    
    if (texture.contentWidth == texture.width && texture.contentHeight == texture.height) {
        glTexImage2DCS(GL_TEXTURE_2D, 0, internalTextureFormat, texture.width, texture.height, 0, textureFormat, textureType, data);
    }
    else {
        glTexImage2DCS(GL_TEXTURE_2D, 0, internalTextureFormat, texture.width, texture.height, 0, textureFormat, textureType, NULL);
        if (data) {
            glTexSubImage2DCS(GL_TEXTURE_2D, 0, 0, 0, texture.contentWidth, texture.contentHeight, textureFormat, textureType, data);
        }
    }
    
    GLenum err = glGetErrorCS();
    
    if (err) {
        CSErrorLog("raw format loading error:%04x, %04x", textureFormat, err);
        
        glDeleteTexturesCS(1, &texture.object[0]);
        
        return false;
    }
    
    texture.memory = texture.width * texture.height * bpp;
    
    if (format & CSImageFormatMipmap) {
        glGenerateMipmapCS(GL_TEXTURE_2D);
        texture.memory = texture.memory * 4 / 3;
    }
    
    CSLog("raw image loaded(%d-%d):%d, %d = %d", format & 0xff, (format & CSImageFormatMipmap) != 0, texture.width, texture.height, texture.memory);
    
    return true;
}
