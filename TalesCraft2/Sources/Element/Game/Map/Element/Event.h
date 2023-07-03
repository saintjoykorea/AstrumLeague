//
//  Event.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include "Asset.h"

struct Event {
    Animation* animation;
    CSColor color;
    CSVector2 point;
    
    Event(const AnimationIndex& animation, const CSColor& color, const CSVector2& point);
    ~Event();
};

#endif /* Event_h */
