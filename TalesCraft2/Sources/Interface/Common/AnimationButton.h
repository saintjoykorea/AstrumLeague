//
//  AnimationButton.h
//  TalesCraft2
//
//  Created by Jae Hong Kim on 2019. 2. 15..
//Copyright © 2019년 brgames. All rights reserved.
//

#ifndef AnimationButton_h
#define AnimationButton_h

#include "AssetIndex.h"
#include "Animation.h"

class AnimationButton : public CSLayer {
private:
    Animation* _animations[2];
    CSAlign _align;
    bool _touch;
public:
    AnimationButton(Animation* idleAnimation, Animation* pushedAnimation, CSAlign align = CSAlignCenterMiddle);
private:
    ~AnimationButton();
public:
    static inline AnimationButton* button(Animation* idleAnimation, Animation* pushedAnimation, CSAlign align = CSAlignCenterMiddle) {
        return autorelease(new AnimationButton(idleAnimation, pushedAnimation, align));
    }
protected:
    virtual void onFrameChanged() override;
    virtual void onTimeout() override;
    virtual void onDraw(CSGraphics* graphics) override;
    virtual void onTouchesBegan() override;
};

#endif /* AnimationButton_h */
