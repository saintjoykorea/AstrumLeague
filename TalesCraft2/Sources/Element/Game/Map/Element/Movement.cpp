//
//  Movement.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 19..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Movement.h"

#include "Object.h"

Movement::Movement() : _rounders(NULL), dir(0) {
    
}
Movement::~Movement() {
    CSObject::release(_rounders);
}
void Movement::update() {
    CSObject::release(_rounders);
}
void Movement::hold(const Object* target) {
    if (!_rounders) {
        _rounders = new CSArray<const Object*>(3);
    }
    _rounders->addObject(target);
}
bool Movement::round(const Object* target) const {
    return !_rounders || !_rounders->containsObjectIdenticalTo(target);
}

