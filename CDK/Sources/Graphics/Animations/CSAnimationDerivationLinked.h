//
//  CSAnimationDerivationLinked.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationDerivationLinked__
#define __CDK__CSAnimationDerivationLinked__

#include "CSAnimationDerivation.h"

class CSAnimationDerivationLinked : public CSAnimationDerivation {
private:
    uint _loopCount;
    uint _loopProgress;
    uint _current;
    uint _count;
public:
    inline CSAnimationDerivationLinked() : _loopCount(1) {
    
    }
    CSAnimationDerivationLinked(CSBuffer* buffer);
    CSAnimationDerivationLinked(const CSAnimationDerivationLinked* other, bool capture);
private:
    inline ~CSAnimationDerivationLinked() {
    
    }
public:
    static inline CSAnimationDerivationLinked* derivation() {
        return autorelease(new CSAnimationDerivationLinked());
    }
    static inline CSAnimationDerivationLinked* derivationWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSAnimationDerivationLinked(buffer));
    }
    static inline CSAnimationDerivationLinked* derivationWithDerivation(const CSAnimationDerivationLinked* other, bool capture) {
        return autorelease(new CSAnimationDerivationLinked(other, capture));
    }
    
    inline void setLoopCount(uint loopCount) {
        _loopCount = loopCount;
    }
    inline uint loopCount() const {
        return _loopCount;
    }
    
    CSAnimationDerivationType type() const override {
        return CSAnimationDerivationTypeLinked;
    }
    
    float duration(const CSCamera* camera, CSAnimationDuration type, float duration) const override;
	bool isFinite(bool finite) const override;
private:
    void rewindProgress();
public:
    void rewind() override;
    
    CSAnimationState update(float delta, const CSCamera* camera, bool alive) override;
	bool isVisible(uint visibleKey) const override;
    void draw(CSGraphics* graphics, uint visibleKey, bool shadow) override;
    
    inline CSAnimationDerivation* copy(bool capture) const override {
        return new CSAnimationDerivationLinked(this, capture);
    }
};

#endif /* defined(__CDK__CSAnimationDerivationLinked__) */
