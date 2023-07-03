//
//  CSFile.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSFile.h"

#import "CSFileRawImpl.h"

#import <Foundation/Foundation.h>

void* CSFile::createRawWithContentOfFile(const char* path, uint offset, uint& length, bool compression) {
    if (compression) {
        return ::createRawWithCompressedContentOfFile(path, offset, length);
    }
    else {
        return ::createRawWithContentOfFile(path, offset, length);
    }
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
	NSString* nsdirpath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	return [[nsdirpath stringByAppendingFormat:@"/%s", subpath] UTF8String];
}

const char* CSFile::bundlePath(const char* subpath) {
	NSString* nsdirpath = [[NSBundle mainBundle] bundlePath];

	return [[nsdirpath stringByAppendingFormat:@"/%s", subpath] UTF8String];
}

bool CSFile::fileExists(const char* path) {
	NSString* nspath = [NSString stringWithUTF8String:path];

	return [[NSFileManager defaultManager] fileExistsAtPath:nspath];
}

bool CSFile::moveFile(const char* srcpath, const char* destpath) {
	NSString* nssrcpath = [NSString stringWithUTF8String:srcpath];
	NSString* nsdestpath = [NSString stringWithUTF8String:destpath];

	return [[NSFileManager defaultManager] moveItemAtPath:nssrcpath
			toPath:nsdestpath
			error:NULL];
}

bool CSFile::deleteFile(const char* path) {
	NSString* nspath = [NSString stringWithUTF8String:path];

	return [[NSFileManager defaultManager] removeItemAtPath:nspath error:NULL];
}

bool CSFile::makeDirectory(const char* path) {
	NSString* nspath = [NSString stringWithUTF8String:path];

	return [[NSFileManager defaultManager] createDirectoryAtPath:nspath withIntermediateDirectories:NO attributes:nil error:NULL];
}

CSArray<CSString>* CSFile::filePaths(const char* dirpath, bool full) {
	NSString* nsdirpath = [NSString stringWithUTF8String:dirpath];

	NSArray* subpaths = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:nsdirpath error:NULL];

	CSArray<CSString>* paths = CSArray<CSString>::arrayWithCapacity(subpaths.count);

	for (NSString* subpath in subpaths) {
		if ([subpath caseInsensitiveCompare:@".ds_store"] != 0) {
            CSString* path = CSString::string([(full ? [nsdirpath stringByAppendingPathComponent:subpath] : subpath) UTF8String]);

			paths->addObject(path);
		}
	}
	return paths;
}

#endif
