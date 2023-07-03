//
//  CSGooglePlay.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 07.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSGooglePlay.h"

#include "CSJNI.h"
#include "CSFile.h"
#include "CSBuffer.h"

static CSGooglePlayDelegate* __delegate = NULL;

extern "C"
{
    void Java_kr_co_brgames_cdk_extension_CSGooglePlay_nativeLoginResult(JNIEnv* env, jclass clazz, jint result) {
        if (__delegate) {
            __delegate->onGooglePlayLogin((CSGooglePlayLoginResult)result);
        }
    }
}

void CSGooglePlay::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGooglePlay::finalize() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGooglePlay::setDelegate(CSGooglePlayDelegate *delegate) {
    __delegate = delegate;
}

void CSGooglePlay::login() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "login", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGooglePlay::logout() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "logout", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

bool CSGooglePlay::isConnected() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "isConnected", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}
	
const char* CSGooglePlay::userId() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "userId", "()Ljava/lang/String;", false);
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

const char* CSGooglePlay::email() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "email", "()Ljava/lang/String;", false);
    jstring jemail = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    char* email;
	
    if (jemail) {
		const char* str = mi.env->GetStringUTFChars(jemail, NULL);
		email = strdup(str);
		autofree(email);
		mi.env->ReleaseStringUTFChars(jemail, str);
        mi.env->DeleteLocalRef(jemail);
    }
	else {
		email = NULL;
	}

    return email;
}

const char* CSGooglePlay::pictureUrl() {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "pictureUrl", "()Ljava/lang/String;", false);
    jstring jpictureUrl = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);

    char* pictureUrl;
	
    if (jpictureUrl) {
		const char* str = mi.env->GetStringUTFChars(jpictureUrl, NULL);
		pictureUrl = strdup(str);
		autofree(pictureUrl);
		mi.env->ReleaseStringUTFChars(jpictureUrl, str);
        mi.env->DeleteLocalRef(jpictureUrl);
    }
	else {
		pictureUrl = NULL;
	}

    return pictureUrl;
}

void CSGooglePlay::reportAchievementsSteps(const char* id, int value) {
    CSJNIMethod mi;
    jstring jid;

    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "reportAchievementSteps", "(Ljava/lang/String;I)V", false);
    jid = mi.env->NewStringUTF(id);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jid, value);

    mi.env->DeleteLocalRef(jid);
}

void CSGooglePlay::reportAchievementsIncrement(const char* id, int value) {
    CSJNIMethod mi;
    jstring jid;

    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "reportAchievementIncrement", "(Ljava/lang/String;I)V", false);
    jid = mi.env->NewStringUTF(id);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jid, value);

    mi.env->DeleteLocalRef(jid);
}

void CSGooglePlay::showAchievements() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "showAchievements", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGooglePlay::reportLeaderboards(int score) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "reportLeaderboards", "(I)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, score);
}

void CSGooglePlay::showLeaderboards() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "showLeaderboards", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSGooglePlay::showReview() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassGooglePlay, "showReview", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
