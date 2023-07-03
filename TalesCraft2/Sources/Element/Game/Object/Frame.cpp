//
//  Frame.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 2..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Frame.h"

#include "Map.h"

#include "VirtualUnit.h"

#define BlinkDuration	1.5f

Frame::Frame(const Force* force, const UnitIndex& index, CommandBuildTarget target, bool collision) : force(force), index(index), target(target) {
	_type = ObjectTypeFrame;
	_collider = data().collider;
	_collision = collision;
	_radius = data().radius;
	
	const MotionData* motion = data().skin(force ? force->unitSkin(index) : 0)->motions[MotionStop];

    CSAssert(motion, "invalid data");

    _motion.animation = new Animation(motion->animation, Map::ObjectAnimationSoundDelegate, false);
    
    Map::sharedMap()->registerObject(this);
}

Frame::~Frame() {
    release(_motion.animation);
}

Frame* Frame::frame(const Force* force, const UnitIndex& index, CommandBuildTarget target, bool collision) {
    Frame* frame = new Frame(force, index, target, collision);
    frame->release();
    return frame;
}

CSVector3 Frame::worldPoint(ObjectLayer layer) const {
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

void Frame::locate(const FPoint& point) {
    Object::locate(point);
    
	_motion.relocated = true;
}

ObjectDisplay Frame::display() const {
    return ObjectDisplay(_located && isVisible(Map::sharedMap()->vision()) ? ObjectVisibleEnvironment : ObjectVisibleNone, 1);
}

bool Frame::update(bool running) {
    Object::update(running);
    
    _motion.remaining += Map::sharedMap()->frameDelayFloat();

	return retainCount() > 1;
}

void Frame::draw(CSGraphics* graphics, ObjectLayer layer) {
    static const CSColor motionColors[2][3] = {
		{
			CSColor(0.5f, 1.0f, 0.5f, 0.5f),
			CSColor(1.0f, 0.25f, 0.25f, 0.5f),
			CSColor(1.0f, 0.75f, 0.25f, 0.5f)
		},
		{
			CSColor(0.5f, 1.0f, 0.5f, 0.75f),
			CSColor(1.0f, 0.25f, 0.25f, 0.5f),
			CSColor(1.0f, 0.75f, 0.25f, 0.5f)
		}
    };
    static const CSColor fillColors[3] = {
        CSColor(CSColor3::Green, 0.1f),
        CSColor(CSColor3::Red, 0.2f),
        CSColor(CSColor3::Orange, 0.2f)
    };
    static const CSColor lineColors[3] = {
        CSColor(CSColor3::Green, 1.0f),
        CSColor(CSColor3::Red, 1.0f),
        CSColor(CSColor3::Orange, 1.0f)
    };
    switch (layer) {
        case ObjectLayerReady:
			{
				float remaining = _motion.remaining;
				bool relocated = _motion.relocated;
				_motion.remaining = 0;
				_motion.relocated = false;
				if (relocated) {
					_motion.animation->setClientPosition(worldPoint(ObjectLayerBottom));
				}
				if (remaining) {
					_motion.animation->update(remaining, &graphics->camera());
				}
			}
            break;
        case ObjectLayerBottomCursor:
            if (target) {
                const UnitData& data = this->data();
                
                if (Object::compareType(data.type, ObjectMaskTower)) {
                    VirtualUnit* unit = VirtualUnit::unit(force, index, force ? force->unitLevel(index) : 1, force ? force->unitSkin(index) : 0);
                    float r = unit->ability(AbilityAttackRange);
                    
                    if (r) {
                        r *= TileSize;
                        
                        CSVector3 p = worldPoint(ObjectLayerBottom);
                        
                        graphics->pushColor();
                        graphics->setColor(CSColor(1.0f, 1.0f, 0.0f, 0.75f));
                        graphics->drawCircle(CSZRect(p.x - r, p.y - r, p.z, r * 2, r * 2), false);
                        graphics->popColor();
                    }
                }
            }
            break;
        case ObjectLayerBottom:
        case ObjectLayerTop:
            _motion.animation->drawLayered(graphics, animationVisibleKey(0, layer));
            break;
        case ObjectLayerMiddle:
            {
                graphics->pushColor();

				const UnitData& data = this->data();

				MapLocation allrtn = Map::sharedMap()->buildable(this, data.type, _point, data.collider, force, target);

				graphics->setColor(motionColors[target != CommandBuildTargetNone][allrtn]);

				_motion.animation->drawLayered(graphics, animationVisibleKey(0, ObjectLayerMiddle));

				if (target && compareType(data.type, ObjectMaskTiled)) {
					IBounds bounds = this->bounds();
					int w = bounds.dest.x - bounds.src.x + 1;
					int h = bounds.dest.y - bounds.src.y + 1;
					MapLocation* rtns = (MapLocation*)alloca(w * h * sizeof(MapLocation));

					static const uint ObjectMaskTiledMatch = ObjectMaskBase | ObjectMaskRefinery;

					bool match = compareType(data.type, ObjectMaskTiledMatch);

					int i = 0;
					for (int y = bounds.src.y; y <= bounds.dest.y; y++) {
						for (int x = bounds.src.x; x <= bounds.dest.x; x++) {
							rtns[i++] = match ? allrtn : (allrtn == MapLocationEnabled ? MapLocationEnabled : Map::sharedMap()->buildable(this, data.type, FPoint(x + fixed::Half, y + fixed::Half), fixed::Half, force, target));
						}
					}
					float z = Map::sharedMap()->terrain()->altitude(_point) * TileSize;
					i = 0;
					for (int y = bounds.src.y; y <= bounds.dest.y; y++) {
						for (int x = bounds.src.x; x <= bounds.dest.x; x++) {
							graphics->setColor(fillColors[rtns[i++]]);
							graphics->drawRect(CSZRect(x * TileSize, y * TileSize, z, TileSize, TileSize), true);
						}
					}
					i = 0;
					for (int y = bounds.src.y; y <= bounds.dest.y; y++) {
						for (int x = bounds.src.x; x <= bounds.dest.x; x++) {
							graphics->setColor(lineColors[rtns[i++]]);

							float x0 = x * TileSize;
							float x1 = x0 + TileSize * 0.5f;
							float x2 = x0 + TileSize;
							float y0 = y * TileSize;
							float y1 = y0 + TileSize * 0.5f;
							float y2 = y0 + TileSize;

							graphics->drawGradientLine(CSVector3(x0, y0, z), CSColor::White, CSVector3(x1, y0, z), CSColor::Transparent);
							graphics->drawGradientLine(CSVector3(x0, y0, z), CSColor::White, CSVector3(x0, y1, z), CSColor::Transparent);

							graphics->drawGradientLine(CSVector3(x2, y0, z), CSColor::White, CSVector3(x1, y0, z), CSColor::Transparent);
							graphics->drawGradientLine(CSVector3(x2, y0, z), CSColor::White, CSVector3(x2, y1, z), CSColor::Transparent);

							graphics->drawGradientLine(CSVector3(x0, y2, z), CSColor::White, CSVector3(x1, y2, z), CSColor::Transparent);
							graphics->drawGradientLine(CSVector3(x0, y2, z), CSColor::White, CSVector3(x0, y1, z), CSColor::Transparent);

							graphics->drawGradientLine(CSVector3(x2, y2, z), CSColor::White, CSVector3(x1, y2, z), CSColor::Transparent);
							graphics->drawGradientLine(CSVector3(x2, y2, z), CSColor::White, CSVector3(x2, y1, z), CSColor::Transparent);
						}
					}
				}
                graphics->popColor();
            }
            break;
    }
}
