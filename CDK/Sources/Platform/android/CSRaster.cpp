//
//  CSRaster.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSRaster.h"

#include "CSJNI.h"

#include "CSMemory.h"
#include "CSString.h"

#include <android/bitmap.h>

void CSRaster::loadFontHandle(const char* name, CSFontStyle style, const char* path) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFont, "load", "(Ljava/lang/String;ILjava/lang/String;)V", false);
    jstring jname = mi.env->NewStringUTF(name);
    jstring jpath = mi.env->NewStringUTF(path);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jname, style, jpath);
    mi.env->DeleteLocalRef(jname);
    mi.env->DeleteLocalRef(jpath);
}

void* CSRaster::createSystemFontHandle(float size, CSFontStyle style) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFont, "<init>", "(FI)V", true);
    
	jobject h = mi.env->NewObject(mi.classId, mi.methodId, size, style);
    
    void* handle = (void*)mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);
    
	return handle;
}

void* CSRaster::createFontHandle(const char* name, float size, CSFontStyle style) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFont, "<init>", "(Ljava/lang/String;FI)V", true);

    jstring jname = mi.env->NewStringUTF(name);

    jobject h = mi.env->NewObject(mi.classId, mi.methodId, jname, size, style);
    
    mi.env->DeleteLocalRef(jname);
    
    void* handle = (void*)mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);
    
    return handle;
}

void CSRaster::destroyFontHandle(void* handle) {
    CSJNI::getEnv()->DeleteGlobalRef((jobject)handle);
}

CSSize CSRaster::characterSize(const uchar* cc, uint cclen, const CSFont* font) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassRaster, "characterSize", "(Lkr/co/brgames/cdk/CSFont;Ljava/lang/String;[F)V", false);

    jstring jcc = mi.env->NewString(cc, cclen);

	jfloatArray joutsize = mi.env->NewFloatArray(2);

	mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, (jobject)font->handle(), jcc, joutsize);

    jfloat outsize[2];
    mi.env->GetFloatArrayRegion(joutsize, 0, 2, outsize);

	CSSize value(outsize[0], outsize[1]);

    mi.env->DeleteLocalRef(jcc);
	mi.env->DeleteLocalRef(joutsize);

	return value;
}

void* CSRaster::createRasterFromCharacter(const uchar* cc, uint cclen, const CSFont* font, uint& width, uint& height, float& offy) {
	void* raster = NULL;
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassRaster, "createRasterFromCharacter", "(Lkr/co/brgames/cdk/CSFont;Ljava/lang/String;[F)[B", false);

    jstring jcc = mi.env->NewString(cc, cclen);

    jfloatArray joutsize = mi.env->NewFloatArray(3);

	jbyteArray joutdata = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, (jobject)font->handle(), jcc, joutsize);

    if (joutdata) {
        jfloat outsize[3];
        mi.env->GetFloatArrayRegion(joutsize, 0, 3, outsize);
        width = outsize[0];
        height = outsize[1];
        offy = outsize[2];

		int len = mi.env->GetArrayLength(joutdata);
		if (len && (raster = malloc(len))) {
            mi.env->GetByteArrayRegion(joutdata, 0, len, (jbyte*)raster);
		}
		mi.env->DeleteLocalRef(joutdata);
	}

	mi.env->DeleteLocalRef(jcc);
	mi.env->DeleteLocalRef(joutsize);

	return raster;
}

void* CSRaster::createRasterFromImageSource(const void* source, uint len, uint& width, uint& height) {
	void* raster = NULL;
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassRaster, "createBitmapFromImageSource", "(Ljava/nio/ByteBuffer;)Landroid/graphics/Bitmap;", false);

    jobject jbytebuffer=mi.env->NewDirectByteBuffer((void*)source, len);
	jobject jbitmap = mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jbytebuffer);
    mi.env->DeleteLocalRef(jbytebuffer);
    
	if (jbitmap) {
		AndroidBitmapInfo info;
		int ret = AndroidBitmap_getInfo(mi.env, jbitmap, &info);

		CSAssert(ret >= 0, "AndroidBitmap getInfo failed ! error=%d", ret);

		void* pixels;

		raster = malloc(info.width * info.height * 4);
        
		if (raster) {
            ret = AndroidBitmap_lockPixels(mi.env, jbitmap, &pixels);
            
            CSAssert(ret >= 0, "AndroidBitmap lockPixels failed ! error=%d", ret);
            
            unsigned char* texturePixelPointer = (unsigned char*)raster;
            
            for (int y = 0; y < info.height; y++) {
                for (int x = 0; x < info.width; x++) {
                    switch (info.format) {
                        case ANDROID_BITMAP_FORMAT_RGBA_8888:
                            {
                                unsigned int p = ((unsigned int*)pixels)[x];
                                
                                *(unsigned int*)texturePixelPointer = ((unsigned int*)pixels)[x];
                            }
                            break;
                        case ANDROID_BITMAP_FORMAT_RGBA_4444:
                            {
                                unsigned short p = ((unsigned short*)pixels)[x];
                                unsigned int r = (p >> 12 & 0x0f) << 4;
                                unsigned int g = (p >> 8 & 0x0f) << 4;
                                unsigned int b = (p >> 4 & 0x0f) << 4;
                                unsigned int a = (p & 0x0f) << 4;
                                *(unsigned int*)texturePixelPointer = (a << 24) | (b << 16) | (g << 8) | (r);
                            }
                            break;
                        case ANDROID_BITMAP_FORMAT_RGB_565:
                            {
                                unsigned short p = ((unsigned short*)pixels)[x];
                                
                                unsigned int r = (p >> 11 & 0x1f) << 3;
                                unsigned int g = (p >> 5 & 0x3f) << 2;
                                unsigned int b = (p & 0x1f) << 3;
                                *(unsigned int*)texturePixelPointer = (255 << 24) | (b << 16) | (g << 8) | (r);
                            }
                            break;
                        case ANDROID_BITMAP_FORMAT_A_8:
                            {
                                unsigned char p = ((unsigned char*)pixels)[x];
                                
                                *(unsigned int*)texturePixelPointer = (p << 24) | (p << 16) | (p << 8) | (p);
                            }
                            break;
                    }
                    texturePixelPointer += 4;
                }
                pixels = ((unsigned char*)pixels) + info.stride;
            }
            width = info.width;
            height = info.height;
            
            ret = AndroidBitmap_unlockPixels(mi.env, jbitmap);
            
            CSAssert(ret >= 0, "AndroidBitmap unlockPixels failed ! error=%d", ret);
        }

        CSJNIMethod mi2;
        CSJNI::findMethod(mi2, CSJNIClassRaster, "recycleBitmap", "(Landroid/graphics/Bitmap;)V", false);
        mi2.env->CallStaticVoidMethod(mi2.classId, mi2.methodId, jbitmap);

		mi.env->DeleteLocalRef(jbitmap);
	}

	return raster;
}

bool CSRaster::saveImageFromRaster(const char* path, const void* raster, uint width, uint height) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassRaster, "saveImageFromRaster", "(Ljava/lang/String;[III)Z", false);
    
    jstring jpath = mi.env->NewStringUTF(path);
    jintArray jraster = mi.env->NewIntArray(width * height);
    mi.env->SetIntArrayRegion(jraster, 0, width * height, (const jint*)raster);
    
    jboolean rtn = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId, jpath, jraster, width, height);
    
    mi.env->DeleteLocalRef(jpath);
    mi.env->DeleteLocalRef(jraster);

    return rtn;
}

#endif
