//
//  CSAnimationLoop.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 10. 5..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationLoop.h"

#include "CSBuffer.h"

CSAnimationLoop::CSAnimationLoop() : count(0), finish(false), roundTrip(false) {
    
}

CSAnimationLoop::CSAnimationLoop(int count, bool finish, bool roundTrip) : count(count), finish(finish), roundTrip(roundTrip) {
    
}

CSAnimationLoop::CSAnimationLoop(CSBuffer* buffer) : count(buffer->readShort()), finish(buffer->readBoolean()), roundTrip(buffer->readBoolean()) {
    
}

float CSAnimationLoop::getProgress(float progress, int *random0, int *random1) const {
    int seq = progress;
    
    if (count && seq >= count) {
        seq = count - 1;
        progress = roundTrip && (count & 1) == 0 ? 0 : 1;
    }
    else {
        progress -= seq;
        
        if (roundTrip && (seq & 1) == 1) {
            progress = 1 - progress;
        }
    }
    if (random0) *random0 = roundTrip ? (seq + 1) & 0x7ffffffe : seq;
    if (random1) *random1 = roundTrip ? (seq ^ 1) | 1 : seq;

    return progress;
}

float CSAnimationLoop::getDuration(float duration) const {
    return duration * count;
}

void CSAnimationLoop::getState(float progress, bool &remaining, bool &alive) const {
    int seq = progress;
    
    if (seq < count) {
        remaining = true;
    }
    else if (count && finish) {
        alive = false;
    }
}
