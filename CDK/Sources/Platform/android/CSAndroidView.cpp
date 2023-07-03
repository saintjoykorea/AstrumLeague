//
//  CSAndroidView.cpp
//  CDK
//
//  Created by 김�on 13. 5. 9..
//  Copyright (c) 2012brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSAndroidView.h"

#include "CSJNI.h"
#include "CSFile.h"
#include "CSThread.h"
#include "CSOpenGL.h"
#include "CSGraphicsImpl.h"
#include "CSAudio.h"
#include "CSApplication.h"
#include "CSString.h"
#include "CSSignal.h"
#include "CSJSONParser.h"

static CSAndroidView* __instance = NULL;

extern "C"
{
	void Java_kr_co_brgames_cdk_CSView_nativeStart(JNIEnv* env, jclass clazz, jint width, jint height, jint bufferWidth, jint bufferHeight, jint horizontalEdge, jint verticalEdge, jobject jassetManager, jstring jstoragePath) {
		CSLog("nativeStart(%d, %d, %d, %d, %d, %d)", width, height, bufferWidth, bufferHeight, horizontalEdge, verticalEdge);

		srand(clock());
        
		const char* storagePath = env->GetStringUTFChars(jstoragePath, NULL);
        CSFile::initialize(jassetManager, storagePath);
        env->ReleaseStringUTFChars(jstoragePath, storagePath);
		
        CSThread::initialize();
        CSSignal::initialize();
        CSGraphicsImpl::initialize();
		CSAudio::initialize();

        __instance = new CSAndroidView(width, height, bufferWidth, bufferHeight, horizontalEdge, verticalEdge);

        onStart(__instance->view());
	}
    
    void Java_kr_co_brgames_cdk_CSView_nativeResize(JNIEnv* env, jclass clazz, jint bufferWidth, jint bufferHeight) {
        CSLog("nativeResize(%d, %d)", bufferWidth, bufferHeight);
        
        __instance->resizeBuffer(bufferWidth, bufferHeight);
    }
    
    void Java_kr_co_brgames_cdk_CSView_nativeTimeout(JNIEnv* env, jclass clazz) {
        CSThread::mainThread()->execute();
    }
    
	void Java_kr_co_brgames_cdk_CSView_nativePause(JNIEnv* env, jclass clazz) {
        CSLog("nativePause");

        CSApplication::sharedApplication()->OnPause();
        onPause(__instance->view());
		CSAudio::autoPause();
        CSThread::notifyPauseApp();
	}

    void Java_kr_co_brgames_cdk_CSView_nativeResume(JNIEnv*  env, jclass clazz) {
        CSLog("nativeResume");

        CSThread::notifyResumeApp();
		CSAudio::autoResume();
        CSApplication::sharedApplication()->OnResume();
        onResume(__instance->view());
    }

    void Java_kr_co_brgames_cdk_CSView_nativeReload(JNIEnv*  env, jclass clazz) {
        CSLog("nativeReload");

        CSThread::reload();

        CSApplication::sharedApplication()->expireGLContextIdentifier();
        CSGraphicsImpl::sharedImpl()->reload();
        __instance->reload();
        CSApplication::sharedApplication()->OnReload();
        onReload(__instance->view());
    }

    void Java_kr_co_brgames_cdk_CSView_nativeBackKey(JNIEnv*  env, jclass clazz) {
        CSLog("nativeBackKey");
        __instance->backKey();
    }

	void Java_kr_co_brgames_cdk_CSView_nativeDestroy(JNIEnv* env, jclass clazz) {
        CSLog("nativeDestroy");

        CSApplication::sharedApplication()->expireGLContextIdentifier();

        delete __instance;
        __instance = NULL;

        CSApplication::sharedApplication()->OnDestroy();
        onDestroy();
        
        CSGraphicsImpl::finalize();
		CSAudio::finalize();
        CSThread::finalize();
        CSSignal::finalize();
        CSFile::finalize();

        CSJNI::destroyClasses();
	}
    
    void Java_kr_co_brgames_cdk_CSView_nativeMemoryWarning(JNIEnv* env, jclass clazz, jint level) {
        CSLog("nativeMemoryWarning:%d", level);

        CSApplication::sharedApplication()->OnMemoryWarning((CSMemoryWarningLevel)level);
        
        onMemoryWarning(__instance->view(), (CSMemoryWarningLevel)level);

        if (level == CSMemoryWarningLevelCritical) {
            CSGraphicsImpl::sharedImpl()->purge();
        }
    }
    
    void Java_kr_co_brgames_cdk_CSView_nativeReceiveQuery(JNIEnv* env, jclass clazz, jstring jurl, jstring jquery) {
        const char* query = env->GetStringUTFChars(jquery, NULL);

        CSLog("nativeReceiveQuery:(%s)", query);

        CSDictionary<CSString, CSString>* queries = CSDictionary<CSString, CSString>::dictionary();
        
        CSJSONObject* objs = CSJSONParser::parse(query);
        
        env->ReleaseStringUTFChars(jquery, query);
        
        foreach(CSString *, key, objs->allKeys()) {
            queries->setObject(key, static_cast<CSString*>(objs->objectForKey(key)));
        }
        
        const char* url = NULL;
        
        if (jurl) {
            url = env->GetStringUTFChars(jurl, NULL);
        }

        CSApplication::sharedApplication()->OnReceiveQuery(url, queries);
        onReceiveQuery(__instance->view(), url, queries);
        
        if (jurl) {
            env->ReleaseStringUTFChars(jurl, url);
        }
	}

    void Java_kr_co_brgames_cdk_CSView_nativeTouchesBegan(JNIEnv* env, jclass clazz, jint id, jfloat x, jfloat y) {
		__instance->touchesBegan(1, &id, &x, &y);
	}

	void Java_kr_co_brgames_cdk_CSView_nativeTouchesEnded(JNIEnv* env, jclass clazz, jint id, jfloat x, jfloat y) {
		__instance->touchesEnded(1, &id, &x, &y);
	}

	void Java_kr_co_brgames_cdk_CSView_nativeTouchesMoved(JNIEnv* env, jclass clazz, jintArray ids, jfloatArray xs, jfloatArray ys) {
		int count = env->GetArrayLength(ids);
		jint id[count];
		jfloat x[count];
		jfloat y[count];

		env->GetIntArrayRegion(ids, 0, count, id);
		env->GetFloatArrayRegion(xs, 0, count, x);
		env->GetFloatArrayRegion(ys, 0, count, y);

		__instance->touchesMoved(count, id, x, y);
	}

	void Java_kr_co_brgames_cdk_CSView_nativeTouchesCancelled(JNIEnv* env, jclass clazz, jintArray ids, jfloatArray xs, jfloatArray ys) {
		int count = env->GetArrayLength(ids);
		jint id[count];
		jfloat x[count];
		jfloat y[count];

		env->GetIntArrayRegion(ids, 0, count, id);
		env->GetFloatArrayRegion(xs, 0, count, x);
		env->GetFloatArrayRegion(ys, 0, count, y);

		__instance->touchesCancelled(count, id, x, y);
	}
    
    void Java_kr_co_brgames_cdk_CSView_nativeKeyboardHeightChanged(JNIEnv* env, jclass clazz, jint height) {
        __instance->setKeyboardHeight(height);
    }
}

