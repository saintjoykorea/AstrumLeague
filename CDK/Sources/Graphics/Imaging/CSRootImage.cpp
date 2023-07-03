//
//  CSRootImage2.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSFile.h"
#include "CSBuffer.h"
#include "CSURLConnection.h"

CSArray<CSRootImage*>* CSRootImage::__allImages = new CSArray<CSRootImage*>();

CSRootImage::CSRootImage(uint format, const Texture& texture) :
    _format(format),
    _texture{texture.object[0], texture.object[1]},
    _textureSize(texture.width, texture.height),
    _frame(CSRect(0, 0, texture.contentWidth, texture.contentHeight)),
    _memory(texture.memory)
{
    _identifier.reset();
    
    CSAssert(texture.object[0], "invalid data");

	__allImages->addObject(this);
}

CSRootImage::~CSRootImage() {
    if (_identifier.isValid()) {
        glDeleteTexturesCS(_texture[1] ? 2 : 1, &_texture[0]);
    }

	__allImages->removeObjectIdenticalTo(this);
}

CSRootImage* CSRootImage::createWithFormat(uint format, uint contentWidth, uint contentHeight) {
    return createWithRaw(NULL, format, contentWidth, contentHeight);
}

CSRootImage* CSRootImage::createWithRaw(const void *raw, uint format, uint contentWidth, uint contentHeight) {
    Texture texture;
    texture.contentWidth = contentWidth;
    texture.contentHeight = contentHeight;

    return loadRawData((const byte*)raw, format, texture) ? new CSRootImage(format, texture) : NULL;
}

CSRootImage* CSRootImage::createWithImage(const void* source, uint length, uint format) {
    Texture texture;
    
    return loadImageData((const byte*)source, length, format, texture) ? new CSRootImage(format, texture) : NULL;
}

CSRootImage* CSRootImage::createWithBuffer(CSBuffer* buffer) {
    uint length = buffer->readInt();
    const byte* data = (const byte*)buffer->bytes();
    buffer->skip(length);
    
    uint attr = readShort(data) << 8;
    
    Texture texture;
    texture.contentWidth = readShort(data);
    texture.contentHeight = readShort(data);
    
    int sourceCount = readByte(data);
    for (int i = 0; i < sourceCount; i++) {
        uint format = (CSImageFormat)readByte(data) | attr;
        
        uint length = readInt(data);
        
        bool success = false;
        
        switch (format & 0xff) {
            case CSImageFormatASTC_4x4:
            case CSImageFormatASTC_5x5:
            case CSImageFormatASTC_6x6:
            case CSImageFormatASTC_8x8:
            case CSImageFormatASTC_10x10:
            case CSImageFormatASTC_12x12:
                success = loadASTCData(data, length, format, texture);
                break;
            case CSImageFormatETC2_RGB:
            case CSImageFormatETC2_RGBA:
                success = loadETC2Data(data, length, format, texture);
                break;
            case CSImageFormatETC1_RGB:
            case CSImageFormatETC1_RGBA:
                success = loadETC1Data(data, length, format, texture);
                break;
            case CSImageFormatPVRTC1_RGB:
            case CSImageFormatPVRTC1_RGBA:
                success = loadPVRTC1Data(data, length, format, texture);
                break;
			case CSImageFormatDXT1:
			case CSImageFormatDXT3:
			case CSImageFormatDXT5:
				success = loadDXTData(data, length, format, texture);
				break;
            case CSImageFormatRawRGBA8888:
            case CSImageFormatRawRGBA4444:
            case CSImageFormatRawRGBA5551:
            case CSImageFormatRawRGB565:
            case CSImageFormatRawL8A8:
            case CSImageFormatRawL8:
            case CSImageFormatRawA8:
            case CSImageFormatRawR8:
            case CSImageFormatRawR8G8:
                success = loadRawData(data, format, texture);
                break;
            case CSImageFormatImageRGBA8888:
            case CSImageFormatImageRGBA4444:
            case CSImageFormatImageRGBA5551:
            case CSImageFormatImageRGB565:
            case CSImageFormatImageL8A8:
            case CSImageFormatImageL8:
            case CSImageFormatImageA8:
            case CSImageFormatImageR8:
            case CSImageFormatImageR8G8:
                success = loadImageData(data, length, format, texture);
                break;
        }
        if (success) {
            return new CSRootImage(format, texture);
        }
        else {
            data += length;
        }
    }
    
    return NULL;
}

CSRootImage* CSRootImage::createWithContentOfFile(const char* path, uint format) {
    uint length = 0;
    
    void* source = CSFile::createRawWithContentOfFile(path, 0, length, false);
    
    if (!source) {
        return NULL;
    }
    CSRootImage* image = createWithImage(source, length, format);
    
    free(source);
    
    return image;
}

CSRootImage* CSRootImage::createWithContentOfURL(const CSURLRequest* request, uint format) {
    const CSData* data = CSURLConnection::sendSynchronousRequest(request);
    
    if (!data) {
        return NULL;
    }
    CSRootImage* image = createWithImage(data->bytes(), data->length(), format);
    
    return image;
}

CSRootImage* CSRootImage::imageWithBuffer(CSBuffer* buffer) {
    return autorelease(createWithBuffer(buffer));
}

