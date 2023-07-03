#ifdef CDK_ANDROID

#ifndef __CDK__CSHuaweiIAP__
#define __CDK__CSHuaweiIAP__

#include "CSIAPDelegate.h"

#include "CSTypes.h"

class CSHuaweiIAP {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSIAPDelegate* delegate);
    static void connect();
    static void sync(const char* const* productIds, uint count);
    static void purchase(const char* productId, const char* payload);
};

#endif

#endif