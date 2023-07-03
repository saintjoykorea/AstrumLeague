//
//  CSURLConnectionBridge.m
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSURLConnectionBridge.h"

#include "CSData.h"

#include "CSJNI.h"

extern "C"
{
	void Java_kr_co_brgames_cdk_CSURLConnectionBridge_nativeReceiveResponse(JNIEnv* env, jclass clazz, jlong target, jint statusCode) {
		((CSURLConnection*)target)->onReceiveResponse(statusCode);
	}

	void Java_kr_co_brgames_cdk_CSURLConnectionBridge_nativeReceiveData(JNIEnv* env, jclass clazz, jlong target, jbyteArray jdata) {
        int length = env->GetArrayLength(jdata);
        void* data = fmalloc(length);
        env->GetByteArrayRegion(jdata, 0, length, (jbyte*)data);
		((CSURLConnection*)target)->onReceiveData(data, length);
        free(data);
	}

	void Java_kr_co_brgames_cdk_CSURLConnectionBridge_nativeError(JNIEnv* env, jclass clazz, jlong target) {
		((CSURLConnection*)target)->onError();
	}

	void Java_kr_co_brgames_cdk_CSURLConnectionBridge_nativeFinishLoading(JNIEnv* env, jclass clazz, jlong target) {
		((CSURLConnection*)target)->onFinishLoading();
	}
}

void* CSURLConnectionBridge::createHandle(CSURLConnection* connection, const CSURLRequest* request) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLConnectionBridge, "createHandle", "(Lkr/co/brgames/cdk/CSURLRequest;J)Lkr/co/brgames/cdk/CSURLConnection;", false);

	jobject h = mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, (jobject)request->handle(), (jlong)connection);
    
    void* handle = mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);

	return handle;
}

void CSURLConnectionBridge::destroyHandle(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLConnectionBridge, "destroyHandle", "(Lkr/co/brgames/cdk/CSURLConnection;)V", false);

	mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, (jobject)handle);
    
    mi.env->DeleteGlobalRef((jobject)handle);
}

CSData* CSURLConnectionBridge::sendSynchronousRequest(const CSURLRequest* request, int* statusCode) {
	void* data = NULL;
	int length = 0;

	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLConnectionBridge, "sendSynchronousRequest", "(Lkr/co/brgames/cdk/CSURLRequest;[I)[B", false);

	jintArray joutstatus = mi.env->NewIntArray(2);

	jbyteArray joutdata = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, (jobject)request->handle(), joutstatus);

	if (statusCode) {
        jint outstatus[1];
        mi.env->GetIntArrayRegion(joutstatus, 0, 1, outstatus);
		*statusCode = outstatus[0];
	}

	if (joutdata) {
		length = mi.env->GetArrayLength(joutdata);
		if (length) {
            data=fmalloc(length);
            mi.env->GetByteArrayRegion(joutdata, 0, length, (jbyte*)data);
		}
		mi.env->DeleteLocalRef(joutdata);
	}

	mi.env->DeleteLocalRef(joutstatus);

	return data && length > 0 ? CSData::dataWithBytes(data, length, false) : NULL;
}

void CSURLConnectionBridge::start(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLConnectionBridge, "start", "(Lkr/co/brgames/cdk/CSURLConnection;)V", false);

	mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, (jobject)handle);
}

void CSURLConnectionBridge::cancel(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLConnectionBridge, "cancel", "(Lkr/co/brgames/cdk/CSURLConnection;)V", false);

	mi.env->CallStaticVoidMethod(mi.classId, mi.methodId, (jobject)handle);
}

#endif
