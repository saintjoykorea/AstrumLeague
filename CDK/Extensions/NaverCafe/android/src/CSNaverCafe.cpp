//
//  CSNaverCafe.cpp
//  CDK
//
//  Created by WooyolJung on 2017. 12. 14.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSNaverCafe.h"

#include "CSJNI.h"

void CSNaverCafe::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverCafe, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSNaverCafe::finalize() { }

void CSNaverCafe::startHome() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverCafe, "startHome", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSNaverCafe::startArticle(int articleId) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassNaverCafe, "startArticle", "(I)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, articleId);
}

#endif
