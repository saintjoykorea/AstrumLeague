#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSTextFieldBridge.h"

#include "CSTextFieldImpl.h"

void* CSTextFieldBridge::createHandle(CSTextField* textField) {
	return new CSTextFieldHandle(textField);
}

void CSTextFieldBridge::destroyHandle(void* handle) {
	delete static_cast<CSTextFieldHandle*>(handle);
}

void CSTextFieldBridge::addToView(void* handle) {
	static_cast<CSTextFieldHandle*>(handle)->attach();
}

void CSTextFieldBridge::removeFromView(void* handle) {
	static_cast<CSTextFieldHandle*>(handle)->detach();
}

void CSTextFieldBridge::setFrame(void* handle, const CSRect& frame) {
	
}

const CSString* CSTextFieldBridge::text(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->text();
}

void CSTextFieldBridge::setText(void* handle, const char* text) {
	static_cast<CSTextFieldHandle*>(handle)->setText(text);
}

void CSTextFieldBridge::clearText(void* handle) {
	static_cast<CSTextFieldHandle*>(handle)->clearText();
}

CSColor CSTextFieldBridge::textColor(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->textColor();
}

void CSTextFieldBridge::setTextColor(void* handle, const CSColor& textColor) {
	static_cast<CSTextFieldHandle*>(handle)->setTextColor(textColor);
}

void CSTextFieldBridge::setFont(void* handle, const CSFont* font) {
	
}

const CSString* CSTextFieldBridge::placeholder(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->placeholder();
}

void CSTextFieldBridge::setPlaceholder(void* handle, const char* placeholder) {
	static_cast<CSTextFieldHandle*>(handle)->setPlaceholder(placeholder);
}

CSTextAlignment CSTextFieldBridge::textAlignment(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->textAlignment();
}

void CSTextFieldBridge::setTextAlignment(void* handle, CSTextAlignment textAlignment) {
	static_cast<CSTextFieldHandle*>(handle)->setTextAlignment(textAlignment);
}

CSReturnKeyType CSTextFieldBridge::returnKeyType(void* handle) {
	return CSReturnKeyDefault;
}

void CSTextFieldBridge::setReturnKeyType(void* handle, CSReturnKeyType returnKeyType) {
	
}

CSKeyboardType CSTextFieldBridge::keyboardType(void* handle) {
	return CSKeyboardDefault;
}

void CSTextFieldBridge::setKeyboardType(void* handle, CSKeyboardType keyboardType) {
	
}

bool CSTextFieldBridge::isSecureText(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->isSecureText();
}

void CSTextFieldBridge::setSecureText(void* handle, bool secureText) {
	static_cast<CSTextFieldHandle*>(handle)->setSecureText(secureText);
}

uint CSTextFieldBridge::maxLength(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->maxLength();
}

void CSTextFieldBridge::setMaxLength(void* handle, uint maxLength) {
	static_cast<CSTextFieldHandle*>(handle)->setMaxLength(maxLength);
}

uint CSTextFieldBridge::maxLine(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->maxLine();
}

void CSTextFieldBridge::setMaxLine(void* handle, uint maxLine) {
	static_cast<CSTextFieldHandle*>(handle)->setMaxLine(maxLine);
}

bool CSTextFieldBridge::isFocused(void* handle) {
	return static_cast<CSTextFieldHandle*>(handle)->isFocused();
}

void CSTextFieldBridge::setFocus(void* handle, bool focused) {
	static_cast<CSTextFieldHandle*>(handle)->setFocus(focused);
}

#endif
