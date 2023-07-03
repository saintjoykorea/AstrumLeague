//
//  CSAudio.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSAudio.h"

#include "CSThread.h"

#include "CSJNI.h"

static CSString* __commands = NULL;
static CSLock __commandLock;

void CSAudio::initialize() {
    __commands = new CSString();

    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAudio, "create", "()V", false);

    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

void CSAudio::finalize() {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassAudio, "dispose", "()V", false);

	mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);

	synchronized(__commandLock) {
        __commands->release();
        __commands = NULL;
    }
}

void CSAudio::autoPause() {
    synchronized(__commandLock) {
        __commands->append("autoPause;");
    }
}

void CSAudio::autoResume() {
    synchronized(__commandLock) {
        __commands->append("autoResume;");
    }
}

void CSAudio::play(int control, const char* path, float volume, bool loop, int priority, int category, bool fx, bool single) {
    synchronized(__commandLock) {
        __commands->appendFormat("play,%d,%s,%f,%d,%d,%d,%d,%d;", control, path, volume, loop, priority, category, fx, single);
    }
}

void CSAudio::stop(int control) {
    synchronized(__commandLock) {
        __commands->appendFormat("stop,%d;", control);
    }
}

void CSAudio::pause(int control) {
    synchronized(__commandLock) {
        __commands->appendFormat("pause,%d;", control);
    }
}

void CSAudio::resume(int control) {
    synchronized(__commandLock) {
        __commands->appendFormat("resume,%d;", control);
    }
}

void CSAudio::setVolume(int category, float volume) {
    synchronized(__commandLock) {
        __commands->appendFormat("volume,%d,%f;", category, volume);
    }
}

int CSAudio::singleCounter() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAudio, "singleCounter", "()I", false);

    int counter = mi.env->CallStaticIntMethod(mi.classId, mi.methodId);

    return counter;
}

void CSAudio::vibrate(float time) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassAudio, "vibrate", "(F)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, time);
}

extern "C"
{
    jstring Java_kr_co_brgames_cdk_CSAudio_nativeCommand(JNIEnv* env, jclass clazz) {
        jstring jcommand = NULL;
        synchronized(__commandLock) {
            if (__commands) {
                const char *str = *__commands;
                if (*str) {
                    jcommand = env->NewStringUTF(str);
                    __commands->clear();
                }
            }
        }
        return jcommand;
    }
}

#endif
