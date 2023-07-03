//
//  CSAnimationSubstance.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationSubstance__
#define __CDK__CSAnimationSubstance__

#include "CSGraphics.h"

#include "CSRandom.h"

#include "CSAnimationParam.h"

class CSAnimation;

class CSBuffer;

enum CSAnimationSubstanceType {
    CSAnimationSubstanceTypeSprite = 1,
    CSAnimationSubstanceTypeParticle,
    CSAnimationSubstanceTypeTrail,
    CSAnimationSubstanceTypeChain,
    CSAnimationSubstanceTypeModel
};

class CSAnimationSubstance : public CSObject {
protected:
    CSAnimation* _parent;
    CSResourceDelegate* _resourceDelegate;
    
    inline CSAnimationSubstance() {
    
    }
    inline CSAnimationSubstance(const CSAnimationSubstance* other) : _resourceDelegate(retain(other->_resourceDelegate)) {

    }
    inline virtual ~CSAnimationSubstance() {
        release(_resourceDelegate);
    }
public:
    static CSAnimationSubstance* createWithBuffer(CSBuffer* buffer);
    static inline CSAnimationSubstance* substanceWithBuffer(CSBuffer* buffer) {
        return autorelease(createWithBuffer(buffer));
    }
private:
    inline void attach(CSAnimation* parent) {
        CSAssert(!parent || !_parent, "invalid operation");
        _parent = parent;
    }
public:
    virtual CSAnimationSubstanceType type() const = 0;
    
    inline void setResourceDelegate(CSResourceDelegate* resourceDelegate) {
        retain(_resourceDelegate, resourceDelegate);
    }
    
    virtual float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const = 0;
	virtual bool isFinite(bool finite) const = 0;
    virtual void rewind() = 0;
    virtual CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) = 0;
    virtual void draw(CSGraphics* graphics, bool shadow) = 0;
    virtual void preload(CSResourceDelegate* delegate = NULL) const = 0;
    
    virtual CSAnimationSubstance* copy(bool capture) const = 0;
    
    inline CSObject* copy() const {
        return copy(true);
    }
    
    friend class CSAnimation;
};

#endif /* defined(__CDK__CSAnimationSubstance__) */
