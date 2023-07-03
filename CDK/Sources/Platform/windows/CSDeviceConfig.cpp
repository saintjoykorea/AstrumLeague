#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSDeviceConfig.h"

#include <stdio.h>

static int __width = 0;
static int __height = 0;
static char __locale[8] = {};
static char __country[8] = {};

void deviceConfigInitialize() {
	FILE* fp = fopen("device.cfg", "r");
	char str[256];
	while (fgets(str, sizeof(str), fp)) {
		if (sscanf(str, "size:%d,%d\n", &__width, &__height) == 2) continue;
		if (sscanf(str, "locale:%s\n", &__locale) == 1) continue;
		if (sscanf(str, "country:%s\n", &__country) == 1) continue;
	}
	fclose(fp);
}

int deviceConfigWidth() {
	return __width;
}

int deviceConfigHeight() {
	return __height;
}

const char* deviceConfigLocale() {
	return __locale;
}

const char* deviceConfigCountry() {
	return __country;
}

#endif