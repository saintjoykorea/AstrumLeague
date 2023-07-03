//
//  CSAutoreleasePool.cpp
//  CDK
//
//  Created by Kim Chan on 2017. 1. 9..
//  Copyright © 2017년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAutoreleasePool.h"

#ifdef CDK_IOS
#include "CSIOSAutoreleasePool.h"
#endif

CSAutoreleasePool::CSAutoreleasePool(bool newThread) {
#ifdef CDK_IOS
    _iosAutoreleasePool = newThread ? CSIOSAutoreleasePool::create() : NULL;
#endif
}
CSAutoreleasePool::~CSAutoreleasePool() {
#ifdef CDK_IOS
    if (_iosAutoreleasePool) {
        CSIOSAutoreleasePool::destroy(_iosAutoreleasePool);
    }
#endif
}
void CSAutoreleasePool::add(CSObject* obj) {
    _autoreleases.push_back(obj);
}
void CSAutoreleasePool::add(void* p) {
    _autofrees.push_back(p);
}
void CSAutoreleasePool::drain() {
    while (!_autoreleases.empty()) {
        CSObject* obj = _autoreleases.front();
        _autoreleases.pop_front();
        obj->release();
    }
    for (uint i = 0; i < _autofrees.size(); i++) {
        free(_autofrees[i]);
    }
    _autofrees.clear();
#ifdef CDK_IOS
    if (_iosAutoreleasePool) {
        _iosAutoreleasePool = CSIOSAutoreleasePool::drain(_iosAutoreleasePool);
    }
#endif
}
