#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSLog.h"

#include <stdio.h>

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
	printf("%-28s Warn	", tag);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}

void __CSErrorLog(const char* tag, const char* format, ...) {
	printf("%-28s Error	", tag);
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
	printf("%-28s Warn	", tag);
	vprintf(format, args);
	printf("\n");
}

void __CSErrorLogv(const char* tag, const char* format, va_list args) {
	printf("%-28s Error	", tag);
	vprintf(format, args);
	printf("\n");
}

#endif

#endif