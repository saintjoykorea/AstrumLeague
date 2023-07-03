//
//  CSLog+common.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 4. 10..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLog.h"

#include <string.h>

const char* __FILENAME(const char* path) {
    const char* name0 = strrchr(path, '/');
    if (name0) return name0 + 1;
    const char* name1 = strrchr(path, '\\');
    if (name1) return name1 + 1;
    return path;
}
