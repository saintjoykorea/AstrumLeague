
#ifndef __CDK__CSHuaweiLogin__
#define __CDK__CSHuaweiLogin__

#include "CSDelegate.h"

enum CSHuaweiLoginResult {
	CSHuaweiLoginResultSuccess,
	CSHuaweiLoginResultCancelled,
	CSHuaweiLoginResultError
};

class CSHuaweiLoginDelegate {
public:
	virtual void onHuaweiLogin(int result) = 0;
};

class CSHuaweiLogin {
public:
    static void initialize();
    static void finalize();
    static void setDelegate(CSHuaweiLoginDelegate* delegate);

    static void login();
    static void logout();
    static bool isConnected();
    static const char* userId();
};

#endif