CSAndroidView::CSAndroidView(uint width, uint height, uint bufferWidth, uint bufferHeight, uint horizontalEdge, uint verticalEdge) {
    CSAssert(!__instance, "android create only one view, because opengl context can not be shared");
	_graphics = new CSGraphics(CSRenderTarget::target(bufferWidth, bufferHeight, true));
	_view = new CSView(this);
	_width = width;
	_height = height;
	_horizontalEdge = horizontalEdge;
	_verticalEdge = verticalEdge;
}

CSAndroidView::~CSAndroidView() {
	_graphics->release();
	_view->release();
}

void CSAndroidView::touchesBegan(int num, int ids[], float xs[], float ys[]) {
    CSPlatformTouch * pts = (CSPlatformTouch*)alloca(num * sizeof(CSPlatformTouch));
    uint i = 0;
    for (; i < num; i++) { \
        pts[i].key = (unsigned int)ids[i];
        pts[i].x = xs[i];
        pts[i].y = ys[i];
    }
	_view->touchesBegan(pts, i);
}

void CSAndroidView::touchesMoved(int num, int ids[], float xs[], float ys[]) {
    CSPlatformTouch * pts = (CSPlatformTouch*)alloca(num * sizeof(CSPlatformTouch));
    uint i = 0;
    for (; i < num; i++) { \
        pts[i].key = (unsigned int)ids[i];
        pts[i].x = xs[i];
        pts[i].y = ys[i];
    }
    _view->touchesMoved(pts, i);
}

