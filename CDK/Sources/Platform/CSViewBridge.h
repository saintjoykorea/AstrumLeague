//
//  CSViewBridge.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSViewBridge__
#define __CDK__CSViewBridge__

#include "CSView.h"

class CSViewBridge {
public:
    static void render(void* handle, bool refresh);
    static uint bufferWidth(void* handle);
    static uint bufferHeight(void* handle);
    static CSRect frame(void* handle);
    static CSRect bounds(void* handle);
	static CSEdgeInsets edgeInsets(void* handle);
    static bool screenshot(void* handle, const char* path);
    static void setResolution(void* handle, CSResolution resolution);
    static CSResolution resolution(void* handle);
};

#endif /* defined(__CDK__CSViewBridge__) */

#endif
