//
//  CSNaverLogin.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 13.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSNaverLogin.h"

#include "CSJNI.h"

static CSNaverLoginDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSNaverLogin_nativeLoginResult(JNIEnv *env, jclass clazz, jint result) {
        if (__delegate) {
            __delegate->onNaverLogin(result);
        }
    }
}

void CSNaverLogin::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverLogin, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSNaverLogin::finalize() {
    
}

void CSNaverLogin::setDelegate(CSNaverLoginDelegate* delegate) {
    __delegate = delegate;
}

void CSNaverLogin::login() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverLogin, "login", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSNaverLogin::logout() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverLogin, "logout", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

bool CSNaverLogin::isConnected() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverLogin, "isConnected", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}


const char* CSNaverLogin::accessToken() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverLogin, "accessToken", "()Ljava/lang/String;", false);
    jstring jaccessToken = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    char* accessToken;

    if (jaccessToken) {
		const char* str = mi.env->GetStringUTFChars(jaccessToken, NULL);
		accessToken = strdup(str);
		autofree(accessToken);
		mi.env->ReleaseStringUTFChars(jaccessToken, str);
        mi.env->DeleteLocalRef(jaccessToken);
    }
	else {
		accessToken = NULL;
	}

    return accessToken;
}

#endif
