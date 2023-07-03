//
//  CSLog.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSLog.h"

#include <stdio.h>
#include <alloca.h>
#include <android/log.h>

#ifdef CS_CONSOLE_DEBUG
void __CSLog(const char* tag, const char* format, ...) {
	va_list args;
	va_start(args, format);
    __CSLogv(tag, format, args);
	va_end(args);
}

void __CSWarnLog(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    __CSWarnLogv(tag, format, args);
    va_end(args);
}

void __CSErrorLog(const char* tag, const char* format, ...) {
	va_list args;
	va_start(args, format);
    __CSErrorLogv(tag, format, args);
	va_end(args);
}

void __CSLogv(const char* tag, const char* format, va_list args) {
    __android_log_vprint(ANDROID_LOG_INFO, tag, format, args);
}

void __CSWarnLogv(const char* tag, const char* format, va_list args) {
    __android_log_vprint(ANDROID_LOG_WARN, tag, format, args);
}

void __CSErrorLogv(const char* tag, const char* format, va_list args) {
    __android_log_vprint(ANDROID_LOG_ERROR, tag, format, args);
}

#endif

#endif
