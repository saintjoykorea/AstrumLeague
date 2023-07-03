//
//  CSGoogleAdMob.cpp
//  CDK
//
//  Created by Kim Jae Hong on 2021. 11. 11
//  Copyright © 2021년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSGoogleAdMob.h"

#include "CSJNI.h"

static CSGoogleAdMobDelegate* __delegate = NULL;

extern "C" {
    void Java_kr_co_brgames_cdk_extension_CSGoogleAdMob_nativeReward(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onGoogleAdMobReward();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSGoogleAdMob_nativeClose(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onGoogleAdMobClose();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSGoogleAdMob_nativeFail(JNIEnv* env, jclass clazz, jstring jmsg) {
        const char* msg = env->GetStringUTFChars(jmsg, NULL);
        if (__delegate) {
            __delegate->onGoogleAdMobFail(msg);

            env->ReleaseStringUTFChars(jmsg, msg);
        }
    }
}

void CSGoogleAdMob::show(CSGoogleAdMobDelegate* delegate) {
    if (__delegate) {
        __delegate = NULL;
    }
    __delegate = delegate;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGoogleAdMob, "show", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGoogleAdMob::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGoogleAdMob, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGoogleAdMob::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGoogleAdMob, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
