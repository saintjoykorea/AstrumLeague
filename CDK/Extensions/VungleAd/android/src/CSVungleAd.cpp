//
//  CSVungleAd.cpp
//  CDK
//
//  Created by Kim Jae Hong on 2021. 12. 21
//  Copyright © 2021년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSVungleAd.h"

#include "CSJNI.h"

static CSVungleAdDelegate* __delegate = NULL;

extern "C" {
    void Java_kr_co_brgames_cdk_extension_CSVungleAd_nativeClose(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onVungleAdClose();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSVungleAd_nativeReward(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onVungleAdReward();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSVungleAd_nativeFail(JNIEnv* env, jclass clazz, jstring jmsg) {
        const char* msg = env->GetStringUTFChars(jmsg, NULL);
        if (__delegate) {
            __delegate->onVungleAdFail(msg);

            env->ReleaseStringUTFChars(jmsg, msg);
        }
    }
}

void CSVungleAd::show(CSVungleAdDelegate* delegate) {
    if (__delegate) {
        __delegate = NULL;
    }
    __delegate = delegate;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVungleAd, "show", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSVungleAd::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVungleAd, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSVungleAd::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVungleAd, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
