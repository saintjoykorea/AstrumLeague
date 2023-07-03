//
//  CSAnimationLoop.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 10. 5..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSAnimationLoop_h
#define CSAnimationLoop_h

#include "CSTypes.h"

class CSBuffer;

struct CSAnimationLoop {
public:
    ushort count;
    bool finish;
    bool roundTrip;
public:
    CSAnimationLoop();
    CSAnimationLoop(int count, bool finish, bool roundTrip);
    CSAnimationLoop(CSBuffer* buffer);
    
    float getProgress(float progress, int* random0, int* random1) const;
    float getDuration(float duration) const;
    void getState(float progress, bool& remaining, bool& alive) const;
};

#endif /* CSAnimationLoop_h */
