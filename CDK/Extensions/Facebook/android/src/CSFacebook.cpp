//
//  CSFacebook.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 19.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSFacebook.h"

#include "CSJNI.h"

static CSFacebookDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSFacebook_nativeLoginResult(JNIEnv* env, jclass clazz, jint result) {
        if (__delegate) {
            __delegate->onFacebookLogin((CSFacebookLoginResult)result);
        }
    }
}

void CSFacebook::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSFacebook::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSFacebook::setDelegate(CSFacebookDelegate* delegate) {
    __delegate = delegate;
}

void CSFacebook::login() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "login", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSFacebook::logout() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "logout", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

bool CSFacebook::isConnected() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "isConnected", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

const char* CSFacebook::userId() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "userId", "()Ljava/lang/String;", false);
    jstring juserId = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    char* userId;

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

const char* CSFacebook::pictureUrl() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassFacebook, "pictureUrl", "()Ljava/lang/String;", false);
    jstring jpictureUrl = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

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
