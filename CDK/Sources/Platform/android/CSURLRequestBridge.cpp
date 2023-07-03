//
//  CSURLRequestBridge.m
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSURLRequestBridge.h"

#include "CSData.h"

#include "CSJNI.h"

void* CSURLRequestBridge::createHandle(const char* url, const char* method, bool usingCache, float timeoutInterval) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLRequest, "create", "(Ljava/lang/String;Ljava/lang/String;ZF)Lkr/co/brgames/cdk/CSURLRequest;", false);

	jstring jurl = mi.env->NewStringUTF(url);
    jstring jmethod = mi.env->NewStringUTF(method);
	jobject h = mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jurl, jmethod, usingCache, timeoutInterval);
	mi.env->DeleteLocalRef(jurl);
    mi.env->DeleteLocalRef(jmethod);

    if (h) {
        void* handle = mi.env->NewGlobalRef(h);
        
        mi.env->DeleteLocalRef(h);
        
        return handle;
    }
    else {
        return NULL;
    }
}

void CSURLRequestBridge::destroyHandle(void* handle) {
    CSJNI::getEnv()->DeleteGlobalRef((jobject)handle);
}

void CSURLRequestBridge::setURL(void* handle, const char* url) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setURL", "(Ljava/lang/String;)V", true);

	jstring jurl = mi.env->NewStringUTF(url);
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, jurl);

	mi.env->DeleteLocalRef(jurl);
}

const char* CSURLRequestBridge::URL(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLRequest, "URL", "()Ljava/lang/String;", true);
    
	jstring jurl = (jstring)mi.env->CallObjectMethod((jobject)handle, mi.methodId);
    const char* url = mi.env->GetStringUTFChars(jurl, NULL);
    char* rtn = strdup(url);
    autofree(rtn);
    mi.env->ReleaseStringUTFChars(jurl, url);
    
    return rtn;
}

void CSURLRequestBridge::setMethod(void* handle, const char* method) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setMethod", "(Ljava/lang/String;)V", true);

	jstring jstr = mi.env->NewStringUTF(method);
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, jstr);

	mi.env->DeleteLocalRef(jstr);
}

const char* CSURLRequestBridge::method(void* handle) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "method", "()Ljava/lang/String;", true);

	jstring jmethod = (jstring)mi.env->CallObjectMethod((jobject)handle, mi.methodId);
	const char* method = mi.env->GetStringUTFChars(jmethod, NULL);
	char* rtn = strdup(method);
	autofree(rtn);
	mi.env->ReleaseStringUTFChars(jmethod, method);

	mi.env->DeleteLocalRef(jmethod);

	return rtn;
}

void CSURLRequestBridge::setUsingCache(void* handle, bool usingCache) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setUsingCache", "(Z)V", true);

	mi.env->CallVoidMethod((jobject)handle, mi.methodId, usingCache);
}

bool CSURLRequestBridge::usingCache(void* handle) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "usingCache", "()Z", true);

	bool usingCache = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);

	return usingCache;
}

void CSURLRequestBridge::setTimeoutInterval(void* handle, float timeoutInterval) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setTimeoutInterval", "(F)V", true);

	mi.env->CallVoidMethod((jobject)handle, mi.methodId, timeoutInterval);
}

float CSURLRequestBridge::timeoutInterval(void* handle) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "timeoutInterval", "()F", true);

	jfloat interval = mi.env->CallFloatMethod((jobject)handle, mi.methodId);

	return interval;
}

void CSURLRequestBridge::setHeaderField(void* handle, const char* name, const char* value) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setHeaderField", "(Ljava/lang/String;Ljava/lang/String;)V", true);

	jstring jname = mi.env->NewStringUTF(name);
	jstring jvalue = mi.env->NewStringUTF(value);
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, jname, jvalue);

	mi.env->DeleteLocalRef(jname);
	mi.env->DeleteLocalRef(jvalue);
}

const char* CSURLRequestBridge::headerField(void* handle, const char* name) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "headerField", "(Ljava/lang/String;)Ljava/lang/String;", true);

	jstring jname = mi.env->NewStringUTF(name);
	jstring jvalue = (jstring)mi.env->CallObjectMethod((jobject)handle, mi.methodId, jname);
	mi.env->DeleteLocalRef(jname);

	if (!jvalue) return NULL;

	const char* temp = mi.env->GetStringUTFChars(jvalue, NULL);
	char* value = strdup(temp);
	mi.env->ReleaseStringUTFChars(jvalue, temp);
	mi.env->DeleteLocalRef(jvalue);
	autofree(value);
	return value;
}

const CSDictionary<CSString, CSString>* CSURLRequestBridge::headerFields(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLRequest, "headerFields", "()[Ljava/lang/String;", true);

    jobjectArray jheaders = (jobjectArray)mi.env->CallObjectMethod((jobject)handle, mi.methodId);

	CSDictionary<CSString, CSString>* dic = CSDictionary<CSString, CSString>::dictionary();

	int len = mi.env->GetArrayLength(jheaders);

	for (int i = 0; i < len; i += 2) {
		jstring jkey = (jstring)mi.env->GetObjectArrayElement(jheaders, i);
		jstring jvalue = (jstring)mi.env->GetObjectArrayElement(jheaders, i + 1);

		const char* key = mi.env->GetStringUTFChars(jkey, NULL);
		const char* value = mi.env->GetStringUTFChars(jvalue, NULL);

		dic->setObject(CSString::string(key), CSString::string(value));

		mi.env->ReleaseStringUTFChars(jkey, key);
		mi.env->ReleaseStringUTFChars(jvalue, value);
		mi.env->DeleteLocalRef(jkey);
		mi.env->DeleteLocalRef(jvalue);
	}

	mi.env->DeleteLocalRef(jheaders);

	return dic;
}

void CSURLRequestBridge::setBody(void* handle, const void* data, uint length) {
	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassURLRequest, "setBody", "([B)V", true);

	if (data && length) {
		jbyteArray jbytes = mi.env->NewByteArray(length);
		mi.env->SetByteArrayRegion(jbytes, 0, length, (const jbyte *) data);
		mi.env->CallVoidMethod((jobject)handle, mi.methodId, jbytes);

		mi.env->DeleteLocalRef(jbytes);
	}
	else mi.env->CallVoidMethod((jobject)handle, mi.methodId, NULL);
}

const CSData* CSURLRequestBridge::body(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassURLRequest, "body", "()[B", true);

	jbyteArray jbody = (jbyteArray)mi.env->CallObjectMethod((jobject)handle, mi.methodId);

	CSData* data = NULL;
	if (jbody) {
		int len = mi.env->GetArrayLength(jbody);
		if (len) {
			jbyte* outbytes = mi.env->GetByteArrayElements(jbody, NULL);
			data = CSData::dataWithBytes((void*)outbytes, len);
			mi.env->ReleaseByteArrayElements(jbody, outbytes, JNI_ABORT);
		}
		mi.env->DeleteLocalRef(jbody);
	}
	return data;
}

#endif
