//
//  CSAnimationDerivationRandom.cpp
//  CDK
//
//  Created by 김찬 on 2015. 9. 17..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationDerivationRandom.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSAnimationDerivationRandom::CSAnimationDerivationRandom(CSBuffer* buffer) : CSAnimationDerivation(buffer), _loop(buffer->readBoolean()) {
    select();
}

CSAnimationDerivationRandom::CSAnimationDerivationRandom(const CSAnimationDerivationRandom* other, bool capture) : CSAnimationDerivation(other, capture), _loop(other->_loop) {
    if (capture) {
        if (other->_selection) {
            int index = other->_animations->indexOfObjectIdenticalTo(other->_selection);
            
            if (index >= 0) {
                _selection = _animations->objectAtIndex(index);
                _selection->retain();
            }
        }
    }
    else {
        select();
    }
}

CSAnimationDerivationRandom::~CSAnimationDerivationRandom() {
    release(_selection);
}

bool CSAnimationDerivationRandom::select() {
    int totalWeight = 0;
    foreach(CSAnimation*, animation, _animations) {
        totalWeight += animation->weight();
    }
    if (totalWeight) {
        int randomWeight = randInt(0, totalWeight);
        totalWeight = 0;
        foreach(CSAnimation*, animation, _animations) {
            totalWeight += animation->weight();
            if (randomWeight <= totalWeight) {
                retain(_selection, animation);
                break;
            }
        }
        return true;
    }
    return false;
}

float CSAnimationDerivationRandom::duration(const CSCamera *camera, CSAnimationDuration type, float duration) const {
    if (_loop) {
        return 0.0f;
    }
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

bool CSAnimationDerivationRandom::isFinite(bool finite) const {
	foreach(const CSAnimation*, animation, _animations) {
		if (!animation->isFinite(finite)) {
			return false;
		}
	}
	return _loop ? finite : true;
}

void CSAnimationDerivationRandom::rewind() {
    foreach(CSAnimation*, animation, _animations) {
        animation->rewind();
    }
    select();
}

CSAnimationState CSAnimationDerivationRandom::update(float delta, const CSCamera* camera, bool alive) {
    if (_selection) {
        CSAnimationState state = _selection->update(delta, camera, alive);
        
        if (state == CSAnimationStateStopped) {
            if (alive && _loop && select()) {
                _selection->rewind();
                return CSAnimationStateNone;
            }
            release(_selection);
            return CSAnimationStateStopped;
        }
        else {
            return _loop ? CSAnimationStateNone : state;
        }
    }
    else {
        return CSAnimationStateStopped;
    }
}

bool CSAnimationDerivationRandom::isVisible(uint visibleKey) const {
	return _selection && _selection->isVisible(visibleKey);
}

void CSAnimationDerivationRandom::draw(CSGraphics* graphics, uint visibleKey, bool shadow) {
    if (_selection) {
        _selection->draw(graphics, visibleKey, shadow);
    }
}
