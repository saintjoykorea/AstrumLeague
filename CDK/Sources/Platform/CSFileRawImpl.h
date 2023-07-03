//
//  CSFileRawImpl.h
//  CDK
//
//  Created by 김찬 on 2015. 5. 14..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSFileRawImpl__
#define __CDK__CSFileRawImpl__

#ifdef CDK_IMPL

#include "CSTypes.h"

void* createRawWithCompressedContentOfFile(const char* path, uint offset, uint& length);
void* createRawWithContentOfFile(const char* path, uint offset, uint& length);
bool writeRawToFile(const char* path, const void* data, uint length);
bool writeCompressedRawToFile(const char* path, const void* data, uint length);

#endif

#endif /* defined(__CDK__CSFileRawImpl__) */
