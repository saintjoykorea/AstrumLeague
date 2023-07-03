//
//  CSWebViewBridge.h
//  CDK
//
//  Created by ChangSun on 19. 7. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSVideoViewBridge__
#define __CDK__CSVideoViewBridge__

#include "CSVideoView.h"

class CSVideoViewBridge {
public:
    static void* createHandle(CSVideoView* videoView);
    static void destroyHandle(void* handle);
    static void addToView(void* handle);
    static void removeFromView(void* handle);
    static void setFrame(void* handle, const CSRect& frame);
    static void playStart(void* handle, const char* path);
};

#endif

#endif
