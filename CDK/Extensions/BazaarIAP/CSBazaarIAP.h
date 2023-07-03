
#ifdef CDK_ANDROID

#ifndef __CDK__CSBazaarIAP__
#define __CDK__CSBazaarIAP__

#include "CSIAPDelegate.h"

#include "CSTypes.h"

class CSBazaarIAP {
public:
    static void initialize();
    static void finalize();
    static void setDelegate(CSIAPDelegate* delegate);
    static void connect();
    static void sync(const char* const* productIds, uint count);
    static void purchase(const char* productId, const char* payload);
    static void showReview();
};

#endif

#endif
