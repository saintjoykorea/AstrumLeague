//
//  CSRootImage+Compressed.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"
#include "CSMath.h"

bool CSRootImage::loadCompressedData(const byte* data, uint length, uint format, uint internalFormat, uint block, uint blockLength, bool alphaTexture, Texture &texture) {
    glGetErrorCS();     //consume

    glPixelStoreiCS(GL_UNPACK_ALIGNMENT, 1);
    
    texture.memory = 0;

    texture.object[1] = 0;
    
    const byte* end = data + length;
    
    for (int a = 0; a <= alphaTexture; a++) {
        texture.object[a] = genTexture(format);
        
        uint width = texture.width;
        uint height = texture.height;
        
        int mipmapCount = readByte(data);

        for (int i = 0; i < mipmapCount; i++) {
            uint blockWidth = (width + block - 1) / block;
            uint blockHeight = (height + block - 1) / block;
            uint dataLength = blockWidth * blockHeight * blockLength;
            
            glCompressedTexImage2DCS(GL_TEXTURE_2D, i, internalFormat, width, height, 0, dataLength, data);
            
            GLenum err = glGetErrorCS();
            
            if (err) {
                CSErrorLog("compressed format loading error:%04x / %04x", internalFormat, err);
                for (int d = 0; d <= a; d++) {
                    glDeleteTexturesCS(1, &texture.object[d]);
                }
                return false;
            }
            
            data += dataLength;
            
            texture.memory += dataLength;

            width = CSMath::max(1u, width >> 1);
            height = CSMath::max(1u, height >> 1);
        }
    }
    if (data != end) {
        CSErrorLog("compressed format loading invalid offset(%d-%d)", format & 0xff, (format & CSImageFormatMipmap) != 0);
        for (int a = 0; a <= alphaTexture; a++) {
            glDeleteTexturesCS(1, &texture.object[a]);
        }
        return false;
    }
    
    //CSLog("compressed image loaded(%d-%d):%d, %d = %d", format & 0xff, (format & CSImageFormatMipmap) != 0, texture.width, texture.height, texture.memory);
    
    return true;
}
