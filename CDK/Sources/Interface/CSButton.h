//
//  CSButton.h
//  CDK
//
//  Created by 김찬 on 12. 8. 13..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSButton__
#define __CDK__CSButton__

#include "CSLayer.h"

enum CSButtonScale {
    CSButtonScaleLeft = 1,
    CSButtonScaleRight = 2,
    CSButtonScaleTop = 4,
    CSButtonScaleBottom = 8,
    CSButtonScaleAll = 15
};

class CSButton : public CSLayer {
protected:
    uint _scaleMask;
    float _scaleDegree;
    float _scaleDuration;
    float _scaleProgress;
    float _scaleMaxProgress;
public:
    CSButton();
protected:
    inline virtual ~CSButton() {
    
    }
public:
    static inline CSButton* button() {
        return autorelease(new CSButton());
    }
    
    inline uint scaleMask() const {
        return _scaleMask;
    }
    inline void setScaleMask(uint mask) {
        _scaleMask = mask;
    }
    inline float scaleDegree() const {
        return _scaleDegree;
    }
    inline void setScaleDegree(float degree) {
        _scaleDegree = degree;
    }
    inline float scaleDuration() const {
        return _scaleDuration;
    }
    inline void setScaleDuration(float duration) {
        _scaleDuration = duration;
    }
    
    virtual void onTouchesBegan() override;
    virtual void onTouchesEnded() override;
    virtual void onTouchesCancelled() override;

    virtual bool timeout(CSLayerVisible visible) override;

    virtual void draw(CSGraphics* graphics) override;
};

#endif /* defined(__CDK__CSButton__) */
