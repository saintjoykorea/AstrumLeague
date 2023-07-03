
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSHuaweiAds.h"

#include "CSJNI.h"

static CSHuaweiAdsDelegate* __delegate = NULL;

extern "C" {
    void Java_kr_co_brgames_cdk_extension_CSHuaweiAds_nativeClose(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onHuaweiAdsClose();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSHuaweiAds_nativeReward(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onHuaweiAdsReward();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSHuaweiAds_nativeFail(JNIEnv* env, jclass clazz, jstring jmsg) {
        const char* msg = env->GetStringUTFChars(jmsg, NULL);
        if (__delegate) {
            __delegate->onHuaweiAdsFail(msg);

            env->ReleaseStringUTFChars(jmsg, msg);
        }
    }
}

void CSHuaweiAds::show(CSHuaweiAdsDelegate* delegate) {
    if (__delegate) {
        __delegate = NULL;
    }
    __delegate = delegate;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiAds, "show", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSHuaweiAds::initialize(bool testMode) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiAds, "initialize", "(Z)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, testMode);
}

void CSHuaweiAds::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiAds, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
