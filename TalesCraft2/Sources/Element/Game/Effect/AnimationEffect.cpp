//
//  AnimationEffect.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "AnimationEffect.h"

#include "Map.h"

#define MaxEffectDuration	5.0f

AnimationEffect::AnimationEffect(const AnimationIndex& index, const Object* src, const Object* dest, bool nearby, bool attach, bool ui, const CSColor& color, int tag) :
    Effect(dest ? dest : src, tag),
	_index(index),
    _animation(new Animation(index, Map::ObjectAnimationSoundDelegate, false)),
    _src(dest ? retain(src) : NULL),
    _nearby(nearby),
    _attach(attach),
    _ui(ui),
    _color(color)
{
    _animation->checkForLayered();
    
    float radius = _dest->radius();
    if (radius) {
        _animation->setClientScale(radius);
    }
    updatePosition();

	_finite = _animation->isFinite();
}

AnimationEffect::~AnimationEffect() {
	_animation->release();

    release(_src);
}

AnimationEffect* AnimationEffect::effect(const AnimationIndex& index, const Object* src, const Object* dest, bool nearby, bool attach, bool ui, const CSColor& color, int tag) {
    AnimationEffect* effect = new AnimationEffect(index, src, dest, nearby, attach, ui, color, tag);
    effect->destination()->registerEffect(effect);
    effect->release();
    return effect;
}

bool AnimationEffect::isVisible(const CSArray<Effect>* others, int count) const {
	if (_ui) {
		for (int i = 0; i < count; i++) {
			const Effect* e = others->objectAtIndex(i);
			if (e->type() == EffectTypeAnimation) {
				const AnimationEffect* ae = static_cast<const AnimationEffect*>(e);
				if (ae->_ui && ae->_index == _index && ae->_src == _src) return false;
			}
		}
	}
	return true;
}

void AnimationEffect::dispose() {
    synchronized(this, GameLockEffect) {
		release(_src);
    }
	Effect::dispose();
}

void AnimationEffect::updatePosition() {
    static const uint keys[] = {
        AnimationKeyPosition,
        AnimationKeyPositionBody,
        AnimationKeyPositionHead
    };
    for (int i = 0; i < 3 ; i++) {
        ObjectLayer layer = (ObjectLayer)(i + ObjectLayerBottom);

		CSVector3 dp;

		if (_src) {
			CSVector3 sp = _src->worldPoint(layer);

			if (_src != _dest && sp != (dp = _dest->worldPoint(layer))) {
				if (_nearby) {
					dp += CSVector3::normalize(sp - dp) * (float)_dest->collider() * TileSize * 0.75f;
				}
				_animation->setClientPosition(sp, false, keys[i]);
				_animation->setClientPosition(dp, true, keys[i]);
			}
			else {
				_animation->setClientPosition(sp, keys[i]);
			}
		}
		else {
			dp = _dest->worldPoint(layer);
			_animation->setClientPosition(dp, keys[i]);
		}
    }
}

void AnimationEffect::update() {
	if (_dest) {
		_remaining += Map::sharedMap()->frameDelayFloat();

		if ((_finite || _stop) && _remaining >= MaxEffectDuration) {
			synchronized(this, GameLockEffect) {
				release(_src);
				release(_dest);
			}
		}
	}
	else {
		synchronized(this, GameLockEffect) {
			release(_src);
		}
	}
}

void AnimationEffect::draw(CSGraphics* graphics, ObjectLayer layer, bool cover) {
	if (_dest && cover) {
		switch (layer) {
			case ObjectLayerBottom:
				{
					float remaining = _remaining;
					_remaining = 0;
					synchronized(this, GameLockEffect) {
						if (!_dest) return;
						if (remaining && _attach) {
							updatePosition();
						}
					}
					if (_stop == 1) {
						_animation->stop();
						_stop = 2;
					}
					if (remaining && !_animation->update(remaining, &graphics->camera())) {
						synchronized(this, GameLockEffect) {
							release(_src);
							release(_dest);
						}
						break;
					}
				}
			case ObjectLayerMiddle:
			case ObjectLayerTop:
				graphics->pushColor();
				graphics->setColor(_color, !_ui);
				_animation->drawLayered(graphics, Object::animationVisibleKey(0, layer));
				graphics->popColor();
				break;
		}
	}
}
