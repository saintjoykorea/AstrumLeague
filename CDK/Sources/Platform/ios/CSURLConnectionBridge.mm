//
//  CSURLConnectionBridge.m
//  CDK
//
//  Created by chan on 13. 4. 22..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSURLConnectionBridge.h"

#import "CSData.h"

#import <UIKit/UIKit.h>

@interface NSURLConnectionHandle : NSObject<NSURLConnectionDelegate> {
	NSURLConnection* _component;
	CSURLConnection* _connection;
    int _contentLength;
    int _receiveLength;
}

-(id)initWithConnection:(CSURLConnection*)connection request:(NSURLRequest*)request;

@end

@implementation NSURLConnectionHandle

-(id)initWithConnection:(CSURLConnection*)connection request:(NSURLRequest*)request {
	if (self = [super init]) {
		_component = [[NSURLConnection alloc] initWithRequest:request delegate:self startImmediately:NO];
		_connection = connection;
	}
	return self;
}

-(void)dealloc {
    [_component cancel];
	[_component release];
	[super dealloc];
}

-(void)start {
	[_component start];
}

-(void)cancel {
	[_component cancel];
}

-(void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response {
	int statusCode = [(NSHTTPURLResponse*) response statusCode];

	_connection->onReceiveResponse(statusCode);
    
    _contentLength = [[((NSHTTPURLResponse*)response).allHeaderFields objectForKey:@"Content-Length"] intValue];
}

-(void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data {
	_connection->onReceiveData(data.bytes, data.length);
    _receiveLength += data.length;
}

-(void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error {
	_connection->onError();
}

-(void)connectionDidFinishLoading:(NSURLConnection*)connection {
    if (!_contentLength || _receiveLength == _contentLength) {
        _connection->onFinishLoading();
    }
    else {
        _connection->onError();
    }
}

- (void)connection:(NSURLConnection *)connection willSendRequestForAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge {
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]) {
        SecTrustRef trust = challenge.protectionSpace.serverTrust;
        NSURLCredential *cred;
        cred = [NSURLCredential credentialForTrust:trust];
        [challenge.sender useCredential:cred forAuthenticationChallenge:challenge];
    }
}

@end

void* CSURLConnectionBridge::createHandle(CSURLConnection* connection, const CSURLRequest* request) {
	return [[NSURLConnectionHandle alloc] initWithConnection:connection request:(NSURLRequest*)request->handle()];
}

void CSURLConnectionBridge::destroyHandle(void* handle) {
	[(NSURLConnectionHandle*) handle release];
}

CSData* CSURLConnectionBridge::sendSynchronousRequest(const CSURLRequest* request, int* statusCode) {
	NSURLResponse* response;

	NSError* error;

	NSData* data = [NSURLConnection sendSynchronousRequest:(NSURLRequest*)request->handle() returningResponse:&response error:&error];

	int receiveStatusCode = [(NSHTTPURLResponse*) response statusCode];

    int contentLength = [[((NSHTTPURLResponse*) response).allHeaderFields objectForKey:@"Content-Length"] intValue];
    
	if (statusCode) {
		*statusCode = receiveStatusCode;
	}

	return data && receiveStatusCode == 200 && (!contentLength || contentLength == data.length) ? CSData::dataWithBytes(data.bytes, data.length) : NULL;
}

void CSURLConnectionBridge::start(void* handle) {
	[(NSURLConnectionHandle*) handle start];
}

void CSURLConnectionBridge::cancel(void* handle) {
	[(NSURLConnectionHandle*) handle cancel];
}

#endif
