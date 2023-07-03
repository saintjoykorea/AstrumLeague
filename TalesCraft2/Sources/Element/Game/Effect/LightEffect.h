//
//  LightEffect.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef LightEffect_h
#define LightEffect_h

#include "Effect.h"

class LightEffect : public Effect {
private:
    EffectDisplay _display;
    float _progress;

    LightEffect(const Object* dest, EffectDisplay display, int tag = 0);
    inline ~LightEffect() {
    
    }
public:
    static LightEffect* effect(const Object* dest, EffectDisplay display, int tag = 0);
    
	inline EffectType type() const override {
		return EffectTypeLight;
	}

    inline const Object* source() const override {
        return NULL;
    }
    
    void update() override;
    void draw(CSGraphics* graphics, ObjectLayer layer, bool cover) override;
};

#endif /* LightEffect_h */
