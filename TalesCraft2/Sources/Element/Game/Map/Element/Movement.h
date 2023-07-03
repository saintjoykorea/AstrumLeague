//
//  Movement.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 19..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef Movement_h
#define Movement_h

#include "Object.h"

class Movement {
private:
    CSArray<const Object*>* _rounders;
public:
    sbyte dir;
public:
    Movement();
    ~Movement();
    
    inline Movement(const Movement&) {
        CSAssert(false, "invalid operation");
    }
    inline Movement& operator =(const Movement&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    void update();
    void hold(const Object* target);
    bool round(const Object* target) const;
};

#endif /* Movement_h */
