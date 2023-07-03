//
//  CSURLConnection.cpp
//  CDK
//
//  Created by chan on 13. 4. 22..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSURLConnection.h"

#include "CSURLConnectionBridge.h"

#include "CSData.h"

class CSAsyncURLConnection : public CSURLConnectionDelegate {
private:
    CSData* _data;
    CSURLConnection* _connection;
    CSAsynchronousURLRequestDelegate* _delegate;
    int _statusCode;
public:
    CSAsyncURLConnection(const CSURLRequest* request, CSAsynchronousURLRequestDelegate* delegate) {
        _data = NULL;
        _connection = new CSURLConnection(request);
        _delegate = retain(delegate);
        _connection->setDelegate(this);
        _connection->start();
    }
private:
    ~CSAsyncURLConnection() {
        _connection->setDelegate(NULL);
        _connection->release();
        CSObject::release(_delegate);
        if (_data) {
            _data->release();
        }
    }
    
public:
    void urlConnectionError(CSURLConnection* connection) {
        if (_delegate) {
            (*_delegate)(NULL, -1);
        }
        autorelease();
    }
    
    void urlConnectionReceiveResponse(CSURLConnection* connection, int statusCode) {
        _statusCode = statusCode;
    }
    
    void urlConnectionReceiveData(CSURLConnection* connection, const void* data, uint length) {
        if (!_data) {
            _data = new CSData();
        }
        _data->appendBytes(data, length);
    }
    
    void urlConnectionFinishLoading(CSURLConnection* connection) {
        if (_delegate) {
            (*_delegate)(_data, _statusCode);
        }
        autorelease();
    }
};

CSURLConnection::CSURLConnection(const CSURLRequest* request) : _request(retain(request)) {
    _handle = CSURLConnectionBridge::createHandle(this, request);
}

CSURLConnection::~CSURLConnection() {
    CSURLConnectionBridge::destroyHandle(_handle);

	_request->release();
}

CSData* CSURLConnection::sendSynchronousRequest(const CSURLRequest* request, int* statusCode) {
    return CSURLConnectionBridge::sendSynchronousRequest(request, statusCode);
}

void CSURLConnection::sendAsynchronousRequest(const CSURLRequest* request, CSAsynchronousURLRequestDelegate* delegate) {
    new CSAsyncURLConnection(request, delegate);
}

void CSURLConnection::start() {
    CSURLConnectionBridge::start(_handle);
}

void CSURLConnection::cancel() {
    CSURLConnectionBridge::cancel(_handle);
}

void CSURLConnection::onError() {
    if (_delegate) {
        _delegate->urlConnectionError(this);
    }
}

void CSURLConnection::onReceiveResponse(int statusCode) {
    if (_delegate) {
        _delegate->urlConnectionReceiveResponse(this, statusCode);
    }
}

void CSURLConnection::onReceiveData(const void* data, uint length) {
    if (_delegate) {
        _delegate->urlConnectionReceiveData(this, data, length);
    }
}

void CSURLConnection::onFinishLoading() {
    if (_delegate) {
        _delegate->urlConnectionFinishLoading(this);
    }
}
