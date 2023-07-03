//
//  CSLog.mm
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSLog.h"

#import <stdio.h>

#ifdef CS_CONSOLE_DEBUG

void __CSLog(const char* tag, const char* format, ...) {
	printf("%-28s ", tag);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}

void __CSWarnLog(const char* tag, const char* format, ...) {
    printf("%-28s 🔶 ", tag);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void __CSErrorLog(const char* tag, const char* format, ...) {
	printf("%-28s 🔴 ", tag);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}

void __CSLogv(const char* tag, const char* format, va_list args) {
    printf("%-28s ", tag);
    vprintf(format, args);
    printf("\n");
}

void __CSWarnLogv(const char* tag, const char* format, va_list args) {
    printf("%-28s 🔶 ", tag);
    vprintf(format, args);
    printf("\n");
}

void __CSErrorLogv(const char* tag, const char* format, va_list args) {
    printf("%-28s 🔴 ", tag);
    vprintf(format, args);
    printf("\n");
}

#endif

#endif
