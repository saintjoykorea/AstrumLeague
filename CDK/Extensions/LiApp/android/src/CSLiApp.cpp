//
//  CSAppsFlyer.cpp
//  CDK
//
//  Created by ChnagSun on 2018. 8. 14.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSLiApp.h"

#include "CSJNI.h"

void CSLiApp::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLiApp, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSLiApp::finalize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassLiApp, "dispose", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}
#endif
