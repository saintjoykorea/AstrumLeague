//
//  CSSprite.cpp
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSSprite.h"

#include "CSBuffer.h"

#include "CSAnimation.h"

CSSprite::CSSprite(const CSSprite* other, bool capture) :
    CSAnimationSubstance(other),
    _object(retain(other->_object)),
    _delegate(retain(other->_delegate)),
    _delay(other->_delay),
    _loop(other->_loop)
{
    if (capture) {
        _progress = other->_progress;
    }
}
CSSprite::CSSprite(CSBuffer* buffer) {
    _delay = buffer->readFloat();
    new (&_loop) CSAnimationLoop(buffer);
    _object = retain(buffer->readArray<byte, 2>());
}

CSSprite::~CSSprite() {
    release(_object);
    release(_delegate);
}

float CSSprite::duration(const CSCamera* camera, CSAnimationDuration type, float duration) const {
    return _loop.getDuration(_delay * _object->count());
}

bool CSSprite::isFinite(bool finite) const {
	if (_loop.count && _loop.finish) {
		finite = true;
	}
	return finite;
}

void CSSprite::rewind() {
    _progress = 0.0f;
}

CSAnimationState CSSprite::update(float delta, const CSCamera* camera, bool alive) {
    _progress += delta;
    
    if (_loop.count == 0) {
        return CSAnimationStateNone;
    }
    uint frame = (uint)(_progress / _delay);
    if (_object && frame < _object->count() * _loop.count) {
        return alive ? CSAnimationStateAlive : CSAnimationStateFinishing;
    }
    else {
        return _loop.finish ? CSAnimationStateStopped : CSAnimationStateNone;
    }
}

void CSSprite::draw(CSGraphics* graphics, bool shadow) {
    if (_object) {
        float progress = _loop.getProgress(_progress / (_delay * _object->count()), NULL, NULL);
        
        uint frame = CSMath::min((uint)(progress * _object->count()), _object->count() - 1);
        
        const CSArray<byte>* sprite = _object->objectAtIndex(frame);
        
        if (sprite) {
            if (_parent) {
                CSMatrix capture;
                if (_parent->capture(_progress, &graphics->camera(), capture)) {
                    graphics->pushTransform();
                    graphics->transform(capture);
                    graphics->drawSprite(sprite, shadow, _resourceDelegate, _delegate);
                    graphics->popTransform();
                }
            }
            else {
                graphics->drawSprite(sprite, shadow, _resourceDelegate, _delegate);
            }
        }
    }
}
void CSSprite::preload(CSResourceDelegate* delegate) const {
    if (!delegate) delegate = _resourceDelegate;
    if (_object && delegate) {
        foreach(const CSArray<byte>*, sprite, _object) {
            if (sprite) {
                CSGraphics::preloadSprite(sprite, delegate);
            }
        }
    }
}
