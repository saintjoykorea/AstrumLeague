//
//  Object.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Object.h"

#include "Map.h"

Object::Object() : _collision(true) {
    GameLeak_addObject(this);
}

Object::~Object() {
    CSAssert(!_located, "invalid state");
    
    release(_effects);
    
    GameLeak_removeObject(this);
}

void Object::registerEffect(Effect* effect) const {
    synchronized(this, GameLockObject) {
        if (!_effects) {
            _effects = new CSArray<Effect>(1);
        }
        _effects->addObject(effect);
    }
}

CSVector3 Object::worldPoint(ObjectLayer layer) const {
    return Map::sharedMap()->convertMapToWorldSpace(_point);
}

FPoint Object::facingPoint(const FPoint& point, fixed range) const {
    FPoint diff = point - _point;
    fixed length = collider() + range;
    diff.normalize();
    diff *= length;
    return _point + diff;
}

bool Object::mark(uint mark) const {
    if (_mark != mark) {
        _mark = mark;
        return true;
    }
    return false;
}

int Object::direction(const FPoint& point) const {
    fixed a = fixed::Pi * 285 / 180 + CSMath::atan2(point.y - _point.y, point.x - _point.x);
    if (a >= fixed::TwoPi) {
        a -= fixed::TwoPi;
    }
    return (int)(a * 6 / fixed::Pi);
}

void Object::locate(const FPoint& point) {
    if (_located) {
        fixed collider = this->collider();
        if (IBounds(_point, collider, true) != IBounds(point, collider, true)) {
            Map::sharedMap()->unlocate(this);
            _point = point;
            Map::sharedMap()->locate(this);
        }
        else {
            _point = point;
        }
    }
    else {
        _point = point;
        _located = true;
        Map::sharedMap()->locate(this);
    }
}

void Object::unlocate() {
    if (_located) {
        Map::sharedMap()->unlocate(this);
        _located = false;
    }
}

ObjectDisplay Object::display() const {
    int layer;
	if (compareType(ObjectMaskAir)) layer = 2;
	else if (compareType(ObjectMaskGround)) layer = 1;
	else layer = 0;

    return ObjectDisplay(_located ? ObjectVisibleNormal : ObjectVisibleNone, layer);
}

float Object::displayRange() const {
	float range = 0;

	if (_effects) {
		synchronized(this, GameLockObject) {
			foreach(const Effect*, effect, _effects) {
				const Object* source = effect->source();

				if (source) {
					float r = (float)_point.distance(source->point()) + source->_radius;

					if (r > range) range = r;
				}
			}
		}
	}
	return range + _radius;
}

void Object::dispose() {
    synchronized(this, GameLockObject) {
        release(_effects);
    }
    unlocate();
}

bool Object::update(bool running) {
    //if (running && _effects) {
    if (_effects) {     //TODO:CHECK FIX, 2022-01-24
		int i = 0;
		for (;;) {
			Effect* effect = NULL;
			synchronized(this, GameLockObject) {
				if (!_effects || i >= _effects->count()) goto fin;
				effect = _effects->objectAtIndex(i);
			}
			effect->update();

			if (effect->destination() == this) {
				i++;
			}
			else {
				synchronized(this, GameLockObject) {
					if (_effects) {
						_effects->removeObjectAtIndex(i);
						if (!_effects->count()) {
							_effects->release();
							_effects = NULL;
						}
					}
				}
			}
		}
    }
fin:
    return true;
}

void Object::drawEffect(CSGraphics *graphics, ObjectLayer layer, bool cover) {
    if (_effects) {
		int i = 0;
		for (;;) {
			Effect* effect = NULL;
			synchronized(this, GameLockObject) {
				if (!_effects || i >= _effects->count()) return;
				effect = _effects->objectAtIndex(i);
				if (effect->isVisible(_effects, i)) effect->retain();
				else effect = NULL;
			}
			if (effect) {
				effect->draw(graphics, layer, cover);
				effect->release();
			}
			i++;
		}
    }
}

uint Object::animationVisibleKey(int dir, ObjectLayer layer) {
    uint rtn;
    switch(layer) {
        case ObjectLayerShadow:
            rtn = AnimationKeyLayerBottom | AnimationKeyLayerMiddle | AnimationKeyLayerTop;
            break;
        case ObjectLayerBottom:
            rtn = AnimationKeyLayerBottom;
            break;
        case ObjectLayerMiddle:
            rtn = AnimationKeyLayerMiddle;
            break;
        case ObjectLayerTop:
            rtn = AnimationKeyLayerTop;
            break;
        default:
            return 0;
    }
    rtn |= AnimationKeyPosition | AnimationKeyScaleRotation | AnimationKeyPositionBody | AnimationKeyPositionHead;
    if (Map::sharedMap()->camera()->inverted()) dir = (dir + 6) % 12;        //TODO:CHECK
    rtn |= (AnimationKeyDirection << dir);
    return rtn;
}
