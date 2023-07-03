//
//  CSAppSafer.cpp
//  CDK
//
//  Created by Kim Chan on 2021. 9. 28.
//  Copyright © 2021년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSAppSafer.h"

#include "CSJNI.h"

void CSAppSafer::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAppSafer, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSAppSafer::finalize() {
    //nothing to do
}

void CSAppSafer::setUserId(const char* userId) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAppSafer, "setUserId", "(Ljava/lang/String;)V", false);

	jstring juserId = mi.env->NewStringUTF(userId);

    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserId);
	
    mi.env->DeleteLocalRef(juserId);
}

#endif
