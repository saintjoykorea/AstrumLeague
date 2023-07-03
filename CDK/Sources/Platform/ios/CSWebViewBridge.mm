//
//  CSWebViewBridge.mm
//  CDK
//
//  Created by 김찬 on 13. 7. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSWebViewBridge.h"

#import "CSView.h"

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

@interface CSWebViewHandle : NSObject<WKNavigationDelegate> {
    WKWebView* _component;
    CSWebView* _webView;
}
@property (nonatomic, readonly) WKWebView* component;
@property (nonatomic, readonly) CSWebView* webView;

-(id)initWithWebView:(CSWebView*)webView;

@end

@implementation CSWebViewHandle

@synthesize component = _component, webView = _webView;


-(id)initWithWebView:(CSWebView*)webView {
    if (self = [super init]) {
        _component = [[WKWebView alloc] init];
        _component.navigationDelegate = self;
        _component.backgroundColor=[UIColor clearColor];
        _component.opaque=NO;
        _webView = webView;
    }
    return self;
}

-(void)dealloc {
    [_component setNavigationDelegate:nil];
    [_component removeFromSuperview];
    [_component release];
    [super dealloc];
}

//WebKit 안에서 링크를 클릭시 사파리로 이동해 주는 함수
- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(nonnull WKNavigationAction *)navigationAction decisionHandler:(nonnull void (^)(WKNavigationActionPolicy))decisionHandler {
    /*
    if (navigationAction.navigationType == UIWebViewNavigationTypeLinkClicked) {
        NSURL *url = navigationAction.request.URL;
        if ([[UIApplication sharedApplication] canOpenURL:url]) {
            [[UIApplication sharedApplication] openURL:url];
            decisionHandler(WKNavigationActionPolicyCancel);
        } else {
            decisionHandler(WKNavigationActionPolicyAllow);
        }
    } else {
        decisionHandler(WKNavigationActionPolicyAllow);
    }
     */
    /*
    NSURLRequest *request = navigationAction.request;
    NSString *url = [[request URL]absoluteString];
    
    decisionHandler(WKNavigationActionPolicyAllow);
    */
    NSURLRequest *request = navigationAction.request;
    decisionHandler(WKNavigationActionPolicyAllow);
    _webView->onURLLink(request.URL.description.UTF8String);
}

-(void)webView:(WKWebView *)webView didCommitNavigation:(null_unspecified WKNavigation *) navigation {
    _webView->onURLStartLoad();
}

-(void)webView:(WKWebView *)webView didFinishNavigation:(null_unspecified WKNavigation *) navigation {
    _webView->onURLFinishLoad();
}

-(void)webView:(WKWebView *)webView didFailNavigation:(null_unspecified WKNavigation *) navigation withError:(NSError *) error {
    _webView->onURLError();
}

@end

void* CSWebViewBridge::createHandle(CSWebView* webView) {
    return [[CSWebViewHandle alloc] initWithWebView:webView];
}

void CSWebViewBridge::destroyHandle(void* handle) {
    [(CSWebViewHandle*)handle release];
}

void CSWebViewBridge::addToView(void* handle) {
    [(UIView*) ((CSWebViewHandle*) handle).webView->view()->handle() addSubview:[(CSWebViewHandle*) handle component]];
}

void CSWebViewBridge::removeFromView(void* handle) {
    [[(CSWebViewHandle*) handle component] removeFromSuperview];
}

void CSWebViewBridge::setFrame(void* handle, const CSRect& frame) {
    CGRect cgframe = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    
    [CATransaction begin];
    [CATransaction setValue:(id)kCFBooleanTrue forKey:kCATransactionDisableActions];
    [[(CSWebViewHandle*) handle component] setFrame:cgframe];
    [CATransaction commit];
}

void CSWebViewBridge::loadData(void* handle, const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl) {
    [[(CSWebViewHandle*)handle component] loadData:[NSData dataWithBytes:data length:length]
                                          MIMEType:[NSString stringWithUTF8String:mimeType]
                                  characterEncodingName:[NSString stringWithUTF8String:textEncodingName]
                                           baseURL:baseUrl ? [NSURL URLWithString:[NSString stringWithUTF8String:baseUrl]] : nil];
}

void CSWebViewBridge::loadHTML(void* handle, const char* html, const char* baseUrl) {
    [[(CSWebViewHandle*)handle component] loadHTMLString:[NSString stringWithUTF8String:html]
                                                 baseURL:baseUrl ? [NSURL URLWithString:[NSString stringWithUTF8String:baseUrl]] : nil];
}

void CSWebViewBridge::loadRequest(void* handle, const CSURLRequest* request) {
    [[(CSWebViewHandle*)handle component] loadRequest:(NSURLRequest*)request->handle()];
}

bool CSWebViewBridge::isLoading(void* handle) {
    return [[(CSWebViewHandle*)handle component] isLoading];
}

void CSWebViewBridge::stopLoading(void* handle) {
    [[(CSWebViewHandle*)handle component] stopLoading];
}

void CSWebViewBridge::reload(void* handle) {
    [[(CSWebViewHandle*)handle component] reload];
}

bool CSWebViewBridge::canGoBack(void* handle) {
    return [[(CSWebViewHandle*)handle component] canGoBack];
}

bool CSWebViewBridge::canGoForward(void* handle) {
    return [[(CSWebViewHandle*)handle component] canGoForward];
}

void CSWebViewBridge::goBack(void* handle) {
    [[(CSWebViewHandle*)handle component] goBack];
}

void CSWebViewBridge::goForward(void* handle) {
    [[(CSWebViewHandle*)handle component] goForward];
}

bool CSWebViewBridge::scaleToFit(void* handle) {
    return true;
}

void CSWebViewBridge::setScaleToFit(void* handle, bool scaleToFit) {

}

#endif

