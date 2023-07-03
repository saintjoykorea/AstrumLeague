//
//  CSUnityAds.cpp
//  CDK
//
//  Created by Kim Jae Hong on 2021. 12. 27
//  Copyright © 2021년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSUnityAds.h"

#include "CSJNI.h"

static CSUnityAdsDelegate* __delegate = NULL;

extern "C" {
    void Java_kr_co_brgames_cdk_extension_CSUnityAds_nativeReward(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onUnityAdsReward();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSUnityAds_nativeClose(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onUnityAdsClose();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSUnityAds_nativeFail(JNIEnv* env, jclass clazz, jstring jmsg) {
        const char* msg = env->GetStringUTFChars(jmsg, NULL);
        if (__delegate) {
            __delegate->onUnityAdsFail(msg);

            env->ReleaseStringUTFChars(jmsg, msg);
        }
    }
}

void CSUnityAds::show(CSUnityAdsDelegate* delegate) {
    if (__delegate) {
        __delegate = NULL;
    }
    __delegate = delegate;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassUnityAds, "show", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSUnityAds::initialize(bool testMode) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassUnityAds, "initialize", "(Z)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, testMode);
}

void CSUnityAds::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassUnityAds, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
