//
//  Prop.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 10. 26..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define GameImpl

#include "Prop.h"

#include "Map.h"

Prop::Prop(const AnimationIndex& index, float translation, float scale, const CSQuaternion& rotation, float radius, bool environment) :
    _translation(translation),
    _scale(scale),
    _rotation(rotation),
    _environment(environment)
{
	_type = environment ? ObjectTypeAir : ObjectTypeGround;
	_radius = radius;

    _motion.animation = new Animation(index, Map::ObjectAnimationSoundDelegate, false);
    _motion.animation->checkForLayered();
    _motion.animation->setClientScale(scale);
    _motion.animation->setClientRotation(rotation);
}

Prop::~Prop() {
    _motion.animation->release();
}

Prop* Prop::prop(const AnimationIndex& index, float translation, float scale, const CSQuaternion& rotation, float radius, bool environment) {
    Prop* prop = new Prop(index, translation, scale, rotation, radius, environment);
    Map::sharedMap()->registerObject(prop);
    prop->release();
    return prop;
}

void Prop::locate(const FPoint& point) {
    Object::locate(point);

	_motion.relocated = true;
}

ObjectDisplay Prop::display() const {
    return ObjectDisplay(_located ? (_environment ? ObjectVisibleEnvironment : ObjectVisibleNormal) : ObjectVisibleNone, _environment ? 2 : 1);
}

bool Prop::update(bool running) {
    Object::update(running);
    
    _motion.remaining += Map::sharedMap()->frameDelayFloat();

	return _located;
}

void Prop::draw(CSGraphics* graphics, ObjectLayer layer) {
    switch (layer) {
		case ObjectLayerReady:
			{
				float remaining = _motion.remaining;
				bool relocated = _motion.relocated;
				_motion.remaining = 0;
				_motion.relocated = false;
				if (relocated) {
					CSVector3 wp = Map::sharedMap()->convertMapToWorldSpace(_point);
					wp.z += _translation;
					_motion.animation->setClientPosition(wp);
				}
				if (remaining) {
					_motion.animation->update(remaining, &graphics->camera());
				}
			}
			break;
		case ObjectLayerShadow:
		case ObjectLayerBottom:
        case ObjectLayerMiddle:
        case ObjectLayerTop:
            _motion.animation->drawLayered(graphics, animationVisibleKey(0, layer), layer == ObjectLayerShadow);
            break;
    }
}
