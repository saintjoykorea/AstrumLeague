//
//  CSFirebase.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 05.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSFirebase.h"

#include "CSJNI.h"

#include "CSArray.h"

static CSFirebaseDelegate* __delegate = NULL;
static char* __token = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSFirebase_nativeTokenRefresh(JNIEnv* env, jclass clazz, jstring jtoken) {
        const char* token = env->GetStringUTFChars(jtoken, NULL);
		if (__token) free(__token);
		__token = strdup(token);
		env->ReleaseStringUTFChars(jtoken, token);

		if (__delegate) __delegate->onFirebaseTokenRefresh(__token);
    }
}

void CSFirebase::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFirebase, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSFirebase::finalize() {
	if (__token) {
		free(__token);
		__token = NULL;
	}
}

const char* CSFirebase::token() {
	return __token;
}

void CSFirebase::setDelegate(CSFirebaseDelegate* delegate) {
	__delegate = delegate;
}

void CSFirebase::addTopic(const char* topic) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFirebase, "addTopic", "(Ljava/lang/String;)V", false);
    jstring jtopic = mi.env->NewStringUTF(topic);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jtopic);
    mi.env->DeleteLocalRef(jtopic);
}

void CSFirebase::removeTopic(const char* topic) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFirebase, "removeTopic", "(Ljava/lang/String;)V", false);
    jstring jtopic = mi.env->NewStringUTF(topic);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jtopic);
    mi.env->DeleteLocalRef(jtopic);
}

void CSFirebase::log(const char *name, int paramCount, ...) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFirebase, "log", "(Ljava/lang/String;[Ljava/lang/String;)V", false);

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

void CSFirebase::setUserId(const char *userId) {
    CSJNIMethod  mi;
    CSJNI::findMethod(mi, CSJNIClassFirebase, "setUserId", "(Ljava/lang/String;)V", false);
    jstring juserId = mi.env->NewStringUTF(userId);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserId);
    mi.env->DeleteLocalRef(juserId);
}

#endif
