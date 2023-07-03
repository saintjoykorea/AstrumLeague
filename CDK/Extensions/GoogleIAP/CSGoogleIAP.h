//
//  CSGoogleIAP.h
//  CDK
//
//  Created by WooyolJung on 2017. 12. 07.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#ifndef __CDK__CSGoogleIAP__
#define __CDK__CSGoogleIAP__

#include "CSIAPDelegate.h"

#include "CSTypes.h"

class CSGoogleIAP {
public:
    static void initialize();
    static void finalize();
	
    static void setDelegate(CSIAPDelegate* delegate);
    static void connect();
    static void sync(const char* const* productIds, uint count);
    static void purchase(const char* productId, const char* payload);
    static void setUserId(const char* userId);
};

#endif

#endif
