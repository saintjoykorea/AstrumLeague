#define CDK_IMPL

#include "CSJNI.h"

#include "CSTCGME.h"

static CSTCGMEDelegate* _delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeEnterRoom(JNIEnv* env, jclass clazz, jboolean success) {
        if (_delegate) {
            _delegate->onTCGMEEnterRoom(success);
        }
    }
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeExitRoom(JNIEnv* env, jclass clazz, jboolean intensive) {
        if (_delegate) {
            _delegate->onTCGMEExitRoom(intensive);
        }
    }
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeNotifyEnterRoom(JNIEnv* env, jclass clazz, jlong userId) {
        if (_delegate) {
            _delegate->onTCGMENotifyEnterRoom(userId);
        }
    }
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeNotifyExitRoom(JNIEnv* env, jclass clazz, jlong userId) {
        if (_delegate) {
            _delegate->onTCGMENotifyExitRoom(userId);
        }
    }
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeTalk(JNIEnv* env, jclass clazz, jlong userId, jboolean on) {
        if (_delegate) {
            _delegate->onTCGMETalk(userId, on);
        }
    }
    void Java_kr_co_brgames_cdk_extension_CSTCGME_nativeReconnect(JNIEnv* env, jclass clazz, jboolean success) {
        if (_delegate) {
            _delegate->onTCGMEReconnect(success);
        }
    }
}

static bool __enabled = false;

bool CSTCGME::isEnabled() {
    return __enabled;
}

void CSTCGME::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "isEnabled", "()Z", false);
    __enabled = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    
    if (!__enabled) return;

    CSJNI::findMethod(mi, CSJNIClassTCGME, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTCGME::finalize() {
    if (!__enabled) return;

    _delegate = NULL;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTCGME::connect(int64 userId) {
    if (!__enabled) return;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "connect", "(J)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, userId);
}

void CSTCGME::disconnect() {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "disconnect", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTCGME::setDelegate(CSTCGMEDelegate* delegate) {
    if (!__enabled) return;
    
    _delegate = delegate;
}

void CSTCGME::enterRoom(const char* roomId, CSTCGMESoundQuality quality) {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "enterRoom", "(Ljava/lang/String;I)V", false);
    jstring jroomId = mi.env->NewStringUTF(roomId);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jroomId, quality);
    mi.env->DeleteLocalRef(jroomId);
}

bool CSTCGME::isRoomEntered() {
    if (__enabled) return false;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "isRoomEntered", "()Z", false);
    jboolean rtn = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTCGME::exitRoom() {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "exitRoom", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTCGME::setMicEnabled(bool enabled) {
    if (!__enabled) return;

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "setMicEnabled", "(Z)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, enabled);
}

bool CSTCGME::isMicEnabled() {
    if (!__enabled) return false;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "isMicEnabled", "()Z", false);
    jboolean rtn = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTCGME::setSpeakerEnabled(bool enabled) {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "setSpeakerEnabled", "(Z)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, enabled);
}

bool CSTCGME::isSpeakerEnabled() {
    if (!__enabled) return false;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "isSpeakerEnabled", "()Z", false);
    jboolean rtn = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTCGME::setMicVolume(float volume) {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "setMicVolume", "(F)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, volume);
}

float CSTCGME::micVolume() {
    if (!__enabled) return 0;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "micVolume", "()F", false);
    jfloat rtn = mi.env->CallStaticFloatMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTCGME::setSpeakerVolume(float volume) {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "setSpeakerVolume", "(F)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, volume);
}

float CSTCGME::speakerVolume() {
    if (!__enabled) return 0;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "speakerVolume", "()F", false);
    jfloat rtn = mi.env->CallStaticFloatMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTCGME::pause() {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "pause", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTCGME::resume() {
    if (!__enabled) return;
    
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTCGME, "resume", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}
