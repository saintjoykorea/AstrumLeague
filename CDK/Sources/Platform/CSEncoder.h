//
//  CSEncoder.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSEncoder__
#define __CDK__CSEncoder__

#include "CSString.h"

class CSEncoder {
public:
    static CSString* createStringWithBytes(const void* bytes, uint length, CSStringEncoding encoding);
    static void* createRawWithString(const char* str, uint& length, CSStringEncoding encoding);
};

#endif /* defined(__CDK__CSEncoder__) */

#endif
