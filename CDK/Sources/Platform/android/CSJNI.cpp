#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSJNI.h"

#include "CSLog.h"

#include "CSThread.h"

static JavaVM* __javaVM = NULL;

static std::vector<pthread_t> __attaches;

extern "C"
{
	jint JNI_OnLoad(JavaVM *vm, void *reserved) {
#if defined(__arm__)
        #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#endif
        JNIEnv *env;
        if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
            CSErrorLog("JNI_OnLoad error: %s", ABI);
            return -1;
        }
        CSLog("JNI_OnLoad: %s", ABI);
        env->GetJavaVM(&__javaVM);
		return JNI_VERSION_1_6;
	}
}

static const char* __classNames[] = {
    "kr/co/brgames/cdk/CSActivity",    
    "kr/co/brgames/cdk/CSDevice",
    "kr/co/brgames/cdk/CSFont",
    "kr/co/brgames/cdk/CSRaster",
    "kr/co/brgames/cdk/CSEncoder",
    "kr/co/brgames/cdk/CSTextField",
    "kr/co/brgames/cdk/CSWebView",
    "kr/co/brgames/cdk/CSVideoView",
    "kr/co/brgames/cdk/CSAudio",
    "kr/co/brgames/cdk/CSURLRequest",
    "kr/co/brgames/cdk/CSURLConnectionBridge",
    "kr/co/brgames/cdk/CSLocalNotification",
    "kr/co/brgames/cdk/CSCrypto",
    "kr/co/brgames/cdk/extension/CSAndroidPermissions",
    "kr/co/brgames/cdk/extension/CSFirebase",
    "kr/co/brgames/cdk/extension/CSGooglePlay",
    "kr/co/brgames/cdk/extension/CSGoogleIAP",
    "kr/co/brgames/cdk/extension/CSOneStoreIAP",
    "kr/co/brgames/cdk/extension/CSFacebook",
    "kr/co/brgames/cdk/extension/CSIGAWorks",
    "kr/co/brgames/cdk/extension/CSAppsFlyer",
    "kr/co/brgames/cdk/extension/CSFlurry",
    "kr/co/brgames/cdk/extension/CSNaverLogin",
    "kr/co/brgames/cdk/extension/CSNaverCafe",
    "kr/co/brgames/cdk/extension/CSLineLogin",
    "kr/co/brgames/cdk/extension/CSTeamSpeak",
    "kr/co/brgames/cdk/extension/CSTCGME",
    "kr/co/brgames/cdk/extension/CSScreenRecord",
    "kr/co/brgames/cdk/extension/CSLiApp",
	"kr/co/brgames/cdk/extension/CSAppSafer",
	"kr/co/brgames/cdk/extension/CSGoogleAdMob",
    "kr/co/brgames/cdk/extension/CSVungleAd",
    "kr/co/brgames/cdk/extension/CSUnityAds",
    "kr/co/brgames/cdk/extension/CSZarinPalIAP",
	"kr/co/brgames/cdk/extension/CSMyketIAP",
	"kr/co/brgames/cdk/extension/CSBazaarIAP",
	"kr/co/brgames/cdk/extension/CSHuaweiIAP",
	"kr/co/brgames/cdk/extension/CSHuaweiLogin",
	"kr/co/brgames/cdk/extension/CSHuaweiAds",
	"kr/co/brgames/cdk/extension/CSHuaweiAnalytics"
};

static jclass __classes[CSJNIClassCount] = {NULL,};

static void loadClass(JNIEnv* env, CSJNIClassId classId) {
    jclass localClass = env->FindClass(__classNames[classId]);
    jclass globalClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    __classes[classId] = globalClass;
}

static void loadClassOnMainThread(CSJNIClassId classId) {
    if (!__classes[classId]) {
        JNIEnv* env;
        __javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
        loadClass(env, classId);
    }
}

static jclass findClass(JNIEnv* env, CSJNIClassId classId) {
    if (!__classes[classId]) {
        if (CSThread::mainThread()->isActive()) {
            loadClass(env, classId);
        }
        else {
            CSDelegate0<void>* delegate = new CSDelegate0<void>([classId]() {
                if (!__classes[classId]) {
                    JNIEnv* env;
                    __javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
                    loadClass(env, classId);
                }
            });
            CSThread::mainThread()->start(CSTask::task(delegate, 0.0f, CSTaskActivityWait));
            delegate->release();
        }
    }
    return __classes[classId];
}

JavaVM* CSJNI::getJavaVM() {
    return __javaVM;
}

JNIEnv* CSJNI::getEnv() {
    JNIEnv* env = NULL;
    
    int res = __javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (res == JNI_EDETACHED || res == JNI_EVERSION) {
        res = __javaVM->AttachCurrentThread(&env, NULL);
        
        if (res == JNI_OK) {
            __attaches.push_back(pthread_self());
        }
    }
    CSAssert(res == JNI_OK, "failed to get the environment");
    
    return env;
}

void CSJNI::findMethod(CSJNIMethod& mi, CSJNIClassId classId, const char* methodName, const char* sig, bool dynamic) {
    mi.env = getEnv();

    mi.classId = findClass(mi.env, classId);

    CSAssert(mi.classId, "failed to find class of %s", __classNames[classId]);
    
    mi.methodId = dynamic ? mi.env->GetMethodID(mi.classId, methodName, sig) : mi.env->GetStaticMethodID(mi.classId, methodName, sig);

    CSAssert(mi.methodId, "failed to find method id of %s", methodName);
}

void CSJNI::destroyClasses() {
    JNIEnv* env;
    int res = __javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    CSAssert(res == JNI_OK, "invalid operation");
    for (int i = 0; i < CSJNIClassCount; i++) {
        if (__classes[i]) {
            env->DeleteGlobalRef(__classes[i]);
            __classes[i] = NULL;
        }
    }
}

void CSJNI::detachThread() {
    pthread_t pt = pthread_self();
    for (std::vector<pthread_t>::iterator i = __attaches.begin(); i != __attaches.end(); i++) {
        pthread_t t = *i;
        
        if (pthread_equal(pt, t)) {
            int res = __javaVM->DetachCurrentThread();
            
            if (res == JNI_OK) {
                __attaches.erase(i);
            }
            break;
        }
    }
}

#endif
