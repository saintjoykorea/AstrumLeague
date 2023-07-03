//
//  CSAnimationDerivationLinked.cpp
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationDerivationLinked.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

CSAnimationDerivationLinked::CSAnimationDerivationLinked(CSBuffer* buffer) : CSAnimationDerivation(buffer), _loopCount(buffer->readInt()) {
    _count = _animations->count() ? 1 : 0;
}

CSAnimationDerivationLinked::CSAnimationDerivationLinked(const CSAnimationDerivationLinked* other, bool capture) : CSAnimationDerivation(other, capture), _loopCount(other->_loopCount) {
    if (capture) {
        _current = other->_current;
        _count = other->_count;
    }
    else {
        _count = _animations->count() ? 1 : 0;
    }
}

float CSAnimationDerivationLinked::duration(const CSCamera* camera, CSAnimationDuration type, float duration) const {
    float rtn = 0.0f;
    
    if (type == CSAnimationDurationMin) {
        foreach(CSAnimation*, animation, _animations) {
            rtn += animation->duration(camera, CSAnimationDurationMin);
        }
    }
    else {
        float min = 0.0f;
        foreach(CSAnimation*, animation, _animations) {
            float max = min + animation->duration(camera, type);
            if (max > rtn) rtn = max;
            min += animation->duration(camera, CSAnimationDurationMin);
        }
    }
    if (_loopCount) {
        rtn *= _loopCount;
    }
    else {
        rtn += duration;
    }
    return rtn;
}

bool CSAnimationDerivationLinked::isFinite(bool finite) const {
	foreach(const CSAnimation*, animation, _animations) {
		if (!animation->isFinite(finite)) {
			return false;
		}
	}
	return _loopCount == 0 ? finite : true;
}

void CSAnimationDerivationLinked::rewindProgress() {
    foreach(CSAnimation*, animation, _animations) {
        animation->rewind();
    }
    _current = 0;
    _count = _animations->count() ? 1 : 0;
}

void CSAnimationDerivationLinked::rewind() {
    rewindProgress();
    _loopProgress = 0;
}

CSAnimationState CSAnimationDerivationLinked::update(float delta, const CSCamera* camera, bool alive) {
    _count = 0;
    
    for (int i = _current; i < _animations->count(); i++) {
        switch (_animations->objectAtIndex(i)->update(delta, camera, alive)) {
            case CSAnimationStateStopped:
                if (i == _current) {
                    _current++;
                }
                break;
            case CSAnimationStateFinishing:
                _count++;
                break;
            case CSAnimationStateAlive:
                _count++;
                return CSAnimationStateAlive;
        }
    }
    if (_current >= _animations->count()) {
        _loopProgress++;
        
        if (_loopCount == 0) {
            if (alive) {
                rewindProgress();
                return CSAnimationStateNone;
            }
        }
        else if (_loopProgress < _loopCount) {
            rewindProgress();
            return CSAnimationStateAlive;
        }
        return CSAnimationStateStopped;
    }
    return CSAnimationStateFinishing;
}

bool CSAnimationDerivationLinked::isVisible(uint visibleKey) const {
	uint max = CSMath::min(_current + _count, _animations->count());

	for (uint i = _current; i < max; i++) {
		if (_animations->objectAtIndex(i)->isVisible(visibleKey)) {
			return true;
		}
	}
	return false;
}

void CSAnimationDerivationLinked::draw(CSGraphics* graphics, uint visibleKey, bool shadow) {
    uint max = CSMath::min(_current + _count, _animations->count());
    
    for (uint i = _current; i < max; i++) {
        _animations->objectAtIndex(i)->draw(graphics, visibleKey, shadow);
    }
}
