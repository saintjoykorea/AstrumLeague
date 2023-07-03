//
//  CSAnimationDerivationEmission.cpp
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationDerivationEmission.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

CSAnimationDerivationEmission::CSAnimationDerivationEmission() : _instances(new CSArray<CSAnimation>()) {

}

CSAnimationDerivationEmission::CSAnimationDerivationEmission(CSBuffer* buffer) :
    CSAnimationDerivation(buffer),
    _delay(buffer->readFloat()),
    _capacity(buffer->readInt()),
    _prewarm(buffer->readBoolean()),
    _instances(new CSArray<CSAnimation>())
{
    if (_prewarm) {
        _counter = _capacity * _delay;
    }
}

CSAnimationDerivationEmission::CSAnimationDerivationEmission(const CSAnimationDerivationEmission* other, bool capture) :
    CSAnimationDerivation(other, capture),
    _delay(other->_delay),
    _capacity(other->_capacity),
    _prewarm(other->_prewarm),
    _instances(new CSArray<CSAnimation>(other->_instances->capacity()))
{
    if (capture) {
        _index = other->_index;
        _counter = other->_counter;
        
        foreach(CSAnimation*, instance, other->_instances) {
            _instances->addObject(CSAnimation::animationWithAnimation(instance, false));
        }
    }
    else {
        if (_prewarm) {
            _counter = _capacity * _delay;
        }
    }
}

CSAnimationDerivationEmission::~CSAnimationDerivationEmission() {
    _instances->release();
}

float CSAnimationDerivationEmission::duration(const CSCamera* camera, CSAnimationDuration type, float duration) const {
    float rtn = 0.0f;
    
    foreach(CSAnimation*, animation, _animations) {
        float d = animation->duration(camera, type);
        if (rtn < d) {
            rtn = d;
        }
    }
    rtn += duration;
    return rtn;
}

void CSAnimationDerivationEmission::rewind() {
    _instances->removeAllObjects();
    _index = 0;
    _counter = _prewarm ? _capacity * _delay : 0.0f;
}

CSAnimationState CSAnimationDerivationEmission::update(float delta, const CSCamera* camera, bool alive) {
    uint i = 0;
    while (i < _instances->count()) {
        CSAnimation* instance = _instances->objectAtIndex(i);
        
        if (instance->update(delta, camera, alive) != CSAnimationStateStopped) {
            i++;
        }
        else {
            _instances->removeObjectAtIndex(i);
        }
    }
    
    if (_animations->count() && alive && _delay) {
        _counter += delta;
        
        while (_counter >= _delay) {
            _counter -= _delay;
            
            if (_capacity == 0 || _instances->count() < _capacity) {
                if (_index >= _animations->count()) {
                    _index = 0;
                }
                const CSAnimation* animation = _animations->objectAtIndex(_index);
                
                CSMatrix capture;
                
                if (_parent->capture(_parent->progress() - _counter, camera, capture)) {
                    CSAnimation* instance = new CSAnimation(animation, true);
                    instance->setTransform(&capture);
                    _instances->addObject(instance);
                    instance->release();
                    instance->rewind();
                    instance->update(_counter, camera, true);
                }
                _index++;
            }
        }
    }
    if (alive) return CSAnimationStateAlive;
    if (_instances->count()) return CSAnimationStateFinishing;
    return CSAnimationStateStopped;
}

bool CSAnimationDerivationEmission::isVisible(uint visibleKey) const {
	foreach(CSAnimation*, animation, _animations) {
		if (animation->isVisible(visibleKey)) {
			return true;
		}
	}
	return false;
}

void CSAnimationDerivationEmission::draw(CSGraphics* graphics, uint visibleKey, bool shadow) {
    foreach (CSAnimation*, instance, _instances) {
        instance->draw(graphics, visibleKey, shadow);
    }
}
