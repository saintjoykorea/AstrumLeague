//
//  CSURLRequestBridge.cpp
//  CDK
//
//  Created by chan on 13. 6. 13..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSURLRequestBridge.h"

#import "CSData.h"

#import <Foundation/Foundation.h>

void* CSURLRequestBridge::createHandle(const char* url, const char* method, bool usingCache, float timeoutInterval) {
    NSString* nsurlstr = [[NSString stringWithUTF8String:url] stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];

	NSURL* nsurl = [NSURL URLWithString:nsurlstr];

    if (nsurl) {
        NSURLRequestCachePolicy cachePolicy = usingCache ? NSURLRequestReturnCacheDataElseLoad : NSURLRequestReloadIgnoringCacheData;

        NSMutableURLRequest* handle = [[NSMutableURLRequest alloc] initWithURL:nsurl cachePolicy:cachePolicy timeoutInterval:timeoutInterval];
        
        [handle setHTTPMethod:[NSString stringWithUTF8String:method]];
        
        return handle;
    }
    else {
        return NULL;
    }
}

void CSURLRequestBridge::destroyHandle(void* handle) {
	[(NSMutableURLRequest*) handle release];
}

void CSURLRequestBridge::setURL(void* handle, const char* url) {
    NSString* nsurlstr = [[NSString stringWithUTF8String:url] stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];

	NSURL* nsurl = [NSURL URLWithString:nsurlstr];

	[(NSMutableURLRequest*) handle setURL:nsurl];
}

const char* CSURLRequestBridge::URL(void* handle) {
    return [(NSMutableURLRequest*)handle URL].description.UTF8String;
}

void CSURLRequestBridge::setMethod(void* handle, const char* method) {
	NSString* nsmethod = [NSString stringWithUTF8String:method];

	[(NSMutableURLRequest*) handle setHTTPMethod:nsmethod];
}

const char* CSURLRequestBridge::method(void* handle) {
	NSString* nsmethod = [(NSMutableURLRequest*) handle HTTPMethod];

	return nsmethod.UTF8String;
}

void CSURLRequestBridge::setUsingCache(void* handle, bool usingCache) {
	NSURLRequestCachePolicy cachePolicy = usingCache ? NSURLRequestReturnCacheDataElseLoad : NSURLRequestReloadIgnoringCacheData;

	[(NSMutableURLRequest*) handle setCachePolicy:cachePolicy];
}

bool CSURLRequestBridge::usingCache(void* handle) {
	return [(NSMutableURLRequest*) handle cachePolicy] == NSURLRequestReturnCacheDataElseLoad;
}

void CSURLRequestBridge::setTimeoutInterval(void* handle, float timeoutInterval) {
	[(NSMutableURLRequest*) handle setTimeoutInterval:timeoutInterval];
}

float CSURLRequestBridge::timeoutInterval(void* handle) {
	return [(NSMutableURLRequest*) handle timeoutInterval];
}

void CSURLRequestBridge::setHeaderField(void* handle, const char* name, const char* value) {
	NSString* nsname = [NSString stringWithUTF8String:name];
	NSString* nsvalue = [NSString stringWithUTF8String:value];
    [(NSMutableURLRequest*) handle setValue:nsvalue forHTTPHeaderField:nsname];
}

const char* CSURLRequestBridge::headerField(void* handle, const char* name) {
	NSString* nsname = [NSString stringWithUTF8String:name];
    
	NSString* nsvalue = [(NSMutableURLRequest*) handle valueForHTTPHeaderField:nsname];
	
	return nsvalue.UTF8String;
}

const CSDictionary<CSString, CSString>* CSURLRequestBridge::headerFields(void* handle) {
	NSDictionary* nsdic = [(NSMutableURLRequest*) handle allHTTPHeaderFields];

	CSDictionary<CSString, CSString>* dic = CSDictionary<CSString, CSString>::dictionary();

	for (NSString* key in nsdic.allKeys) {
		NSString* value = [nsdic objectForKey:key];

		dic->setObject(CSString::string(key.UTF8String), CSString::string(value.UTF8String));
	}
	return dic;
}

void CSURLRequestBridge::setBody(void* handle, const void* data, uint length) {
	NSData* nsdata = [NSData dataWithBytes:data length:length];

	[(NSMutableURLRequest*) handle setHTTPBody:nsdata];
}

const CSData* CSURLRequestBridge::body(void* handle) {
	NSData* nsdata = [(NSMutableURLRequest*) handle HTTPBody];

    return nsdata ? CSData::dataWithBytes(nsdata.bytes, nsdata.length) : NULL;
}

#endif
