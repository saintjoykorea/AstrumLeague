//
//  CSObject.cpp
//  CDK
//
//  Created by 김찬 on 12. 7. 31..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSObject.h"

#include "CSString.h"

#include "CSThread.h"

CSObject::CSObject() : _retainCount(1) {

}

CSObject::~CSObject() {
    if (_lock) {
        delete _lock;
    }
}

void CSObject::release() {
    if (_retainCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete this;
    }
}

void CSObject::retain() {
    _retainCount.fetch_add(1, std::memory_order_relaxed);
}

void CSObject::autorelease() {
    CSThread::currentThread()->autoreleasePool()->add(this);
}

uint CSObject::retainCount() const {
    return _retainCount.load(std::memory_order_acquire);
}

void* CSObject::operator new(size_t size) {
    void* ptr = ::operator new(size);
    memset(ptr, 0, size);
    return ptr;
}

CSObject* CSObject::copy() const {
    return const_cast<CSObject*>(retain(this));
}

void CSObject::useLock() const {
    if (!_lock) {
        static CSLock atomicLock;
        
        atomicLock.lock();
        if (!_lock) {
            _lock = new CSLock(true);
        }
        atomicLock.unlock();
    }
}
