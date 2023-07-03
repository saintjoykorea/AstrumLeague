//
//  CSCrypto.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 29..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSCrypto.h"

#import "CSLog.h"

#import "CSMemory.h"

#import "NSData+AES.h"

#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>

const void* CSCrypto::encrypt(const void* data, uint& length, const char* key) {
    CSAssert(strlen(key) == 16 || strlen(key) == 32, "AES key length must be 16 or 32 bytes");
    
    NSData* nsencryptData = [[NSData dataWithBytesNoCopy:(void*)data length:length freeWhenDone:NO] encryptWithKey:key];

    length = nsencryptData.length;
    
    return nsencryptData.bytes;
}

const void* CSCrypto::decrypt(const void* data, uint& length, const char* key) {
    CSAssert(strlen(key) == 16 || strlen(key) == 32, "AES key length must be 16 or 32 bytes");
    
    NSData* nsdecryptData = [[NSData dataWithBytesNoCopy:(void*)data length:length freeWhenDone:NO] decryptWithKey:key];
    
    length = nsdecryptData.length;
    
    return nsdecryptData.bytes;
}

CSString* CSCrypto::sha1(const void* data, uint length) {
    byte hashBytes[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1(data, length, hashBytes);
    CSString* hash = CSString::string();
    for (int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++) {
        hash->appendFormat("%02x", hashBytes[i]);
    }
    return hash;
}

#endif
