//
//  CSAutoreleasePool.h
//  CDK
//
//  Created by Kim Chan on 2017. 1. 9..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSAutoreleasePool__
#define __CDK__CSAutoreleasePool__

#include "CSObject.h"

#include <vector>
#include <deque>

class CSAutoreleasePool {
private:
    std::deque<CSObject*> _autoreleases;
    std::vector<void*> _autofrees;
#ifdef CDK_IOS
    void* _iosAutoreleasePool;
#endif
#ifdef CDK_IMPL
public:
#else
private:
#endif
    CSAutoreleasePool(bool newThread);
    ~CSAutoreleasePool();
public:
#ifdef CDK_IMPL
    void add(CSObject* obj);
    void add(void* p);
#endif
    void drain();
};

#endif /* __CDK__CSAutoreleasePool__ */
