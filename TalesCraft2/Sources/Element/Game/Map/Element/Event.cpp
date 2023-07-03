//
//  Event.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Event.h"

Event::Event(const AnimationIndex& animation, const CSColor& color, const CSVector2& point) : animation(new Animation(animation, SoundControlEffect, false)), color(color), point(point) {
    
}
Event::~Event() {
    animation->release();
}

