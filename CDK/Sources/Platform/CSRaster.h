//
//  CSRaster.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSRaster__
#define __CDK__CSRaster__

#include "CSSize.h"

#include "CSFont.h"

class CSRaster {
public:
	static void loadFontHandle(const char* name, CSFontStyle style, const char* path);
    static void* createSystemFontHandle(float size, CSFontStyle style);
    static void* createFontHandle(const char* name, float size, CSFontStyle style);
    static void destroyFontHandle(void* handle);
    static CSSize characterSize(const uchar* cc, uint cclen, const CSFont* font);
    static void* createRasterFromCharacter(const uchar* cc, uint cclen, const CSFont* font, uint& width, uint& height, float& offy);
    
    static void* createRasterFromImageSource(const void* source, uint len, uint& width, uint& height);
    
    static bool saveImageFromRaster(const char* path, const void* raster, uint width, uint height);
};

#endif /* defined(__CDK__CSRaster__) */

#endif
