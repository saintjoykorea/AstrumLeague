//
//  CSAppleIAP.h
//  CDK
//
//  Created by 김수현 on 2017. 11. 23..
//  Copyright © 2017년 brgames. All rights reserved.
//
#ifdef CDK_IOS

#ifndef __CDK__CSAppleIAP_h
#define __CDK__CSAppleIAP_h

#include "CSIAPDelegate.h"

#include "CSTypes.h"

class CSAppleIAP {
public:
    static void initialize();
    static void finalize();
    
	static void setDelegate(CSIAPDelegate* delegate);
    static void connect();
    static void sync(const char* const* productIds, uint count);
    static void purchase(const char* productId, const char* payload);
};

#endif /* CSAppleIAP */

#endif
