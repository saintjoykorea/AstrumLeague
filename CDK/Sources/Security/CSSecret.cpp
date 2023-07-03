//
//  CSSecret.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 9. 8..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSSecret.h"

#include "CSLog.h"
#include "CSRandom.h"
#include "CSThread.h"

CSSecret CSSecret::_sharedSecret;

CSSecret::CSSecret() {
    _values = new CSArray<CSSecretValueBase*>();
    for (int i = 0; i < CSSecretCapacity; i++) {
        table[i] = randLong();
    }
    valueMask = randLong();
    checkMask = randLong();
}

CSSecret::~CSSecret() {
    _values->release();
}

void CSSecret::addValue(CSSecretValueBase* value) {
#ifdef CS_SECRET_THREAD_SAFE
    _lock.assertOnActive();
#endif
    _values->addObject(value);
}

void CSSecret::removeValue(CSSecretValueBase* value) {
#ifdef CS_SECRET_THREAD_SAFE
    _lock.assertOnActive();
#endif
    _values->removeObjectIdenticalTo(value);
}

void CSSecret::clear() {
    lock();
    _values->removeAllObjects();
    unlock();
}

void CSSecret::reset(bool all) {
    lock();
    
    uint64 newValueMask = randLong();
    uint64 newCheckMask = randLong();
    
    if (all) {
        uint64 newTable[CSSecretCapacity];
        for (int i = 0; i < CSSecretCapacity; i++) {
            newTable[i] = randLong();
        }
        foreach(CSSecretValueBase*, value, _values) {
            value->reset(newTable, newValueMask, newCheckMask);
        }
        memcpy(table, newTable, CSSecretCapacity * sizeof(uint64));
    }
    else {
        foreach(CSSecretValueBase*, value, _values) {
            value->reset(NULL, newValueMask, newCheckMask);
        }
    }
    valueMask = newValueMask;
    checkMask = newCheckMask;
    
    unlock();
}

void CSSecret::occurError() {
    OnError();
}
