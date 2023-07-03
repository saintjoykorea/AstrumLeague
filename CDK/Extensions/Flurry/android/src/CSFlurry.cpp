//
//  CSFlurry.cpp
//  CDK
//
//  Created by ChnagSun on 2020. 8. 19.
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSFlurry.h"

#include "CSJNI.h"

void CSFlurry::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFlurry, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSFlurry::finalize() {

}

void CSFlurry::log(const char* name, int paramCount, ...) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFlurry, "log", "(Ljava/lang/String;[Ljava/lang/String;)V", false);

    jstring jname = mi.env->NewStringUTF(name);
    jobjectArray jparams = mi.env->NewObjectArray(paramCount * 2, mi.env->FindClass("java/lang/String"), NULL);

	va_list ap;
    va_start(ap, paramCount);
    for (int i = 0; i < paramCount * 2; i++) {
		const char* param = va_arg(ap, const char*);
		mi.env->SetObjectArrayElement(jparams, i, mi.env->NewStringUTF(param));
	}
    va_end(ap);

    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jname, jparams);
    mi.env->DeleteLocalRef(jname);
    mi.env->DeleteLocalRef(jparams);
}

#endif
