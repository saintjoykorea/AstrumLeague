//
//  StrokeEffect.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "StrokeEffect.h"

#include "Map.h"

#define EffectDuration    1.5f
#define EffectWidth       3

StrokeEffect::StrokeEffect(const Object* dest, EffectDisplay display, int tag) : Effect(dest, tag), _display(display) {
    
}

StrokeEffect* StrokeEffect::effect(const Object* dest, EffectDisplay display, int tag) {
    StrokeEffect* effect = new StrokeEffect(dest, display, tag);
    effect->destination()->registerEffect(effect);
    effect->release();
    return effect;
}

void StrokeEffect::update() {
    switch (_display) {
        case EffectDisplayBlink:
            {
                int duration = EffectDuration * Map::sharedMap()->framePerSecond();
                int frame = Map::sharedMap()->frame() % duration;
                if (frame > duration / 2) {
                    frame = duration - frame;
                }
                _progress = (float)frame / duration;		//0.0 ~ 0.5
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

void StrokeEffect::draw(CSGraphics* graphics, ObjectLayer layer, bool cover) {
    if (!cover) {
		const Object* dest = NULL;
		synchronized(this, GameLockEffect) {
			dest = retain(_dest);
		}
		if (dest) {
			CSColor strokeColor = dest->color(true, true);
			strokeColor.a *= 1.0f - _progress;
			graphics->setStrokeColor(strokeColor);
			graphics->setStrokeWidth(EffectWidth);

			dest->release();
		}
    }
}
