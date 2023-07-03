//
//  CSCrypto.h
//  CDK
//
//  Created by 김찬 on 13. 7. 29..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSCrypto__
#define __CDK__CSCrypto__

#include "CSString.h"

class CSCrypto {
public:
    static const void* encrypt(const void* data, uint& length, const char* key);
    static const void* decrypt(const void* data, uint& length, const char* key);
    static CSString* sha1(const void* data, uint length);
};

#endif

#endif
