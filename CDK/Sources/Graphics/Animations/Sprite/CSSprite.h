//
//  CSSprite.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSSprite__
#define __CDK__CSSprite__

#include "CSAnimationSubstance.h"

#include "CSAnimationParam.h"

#include "CSArray.h"

class CSBuffer;

class CSSprite : public CSAnimationSubstance {
private:
    const CSArray<byte, 2>* _object;
    CSSpriteDelegate* _delegate;
    CSAnimationLoop _loop;
    float _delay;
    float _progress;
public:
    inline CSSprite() {
    
    }
    CSSprite(const CSSprite* sprite, bool capture);
    CSSprite(CSBuffer* buffer);
private:
    ~CSSprite();
public:
    static inline CSSprite* sprite() {
        return autorelease(new CSSprite());
    }
    static CSSprite* spriteWithSprite(const CSSprite* other, bool capture) {
        return autorelease(new CSSprite(other, capture));
    }
    static CSSprite* spriteWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSSprite(buffer));
    }
    
    inline void setObject(const CSArray<byte, 2>* object) {
        retain(_object, object);
    }
    inline const CSArray<byte, 2>* object() const {
        return _object;
    }
    inline void setLoop(CSAnimationLoop loop) {
        _loop = loop;
    }
    inline CSAnimationLoop loop() const {
        return _loop;
    }
    inline void setDelay(float delay) {
        _delay = delay;
    }
    inline float delay() const {
        return _delay;
    }
    inline void setDelegate(CSSpriteDelegate* delegate) {
        retain(_delegate, delegate);
    }
    inline CSAnimationSubstanceType type() const override {
        return CSAnimationSubstanceTypeSprite;
    }
    
    float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) override;
    void draw(CSGraphics* graphics, bool shadow) override;
    void preload(CSResourceDelegate* delegate = NULL) const override;
    
    inline CSAnimationSubstance* copy(bool capture) const override {
        return new CSSprite(this, capture);
    }
};

#endif /* defined(__CDK__CSSprite__) */
