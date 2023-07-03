#define CDK_IMPL

#include "CSJNI.h"

#include "CSTeamSpeak.h"

#include "CSThread.h"

extern "C"
{
    //void ts3client_android_initJni(void* java_vm, void* context);

    jboolean Java_kr_co_brgames_cdk_extension_CSTeamSpeak_nativeInitializeLibrary(JNIEnv* env, jclass clazz, jobject applicationContext, jstring jnativeLibraryDir) {
        CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();

        if (!teamSpeak) {
            return false;
        }

//        ts3client_android_initJni(CSJNI::getJavaVM(), applicationContext);

        const char* nativeLibraryDir = env->GetStringUTFChars(jnativeLibraryDir, NULL);

        bool rtn = teamSpeak->initializeLibrary(nativeLibraryDir);

        env->ReleaseStringUTFChars(jnativeLibraryDir, nativeLibraryDir);
        env->DeleteLocalRef(jnativeLibraryDir);

        return rtn;
    }
    
    void Java_kr_co_brgames_cdk_extension_CSTeamSpeak_nativeCheckPermissionResult(JNIEnv* env, jclass clazz, jboolean granted) {
        CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
        
        if (teamSpeak) {
            teamSpeak->checkPermissionResultAndroid(granted);
        }
    }
}

bool CSTeamSpeak::initializeLibraryAndroid() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTeamSpeak, "initializeLibrary", "()Z", false);
    bool rtn = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return rtn;
}

void CSTeamSpeak::checkPermissionAndroid(bool request) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTeamSpeak, "checkPermission", "(Z)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, (jboolean)request);
}

void CSTeamSpeak::checkPermissionResultAndroid(bool granted) {
    _permissionGranted = granted;
    
    if (_delegate) _delegate->onTeamSpeakCheckPermission(granted);
}

static CSTask* _updateAudioTask = NULL;

static bool updateAudioEnabledAndroid() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTeamSpeak, "updateAudioEnabled", "()Z", false);
    return mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
}

static void updateAudioAndroid() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTeamSpeak, "updateAudio", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSTeamSpeak::prepareAudioAndroid() {
    if (!_updateAudioTask && updateAudioEnabledAndroid()) {
        updateAudioAndroid();

        _updateAudioTask = new CSTask(CSInvocation0<void>::invocation(&updateAudioAndroid),
                                      5.0f, CSTaskActivityRepeat);

        CSThread::currentThread()->start(_updateAudioTask);
    }
}

void CSTeamSpeak::restoreAudioAndroid() {
    if (_updateAudioTask) {
        CSThread::currentThread()->stop(_updateAudioTask);
        _updateAudioTask->release();
        _updateAudioTask = NULL;

        CSJNIMethod mi;
        CSJNI::findMethod(mi, CSJNIClassTeamSpeak, "restoreAudio", "()V", false);
        mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
    }
}
