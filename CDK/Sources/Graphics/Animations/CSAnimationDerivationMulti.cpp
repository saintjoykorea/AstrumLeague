//
//  CSAnimationDerivationMulti.cpp
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationDerivationMulti.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

float CSAnimationDerivationMulti::duration(const CSCamera* camera, CSAnimationDuration type, float duration) const {
    float min = 0.0f;
    float closing = 0.0f;
    
    if (type == CSAnimationDurationMin) {
        foreach(CSAnimation*, animation, _animations) {
            float d = animation->duration(camera, type);
            if (animation->closing()) {
                if (closing < d) {
                    closing = d;
                }
            }
            else {
                if (min < d) {
                    min = d;
                }
            }
        }
        return min + closing;
    }
    else {
        float max = 0.0f;
        
        foreach(CSAnimation*, animation, _animations) {
            float maxd = animation->duration(camera, type);
            if (animation->closing()) {
                if (closing < maxd) {
                    closing = maxd;
                }
            }
            else {
                float mind = animation->duration(camera, CSAnimationDurationMin);
                
                if (min < mind) {
                    min = mind;
                }
                if (max < maxd) {
                    max = maxd;
                }
            }
        }
        return CSMath::max(min + closing, max);
    }
}

bool CSAnimationDerivationMulti::isFinite(bool finite) const {
	foreach(const CSAnimation*, animation, _animations) {
		if (!animation->isFinite(finite)) {
			return false;
		}
	}
	return true;
}

void CSAnimationDerivationMulti::rewind() {
    foreach(CSAnimation*, animation, _animations) {
        animation->rewind();
    }
}

CSAnimationState CSAnimationDerivationMulti::update(float delta, const CSCamera* camera, bool alive) {
    CSAnimationState rtn = CSAnimationStateStopped;
    foreach(CSAnimation*, animation, _animations) {
        switch (animation->update(delta, camera, alive)) {
            case CSAnimationStateAlive:
                rtn = CSAnimationStateAlive;
                break;
            case CSAnimationStateFinishing:
                if (rtn == CSAnimationStateStopped) {
                    rtn = CSAnimationStateFinishing;
                }
                break;
        }
    }
    return rtn;
}

bool CSAnimationDerivationMulti::isVisible(uint visibleKey) const {
	foreach(CSAnimation*, animation, _animations) {
		if (animation->isVisible(visibleKey)) {
			return true;
		}
	}
	return false;
}

void CSAnimationDerivationMulti::draw(CSGraphics* graphics, uint visibleKey, bool shadow) {
    foreach(CSAnimation*, animation, _animations) {
        animation->draw(graphics, visibleKey, shadow);
    }
}
