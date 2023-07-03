//
//  CSFile.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSFile__
#define __CDK__CSFile__

#include "CSString.h"

#include "CSArray.h"

class CSFile {
public:
#ifdef CDK_IMPL
#ifdef CDK_WINDOWS
	static int initialize();
	static void finalize();
#elif defined(CDK_ANDROID)
	static void initialize(void* assetManager, const char* storagePath);
	static void finalize();
#endif
    static void* createRawWithCompressedData(const void* src, uint srcLength, uint destOffset, uint& destLength);
    static void* createCompressedRawWithData(const void* src, uint srcLength, uint& destLength);

    static void* createRawWithContentOfFile(const char* path, uint offset, uint& length, bool compression);
    static bool writeRawToFile(const char* path, const void* data, uint length, bool compression);
#endif
    static const char* findPath(const char* subpath);
    static const char* storagePath(const char* subpath);
    static const char* bundlePath(const char* subpath);
    static bool fileExists(const char* path);
    static bool moveFile(const char* srcpath, const char* destpath);
    static bool deleteFile(const char* path);
    static bool makeDirectory(const char* path);
    static CSArray<CSString>* filePaths(const char* dirpath, bool full);
};

#endif /* defined(__CDK__CSFile__) */
