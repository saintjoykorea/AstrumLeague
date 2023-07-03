#ifdef CDK_WINDOWS

#ifdef CDK_IMPL

#ifndef __CDK__CSDeviceConfig__
#define __CDK__CSDeviceConfig__

void deviceConfigInitialize();
int deviceConfigWidth();
int deviceConfigHeight();
const char* deviceConfigLocale();
const char* deviceConfigCountry();

#endif

#endif

#endif
