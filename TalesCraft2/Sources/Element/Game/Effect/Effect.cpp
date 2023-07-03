//
//  Effect.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "Effect.h"

#include "Map.h"

Effect::Effect(const Object* dest, int tag) : _dest(retain(dest)), _tag(tag) {
    GameLeak_addObject(this);
}

Effect::~Effect() {
    release(_dest);
    
    GameLeak_removeObject(this);
}

void Effect::dispose() {
    const Object* dest = NULL;
    synchronized(this, GameLockEffect) {
        if (!_dest) return;
        dest = _dest;
        _dest = NULL;
    }
    dest->release();
}

void Effect::setDestination(const Object* dest) {
    const Object* prev = NULL;
    synchronized(this, GameLockEffect) {
        if (!_dest || _dest == dest) return;
        prev = _dest;
        _dest = retain(dest);
    }
    dest->registerEffect(this);
    prev->release();
}
