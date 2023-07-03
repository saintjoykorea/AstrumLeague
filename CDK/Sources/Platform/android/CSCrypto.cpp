//
//  CSCrypto.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 29..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSCrypto.h"

#include "CSMemory.h"

#include "CSLog.h"

#include "CSJNI.h"

const void* CSCrypto::encrypt(const void* data, uint& length, const char* key) {
    uint keyLength = strlen(key);
    
    CSAssert(keyLength == 16 || keyLength == 32, "AES key length must be 16 or 32 bytes");
    
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassCrypto, "encrypt", "([B[B)[B", false);

    jbyteArray jkey = mi.env->NewByteArray(keyLength);
    mi.env->SetByteArrayRegion(jkey, 0, keyLength, (const jbyte*)key);
    
    jbyteArray jindata = mi.env->NewByteArray(length);
	mi.env->SetByteArrayRegion(jindata, 0, length, (const jbyte*)data);
    
    jbyteArray joutdata = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jindata, jkey);
    
    mi.env->DeleteLocalRef(jindata);

    void* rtndata = NULL;
    
    if (joutdata) {
        length = mi.env->GetArrayLength(joutdata);
        if ((rtndata = malloc(length ? length : 1))) {
            if (length) mi.env->GetByteArrayRegion(joutdata, 0, length, (jbyte*)rtndata);
            autofree(rtndata);
        }
        mi.env->DeleteLocalRef(joutdata);
    }
    else {
        length = 0;
    }
    
    mi.env->DeleteLocalRef(jkey);
    
    return rtndata;
}

const void* CSCrypto::decrypt(const void* data, uint& length, const char* key) {
    uint keyLength = strlen(key);
    
    CSAssert(keyLength == 16 || keyLength == 32, "AES key length must be 16 or 32 bytes");
    
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassCrypto, "decrypt", "([B[B)[B", false);
    
    jbyteArray jkey = mi.env->NewByteArray(keyLength);
    mi.env->SetByteArrayRegion(jkey, 0, keyLength, (const jbyte*)key);
    
    jbyteArray jindata = mi.env->NewByteArray(length);
	mi.env->SetByteArrayRegion(jindata, 0, length, (const jbyte*)data);
    
    jbyteArray joutdata = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jindata, jkey);
    
    mi.env->DeleteLocalRef(jindata);
    
    void* rtndata = NULL;
    
    if (joutdata) {
        length = mi.env->GetArrayLength(joutdata);
        if ((rtndata = malloc(length ? length : 1))) {
            if (length) mi.env->GetByteArrayRegion(joutdata, 0, length, (jbyte*)rtndata);
            autofree(rtndata);
        }
        mi.env->DeleteLocalRef(joutdata);
    }
    else {
        length = 0;
    }
    
    mi.env->DeleteLocalRef(jkey);
    
    return rtndata;
}

CSString* CSCrypto::sha1(const void* data, uint length) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassCrypto, "sha1", "([B)[B", false);

    jbyteArray jindata = mi.env->NewByteArray(length);
    mi.env->SetByteArrayRegion(jindata, 0, length, (const jbyte*)data);
    
    jbyteArray joutbytes = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jindata);

    mi.env->DeleteLocalRef(jindata);
    
    CSString* rtn = NULL;

    if (joutbytes) {
        int length = mi.env->GetArrayLength(joutbytes);
        byte bytes[length];
        mi.env->GetByteArrayRegion(joutbytes, 0, length, (jbyte*)bytes);

        rtn = CSString::string();
        for (int i = 0; i < length; i++) {
            rtn->appendFormat("%02x", bytes[i]);
        }
        mi.env->DeleteLocalRef(joutbytes);
    }

    return rtn;
}

#endif
