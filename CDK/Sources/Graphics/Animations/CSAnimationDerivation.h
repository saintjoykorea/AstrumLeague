//
//  CSAnimationDerivation.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationDerivation__
#define __CDK__CSAnimationDerivation__

#include "CSGraphics.h"

#include "CSRandom.h"

#include "CSAnimationParam.h"

class CSBuffer;
class CSAnimation;

enum CSAnimationDerivationType {
    CSAnimationDerivationTypeMulti = 1,
    CSAnimationDerivationTypeLinked,
    CSAnimationDerivationTypeEmission,
    CSAnimationDerivationTypeRandom
};

class CSAnimationDerivation : public CSObject {
protected:
    CSAnimation* _parent;
    CSArray<CSAnimation, 1, false>* _animations;
    
    CSAnimationDerivation();
    CSAnimationDerivation(CSBuffer* buffer);
    CSAnimationDerivation(const CSAnimationDerivation* other, bool capture);
    
    virtual ~CSAnimationDerivation();
public:
    static CSAnimationDerivation* createWithBuffer(CSBuffer* buffer);
    static inline CSAnimationDerivation* derivationWithBuffer(CSBuffer* buffer) {
        return autorelease(createWithBuffer(buffer));
    }
private:
    void attach(CSAnimation* parent);
public:

    inline CSAnimation* parent() {
        return _parent;
    }
    inline const CSAnimation* parent() const {
        return _parent;
    }
    
    virtual CSAnimationDerivationType type() const = 0;
    
    const CSArray<CSAnimation, 1, false>* animations();
    const CSArray<CSAnimation, 1, true>* animations() const;
    
    void addAnimation(CSAnimation* animation);
    void insertAnimation(uint index, CSAnimation* animation);
    void removeAnimation(CSAnimation* animation);
    void removeAnimationAtIndex(uint index);
    void removeAllAnimations();
    
    virtual float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const = 0;
	virtual bool isFinite(bool finite) const = 0;
private:
    virtual void rewind() = 0;
    virtual CSAnimationState update(float delta, const CSCamera* camera, bool alive) = 0;
	virtual bool isVisible(uint visibleKey) const = 0;
	virtual void draw(CSGraphics* graphics, uint visibleKey, bool shadow) = 0;
    
    void preload(CSResourceDelegate* delegate) const;
    
    virtual CSAnimationDerivation* copy(bool capture) const = 0;
    
    inline CSObject* copy() const override {
        return copy(true);
    }
    
    friend class CSAnimation;
};

#endif
