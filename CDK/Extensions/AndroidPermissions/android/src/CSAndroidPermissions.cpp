//
//  CSAndroidPermissions.cpp
//  CDK
//
//  Created by WooyolJung on 2018. 04. 19.
//  Copyright © 2018 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSAndroidPermissions.h"

#include "CSJNI.h"

static CSAndroidPermissionsDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSAndroidPermissions_nativeCheckDone(JNIEnv* env, jclass clazz, jint result) {
        if (__delegate) {
			__delegate->onAndroidPermissionsCheckDone((CSAndroidPermissionResult)result);
        }
    }
}

void CSAndroidPermissions::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSAndroidPermissions::finalize() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
	
	__delegate = NULL;
}

void CSAndroidPermissions::setDelegate(CSAndroidPermissionsDelegate* delegate) {
    __delegate = delegate;
}

void CSAndroidPermissions::addEssentialPermission(const char* essentialPermission) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "addEssentialPermission", "(Ljava/lang/String;)V", false);
    jstring jessentialPermission = mi.env->NewStringUTF(essentialPermission);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jessentialPermission);
    mi.env->DeleteLocalRef(jessentialPermission);
}

void CSAndroidPermissions::checkPermissions() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "checkPermissions", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSAndroidPermissions::requestPermissions() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "requestPermissions", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSAndroidPermissions::showMoveToOptions(const char* title, const char* message, const char* positive, const char* negative) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAndroidPermissions, "showMoveToOptions", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
    jstring jtitle = mi.env->NewStringUTF(title);
    jstring jmessage = mi.env->NewStringUTF(message);
    jstring jpositive = mi.env->NewStringUTF(positive);
    jstring jnegative = mi.env->NewStringUTF(negative);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jtitle, jmessage, jpositive, jnegative);
    mi.env->DeleteLocalRef(jtitle);
    mi.env->DeleteLocalRef(jmessage);
    mi.env->DeleteLocalRef(jpositive);
    mi.env->DeleteLocalRef(jnegative);
}

#endif
