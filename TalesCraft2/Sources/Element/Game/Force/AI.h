//
//  AI.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 25..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef AI_h
#define AI_h

#include "AssetIndex.h"

#include "Area.h"

class Force;
class Map;

struct AI {
private:
    byte level;
    bool fully;
    //===========================
    ushort waiting;
    //===========================
    fixed lifetime;
    EmoticonIndex emoticonResponseIndex;
    /*

    struct Thread {
        struct Stack {
            ushort func;
            ushort ip;
            const Area* area;
            Stack* prev;
            
            Stack(int func, const Area* area);
        };
        int threadId;
        Stack* current;
        fixed waiting;
        
        Thread(int threadId, int func, const Area* area);
        ~Thread();
    };
    int threadId;
    CSArray<Thread*>* threads;
    CSArray<const Area*>* areas;
    fixed values[1000];
    struct {
        const Thread* thread;
        fixed remaining;
        byte priority;
    } resourceWaiting;
    */
    AI(int level, bool fully, fixed lifetime);
    ~AI();
    /*
    inline fixed& value(int index) {
        CSAssert(index >= 0 && index < countof(values), "invalid operation");
        return values[index];
    }
    */
    friend class Force;
    friend class Map;
};

#endif /* AI_h */
