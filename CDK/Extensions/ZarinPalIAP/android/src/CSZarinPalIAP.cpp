
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSZarinPalIAP.h"

#include "CSJNI.h"

static CSIAPDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSZarinPalIAP_nativeConnected(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onIAPConnected();
        }
    }

    void Java_kr_co_brgames_cdk_extension_CSZarinPalIAP_nativePurchaseComplete(JNIEnv *env, jclass clazz, jstring jproductId, jstring jchargeData1, jstring jchargeData2, jstring jpayload) {
        if (__delegate) {
            const char* productId = env->GetStringUTFChars(jproductId, NULL);
            const char* chargeData1 = env->GetStringUTFChars(jchargeData1, NULL);
            const char* chargeData2 = env->GetStringUTFChars(jchargeData2, NULL);
            const char* payload = env->GetStringUTFChars(jpayload, NULL);
            
            __delegate->onIAPComplete(productId, chargeData1, chargeData2, payload);
            
            env->ReleaseStringUTFChars(jproductId, productId);
            env->ReleaseStringUTFChars(jchargeData1, chargeData1);
            env->ReleaseStringUTFChars(jchargeData2, chargeData2);
            env->ReleaseStringUTFChars(jpayload, payload);
        }
    }
    
    void Java_kr_co_brgames_cdk_extension_CSZarinPalIAP_nativePurchaseCancelled(JNIEnv* env, jclass clazz) {
        if (__delegate) {
            __delegate->onIAPCancelled();
        }
    }
    
    void Java_kr_co_brgames_cdk_extension_CSZarinPalIAP_nativeError(JNIEnv* env, jclass clazz, jint errorCode) {
        if (__delegate) {
            __delegate->onIAPError(errorCode);
        }
    }
}

void CSZarinPalIAP::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassZarinPalIAP, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSZarinPalIAP::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassZarinPalIAP, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSZarinPalIAP::setDelegate(CSIAPDelegate* delegate) {
    __delegate = delegate;
}

void CSZarinPalIAP::connect() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassZarinPalIAP, "connect", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSZarinPalIAP::sync(const char* const* productIds, uint count) {
    if (__delegate) __delegate->onIAPSync();
}

void CSZarinPalIAP::purchase(const char* productId, const char* payload) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassZarinPalIAP, "purchase", "(Ljava/lang/String;Ljava/lang/String;)V", false);
    jstring jproductId = mi.env->NewStringUTF(productId);
    jstring jpayload = mi.env->NewStringUTF(payload);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jproductId, jpayload);
    mi.env->DeleteLocalRef(jproductId);
    mi.env->DeleteLocalRef(jpayload);
}

void CSZarinPalIAP::setUserId(const char *userId) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassZarinPalIAP, "setUserId", "(Ljava/lang/String;)V", false);
    jstring juserId = mi.env->NewStringUTF(userId);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserId);
    mi.env->DeleteLocalRef(juserId);
}

#endif
