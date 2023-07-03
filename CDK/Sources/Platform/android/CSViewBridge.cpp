//
//  CSViewBridge.cpp
//  CDKDemo
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSViewBridge.h"

#include "CSAndroidView.h"

uint CSViewBridge::bufferWidth(void* handle) {
	return ((CSAndroidView*)handle)->bufferWidth();
}

uint CSViewBridge::bufferHeight(void* handle) {
	return ((CSAndroidView*)handle)->bufferHeight();
}

void CSViewBridge::render(void* handle, bool refresh) {
	((CSAndroidView*)handle)->render(refresh);
}

CSRect CSViewBridge::frame(void* handle) {
	return CSRect(0, 0, ((CSAndroidView*)handle)->width(), ((CSAndroidView*)handle)->height());
}

CSRect CSViewBridge::bounds(void* handle) {
	return CSRect(0, 0, ((CSAndroidView*)handle)->width(), ((CSAndroidView*)handle)->height());
}

CSEdgeInsets CSViewBridge::edgeInsets(void* handle) {
	CSEdgeInsets rtn;
	rtn.left = rtn.right = ((CSAndroidView*)handle)->horizontalEdge();
	rtn.top = rtn.bottom = ((CSAndroidView*)handle)->verticalEdge();
	return rtn;
}

bool CSViewBridge::screenshot(void* handle, const char* path) {
    return ((CSAndroidView*)handle)->graphics()->target()->screenshot(path);
}

#endif
