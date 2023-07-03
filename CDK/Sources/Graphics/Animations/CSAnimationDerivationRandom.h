//
//  CSAnimationDerivationRandom.h
//  CDK
//
//  Created by 김찬 on 2015. 9. 17..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationDerivationRandom__
#define __CDK__CSAnimationDerivationRandom__

#include "CSAnimationDerivation.h"

class CSAnimationDerivationRandom : public CSAnimationDerivation {
private:
    CSAnimation* _selection;
    bool _loop;
public:
    inline CSAnimationDerivationRandom() : _loop(true) {
    
    }
    CSAnimationDerivationRandom(CSBuffer* buffer);
    CSAnimationDerivationRandom(const CSAnimationDerivationRandom* other, bool capture);
private:
    ~CSAnimationDerivationRandom();
public:
    static inline CSAnimationDerivationRandom* derivation() {
        return autorelease(new CSAnimationDerivationRandom());
    }
    static inline CSAnimationDerivationRandom* derivationWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSAnimationDerivationRandom(buffer));
    }
    static inline CSAnimationDerivationRandom* derivationWithDerivation(const CSAnimationDerivationRandom* other, bool capture) {
        return autorelease(new CSAnimationDerivationRandom(other, capture));
    }
    
    inline void setLoop(bool loop) {
        _loop = loop;
    }
    inline bool loop() const {
        return _loop;
    }
    
private:
    bool select();
public:
    CSAnimationDerivationType type() const override {
        return CSAnimationDerivationTypeRandom;
    }
    
    float duration(const CSCamera* camera, CSAnimationDuration type, float duration) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
    CSAnimationState update(float delta, const CSCamera* camera, bool alive) override;
	bool isVisible(uint visibleKey) const override;
    void draw(CSGraphics* graphics, uint visibleKey, bool shadow) override;
    
    inline CSAnimationDerivation* copy(bool capture) const override {
        return new CSAnimationDerivationRandom(this, capture);
    }
};

#endif /* defined(__CDK__CSAnimationDerivationRandom__) */
