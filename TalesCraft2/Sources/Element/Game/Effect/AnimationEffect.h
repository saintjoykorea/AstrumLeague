//
//  AnimationEffect.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef AnimationEffect_h
#define AnimationEffect_h

#include "Effect.h"

class AnimationEffect : public Effect {
private:
	AnimationIndex _index;
    Animation* _animation;
    float _remaining;
    const Object* _src;
    bool _nearby;
    bool _attach;
    bool _ui;
	bool _finite;
	byte _stop;
	CSColor _color;

    AnimationEffect(const AnimationIndex& index, const Object* src, const Object* dest, bool nearby, bool attach, bool ui, const CSColor& color, int tag);
    ~AnimationEffect();
public:
    static AnimationEffect* effect(const AnimationIndex& index, const Object* src, const Object* dest, bool nearby, bool attach, bool ui, const CSColor& color = CSColor::White, int tag = 0);
    
	inline EffectType type() const override {
		return EffectTypeAnimation;
	}

    inline const Object* source() const override {
        return _src;
    }

	inline const AnimationIndex& index() const {
		return _index;
	}

    inline void setColor(const CSColor& color) {
        _color = color;
    }

	inline void stop() {
		_stop = 1;
	}

	bool isVisible(const CSArray<Effect>* others, int count) const override;
private:
    void updatePosition();
public:
    void dispose() override;
    void update() override;
    void draw(CSGraphics* graphics, ObjectLayer layer, bool cover) override;
};

#endif /* AnimationEffect_h */
