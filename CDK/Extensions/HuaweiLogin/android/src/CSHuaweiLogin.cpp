
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSHuaweiLogin.h"

#include "CSJNI.h"

static CSHuaweiLoginDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSHuaweiLogin_nativeLoginResult(JNIEnv *env, jclass clazz, jint result) {
        if (__delegate) {
            __delegate->onHuaweiLogin((int)result);
        }
    }
}

void CSHuaweiLogin::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiLogin, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSHuaweiLogin::finalize() {
    
}

void CSHuaweiLogin::setDelegate(CSHuaweiLoginDelegate* delegate) {
    __delegate = delegate;
}

void CSHuaweiLogin::login() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiLogin, "login", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSHuaweiLogin::logout() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiLogin, "logout", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

bool CSHuaweiLogin::isConnected() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiLogin, "isConnected", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

const char* CSHuaweiLogin::userId() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiLogin, "userId", "()Ljava/lang/String;", false);
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



#endif
