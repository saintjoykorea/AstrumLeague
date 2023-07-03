//
//  CSURLRequest.cpp
//  CDK
//
//  Created by chan on 13. 6. 13..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSURLRequest.h"

#include "CSURLRequestBridge.h"

#include "CSData.h"

CSURLRequest::~CSURLRequest() {
    CSURLRequestBridge::destroyHandle(_handle);
}

CSURLRequest* CSURLRequest::create(const char* url, const char* method, bool usingCache, float timeoutInterval) {
    void* handle = CSURLRequestBridge::createHandle(url, method, usingCache, timeoutInterval);
    
    return handle ? new CSURLRequest(handle) : NULL;
}

void CSURLRequest::setURL(const char* url) {
	CSURLRequestBridge::setURL(_handle, url);
}

const char* CSURLRequest::URL() const {
    return CSURLRequestBridge::URL(_handle);
}

void CSURLRequest::setMethod(const char* method) {
	CSURLRequestBridge::setMethod(_handle, method);
}

const char* CSURLRequest::method() const {
	return CSURLRequestBridge::method(_handle);
}

void CSURLRequest::setUsingCache(bool usingCache) {
	CSURLRequestBridge::setUsingCache(_handle, usingCache);
}

bool CSURLRequest::usingCache() const {
    return CSURLRequestBridge::usingCache(_handle);
}

void CSURLRequest::setTimeoutInterval(float timeoutInterval) {
	CSURLRequestBridge::setTimeoutInterval(_handle, timeoutInterval);
}

float CSURLRequest::timeoutInterval() const {
    return CSURLRequestBridge::timeoutInterval(_handle);
}

void CSURLRequest::setHeaderField(const char* name, const char* value) {
    CSURLRequestBridge::setHeaderField(_handle, name, value);
}

const char* CSURLRequest::headerField(const char* name) const {
	return CSURLRequestBridge::headerField(_handle, name);
}

const CSDictionary<CSString, CSString>* CSURLRequest::headerFields() const {
	return CSURLRequestBridge::headerFields(_handle);
}

void CSURLRequest::setBody(const CSData* data) {
    CSURLRequestBridge::setBody(_handle, data->bytes(), data->length());
}

void CSURLRequest::setBody(const void* data, uint length) {
    CSURLRequestBridge::setBody(_handle, data, length);
}

const CSData* CSURLRequest::body() const {
	return CSURLRequestBridge::body(_handle);
}
