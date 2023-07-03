//
//  CSDevice.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSDevice.h"

#include "CSJNI.h"

#include "CSMemory.h"

const char* CSDevice::brand() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "brand", "()Ljava/lang/String;", false);
    
    jstring jbrand = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);
    
    const char* brand = mi.env->GetStringUTFChars(jbrand, NULL);
    
    char* rtn = (char*)strdup(brand);
    
    mi.env->ReleaseStringUTFChars(jbrand, brand);
    
    mi.env->DeleteLocalRef(jbrand);
    
    autofree(rtn);
    return rtn;
}

const char* CSDevice::model() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "model", "()Ljava/lang/String;", false);

	jstring jmodel = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);
	
    const char* model = mi.env->GetStringUTFChars(jmodel, NULL);
    
    char* rtn = (char*)strdup(model);

    mi.env->ReleaseStringUTFChars(jmodel, model);

	mi.env->DeleteLocalRef(jmodel);

	autofree(rtn);
	return rtn;
}

const char* CSDevice::systemVersion() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "systemVersion", "()Ljava/lang/String;", false);

	jstring jsystemVersion = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);
	
    const char* systemVersion = mi.env->GetStringUTFChars(jsystemVersion, NULL);
    char* rtn = (char*)strdup(systemVersion);

    mi.env->ReleaseStringUTFChars(jsystemVersion, systemVersion);

	mi.env->DeleteLocalRef(jsystemVersion);

	autofree(rtn);
	return rtn;
}

const char* CSDevice::locale() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "locale", "()Ljava/lang/String;", false);

	jstring jlocale = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    const char* locale = mi.env->GetStringUTFChars(jlocale, NULL);
    char* rtn = (char*)strdup(locale);

    mi.env->ReleaseStringUTFChars(jlocale, locale);
    mi.env->DeleteLocalRef(jlocale);
    autofree(rtn);
	return rtn;
}

const char* CSDevice::uuid() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "uuid", "()Ljava/lang/String;", false);

    jstring juuid = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    const char* uuid = mi.env->GetStringUTFChars(juuid, NULL);
    char* rtn = (char*)strdup(uuid);

    mi.env->ReleaseStringUTFChars(juuid, uuid);
    mi.env->DeleteLocalRef(juuid);
    autofree(rtn);

    return rtn;
}

const char* CSDevice::countryCode() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "countryCode", "()Ljava/lang/String;", false);

    jstring jcountryCode = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    const char*  countryCode = mi.env->GetStringUTFChars(jcountryCode, NULL);
    char* rtn = (char*)strdup(countryCode);

    mi.env->ReleaseStringUTFChars(jcountryCode, countryCode);
    mi.env->DeleteLocalRef(jcountryCode);
    autofree(rtn);
    return rtn;
}

CSMemoryUsage CSDevice::memoryUsage() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "memoryUsage", "([J)V", false);
    
    jlongArray jmemoryUsage = mi.env->NewLongArray(3);
    
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jmemoryUsage);
    
    jlong out[3];
    mi.env->GetLongArrayRegion(jmemoryUsage, 0, 3, out);
    mi.env->DeleteLocalRef(jmemoryUsage);

    CSMemoryUsage usage;
    usage.totalMemory = out[0];
    usage.freeMemory = out[1];
    usage.threshold = out[2];
    
    return usage;
}

int CSDevice::battery() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassDevice, "battery", "()I", false);

    jint battery = mi.env->CallStaticIntMethod(mi.classId, mi.methodId);

    return battery;
}

#endif
