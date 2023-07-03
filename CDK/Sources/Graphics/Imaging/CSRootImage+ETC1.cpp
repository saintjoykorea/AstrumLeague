//
//  CSRootImage+ETC1.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

#define GL_COMPRESSED_RGB8_ETC1     0x8D64

bool CSRootImage::loadETC1Data(const byte *data, uint length, uint format, Texture &texture) {
#ifdef CDK_ANDROID
    bool alphaTexture;
    switch (format & 0xff) {
        case CSImageFormatETC1_RGB:
            alphaTexture = false;
            break;
        case CSImageFormatETC1_RGBA:
            alphaTexture = true;
            break;
        default:
            return false;
    }
    texture.width = pot(texture.contentWidth);
    texture.height = pot(texture.contentHeight);
    
    return loadCompressedData(data, length, format, GL_COMPRESSED_RGB8_ETC1, 4, 8, alphaTexture, texture);
#else
    return false;
#endif
}
