//
//  CSDevice.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSDevice__
#define __CDK__CSDevice__

#include "CSTypes.h"
#include "CSString.h"

struct CSMemoryUsage {
    int64 totalMemory;
    int64 freeMemory;
    int64 threshold;
};

class CSDevice {
public:
    static const char* brand();
    static const char* model();
    static const char* systemVersion();
    static const char* locale();
    static const char* countryCode();
	static const char* uuid();
    static CSMemoryUsage memoryUsage();
    static int battery();
};

#endif /* defined(__CDK__CSDevice__) */
