//
//  CSWebView.cpp
//  CDK
//
//  Created by 김찬 on 13. 7. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSWebView.h"

#include "CSWebViewBridge.h"

#include "CSView.h"

CSWebView::CSWebView() {
    _handle = CSWebViewBridge::createHandle(this);
}

CSWebView::~CSWebView() {
    CSWebViewBridge::destroyHandle(_handle);
}

void CSWebView::onStateChanged() {
    switch (_state) {
        case CSLayerStateFocus:
            applyFrame();
            CSWebViewBridge::addToView(_handle);
            break;
        case CSLayerStateHidden:
        case CSLayerStateDetach:
            CSWebViewBridge::removeFromView(_handle);
            break;
        default:
            break;
    }
    CSLayer::onStateChanged();
}

void CSWebView::onFrameChanged() {
    applyFrame();
    
    CSLayer::onFrameChanged();
}

void CSWebView::onProjectionChanged() {
    applyFrame();
    
    CSLayer::onProjectionChanged();
}

void CSWebView::loadData(const void* data, uint length, const char* mimeType, const char* textEncodingName, const char* baseUrl) {
    CSWebViewBridge::loadData(_handle, data, length, mimeType, textEncodingName, baseUrl);
}

void CSWebView::loadData(const CSData* data, const char* mimeType, const char* textEncodingName, const char* baseUrl) {
    CSWebViewBridge::loadData(_handle, data->bytes(), data->length(), mimeType, textEncodingName, baseUrl);
}

void CSWebView::loadHTML(const char* html, const char* baseUrl) {
    CSWebViewBridge::loadHTML(_handle, html, baseUrl);
}

void CSWebView::loadRequest(const CSURLRequest* request) {
    CSWebViewBridge::loadRequest(_handle, request);
}

bool CSWebView::isLoading() const {
    return CSWebViewBridge::isLoading(_handle);
}

void CSWebView::stopLoading() const {
    CSWebViewBridge::stopLoading(_handle);
}

void CSWebView::reload() {
    CSWebViewBridge::reload(_handle);
}

bool CSWebView::canGoBack() const {
    return CSWebViewBridge::canGoBack(_handle);
}

bool CSWebView::canGoForward() const {
    return CSWebViewBridge::canGoForward(_handle);
}

void CSWebView::goBack() {
    CSWebViewBridge::goBack(_handle);
}

void CSWebView::goForward() {
    CSWebViewBridge::goForward(_handle);
}

bool CSWebView::scaleToFit() const {
    return CSWebViewBridge::scaleToFit(_handle);
}

void CSWebView::setScaleToFit(bool scale) {
    CSWebViewBridge::setScaleToFit(_handle, scale);
}

void CSWebView::onURLStartLoad() {
    OnURLStartLoad(this);
}
void CSWebView::onURLError() {
    OnURLError(this);
}
bool CSWebView::onURLLink(const char* url) {
    bool accept = true;
    OnURLLink(this, url, accept);
    return accept;
}
void CSWebView::onURLFinishLoad() {
    OnURLFinishLoad(this);
}

void CSWebView::applyFrame() {
    CSView* view = this->view();
    
    if (view) {
        CSRect frame = bounds();
        convertToViewSpace(&frame.origin);
        frame = view->convertToUIFrame(frame);
        CSWebViewBridge::setFrame(_handle, frame);
    }
}
