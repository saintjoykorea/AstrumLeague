//
//  Animations.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 13..
//  Copyright © 2019년 brgames. All rights reserved.
//
#include "Animations.h"

Animations::Animations() : _entries(new CSDictionary<AnimationEntryIndex, Animation>()) {
    
}

Animations::~Animations() {
    _entries->release();
}

Animation* Animations::get(const AnimationEntryIndex& index) {
    Animation* animation = _entries->objectForKey(index);
    if (!animation) {
        animation = new Animation(index.group());
        _entries->setObject(index, animation);
        animation->release();
    }
    return animation;
}

bool Animations::update(float delta, const CSCamera* camera) {
    bool refresh = false;
    for (CSDictionary<AnimationEntryIndex, Animation>::Iterator i = _entries->iterator(); i.isValid(); i.next()) {
        if (i.object()->update(delta, camera)) {
            refresh = true;
        }
        else {
            i.remove();
        }
    }
    return refresh;
}
