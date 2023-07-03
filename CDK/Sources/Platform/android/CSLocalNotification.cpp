//
//  CSNaverAuthLogin.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 13.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSLocalNotification.h"

#include "CSJNI.h"

void CSLocalNotification::addNotification(int notificationId, const char* title, const char* message, int delay) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLocalNotification, "addNotification", "(ILjava/lang/String;Ljava/lang/String;I)V", false);
    jstring jtitle = title ? mi.env->NewStringUTF(title) : NULL;
    jstring jmessage = mi.env->NewStringUTF(message);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, notificationId, jtitle, jmessage, delay);
    if (jtitle) mi.env->DeleteLocalRef(jtitle);
    mi.env->DeleteLocalRef(jmessage);
}

void CSLocalNotification::removeNotification(int notificationId) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLocalNotification, "removeNotification", "(I)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, notificationId);
}

void CSLocalNotification::clearNotifications() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLocalNotification, "clearNotifications", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
