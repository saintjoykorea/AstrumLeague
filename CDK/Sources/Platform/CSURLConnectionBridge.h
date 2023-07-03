//
//  CSURLConnectionBridge.h
//  CDK
//
//  Created by chan on 13. 4. 22..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSURLConnectionBridge__
#define __CDK__CSURLConnectionBridge__

#include "CSURLConnection.h"

class CSURLConnectionBridge {
public:
    static void* createHandle(CSURLConnection* connection, const CSURLRequest* request);
    static void destroyHandle(void* handle);
    static CSData* sendSynchronousRequest(const CSURLRequest* request, int* statusCode);
    static void start(void* handle);
    static void cancel(void* handle);
};

#endif

#endif
