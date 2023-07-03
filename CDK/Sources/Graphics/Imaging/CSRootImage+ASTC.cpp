//
//  CSRootImage+ASTC.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

#if defined(CDK_IOS) || defined(CDK_WINDOWS)
#define GL_COMPRESSED_RGBA_ASTC_4x4     GL_COMPRESSED_RGBA_ASTC_4x4_KHR
#define GL_COMPRESSED_RGBA_ASTC_5x5     GL_COMPRESSED_RGBA_ASTC_5x5_KHR
#define GL_COMPRESSED_RGBA_ASTC_6x6     GL_COMPRESSED_RGBA_ASTC_6x6_KHR
#define GL_COMPRESSED_RGBA_ASTC_8x8     GL_COMPRESSED_RGBA_ASTC_8x8_KHR
#define GL_COMPRESSED_RGBA_ASTC_10x10   GL_COMPRESSED_RGBA_ASTC_10x10_KHR
#define GL_COMPRESSED_RGBA_ASTC_12x12   GL_COMPRESSED_RGBA_ASTC_12x12_KHR
#endif

bool CSRootImage::loadASTCData(const byte *data, uint length, uint format, Texture &texture) {
    static bool astcSupported = glIsSupportedExtensionCS("GL_KHR_texture_compression_astc_hdr") || glIsSupportedExtensionCS("GL_KHR_texture_compression_astc_ldr");
    
    if (!astcSupported) return false;
    
    uint internalFormat;
    uint block;
    switch (format & 0xff) {
        case CSImageFormatASTC_4x4:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4;
            block = 4;
            break;
        case CSImageFormatASTC_5x5:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5;
            block = 5;
            break;
        case CSImageFormatASTC_6x6:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6;
            block = 6;
            break;
        case CSImageFormatASTC_8x8:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8;
            block = 8;
            break;
        case CSImageFormatASTC_10x10:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10;
            block = 10;
            break;
        case CSImageFormatASTC_12x12:
            internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12;
            block = 12;
            break;
        default:
            return false;
    }
    
    texture.width = texture.contentWidth;
    texture.height = texture.contentHeight;
    
    return loadCompressedData(data, length, format, internalFormat, block, 16, false, texture);
}
