//
//  CSIOSAutoreleasePool.h
//  CDK
//
//  Created by Kim Chan on 2016. 11. 22..
//  Copyright © 2016년 brgames. All rights reserved.
//
#if defined(CDK_IOS) && defined(CDK_IMPL)

#ifndef __CDK__CSIOSAutoreleasePool__
#define __CDK__CSIOSAutoreleasePool__

class CSIOSAutoreleasePool {
public:
    static void* create();
    static void destroy(void* pool);
    static void* drain(void* pool);
};

#endif

#endif
