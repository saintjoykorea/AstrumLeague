#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSURLRequestBridge.h"

#include "CSURLRequestImpl.h"

void* CSURLRequestBridge::createHandle(const char* url, const char* method, bool usingCache, float timeoutInterval) {
	return new CSURLRequestHandle(url, method, usingCache, timeoutInterval);
}

void CSURLRequestBridge::destroyHandle(void* handle) {
	delete static_cast<CSURLRequestHandle*>(handle);
}

void CSURLRequestBridge::setURL(void* handle, const char* url) {
	CSURLRequestHandle* h = static_cast<CSURLRequestHandle*>(handle);
	free(h->url);
	h->url = _strdup(url);
}

const char* CSURLRequestBridge::URL(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->url;
}

void CSURLRequestBridge::setMethod(void* handle, const char* method) {
	CSURLRequestHandle* h = static_cast<CSURLRequestHandle*>(handle);
	free(h->method);
	h->method = _strdup(method);
}

const char* CSURLRequestBridge::method(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->method;
}

void CSURLRequestBridge::setUsingCache(void* handle, bool usingCache) {
	static_cast<CSURLRequestHandle*>(handle)->usingCache = usingCache;
}

bool CSURLRequestBridge::usingCache(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->usingCache;
}

void CSURLRequestBridge::setTimeoutInterval(void* handle, float timeoutInterval) {
	static_cast<CSURLRequestHandle*>(handle)->timeoutInterval = timeoutInterval;
}

float CSURLRequestBridge::timeoutInterval(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->timeoutInterval;
}

void CSURLRequestBridge::setHeaderField(void* handle, const char* name, const char* value) {
	static_cast<CSURLRequestHandle*>(handle)->headerFields->setObject(CSString::string(name), CSString::string(value));
}

const char* CSURLRequestBridge::headerField(void* handle, const char* name) {
	return *static_cast<CSURLRequestHandle*>(handle)->headerFields->objectForKey(CSString::string(name));
}

const CSDictionary<CSString, CSString>* CSURLRequestBridge::headerFields(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->headerFields;
}

void CSURLRequestBridge::setBody(void* handle, const void* data, uint length) {
	CSURLRequestHandle* h = static_cast<CSURLRequestHandle*>(handle);
	if (h->body) h->body->release();
	h->body = new CSData(data, length);
}

const CSData* CSURLRequestBridge::body(void* handle) {
	return static_cast<CSURLRequestHandle*>(handle)->body;
}

#endif
