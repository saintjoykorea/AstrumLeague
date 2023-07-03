//
//  LightEffect.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "LightEffect.h"

#include "Map.h"

#define EffectDuration     1.5f

LightEffect::LightEffect(const Object* dest, EffectDisplay display, int tag) : Effect(dest, tag), _display(display) {

}

LightEffect* LightEffect::effect(const Object* dest, EffectDisplay display, int tag) {
    LightEffect* effect = new LightEffect(dest, display, tag);
    effect->destination()->registerEffect(effect);
    effect->release();
    return effect;
}

void LightEffect::update() {
    switch (_display) {
        case EffectDisplayBlink:
            {
                int duration = EffectDuration * Map::sharedMap()->framePerSecond();
                int frame = Map::sharedMap()->frame() % duration;
                if (frame > duration / 2) {
                    frame = duration - frame;
                }
                _progress = 2.4f * frame / duration;		//0.0 ~ 1.2
            }
            break;
        case EffectDisplayVanish:
			_progress += Map::sharedMap()->frameDelayFloat() / EffectDuration;
            if (_progress >= 1.0f) {
				_progress = 1.0f;
                synchronized(this, GameLockEffect) {
                    release(_dest);
                }
            }
            break;
    }
}

void LightEffect::draw(CSGraphics* graphics, ObjectLayer layer, bool cover) {
    if (!cover) {
		graphics->setBrightness(2.0f - _progress);
    }
}
