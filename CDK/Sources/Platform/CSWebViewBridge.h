//
//  CSWebViewBridge.h
//  CDK
//
//  Created by 김찬 on 13. 7. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSWebViewBridge__
#define __CDK__CSWebViewBridge__

#include "CSWebView.h"

class CSWebViewBridge {
public:
    static void* createHandle(CSWebView* webView);
    static void destroyHandle(void* handle);
    static void addToView(void* handle);
    static void removeFromView(void* handle);
    static void setFrame(void* handle, const CSRect& frame);
    static void loadData(void* handle, const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl);
    static void loadHTML(void* handle, const char* html, const char* baseUrl);
    static void loadRequest(void* handle, const CSURLRequest* request);
    static bool isLoading(void* handle);
    static void stopLoading(void* handle);
    static void reload(void* handle);
    static bool canGoBack(void* handle);
    static bool canGoForward(void* handle);
    static void goBack(void* handle);
    static void goForward(void* handle);
    static bool scaleToFit(void* handle);
    static void setScaleToFit(void* handle, bool scaleToFit);
};

#endif

#endif
