//
//  CSRootImage+Image.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 18..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"
#include "CSRaster.h"

bool CSRootImage::loadImageData(const byte *data, uint length, uint& format, Texture &texture) {
    texture.contentWidth = texture.contentHeight = 0;
    
    void* raw = CSRaster::createRasterFromImageSource(data, length, texture.contentWidth, texture.contentHeight);
    
    if (!raw) {
        return false;
    }
    
    void* cnvraw;
    uint rawFormat;
    
    switch (format & 0xff) {
        case CSImageFormatImageRGBA8888:
            rawFormat = CSImageFormatRawRGBA8888;
            break;
        case CSImageFormatImageRGBA4444:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight * 2);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((ushort*)cnvraw)[i]
                = (((p & 0xff) >> 4) << 12)
                | ((((p >> 8) & 0xff) >> 4) << 8)
                | ((((p >> 16) & 0xff) >> 4) << 4)
                | (((p >> 24) & 0xff) >> 4);
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawRGBA4444;
            break;
        case CSImageFormatImageRGBA5551:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight * 2);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((ushort*)cnvraw)[i]
                = (((p & 0xff) >> 3) << 11)
                | ((((p >> 8) & 0xff) >> 3) << 6)
                | ((((p >> 16) & 0xff) >> 3) << 1)
                | (((p >> 24) & 0xff) >> 7);
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawRGBA5551;
            break;
        case CSImageFormatImageRGB565:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight * 2);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((ushort*)cnvraw)[i]
                = (((p & 0xff) >> 3) << 11)
                | ((((p >> 8) & 0xff) >> 2) << 5)
                | (((p >> 16) & 0xff) >> 3);
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawRGB565;
            break;
        case CSImageFormatImageL8:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((byte*)cnvraw)[i] = ((p & 0xff) + ((p >> 8) & 0xff) + ((p >> 16) & 0xff)) / 3;
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawL8;
            break;
        case CSImageFormatImageL8A8:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight * 2);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((ushort*)cnvraw)[i] = (((p & 0xff) + ((p >> 8) & 0xff) + ((p >> 16) & 0xff)) / 3) << 8 | ((p >> 24) & 0xff);
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawL8A8;
            break;
        case CSImageFormatImageA8:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((byte*)cnvraw)[i] = (p >> 24) & 0xff;
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawA8;
            break;
        case CSImageFormatImageR8:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((byte*)cnvraw)[i] = p & 0xff;
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawR8;
            break;
        case CSImageFormatImageR8G8:
            cnvraw = malloc(texture.contentWidth * texture.contentHeight * 2);
            if (!cnvraw) {
                free(raw);
                return false;
            }
            length = texture.contentWidth * texture.contentHeight;
            for (uint i = 0; i < length; i++) {
                uint p = ((uint*)raw)[i];
                
                ((ushort*)cnvraw)[i] = (p & 0xff) << 8 | ((p >> 8) & 0xff);
            }
            free(raw);
            raw = cnvraw;
            rawFormat = CSImageFormatRawR8G8;
            break;
        default:
            CSErrorLog("image unknown format:%d", format);
            return false;
    }
    
    format &= 0xffffff00;
    format |= rawFormat;
    
    bool rtn = loadRawData((const byte*)raw, format, texture);
    
    free(raw);
    
    return rtn;
}
