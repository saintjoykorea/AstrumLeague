//
//  CSButton.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 13..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSButton.h"

#include "CSView.h"

CSButton::CSButton() {
    _scaleMask = CSButtonScaleAll;
    _scaleDegree = 0.04f;
    _scaleDuration = 0.25f;
}

void CSButton::onTouchesBegan() {
    _scaleMaxProgress = 0.25f;
    while (_scaleProgress > _scaleMaxProgress) {
        _scaleMaxProgress += 1;
    }
    CSLayer::onTouchesBegan();
}

void CSButton::onTouchesEnded() {
    _scaleMaxProgress = 1;
    while (_scaleProgress + 0.5f > _scaleMaxProgress) {
        _scaleMaxProgress += 1;
    }
    CSLayer::onTouchesEnded();
}

void CSButton::onTouchesCancelled() {
    _scaleMaxProgress = 1;
    while (_scaleProgress + 0.5f > _scaleMaxProgress) {
        _scaleMaxProgress += 1;
    }
    CSLayer::onTouchesCancelled();
}

bool CSButton::timeout(CSLayerVisible visible) {
    if (_scaleProgress != _scaleMaxProgress) {
        _scaleProgress = CSMath::min(_scaleProgress + timeoutInterval() / _scaleDuration, _scaleMaxProgress);
        refresh();
    }
    else if (!isTouching()) {
        _scaleProgress = 0;
        _scaleMaxProgress = 0;
    }
    return CSLayer::timeout(visible);
}

void CSButton::draw(CSGraphics* graphics) {
    graphics->push();
    graphics->translate(_frame.origin);
    
    if (transitionForward(graphics)) {
        if (_scaleProgress) {
            CSVector2 centerMiddle = this->centerMiddle();
            CSVector3 translation = centerMiddle;
            CSVector3 scale(1, 1, 1);
            
            float scaleDegree = -_scaleDegree * CSMath::sin(_scaleProgress * FloatTwoPi);
            
            if (_scaleMask & CSButtonScaleLeft) {
                translation.x += centerMiddle.x;
                scale.x += scaleDegree;
            }
            if (_scaleMask & CSButtonScaleRight) {
                translation.x -= centerMiddle.x;
                scale.x += scaleDegree;
            }
            if (_scaleMask & CSButtonScaleTop) {
                translation.y += centerMiddle.y;
                scale.y += scaleDegree;
            }
            if (_scaleMask & CSButtonScaleBottom) {
                translation.y -= centerMiddle.y;
                scale.y += scaleDegree;
            }
            graphics->translate(translation);
            graphics->scale(scale);
            graphics->translate(-translation);
        }
        
		graphics->push();
		
		commitDraw(graphics);
		graphics->reset();
        
        if (_layers->count()) {
            int i;
            int len = _layers->count();
            
            for (i = len - 1; i > 0 && !_layers->objectAtIndex(i)->covered(); i--);
            
            if (i > 0 && _layers->objectAtIndex(i)->transitionBackward(graphics)) {
                int j;
                for (j = i - 1; j > 0 && !_layers->objectAtIndex(j)->covered(); j--);
                for (; j < i; j++) {
                    _layers->objectAtIndex(j)->draw(graphics);
                }
                graphics->reset();
            }
            for (; i < len; i++) {
                _layers->objectAtIndex(i)->draw(graphics);
            }
            graphics->reset();
        }
        
		commitDrawCover(graphics);

		graphics->pop();
        
#ifdef CS_DISPLAY_LAYER_FRAME
        if (view()->displayLayerFrame()) {
            graphics->setColor(CSColor::Magenta);
            graphics->drawRect(bounds(), false);
        }
#endif
    }
    graphics->pop();
}

