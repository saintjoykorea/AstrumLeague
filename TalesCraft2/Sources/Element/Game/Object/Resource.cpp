//
//  Resource.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Resource.h"

#include "Map.h"

Resource::Resource(int kind, const ResourceIndex& index, uint amount) : 
	key(Map::sharedMap()->registerKey()), 
	kind(kind), 
	index(index), 
	amount(amount), 
	_remaining(amount) 
{
    AssertOnUpdateThread();

	_type = (ObjectType)(ObjectTypeMineral + kind);
	_collider = data().collider;
	_radius = data().radius;
    
    resetMotion();
	
	CSAssert(amount && _motion.animation, "invalid state");

    Map::sharedMap()->registerResource(this);
    Map::sharedMap()->registerObject(this);
}

Resource::~Resource() {
	_motion.animation->release();
}

Resource* Resource::resource(int kind, const ResourceIndex& index, uint amount) {
    Resource* resource = new Resource(kind, index, amount);
    resource->release();
    return resource;
}

bool Resource::resetMotion() {
    const ResourceData& data = this->data();
    for (int i = 0; i < data.levels->count(); i++) {
        const ResourceLevelData& levelData = data.levels->objectAtIndex(i);
        
        if (_remaining >= levelData.amount) {
			if (_motion.animation && _level == i) {
				break;
			}
			_level = i;
			
			synchronized(this, GameLockObject) {
				if (_motion.animation) _motion.animation->release();
				_motion.animation = new Animation(levelData.animation, Map::ObjectAnimationSoundDelegate, false);
				_motion.animation->checkForLayered();
				if (_located) {
					_motion.animation->setClientPosition(worldPoint(ObjectLayerBottom));
				}
			}
			return true;
        }
    }
	return false;
}

Animation* Resource::captureMotion() const {
	Animation* capture = NULL;
	synchronized(this, GameLockObject) {
		capture = copy(_motion.animation);
	}
	return capture;
}

CSVector3 Resource::worldPoint(ObjectLayer layer) const {
	CSVector3 wp = Map::sharedMap()->convertMapToWorldSpace(_point);
	switch (layer) {
		case ObjectLayerMiddle:
			wp += Map::sharedMap()->camera()->up() * data().body;
			break;
		case ObjectLayerTop:
			wp += Map::sharedMap()->camera()->up() * data().top;
			break;
	}
	return wp;
}

void Resource::locate(const FPoint& point) {
    Object::locate(point);
    
	_motion.relocated = true;
}

void Resource::unlocate() {
    if (_located) {
        Map::sharedMap()->removeSelection(this);
        Object::unlocate();
    }
}

bool Resource::isGatherabled(const Force* force) const {
    if (!_located) {
        return false;
    }
    if (kind == 1) {
        if (!remaining()) {
            return false;
        }
        
        const Unit* mine = Map::sharedMap()->findRefinery(_point);
        
        if (!mine || mine->force() != force || mine->building() > BuildingTransform) {
            return false;
        }
    }
    return true;
}
fixed Resource::gather(fixed amount) {
    if (_remaining >= amount) {
        _remaining -= amount;
    }
    else {
        amount = _remaining;
        _remaining = fixed::Zero;
    }
    if (kind == 0 && !_remaining) {
        unlocate();
    }
    else {
		_motion.reset = true;
    }
    return amount;
}
void Resource::dispose() {
    Object::dispose();
    
    Map::sharedMap()->unregisterResource(this);
}

bool Resource::isVisible(int alliance) const {
    return _located && Map::sharedMap()->isVisible(bounds(), alliance);
}

bool Resource::isVisible(const Force* force) const {
    return _located && (!force || Map::sharedMap()->isVisible(bounds(), force->alliance()));
}

bool Resource::isDisplaying(int alliance) const {
    if (kind == 1) {
        const Unit* mine = Map::sharedMap()->findRefinery(_point);
        
        if (mine && mine->isAlive() && mine->isDetected(alliance)) {
            return false;
        }
    }
    return true;
}

bool Resource::isDisplaying(const Force* force) const {
    if (kind == 1) {
        const Unit* mine = Map::sharedMap()->findRefinery(_point);
        
        if (mine && mine->isAlive() && mine->isDetected(force)) {
            return false;
        }
    }
    return true;
}

ObjectDisplay Resource::display() const {
    const Force* vision = Map::sharedMap()->vision();
    
    return ObjectDisplay(_located && isVisible(vision) && isDisplaying(vision) ? ObjectVisibleNormal : ObjectVisibleNone, 1);
}

bool Resource::update(bool running) {
    Object::update(running);
    
    _motion.remaining += Map::sharedMap()->frameDelayFloat();

	return _located;
}

void Resource::drawBottomCursor(CSGraphics* graphics) {
    bool selected;
    synchronized(Map::sharedMap()->selectionLock(), GameLockMap) {
        selected = (Map::sharedMap()->selectedTarget() == this);
    }
    if (selected) {
        CSVector3 p = worldPoint(ObjectLayerBottom);
        
		int r = _radius;
        if (r > 4) {
            r = 4;
        }
		float hw = _radius * TileSize;

        graphics->pushColor();
        graphics->setColor(Asset::neutralColor);
		graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSelectCursor_0 + r), CSZRect(p.x - hw, p.y - hw, p.z, hw * 2 , hw * 2));
        graphics->popColor();
    }
}

void Resource::draw(CSGraphics* graphics, ObjectLayer layer) {
    switch (layer) {
        case ObjectLayerReady:
			{
				float remaining = _motion.remaining;
				bool relocated = _motion.relocated;
				bool reset = _motion.reset;
				_motion.remaining = 0;
				_motion.relocated = false;
				_motion.reset = false;

				if (reset && resetMotion()) break;

				if (relocated) {
					_motion.animation->setClientPosition(worldPoint(ObjectLayerBottom));
				}
				if (remaining) {
					_motion.animation->update(remaining, &graphics->camera());
				}
			}
			break;
		case ObjectLayerBottomCursor:
			drawBottomCursor(graphics);
			break;
#ifndef UsePathTest
		case ObjectLayerShadow:
		case ObjectLayerBottom:
        case ObjectLayerMiddle:
        case ObjectLayerTop:
            _motion.animation->drawLayered(graphics, animationVisibleKey(0, layer), layer == ObjectLayerShadow);
            break;
#endif
    }
}
