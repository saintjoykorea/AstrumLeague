//
//  StrokeEffect.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef StrokeEffect_h
#define StrokeEffect_h

#include "Effect.h"

class StrokeEffect : public Effect {
private:
    EffectDisplay _display;
    float _progress;

    StrokeEffect(const Object* dest, EffectDisplay display, int tag = 0);
    inline ~StrokeEffect() {
    
    }
public:
    static StrokeEffect* effect(const Object* dest, EffectDisplay display, int tag = 0);
    
	inline EffectType type() const override {
		return EffectTypeStroke;
	}

    inline const Object* source() const override {
        return NULL;
    }
    
    void update() override;
    void draw(CSGraphics* graphics, ObjectLayer layer, bool cover) override;
};

#endif /* StrokeEffect_h */
