#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSTextViewBridge.h"
#include "CSTextFieldBridge.h"

void* CSTextViewBridge::createHandle(CSTextView* textView) {
	return NULL;		//TODO:IMPL
}

void CSTextViewBridge::destroyHandle(void* handle) {
    CSTextFieldBridge::destroyHandle(handle);
}

void CSTextViewBridge::addToView(void* handle) {
    CSTextFieldBridge::addToView(handle);
}

void CSTextViewBridge::removeFromView(void* handle) {
    CSTextFieldBridge::removeFromView(handle);
}

void CSTextViewBridge::setFrame(void* handle, const CSRect& frame) {
    CSTextFieldBridge::setFrame(handle, frame);
}

CSString* CSTextViewBridge::text(void* handle) {
    return CSTextFieldBridge::text(handle);
}

void CSTextViewBridge::setText(void* handle, const char* text) {
    CSTextFieldBridge::setText(handle, text);
}

void CSTextViewBridge::clearText(void* handle) {
    CSTextFieldBridge::clearText(handle);
}

CSColor CSTextViewBridge::textColor(void* handle) {
    return CSTextFieldBridge::textColor(handle);
}

void CSTextViewBridge::setTextColor(void* handle, const CSColor& textColor) {
    CSTextFieldBridge::setTextColor(handle, textColor);
}

void CSTextViewBridge::setFont(void* handle, const CSFont* font) {
    CSTextFieldBridge::setFont(handle, font);
}

CSTextAlignment CSTextViewBridge::textAlignment(void* handle) {
    return CSTextFieldBridge::textAlignment(handle);
}

void CSTextViewBridge::setTextAlignment(void* handle, CSTextAlignment textAlignment) {
    CSTextFieldBridge::setTextAlignment(handle, textAlignment);
}

CSReturnKeyType CSTextViewBridge::returnKeyType(void* handle) {
    return CSTextFieldBridge::returnKeyType(handle);
}

void CSTextViewBridge::setReturnKeyType(void* handle, CSReturnKeyType returnKeyType) {
    CSTextFieldBridge::setReturnKeyType(handle, returnKeyType);
}

CSKeyboardType CSTextViewBridge::keyboardType(void* handle) {
    return CSTextFieldBridge::keyboardType(handle);
}

void CSTextViewBridge::setKeyboardType(void* handle, CSKeyboardType keyboardType) {
    CSTextFieldBridge::setKeyboardType(handle, keyboardType);
}

bool CSTextViewBridge::secureText(void* handle) {
    return CSTextFieldBridge::secureText(handle);
}

void CSTextViewBridge::setSecureText(void* handle, bool secureText) {
    CSTextFieldBridge::setSecureText(handle, secureText);
}

int CSTextViewBridge::maxLength(void* handle) {
    return CSTextFieldBridge::maxLength(handle);
}

void CSTextViewBridge::setMaxLength(void* handle, int maxLength) {
    CSTextFieldBridge::setMaxLength(handle, maxLength);
}

bool CSTextViewBridge::isFocused(void* handle) {
    return CSTextFieldBridge::isFocused(handle);
}

void CSTextViewBridge::setFocus(void* handle, bool focused) {
    CSTextFieldBridge::setFocus(handle, focused);
}

int CSTextViewBridge::maxLine(void* handle) {
    return 1;		//TODO:IMPL
}

void CSTextViewBridge::setMaxLine(void* handle, int maxLine) {
	//TODO:IMPL
}

#endif
