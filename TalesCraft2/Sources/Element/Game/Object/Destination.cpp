//
//  Destination.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 5. 29..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "Destination.h"

#include "Map.h"

Destination::Destination(const Force* force, float radius) : _force(force) {
	_type = ObjectTypeGround;
	_radius = radius;

	Map::sharedMap()->registerObject(this);
}

Destination::~Destination() {

}

Destination* Destination::destination(const Force* force, float radius) {
    Destination* destination = new Destination(force, radius);
    destination->release();
    return destination;
}

ObjectDisplay Destination::display() const {
	ObjectVisible visible = ObjectVisibleNone;

	if (_located && hasEffect()) {
		const Force* vision = Map::sharedMap()->vision();

		const Force* force = _force;

		if (!vision || !force || vision->alliance() == force->alliance() || Map::sharedMap()->isVisible(bounds(), vision->alliance())) {
			visible = ObjectVisibleEnvironment;
		}
	}
	return ObjectDisplay(visible, 1);
}

