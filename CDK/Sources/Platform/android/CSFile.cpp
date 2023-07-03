//
//  CSFile.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSFile.h"

#include "CSFileRawImpl.h"
#include "CSMath.h"

#include "CSJNI.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

static jobject _jassetManager;
static AAssetManager* _assetManager;
static char* _storagePath;

static const char* BundlePrefix = "assets/";
static const int BundlePrefixLength = 7;

void CSFile::initialize(void* jassetManager, const char* storagePath) {
	JNIEnv* env = CSJNI::getEnv();
	_jassetManager = env->NewGlobalRef((jobject)jassetManager);
	_assetManager = AAssetManager_fromJava(env, _jassetManager);

	_storagePath = strdup(storagePath);
}

void CSFile::finalize() {
	free(_storagePath);

	JNIEnv* env = CSJNI::getEnv();
	env->DeleteGlobalRef(_jassetManager);
}

static bool isBundle(const char* path) {
	return strstr(path, BundlePrefix) == path;
}

void* CSFile::createRawWithContentOfFile(const char* path, uint offset, uint& length, bool compression) {
	void* data = NULL;

	if (isBundle(path)) {
		AAsset* asset = AAssetManager_open(_assetManager, path + BundlePrefixLength, AASSET_MODE_BUFFER);

		if (asset) {
			uint originLength = AAsset_getLength(asset);

			if (compression) {
				void* origin = fmalloc(originLength);

				if (AAsset_read(asset, origin, originLength) == originLength) {
					data = createRawWithCompressedData(origin, originLength, offset, length);
				}
				else length = 0;

				free(origin);
			}
			else if (offset < originLength) {
				if (offset) AAsset_seek(asset, offset, SEEK_SET);
				uint readLength = originLength - offset;
				if (length && readLength > length) readLength = length;
				data = fmalloc(readLength);
				if (AAsset_read(asset, data, readLength) == readLength) {
					length = readLength;
				}
				else {
					free(data);
					data = NULL;
					length = 0;
				}
			}
			else length = 0;
			
			AAsset_close(asset);
		}
		else length = 0;
	}
	else {
        if (compression) {
            data = ::createRawWithCompressedContentOfFile(path, offset, length);
        }
        else {
            data = ::createRawWithContentOfFile(path, offset, length);
        }
	}
	return data;
}

bool CSFile::writeRawToFile(const char* path, const void* data, uint length, bool compression) {
    if (compression) {
        return ::writeCompressedRawToFile(path, data, length);
    }
    else {
        return ::writeRawToFile(path, data, length);
    }
}

const char* CSFile::findPath(const char* subpath) {
	const char* path = storagePath(subpath);
	if (fileExists(path)) {
		return path;
	}
	path = bundlePath(subpath);
	if (fileExists(path)) {
		return path;
	}
	return NULL;
}

const char* CSFile::storagePath(const char* subpath) {
	return CSString::cstringWithFormat("%s/%s", _storagePath, subpath);
}

const char* CSFile::bundlePath(const char* subpath) {
	return CSString::cstringWithFormat("%s%s", BundlePrefix, subpath);
}

bool CSFile::fileExists(const char* path) {
	if (isBundle(path)) {
		AAsset* asset = AAssetManager_open(_assetManager, path + BundlePrefixLength, AASSET_MODE_UNKNOWN);
		
		if (!asset) return false;
		
		AAsset_close(asset);
		return true;
	}
	else {
		return access(path, F_OK) != -1;
	}
}

bool CSFile::moveFile(const char* srcpath, const char* destpath) {
	if (isBundle(srcpath) || isBundle(destpath)) {
		return false;
	}
	FILE* rfp;
	FILE* wfp;

	if (!(rfp = fopen(srcpath, "rb"))) {
		return false;
	}
	if (!(wfp = fopen(destpath, "wb"))) {
		fclose(rfp);
		return false;
	}

	void* data = alloca(512);
	int length;
	while ((length = fread(data, sizeof(byte), 512, rfp)) > 0) {
		fwrite(data, 1, length, wfp);
	}
    fflush(wfp);
	fclose(wfp);
	fclose(rfp);
	return true;
}

bool CSFile::deleteFile(const char* path) {
	if (isBundle(path)) return false;

	return ::remove(path) == 0;
}

bool CSFile::makeDirectory(const char* path) {
	if (isBundle(path)) return false;
	
	return ::mkdir(path, 0770) == 0;
}

CSArray<CSString>* CSFile::filePaths(const char* dirpath, bool full) {
	CSArray<CSString>* paths = CSArray<CSString>::array();

	if (isBundle(dirpath)) {
		AAssetDir* assetDir = AAssetManager_openDir(_assetManager, dirpath + BundlePrefixLength);
		
		if (assetDir) {
			const char* filename;
			while((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
				CSString* path = full ? CSString::createWithFormat("%s/%s", dirpath, filename) : new CSString(filename);
				paths->addObject(path);
				path->release();
			}
			
			AAssetDir_close(assetDir);
		}
	}
	else {
		DIR* dir;
		struct dirent* direntry;

		if ((dir = opendir(dirpath)) != NULL) {
			while ((direntry = readdir(dir)) != NULL) {
				if (direntry->d_name[0] != '.') {
                    CSString* path = full ? CSString::createWithFormat("%s/%s", dirpath, direntry->d_name) : new CSString(direntry->d_name);
					paths->addObject(path);
					path->release();
				}
			}
		}
	}
	return paths;
}

#endif
