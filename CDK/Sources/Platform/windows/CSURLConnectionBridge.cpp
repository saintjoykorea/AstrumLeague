//
//  CSURLConnectionBridge.m
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSURLConnectionBridge.h"

#include "CSURLConnectionImpl.h"

void* CSURLConnectionBridge::createHandle(CSURLConnection* connection, const CSURLRequest* request) {
	CSURLRequestHandle* rh = static_cast<CSURLRequestHandle*>(request->handle());

	return new CSURLConnectionHandle(rh, connection);
}

void CSURLConnectionBridge::destroyHandle(void* handle) {
	CSURLConnectionHandle* h = static_cast<CSURLConnectionHandle*>(handle);

	CSURLConnectionHandleManager::sharedManager()->cancel(h);

	delete h;
}

CSData* CSURLConnectionBridge::sendSynchronousRequest(const CSURLRequest* request, int* statusCode) {
	CSURLRequestHandle* rh = static_cast<CSURLRequestHandle*>(request->handle());

	return CSURLConnectionHandleManager::sendSynchronizedRequest(rh, statusCode);
}

void CSURLConnectionBridge::start(void* handle) {
	CSURLConnectionHandle* h = static_cast<CSURLConnectionHandle*>(handle);

	CSURLConnectionHandleManager::sharedManager()->start(h);
}

void CSURLConnectionBridge::cancel(void* handle) {
	CSURLConnectionHandle* h = static_cast<CSURLConnectionHandle*>(handle);

	CSURLConnectionHandleManager::sharedManager()->cancel(h);
}

#endif
