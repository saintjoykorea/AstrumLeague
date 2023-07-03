
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSHuaweiAnalytics.h"

#include "CSJNI.h"

void CSHuaweiAnalytics::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiAnalytics, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSHuaweiAnalytics::finalize() {

}

void CSHuaweiAnalytics::log(const char* name, int paramCount, ...) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassHuaweiAnalytics, "log", "(Ljava/lang/String;[Ljava/lang/String;)V", false);

    jstring jname = mi.env->NewStringUTF(name);
    jobjectArray jparams = mi.env->NewObjectArray(paramCount * 2, mi.env->FindClass("java/lang/String"), NULL);

	va_list ap;
    va_start(ap, paramCount);
    for (int i = 0; i < paramCount * 2; i++) {
		const char* param = va_arg(ap, const char*);
		mi.env->SetObjectArrayElement(jparams, i, mi.env->NewStringUTF(param));
	}
    va_end(ap);

    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jname, jparams);
    mi.env->DeleteLocalRef(jname);
    mi.env->DeleteLocalRef(jparams);
}

#endif
