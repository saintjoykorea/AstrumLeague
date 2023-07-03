//
//  AnimationButton.cpp
//  TalesCraft2
//
//  Created by Jae Hong Kim on 2019. 2. 15..
//Copyright © 2019년 brgames. All rights reserved.
//
#include "AnimationButton.h"

AnimationButton::AnimationButton(Animation* idleAnimation, Animation* pushedAnimation, CSAlign align) :
    _animations { retain(idleAnimation), retain(pushedAnimation) },
    _align(align)
{

}

AnimationButton::~AnimationButton() {
    _animations[0]->release();
    _animations[1]->release();
}

void AnimationButton::onFrameChanged() {
    CSLayer::onFrameChanged();

    CSVector2 pos = CSVector2::Zero;
    if (_align & CSAlignCenter) pos.x = center();
    else if (_align & CSAlignRight) pos.x = width();
    if (_align & CSAlignMiddle) pos.y = middle();
    else if (_align & CSAlignBottom) pos.y = height();
    _animations[0]->setClientPosition(pos);
    _animations[1]->setClientPosition(pos);
}

void AnimationButton::onTimeout() {
    CSLayer::onTimeout();

    float delta = timeoutInterval();
    
    if (_touch) {
        if (!_animations[1]->update(delta) || (!isTouching() && _animations[1]->progress() > _animations[1]->duration())) {
            _touch = false;
            _animations[0]->rewind();
        }
    }
    else {
        _animations[0]->update(delta);
    }
    refresh();
}

void AnimationButton::onDraw(CSGraphics* graphics) {
    _animations[_touch]->draw(graphics);
    
    CSLayer::onDraw(graphics);
}

void AnimationButton::onTouchesBegan() {
    CSLayer::onTouchesBegan();

    if (!_touch) {
        _touch = true;
        _animations[1]->rewind();
    }
}

