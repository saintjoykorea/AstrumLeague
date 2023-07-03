//
//  CSURLConnection.h
//  CDK
//
//  Created by chan on 13. 4. 22..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSURLConnection__
#define __CDK__CSURLConnection__

#include "CSURLRequest.h"

#include "CSDelegate.h"

class CSURLConnection;

class CSURLConnectionDelegate : public CSObject {
public:
    virtual void urlConnectionError(CSURLConnection* connection) = 0;
    virtual void urlConnectionReceiveResponse(CSURLConnection* connection, int statusCode) = 0;
    virtual void urlConnectionReceiveData(CSURLConnection* connection, const void* data, uint length) = 0;
    virtual void urlConnectionFinishLoading(CSURLConnection* connection) = 0;
};

typedef CSDelegate<void, CSData*, int> CSAsynchronousURLRequestDelegate;

typedef CSDelegate0<void, CSData*, int> CSAsynchronousURLRequestDelegate0;
template <typename Param0>
using CSAsynchronousURLRequestDelegate1 = CSDelegate1<void, Param0, CSData*, int>;
template <typename Param0, typename Param1>
using CSAsynchronousURLRequestDelegate2 = CSDelegate2<void, Param0, Param1, CSData*, int>;
template <typename Param0, typename Param1, typename Param2>
using CSAsynchronousURLRequestDelegate3 = CSDelegate3<void, Param0, Param1, Param2, CSData*, int>;

class CSURLConnection : public CSObject {
private:
	const CSURLRequest* _request;
    void* _handle;
    CSURLConnectionDelegate* _delegate;
public:
    CSURLConnection(const CSURLRequest* request);
private:
    ~CSURLConnection();
    
public:
    static inline CSURLConnection* connection(const CSURLRequest* request) {
        return autorelease(new CSURLConnection(request));
    }
    
    static CSData* sendSynchronousRequest(const CSURLRequest* request, int* statusCode = NULL);
    static void sendAsynchronousRequest(const CSURLRequest* request, CSAsynchronousURLRequestDelegate* delegate);
    
    void setDelegate(CSURLConnectionDelegate* delegate) {
        _delegate = delegate;
    }
    void start();
    void cancel();
    
    void onError();
    void onReceiveResponse(int statusCode);
    void onReceiveData(const void* data, uint length);
    void onFinishLoading();
};

#endif /* defined(__CDK__CSURLConnection__) */
