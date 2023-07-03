//
//  CSAnimationDerivationMulti.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationDerivationMulti__
#define __CDK__CSAnimationDerivationMulti__

#include "CSAnimationDerivation.h"

class CSAnimationDerivationMulti : public CSAnimationDerivation {
public:
    inline CSAnimationDerivationMulti() {
    
    }
    inline CSAnimationDerivationMulti(CSBuffer* buffer) : CSAnimationDerivation(buffer) {
    
    }
    inline CSAnimationDerivationMulti(const CSAnimationDerivationMulti* other, bool capture) : CSAnimationDerivation(other, capture) {
    
    }
private:
    inline ~CSAnimationDerivationMulti() {
    
    }
public:
    static inline CSAnimationDerivationMulti* derivation() {
        return autorelease(new CSAnimationDerivationMulti());
    }
    static inline CSAnimationDerivationMulti* derivationWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSAnimationDerivationMulti(buffer));
    }
    static inline CSAnimationDerivationMulti* derivationWithDerivation(const CSAnimationDerivationMulti* other, bool capture) {
        return autorelease(new CSAnimationDerivationMulti(other, capture));
    }
    
    CSAnimationDerivationType type() const override {
        return CSAnimationDerivationTypeMulti;
    }
    
    float duration(const CSCamera* camera, CSAnimationDuration type, float duration) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
    CSAnimationState update(float delta, const CSCamera* camera, bool alive) override;
	bool isVisible(uint visibleKey) const override;
    void draw(CSGraphics* graphics, uint visibleKey, bool shadow) override;
    
    inline CSAnimationDerivation* copy(bool capture) const override {
        return new CSAnimationDerivationMulti(this, capture);
    }
};

#endif /* defined(__CDK__CSAnimationDerivationMulti__) */
