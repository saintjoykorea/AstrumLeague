//
//  CSWebViewBridge.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 7..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSWebViewBridge.h"

#include "CSJNI.h"

extern "C"
{
	void Java_kr_co_brgames_cdk_CSWebView_nativeLoadError(JNIEnv* env, jclass clazz, jlong target) {
		((CSWebView*)target)->onURLError();
	}

    jboolean Java_kr_co_brgames_cdk_CSWebView_nativeLink(JNIEnv* env, jclass clazz, jlong target, jstring jurl) {
        const char* url = env->GetStringUTFChars(jurl, NULL);
        
		bool rtn = ((CSWebView*)target)->onURLLink(url);
        
        env->ReleaseStringUTFChars(jurl, url);

        return rtn;
	}

    void Java_kr_co_brgames_cdk_CSWebView_nativeStartLoad(JNIEnv* env, jclass clazz, jlong target) {
		((CSWebView*)target)->onURLStartLoad();
	}

    void Java_kr_co_brgames_cdk_CSWebView_nativeFinishLoad(JNIEnv* env, jclass clazz, jlong target) {
		((CSWebView*)target)->onURLFinishLoad();
	}
}

void* CSWebViewBridge::createHandle(CSWebView* webView) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "<init>", "(J)V", true);
    
	jobject h = mi.env->NewObject(mi.classId, mi.methodId, (jlong)webView);
    
    void* handle = mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);
    
	return handle;
}
void CSWebViewBridge::destroyHandle(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "release", "()V", true);

    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
    
    mi.env->DeleteGlobalRef((jobject)handle);
}
void CSWebViewBridge::addToView(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "addToView", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
void CSWebViewBridge::removeFromView(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "removeFromView", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
void CSWebViewBridge::setFrame(void* handle, const CSRect& frame) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "setFrame", "(FFFF)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
}
void CSWebViewBridge::loadData(void* handle, const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "loadData", "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", true);
    
    jbyteArray jdata = mi.env->NewByteArray(length);
	mi.env->SetByteArrayRegion(jdata, 0, length, (const jbyte*)data);
	
    jstring jmimeType = mi.env->NewStringUTF(mimeType);
    jstring jtextEncodingName = mi.env->NewStringUTF(textEncodingName);
    jstring jbaseUrl = baseUrl ? mi.env->NewStringUTF(baseUrl) : NULL;
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId, jdata, jmimeType, jtextEncodingName, jbaseUrl);
    
    mi.env->DeleteLocalRef(jdata);
    mi.env->DeleteLocalRef(jmimeType);
    mi.env->DeleteLocalRef(jtextEncodingName);
    if (jbaseUrl) {
        mi.env->DeleteLocalRef(jbaseUrl);
    }
}

void CSWebViewBridge::loadHTML(void* handle, const char* html, const char* baseUrl) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "loadHTML", "(Ljava/lang/String;Ljava/lang/String;)V", true);
    
    jstring jhtml = mi.env->NewStringUTF(html);
    jstring jbaseUrl = baseUrl ? mi.env->NewStringUTF(baseUrl) : NULL;
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId, jhtml, jbaseUrl);
    
    mi.env->DeleteLocalRef(jhtml);
    if (jbaseUrl) {
        mi.env->DeleteLocalRef(jbaseUrl);
    }
}

void CSWebViewBridge::loadRequest(void* handle, const CSURLRequest* request) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "loadRequest", "(Lkr/co/brgames/cdk/CSURLRequest;)V", true);
    if (request) {
        mi.env->CallVoidMethod((jobject) handle, mi.methodId, (jobject) request->handle());
    }
}

bool CSWebViewBridge::isLoading(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "isLoading", "()Z", true);
    
    bool loading = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);
    
    return loading;
}
void CSWebViewBridge::stopLoading(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "stopLoading", "()V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}

void CSWebViewBridge::reload(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "reload", "()V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
bool CSWebViewBridge::canGoBack(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "canGoBack", "()Z", true);
    
    bool canGoBack = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);
    
    return canGoBack;
}
bool CSWebViewBridge::canGoForward(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "canGoForward", "()Z", true);
    
    bool canGoForward = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);
    
    return canGoForward;
}
void CSWebViewBridge::goBack(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "goBack", "()V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
void CSWebViewBridge::goForward(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "goForward", "()V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}
bool CSWebViewBridge::scaleToFit(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "scaleToFit", "()Z", true);
    
    bool scaleToFit = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);
    
    return scaleToFit;
}
void CSWebViewBridge::setScaleToFit(void* handle, bool scaleToFit) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassWebView, "setScaleToFit", "(Z)V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId, scaleToFit);
}

#endif
