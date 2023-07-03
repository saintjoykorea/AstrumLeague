//
//  CSWebViewBridge.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 7..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSWebViewBridge.h"

void* CSWebViewBridge::createHandle(CSWebView* webView) {
	return NULL;		//TODO:IMPL
}
void CSWebViewBridge::destroyHandle(void* handle) {
	//TODO:IMPL
}
void CSWebViewBridge::addToView(void* handle) {
	//TODO:IMPL
}
void CSWebViewBridge::removeFromView(void* handle) {
	//TODO:IMPL
}
void CSWebViewBridge::setFrame(void* handle, const CSRect& frame) {
	//TODO:IMPL
}
void CSWebViewBridge::loadData(void* handle, const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl) {
	//TODO:IMPL
}

void CSWebViewBridge::loadHTML(void* handle, const char* html, const char* baseUrl) {
	//TODO:IMPL
}

void CSWebViewBridge::loadRequest(void* handle, const CSURLRequest* request) {
	//TODO:IMPL
}

bool CSWebViewBridge::isLoading(void* handle) {
    return false;	//TODO:IMPL
}
void CSWebViewBridge::stopLoading(void* handle) {
	//TODO:IMPL
}

void CSWebViewBridge::reload(void* handle) {
	//TODO:IMPL
}
bool CSWebViewBridge::canGoBack(void* handle) {
    return false;	//TODO:IMPL
}
bool CSWebViewBridge::canGoForward(void* handle) {
	return false;	//TODO:IMPL
}
void CSWebViewBridge::goBack(void* handle) {
	//TODO:IMPL
}
void CSWebViewBridge::goForward(void* handle) {
	//TODO:IMPL
}
bool CSWebViewBridge::scaleToFit(void* handle) {
	return false;	//TODO:IMPL
}
void CSWebViewBridge::setScaleToFit(void* handle, bool scaleToFit) {
	//TODO:IMPL
}

#endif
