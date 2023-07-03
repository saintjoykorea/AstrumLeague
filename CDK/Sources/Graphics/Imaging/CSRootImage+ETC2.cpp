//
//  CSRootImage+ETC2.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

bool CSRootImage::loadETC2Data(const byte *data, uint length, uint format, Texture &texture) {
#ifdef CDK_ANDROID
    uint internalFormat;
    uint blockLength;
    switch (format & 0xff) {
        case CSImageFormatETC2_RGB:
            internalFormat = GL_COMPRESSED_RGB8_ETC2;
            blockLength = 8;
            break;
        case CSImageFormatETC2_RGBA:
            internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
            blockLength = 16;
            break;
        default:
            return false;
    }
    
    texture.width = texture.contentWidth;
    texture.height = texture.contentHeight;
    
    return loadCompressedData(data, length, format, internalFormat, 4, blockLength, false, texture);
#else
    return false;
#endif
}
