//
//  CSTextFieldBridge.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSTextFieldBridge.h"

#include "CSJNI.h"

#include <endian.h>
#include <netinet/in.h>

extern "C"
{
	void Java_kr_co_brgames_cdk_CSTextField_nativeTextFieldChanged(JNIEnv* env, jclass clazz, jlong target) {
		((CSTextField*)target)->onTextChanged();
	}

	void Java_kr_co_brgames_cdk_CSTextField_nativeTextFieldReturn(JNIEnv* env, jclass clazz, jlong target) {
        ((CSTextField*)target)->onTextReturn();
	}
}

void* CSTextFieldBridge::createHandle(CSTextField* textField) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "<init>", "(J)V", true);

	jobject h = mi.env->NewObject(mi.classId, mi.methodId, (jlong)textField);
    
    void* handle = (void*)mi.env->NewGlobalRef(h);
    
    mi.env->DeleteLocalRef(h);
    
	return handle;
}

void CSTextFieldBridge::destroyHandle(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "release", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
    
    mi.env->DeleteGlobalRef((jobject)handle);
}

void CSTextFieldBridge::addToView(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "addToView", "()V", true);

	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}

void CSTextFieldBridge::removeFromView(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "removeFromView", "()V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}

void CSTextFieldBridge::setFrame(void* handle, const CSRect& frame) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setFrame", "(FFFF)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
}

const CSString* CSTextFieldBridge::text(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "text", "()Ljava/lang/String;", true);
    
	jstring jtext = (jstring)mi.env->CallObjectMethod((jobject)handle, mi.methodId);
	const char* text = mi.env->GetStringUTFChars(jtext, NULL);
	CSString* value = CSString::string(text);
    mi.env->ReleaseStringUTFChars(jtext, text);

	mi.env->DeleteLocalRef(jtext);

	return value;
}

void CSTextFieldBridge::setText(void* handle, const char* text) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setText", "(Ljava/lang/String;)V", true);
    
	jstring jtext = mi.env->NewStringUTF(text);
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, jtext);

	mi.env->DeleteLocalRef(jtext);
}

void CSTextFieldBridge::clearText(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "clearText", "()V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId);
}

CSColor CSTextFieldBridge::textColor(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "textColor", "()I", true);
    
	uint color = htonl(mi.env->CallIntMethod((jobject)handle, mi.methodId));

	return CSColor(color);
}

void CSTextFieldBridge::setTextColor(void* handle, const CSColor& textColor) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setTextColor", "(I)V", true);

	mi.env->CallVoidMethod((jobject)handle, mi.methodId, htonl((uint)textColor));
}

void CSTextFieldBridge::setFont(void* handle, const CSFont* font) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setFont", "(Lkr/co/brgames/cdk/CSFont;)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, (jobject)font->handle());
}

const CSString* CSTextFieldBridge::placeholder(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "placeholder", "()Ljava/lang/String;", true);
    
	jstring jtext = (jstring)mi.env->CallObjectMethod((jobject)handle, mi.methodId);
	const char* text = mi.env->GetStringUTFChars(jtext, NULL);
	CSString* value = CSString::string(text);
    mi.env->ReleaseStringUTFChars(jtext, text);

	mi.env->DeleteLocalRef(jtext);

	return value;
}

void CSTextFieldBridge::setPlaceholder(void* handle, const char* placeholder) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setPlaceholder", "(Ljava/lang/String;)V", true);
    
	jstring jtext = mi.env->NewStringUTF(placeholder);
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, jtext);

	mi.env->DeleteLocalRef(jtext);
}

CSTextAlignment CSTextFieldBridge::textAlignment(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "textAlignment", "()I", true);
    
	int alignment = mi.env->CallIntMethod((jobject)handle, mi.methodId);

	return (CSTextAlignment)alignment;
}

void CSTextFieldBridge::setTextAlignment(void* handle, CSTextAlignment textAlignment) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setTextAlignment", "(I)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, textAlignment);
}

CSReturnKeyType CSTextFieldBridge::returnKeyType(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "returnKeyType", "()I", true);
    
	int returnKeyType = mi.env->CallIntMethod((jobject)handle, mi.methodId);

	return (CSReturnKeyType)returnKeyType;
}

void CSTextFieldBridge::setReturnKeyType(void* handle, CSReturnKeyType returnKeyType) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setReturnKeyType", "(I)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, returnKeyType);
}

CSKeyboardType CSTextFieldBridge::keyboardType(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "keyboardType", "()I", true);
    
	int keyboardType = mi.env->CallIntMethod((jobject)handle, mi.methodId);

	return (CSKeyboardType)keyboardType;
}

void CSTextFieldBridge::setKeyboardType(void* handle, CSKeyboardType keyboardType) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setKeyboardType", "(I)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, keyboardType);
}

bool CSTextFieldBridge::isSecureText(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "isSecureText", "()Z", true);

	bool secured = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);

	return secured;
}

void CSTextFieldBridge::setSecureText(void* handle, bool secureText) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setSecureText", "(Z)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, secureText);
}

uint CSTextFieldBridge::maxLength(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "maxLength", "()I", true);
    
	int length = mi.env->CallIntMethod((jobject)handle, mi.methodId);

	return length;
}

void CSTextFieldBridge::setMaxLength(void* handle, uint maxLength) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setMaxLength", "(I)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, maxLength);
}

uint CSTextFieldBridge::maxLine(void* handle) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "maxLine", "()I", true);
    
    int line = mi.env->CallIntMethod((jobject)handle, mi.methodId);
    
    return line;
}

void CSTextFieldBridge::setMaxLine(void* handle, uint maxLine) {
    CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setMaxLine", "(I)V", true);
    
    mi.env->CallVoidMethod((jobject)handle, mi.methodId, maxLine);
}

bool CSTextFieldBridge::isFocused(void* handle) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "isFocused", "()Z", true);
    
	bool focused = mi.env->CallBooleanMethod((jobject)handle, mi.methodId);

	return focused;
}

void CSTextFieldBridge::setFocus(void* handle, bool focused) {
	CSJNIMethod mi;
    CSJNI::findMethod(mi, CSJNIClassTextField, "setFocus", "(Z)V", true);
    
	mi.env->CallVoidMethod((jobject)handle, mi.methodId, focused);
}

#endif
