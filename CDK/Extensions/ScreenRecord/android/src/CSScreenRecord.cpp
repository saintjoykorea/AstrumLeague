//
//  CSScreenRecord.cpp
//  CDK
//
//  Created by ChnagSun on 2018. 8. 14.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSScreenRecord.h"

#include "CSJNI.h"

void CSScreenRecord::initialize() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "initialize", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSScreenRecord::finalize() {
}

bool CSScreenRecord::isRecording() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "isRecording", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

bool CSScreenRecord::canQualitySelect() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "canQualitySelect", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

bool CSScreenRecord::canMicSelect() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "canMicSelect", "()Z", false);
    bool result = mi.env->CallStaticBooleanMethod(mi.classId, mi.methodId);
    return result;
}

void CSScreenRecord::recordStart(int displayWidth, int displayHeight, bool micEnabled) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "recordStart", "(IIZ)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, displayWidth, displayHeight, micEnabled);
}

void CSScreenRecord::recordEnd() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassScreenRecord, "recordEnd", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
