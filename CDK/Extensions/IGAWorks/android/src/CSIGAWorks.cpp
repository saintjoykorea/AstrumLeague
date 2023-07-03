//
//  CSIGAWorks.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 04.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSIGAWorks.h"

#include "CSJNI.h"

void CSIGAWorks::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassIGAWorks, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSIGAWorks::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassIGAWorks, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSIGAWorks::firstTimeExperience(const char* userActivity) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassIGAWorks, "firstTimeExperience", "(Ljava/lang/String;)V", false);
    jstring juserActivity = mi.env->NewStringUTF(userActivity);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserActivity);
    mi.env->DeleteLocalRef(juserActivity);
}

void CSIGAWorks::retention(const char* userActivity, const char* subActivity) {
    CSJNIMethod mi;
    if(!subActivity) {
        CSJNI::findMethod(mi, CSJNIClassIGAWorks, "retention", "(Ljava/lang/String;)V", false);
        jstring juserActivity = mi.env->NewStringUTF(userActivity);
        mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserActivity);
        mi.env->DeleteLocalRef(juserActivity);
    }
    else {
        CSJNI::findMethod(mi, CSJNIClassIGAWorks, "retention", "(Ljava/lang/String;Ljava/lang/String;)V", false);
        jstring juserActivity = mi.env->NewStringUTF(userActivity);
        jstring jsubActivity = mi.env->NewStringUTF(subActivity);
        mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, juserActivity, jsubActivity);
        mi.env->DeleteLocalRef(juserActivity);
        mi.env->DeleteLocalRef(jsubActivity);
    }
}

void CSIGAWorks::purchase(const char* productId, const char* productName, double price, const char* currency) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassIGAWorks, "purchase", "(Ljava/lang/String;Ljava/lang/String;DLjava/lang/String;)V", false);
    jstring jproductId = mi.env->NewStringUTF(productId);
    jstring jproductName = mi.env->NewStringUTF(productName);
    jstring jcurrency = mi.env->NewStringUTF(currency);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jproductId, jproductName, (jdouble)price, jcurrency);
    mi.env->DeleteLocalRef(jproductId);
    mi.env->DeleteLocalRef(jproductName);
    mi.env->DeleteLocalRef(jcurrency);
}

#endif
