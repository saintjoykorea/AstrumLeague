//
//  CSNaverAuthLogin.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 13.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSLineLogin.h"

#include "CSJNI.h"

static CSLineLoginDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSLineLogin_nativeLoginResult(JNIEnv *env, jclass clazz, jint result) {
        if (__delegate) {
            __delegate->onLineLogin(result);
        }
    }
}

void CSLineLogin::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSLineLogin::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSLineLogin::setDelegate(CSLineLoginDelegate *delegate) {
    __delegate = delegate;
}

void CSLineLogin::login() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "login", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSLineLogin::logout() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "logout", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

bool CSLineLogin::isConnected() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "isConnected", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

const char* CSLineLogin::userId() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "userId", "()Ljava/lang/String;", false);
    jstring juserId = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    const char* userId;

    if (juserId) {
		const char* str = mi.env->GetStringUTFChars(juserId, NULL);
		userId = strdup(str);
		autofree(userId);
		mi.env->ReleaseStringUTFChars(juserId, str);
        mi.env->DeleteLocalRef(juserId);
    }
	else {
		userId = NULL;
	}

    return userId;
}


const char* CSLineLogin::pictureUrl() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLineLogin, "pictureUrl", "()Ljava/lang/String;", false);
    jstring jimageUrl = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    char* pictureUrl;
	
    if (jpictureUrl) {
		const char* str = mi.env->GetStringUTFChars(jpictureUrl, NULL);
		pictureUrl = strdup(str);
		autofree(pictureUrl);
		mi.env->ReleaseStringUTFChars(jpictureUrl, str);
        mi.env->DeleteLocalRef(jpictureUrl);
    }
	else {
		pictureUrl = NULL;
	}

    return pictureUrl;
}

#endif
