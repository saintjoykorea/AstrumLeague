//
//  CSOpenGL.cpp
//  CDK
//
//  Created by chan on 13. 3. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSOpenGL.h"

#include "CSLog.h"

#include "CSThread.h"

#ifdef CS_OES_DEBUG
static int __glerr = GL_NO_ERROR;

void __LOG_GL_ERROR(const char* tag, const char* func, int line) {
    int err = glGetErrorCS();
    if (!CSThread::mainThread()->isActive()) {
        err = GL_INVALID_THREAD;
    }
    if (err != GL_NO_ERROR) {
        __glerr = err;
        CSErrorLog("CSOpenGL::error<%s> = 0x%04X, %s, %d", tag, err, func, line);
    }
}

int glGetErrorCS() {
    int err = __glerr;
    __glerr = GL_NO_ERROR;
    return err;
}
#endif

unsigned int __glDrawCall = 0;

bool glIsSupportedExtensionCS(const char* ext) {
    return (strstr((const char*)glGetString(GL_EXTENSIONS), ext) != NULL);
}
