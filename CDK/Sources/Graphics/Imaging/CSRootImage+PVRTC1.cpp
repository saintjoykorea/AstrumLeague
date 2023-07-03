//
//  CSRootImage+PVRTC1.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

bool CSRootImage::loadPVRTC1Data(const byte *data, uint length, uint format, Texture &texture) {
#ifdef CDK_IOS
    bool alphaTexture;
    switch (format & 0xff) {
        case CSImageFormatPVRTC1_RGB:
            alphaTexture = false;
            break;
        case CSImageFormatPVRTC1_RGBA:
            alphaTexture = true;
            break;
        default:
            return false;
    }
    texture.width = texture.height = pot(CSMath::max(texture.contentWidth, texture.contentHeight));
    
    return loadCompressedData(data, length, format, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 8, 32, alphaTexture, texture);
#else
    return false;
#endif
}
