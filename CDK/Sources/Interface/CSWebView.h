//
//  CSWebView.h
//  CDK
//
//  Created by 김찬 on 13. 7. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSWebView__
#define __CDK__CSWebView__

#include "CSLayer.h"

#include "CSData.h"

#include "CSURLRequest.h"

class CSWebView : public CSLayer {
public:
	CSHandler<CSWebView*> OnURLStartLoad;
	CSHandler<CSWebView*> OnURLError;
	CSHandler<CSWebView*, const char*, bool&> OnURLLink;
	CSHandler<CSWebView*> OnURLFinishLoad;
private:
    void* _handle;
public:
    CSWebView();
protected:
    virtual ~CSWebView();
public:
    static inline CSWebView* webView() {
        return autorelease(new CSWebView());
    }
    
    void loadData(const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl);
    void loadData(const CSData* data, const char* mimeType, const char* textEncodingName, const char* baseUrl);
    void loadHTML(const char* html, const char* baseUrl);
    void loadRequest(const CSURLRequest* request);
    bool isLoading() const;
    void stopLoading() const;
    void reload();
    
    bool canGoBack() const;
    bool canGoForward() const;
    void goBack();
    void goForward();
    
    bool scaleToFit() const;
    void setScaleToFit(bool scaleToFit);
    
    virtual void onURLStartLoad();
    virtual void onURLError();
    virtual bool onURLLink(const char* url);
    virtual void onURLFinishLoad();
    
private:
    void applyFrame();
    
protected:
    virtual void onStateChanged() override;
    virtual void onFrameChanged() override;
    virtual void onProjectionChanged() override;
};

#endif /* defined(__CDK__CSWebView__) */
