#if defined(CDK_ANDROID) && defined(CDK_IMPL)

#ifndef __CDK__JNI__
#define __CDK__JNI__

#include <jni.h>

typedef struct _CSJNIMethod {
	JNIEnv* env;
	jclass classId;
	jmethodID methodId;
}CSJNIMethod;

enum CSJNIClassId {     //TODO:remove pre-defined classes (extension problem)
    CSJNIClassActivity,
    CSJNIClassDevice,
    CSJNIClassFont,
    CSJNIClassRaster,
    CSJNIClassEncoder,
    CSJNIClassTextField,
    CSJNIClassWebView,
	CSJNIClassVideoView,
    CSJNIClassAudio,
    CSJNIClassURLRequest,
    CSJNIClassURLConnectionBridge,
	CSJNIClassLocalNotification,
    CSJNIClassCrypto,
	CSJNIClassAndroidPermissions,
	CSJNIClassFirebase,
	CSJNIClassGooglePlay,
	CSJNIClassGoogleIAP,
	CSJNIClassOneStoreIAP,
	CSJNIClassFacebook,
	CSJNIClassIGAWorks,
	CSJNIClassAppsFlyer,
	CSJNIClassFlurry,
	CSJNIClassNaverLogin,
	CSJNIClassNaverCafe,
	CSJNIClassLineLogin,
	CSJNIClassTeamSpeak,
    CSJNIClassTCGME,
	CSJNIClassScreenRecord,
	CSJNIClassLiApp,
	CSJNIClassAppSafer,
	CSJNIClassGoogleAdMob,
	CSJNIClassVungleAd,
	CSJNIClassUnityAds,
	CSJNIClassZarinPalIAP,
	CSJNIClassMyketIAP,
	CSJNIClassBazaarIAP,
	CSJNIClassHuaweiIAP,
	CSJNIClassHuaweiLogin,
	CSJNIClassHuaweiAds,
	CSJNIClassHuaweiAnalytics,
	CSJNIClassCount
};

class CSJNI {
public:
    static JavaVM* getJavaVM();
    static JNIEnv* getEnv();
    static void findMethod(CSJNIMethod& mi, CSJNIClassId classId, const char* methodName, const char* sig, bool dynamic);
    static void destroyClasses();
    static void detachThread();
};

#endif // __CDK__JNI__

#endif
