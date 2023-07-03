//
//  AI.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 25..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "AI.h"
/*
AI::Thread::Stack::Stack(int func, const Area* area) : func(func), ip(0), area(area), prev(NULL) {
    
}

AI::Thread::Thread(int threadId, int func, const Area* area) : threadId(threadId), current(new Stack(func, area)), waiting(fixed::Zero) {

}

AI::Thread::~Thread() {
    while (current) {
        Stack* prev = current->prev;
        delete current;
        current = prev;
    }
}

AI::AI(int level, bool fully, fixed lifetime) : level(level), fully(fully), lifetime(lifetime), emoticonResponseIndex(EmoticonIndex::None), threadId(0), threads(new CSArray<Thread*>()), areas(new CSArray<const Area*>()) {
    threads->addObject(new Thread(0, 0, NULL));
    memset(values, 0, sizeof(values));
    resourceWaiting.thread = NULL;
    resourceWaiting.remaining = fixed::Zero;
    resourceWaiting.priority = 0;
}

AI::~AI() {
    foreach(Thread*, thread, threads) {
        delete thread;
    }
    threads->release();
    areas->release();
}
*/

AI::AI(int level, bool fully, fixed lifetime) : level(level), fully(fully), waiting(0), lifetime(lifetime), emoticonResponseIndex(EmoticonIndex::None) {

}

AI::~AI() {

}