void CSAndroidView::touchesCancelled(int num, int ids[], float xs[], float ys[]) {
    CSPlatformTouch * pts = (CSPlatformTouch*)alloca(num * sizeof(CSPlatformTouch));
    uint i = 0;
    for (; i < num; i++) { \
        pts[i].key = (unsigned int)ids[i];
        pts[i].x = xs[i];
        pts[i].y = ys[i];
    }
    _view->touchesCancelled(pts, i);
}

void CSAndroidView::touchesEnded(int num, int ids[], float xs[], float ys[]) {
    CSPlatformTouch * pts = (CSPlatformTouch*)alloca(num * sizeof(CSPlatformTouch));
    uint i = 0;
    for (; i < num; i++) { \
        pts[i].key = (unsigned int)ids[i];
        pts[i].x = xs[i];
        pts[i].y = ys[i];
    }
	_view->touchesEnded(pts, i);
}

void CSAndroidView::backKey() {
    _view->backKey();
}

void CSAndroidView::setKeyboardHeight(float height) {
    _view->setKeyboardHeight(height);
}

void CSAndroidView::resizeBuffer(uint bufferWidth, uint bufferHeight) {
    _graphics->target()->resize(bufferWidth, bufferHeight);
}

void CSAndroidView::reload() {
    uint bufferWidth = _graphics->target()->width();
    uint bufferHeight = _graphics->target()->height();
    _graphics->release();
    _graphics = new CSGraphics(CSRenderTarget::target(bufferWidth, bufferHeight, true));
}

void CSAndroidView::render(bool refresh) {
    if (refresh) {
        _view->draw(_graphics);
		
		_graphics->flush();
    }
    glDisableCS(GL_STENCIL_TEST);
	glStencilOpCS(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMaskCS(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glBindFramebufferCS(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
    CSGraphicsImpl::sharedImpl()->fillShader()->draw(_graphics->target()->screen(false), CSRenderBounds::Full);

	glFlushCS();
    
    CSGraphics::invalidate(CSGraphicsValidationTarget | CSGraphicsValidationStencil);
}

void CSApplication::setVersion(const char *version) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "setVersion", "(Ljava/lang/String;)V", false);

    if (version) {
        if (!_version) _version = new CSString(version);
        else _version->set(version);
        
        jstring jversion = mi.env->NewStringUTF(version);
        mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jversion);
        mi.env->DeleteLocalRef(jversion);
    }
    else {
        CSObject::release(_version);
        
        mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, NULL);
    }
}

void CSApplication::setResolution(CSResolution resolution) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "setResolution", "(I)V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, resolution);
}

CSResolution CSApplication::resolution() const {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "resolution", "()I", false);
    return (CSResolution)mi.env->CallStaticIntMethod(mi.classId, mi.methodId);
}

void CSApplication::openURL(const char* url) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "openURL", "(Ljava/lang/String;)V", false);
    jstring jurl = mi.env->NewStringUTF(url);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jurl);
    mi.env->DeleteLocalRef(jurl);
}

const CSString* CSApplication::clipboard() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "clipboard", "()Ljava/lang/String;", false);
    jstring jstr = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId);
    
    CSString* rtn = NULL;
    
    if (jstr) {
        const char* str = mi.env->GetStringUTFChars(jstr, NULL);
        rtn = CSString::string(str);
        mi.env->ReleaseStringUTFChars(jstr, str);
        mi.env->DeleteLocalRef(jstr);
    }
    
    return rtn;
}

void CSApplication::setClipboard(const char* text) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "setClipboard", "(Ljava/lang/String;)V", false);
    jstring jtext = mi.env->NewStringUTF(text);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jtext);
    mi.env->DeleteLocalRef(jtext);
}

void CSApplication::shareUrl(const char* title, const char* message, const char* url) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "shareUrl", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
	jstring jtitle = mi.env->NewStringUTF(title);
	jstring jmessage = mi.env->NewStringUTF(message);
	jstring jurl = mi.env->NewStringUTF(url);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, jtitle, jmessage, jurl);
	mi.env->DeleteLocalRef(jtitle);
	mi.env->DeleteLocalRef(jmessage);
	mi.env->DeleteLocalRef(jurl);
}

void CSApplication::finish() {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassActivity, "finishShared", "()V", false);
    mi.env->CallStaticVoidMethod(mi.classId, mi.methodId);
}

#endif
