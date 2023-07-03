//
//  CSAnimationDerivation.cpp
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationDerivation.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

#include "CSAnimationDerivationMulti.h"
#include "CSAnimationDerivationLinked.h"
#include "CSAnimationDerivationEmission.h"
#include "CSAnimationDerivationRandom.h"

CSAnimationDerivation::CSAnimationDerivation() : _animations(new CSArray<CSAnimation, 1, false>()) {

}

CSAnimationDerivation::CSAnimationDerivation(CSBuffer* buffer) : _animations(new CSArray<CSAnimation, 1, false>())  {
    uint count = buffer->readLength();
    
    for (uint i = 0; i < count; i++) {
        CSAnimation* animation = new CSAnimation(buffer);
        _animations->addObject(animation);
        animation->release();
    }
}

CSAnimationDerivation::CSAnimationDerivation(const CSAnimationDerivation* other, bool capture) : _animations(new CSArray<CSAnimation, 1, false>(other->_animations->capacity())) {
    foreach(CSAnimation*, animation, other->_animations) {
        _animations->addObject(CSAnimation::animationWithAnimation(animation, capture));
    }
}

CSAnimationDerivation::~CSAnimationDerivation() {
    foreach(CSAnimation*, animation, _animations) {
        animation->attach(NULL);
    }
    _animations->release();
}

CSAnimationDerivation* CSAnimationDerivation::createWithBuffer(CSBuffer* buffer) {
    switch (buffer->readByte()) {
        case CSAnimationDerivationTypeMulti:
            return new CSAnimationDerivationMulti(buffer);
        case CSAnimationDerivationTypeLinked:
            return new CSAnimationDerivationLinked(buffer);
        case CSAnimationDerivationTypeEmission:
            return new CSAnimationDerivationEmission(buffer);
        case CSAnimationDerivationTypeRandom:
            return new CSAnimationDerivationRandom(buffer);
    }
    return NULL;
}

void CSAnimationDerivation::attach(CSAnimation* parent) {
    CSAssert(!parent || !_parent, "invalid operation");
    _parent = parent;
    foreach(CSAnimation*, animation, _animations) {
        animation->attach(parent);
    }
}

const CSArray<CSAnimation, 1, false>* CSAnimationDerivation::animations() {
    return _animations;
}

const CSArray<CSAnimation, 1, true>* CSAnimationDerivation::animations() const {
    return _animations->asReadOnly();
}

void CSAnimationDerivation::addAnimation(CSAnimation* animation) {
    animation->attach(_parent);
    _animations->addObject(animation);
}

void CSAnimationDerivation::insertAnimation(uint index, CSAnimation* animation) {
    animation->attach(_parent);
    _animations->insertObject(index, animation);
}

void CSAnimationDerivation::removeAnimation(CSAnimation* animation) {
    int index = _animations->indexOfObjectIdenticalTo(animation);
    
    if (index != CSNotFound) {
        animation->attach(NULL);
        _animations->removeObjectAtIndex(index);
    }
}

void CSAnimationDerivation::removeAnimationAtIndex(uint index) {
    _animations->objectAtIndex(index)->attach(NULL);
    _animations->removeObjectAtIndex(index);
}

void CSAnimationDerivation::removeAllAnimations() {
    foreach(CSAnimation*, animation, _animations) {
        animation->attach(NULL);
    }
    _animations->removeAllObjects();
}

void CSAnimationDerivation::preload(CSResourceDelegate* delegate) const {
    foreach(CSAnimation*, animation, _animations) {
        animation->preload(delegate);
    }
}
