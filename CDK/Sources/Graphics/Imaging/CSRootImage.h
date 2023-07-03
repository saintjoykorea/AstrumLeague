//
//  CSRootImage.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef CSRootImage_h
#define CSRootImage_h

#include "CSImage.h"

#include "CSGLContextIdentifier.h"

#include "CSArray.h"

class CSBuffer;
class CSURLRequest;

enum CSImageFormat {
    CSImageFormatASTC_4x4,
    CSImageFormatASTC_5x5,
    CSImageFormatASTC_6x6,
    CSImageFormatASTC_8x8,
    CSImageFormatASTC_10x10,
    CSImageFormatASTC_12x12,
    CSImageFormatETC2_RGB,
    CSImageFormatETC2_RGBA,
    CSImageFormatETC1_RGB,
    CSImageFormatETC1_RGBA,
    CSImageFormatPVRTC1_RGB,
    CSImageFormatPVRTC1_RGBA,
	CSImageFormatDXT1,
	CSImageFormatDXT3,
	CSImageFormatDXT5,
    CSImageFormatRawRGBA8888,
    CSImageFormatRawRGBA4444,
    CSImageFormatRawRGBA5551,
    CSImageFormatRawRGB565,
    CSImageFormatRawL8A8,
    CSImageFormatRawL8,
    CSImageFormatRawA8,
    CSImageFormatRawR8,
    CSImageFormatRawR8G8,
    CSImageFormatImageRGBA8888,
    CSImageFormatImageRGBA4444,
    CSImageFormatImageRGBA5551,
    CSImageFormatImageRGB565,
    CSImageFormatImageL8A8,
    CSImageFormatImageL8,
    CSImageFormatImageA8,
    CSImageFormatImageR8,
    CSImageFormatImageR8G8
};

#define CSImageFormatMipmap				(3 << 14)
#define CSImageFormatMipmapNearest      (2 << 14)
#define CSImageFormatMipmapLinear       (1 << 14)
#define CSImageFormatWrapSRepeat        (1 << 12)
#define CSImageFormatWrapSMirror        (2 << 12)
#define CSImageFormatWrapTRepeat        (1 << 10)
#define CSImageFormatWrapTMirror        (2 << 10)
#define CSImageFormatMinFilterNearest   (1 << 9)
#define CSImageFormatMagFilterNearest   (1 << 8)

class CSRootImage : public CSImage {
private:
    uint _format;
    uint _texture[2];
    CSSize _textureSize;
    CSRect _frame;
    CSGLContextIdentifier _identifier;
    uint _memory;
    
    struct Texture {
        uint object[2];
        uint contentWidth;
        uint contentHeight;
        uint width;
        uint height;
        uint memory;
    };

	static CSArray<CSRootImage*>* __allImages;

    CSRootImage(uint format, const Texture& texture);
    ~CSRootImage();
public:
    static CSRootImage* createWithFormat(uint format, uint contentWidth, uint contentHeight);
    static CSRootImage* createWithRaw(const void *raw, uint format, uint contentWidth, uint contentHeight);
    static CSRootImage* createWithImage(const void* source, uint length, uint format);
    static CSRootImage* createWithBuffer(CSBuffer* buffer);
    static CSRootImage* createWithContentOfFile(const char* path, uint format);
    static CSRootImage* createWithContentOfURL(const CSURLRequest* request, uint format);
    
    static inline CSRootImage* imageWithFormat(uint format, uint contentWidth, uint contentHeight) {
        return autorelease(createWithFormat(format, contentWidth, contentHeight));
    }
    static inline CSRootImage* imageWithRaw(const void* raw, uint format, uint contentWidth, uint contentHeight) {
        return autorelease(createWithRaw(raw, format, contentWidth, contentHeight));
    }
    static inline CSRootImage* imageWithImage(const void* source, uint length, uint format) {
        return autorelease(createWithImage(source, length, format));
    }
    static CSRootImage* imageWithBuffer(CSBuffer* buffer);
    
    static inline CSRootImage* imageWithContentOfFile(const char* path, uint format) {
        return autorelease(createWithContentOfFile(path, format));
    }
    static inline CSRootImage* imageWithContentOfURL(const CSURLRequest* request, uint format) {
        return autorelease(createWithContentOfURL(request, format));
    }
    
    static uint pot(uint size);
private:
    static uint genTexture(uint attr);

    static bool loadRawData(const byte* data, uint format, Texture& texture);
    static bool loadImageData(const byte* data, uint length, uint& format, Texture& texture);
    static bool loadASTCData(const byte* data, uint length, uint format, Texture& texture);
    static bool loadETC1Data(const byte* data, uint length, uint format, Texture& texture);
    static bool loadETC2Data(const byte* data, uint length, uint format, Texture& texture);
    static bool loadPVRTC1Data(const byte* data, uint length, uint format, Texture& texture);
	static bool loadDXTData(const byte* data, uint length, uint format, Texture& texture);
    static bool loadCompressedData(const byte* data, uint length, uint format, uint internalFormat, uint block, uint blockLength, bool alphaTexture, Texture &texture);
    
public:
    inline uint format() const {
        return _format;
    }
    inline uint texture0() const override {
        _identifier.use();
        return _texture[0];
    }
    inline uint texture1() const override {
        _identifier.use();
        return _texture[1];
    }
    inline const CSSize& textureSize() const override {
        return _textureSize;
    }
    inline const CSRect& frame() const override {
        return _frame;
    }
    inline const CSRootImage* root() const override {
        return this;
    }
    inline bool isValid() const {
        return _identifier.isValid();
    }
    inline uint memory() const {
        return _memory;
    }

	static inline const CSArray<CSRootImage*>* allImages() {
		return __allImages;
	}
};

#endif /* CSRootImage_h */
