//
//  CSViewBridge.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSViewBridge.h"

#import "CSIOSView.h"

uint CSViewBridge::bufferWidth(void* handle) {
    return ((CSIOSView*) handle).bounds.size.width;
}

uint CSViewBridge::bufferHeight(void* handle) {
    return ((CSIOSView*) handle).bounds.size.height;
}

void CSViewBridge::render(void* handle, bool refresh) {
    if (refresh) {
        [(CSIOSView*) handle render];
    }
}

CSRect CSViewBridge::frame(void* handle) {
	CGRect cgframe = [(CSIOSView*) handle frame];

	return CSRect(cgframe.origin.x, cgframe.origin.y, cgframe.size.width, cgframe.size.height);
}

CSRect CSViewBridge::bounds(void* handle) {
	CGRect cgbounds = [(CSIOSView*) handle bounds];

	return CSRect(cgbounds.origin.x, cgbounds.origin.y, cgbounds.size.width, cgbounds.size.height);
}


CSEdgeInsets CSViewBridge::edgeInsets(void* handle) {
    CSEdgeInsets rtn;

    if (@available(iOS 11.0, *)) {
        //UIEdgeInsets safeAreaInsets = [[[UIApplication sharedApplication] keyWindow] safeAreaInsets];
        UIEdgeInsets safeAreaInsets = [[[[UIApplication sharedApplication] windows] objectAtIndex:0] safeAreaInsets];
        //app safer 적용시 키윈도우가 변경된다. 처음 윈도우를 사용
        
        rtn.top = safeAreaInsets.top;
        rtn.left = safeAreaInsets.left;
        rtn.bottom = safeAreaInsets.bottom;
        rtn.right = safeAreaInsets.right;
    }
    else {
        rtn.top = 0;
        rtn.left = 0;
        rtn.bottom = 0;
        rtn.right = 0;
    }
    return rtn;
}

bool CSViewBridge::screenshot(void *handle, const char *path) {
    return ((CSIOSView*) handle).graphics->target()->screenshot(path);
}

#endif
