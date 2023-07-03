#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSDevice.h"

#include "CSDeviceConfig.h"

#include <windows.h>

#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")


const char* CSDevice::brand() {
	return "Microsoft";
}

const char* CSDevice::model() {
	return "Windows";
}

const char* CSDevice::locale() {
	return deviceConfigLocale();
}

const char* CSDevice::countryCode() {
	return deviceConfigCountry();
}

const char* CSDevice::systemVersion() {
	return "";
}

const char* CSDevice::uuid() {
	UUID uid;
	UuidCreate(&uid);	
	wchar_t wszIID[64];
	char* suuid = (char*)fcalloc(64, 1);

	int r = ::StringFromGUID2(uid, wszIID, 64);
	WideCharToMultiByte(CP_ACP, 0, wszIID, -1, suuid, 64, 0, 0);
	
	autofree(suuid);

	return suuid;
}

CSMemoryUsage CSDevice::memoryUsage() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	CSMemoryUsage usage;
	usage.totalMemory = memInfo.ullTotalPhys;
	usage.freeMemory = memInfo.ullAvailPhys;
	usage.threshold = memInfo.ullTotalPhys * 2 / 10;		//20% 메모리를 남김
	return usage;
}

int CSDevice::battery() {
	return 100;
}

#endif
