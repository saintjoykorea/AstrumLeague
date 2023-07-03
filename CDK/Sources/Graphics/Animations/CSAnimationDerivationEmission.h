//
//  CSAnimationDerivationEmission.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationDerivationEmission__
#define __CDK__CSAnimationDerivationEmission__

#include "CSAnimationDerivation.h"

class CSAnimationDerivationEmission : public CSAnimationDerivation {
private:
    float _delay;
    uint _capacity;
    bool _prewarm;
    uint _index;
    float _counter;
    CSArray<CSAnimation>* _instances;
public:
    CSAnimationDerivationEmission();
    CSAnimationDerivationEmission(CSBuffer* buffer);
    CSAnimationDerivationEmission(const CSAnimationDerivationEmission* other, bool capture);
private:
    ~CSAnimationDerivationEmission();
public:
    static inline CSAnimationDerivationEmission* derivation() {
        return autorelease(new CSAnimationDerivationEmission());
    }
    static inline CSAnimationDerivationEmission* derivationWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSAnimationDerivationEmission(buffer));
    }
    static inline CSAnimationDerivationEmission* derivationWithDerivation(const CSAnimationDerivationEmission* other, bool capture) {
        return autorelease(new CSAnimationDerivationEmission(other, capture));
    }
    
    inline void setCapacity(uint capacity) {
        _capacity = capacity;
    }
    inline uint capacity() const {
        return _capacity;
    }
    inline void setDelay(float delay) {
        _delay = delay;
    }
    inline float delay() const {
        return _delay;
    }
    inline void setPrewarm(bool prewarm) {
        _prewarm = prewarm;
    }
    inline bool prewarm() const {
        return _prewarm;
    }

    CSAnimationDerivationType type() const override {
        return CSAnimationDerivationTypeEmission;
    }

    float duration(const CSCamera* camera, CSAnimationDuration type, float duration) const override;
	inline bool isFinite(bool finite) const override {
		return finite;
	}
    void rewind() override;
    CSAnimationState update(float delta, const CSCamera* camera, bool alive) override;
	bool isVisible(uint visibleKey) const override;
    void draw(CSGraphics* graphics, uint visibleKey, bool shadow) override;
    
    inline CSAnimationDerivation* copy(bool capture) const override {
        return new CSAnimationDerivationEmission(this, capture);
    }
};

#endif /* defined(__CDK__CSAnimationDerivationEmission__) */
