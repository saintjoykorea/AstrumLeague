//
//  CSWebViewBridge.cpp
//  CDK
//
//  Created by ChangSun on 19. 7. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSVideoViewBridge.h"

#include "CSJNI.h"

extern "C"
{
    void Java_kr_co_brgames_cdk_CSVideoView_nativePlayFinish(JNIEnv* env, jclass clazz, jlong target) {
		((CSVideoView*)target)->onPlayFinish();
	}
}

void* CSVideoViewBridge::createHandle(CSVideoView* videoView) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "<init>", "(J)V", true);
    
	jobject h = mi.env->NewObject(mi.classId, mi.methodId, (jlong)videoView);
    
    void* handle = mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);
    
	return handle;
}
void CSVideoViewBridge::destroyHandle(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "release", "()V", true);

    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
    
    mi.env->DeleteGlobalRef((jobject)handle);
}
void CSVideoViewBridge::addToView(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "addToView", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
void CSVideoViewBridge::removeFromView(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "removeFromView", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
void CSVideoViewBridge::setFrame(void* handle, const CSRect& frame) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "setFrame", "(FFFF)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
}

void CSVideoViewBridge::playStart(void* handle, const char *path) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassVideoView, "playStart", "(Ljava/lang/String;)V", true);
    
    jstring jpath = mi.env->NewStringUTF(path);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId, jpath);
    
    mi.env->DeleteLocalRef(jpath);
}
#endif
