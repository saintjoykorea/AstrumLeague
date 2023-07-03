//
//  Map+transform.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

bool Map::convertViewToMapSpace(const CSVector2& vp, FPoint& result, bool altitude) const {
    CSVector3 point;

	CSCamera capture;
	_camera->capture(capture);

    CSRay pickRay = capture.pickRay(vp);
        
    if (!pickRay.intersects(CSPlane::Ground, point)) {
        return false;
    }
    if (altitude) {
        float distance2 = FloatMax;
            
        CSVector2 bp(point.x / TileSize, point.y / TileSize);
            
        IBounds bounds;
        bounds.src.x = CSMath::max((int)(bp.x - _camera->pickRange()), 0);
        bounds.src.y = CSMath::max((int)(bp.y - _camera->pickRange()), 0);          //TODO:FIXED, CAMERA CAN INVERTED
        bounds.dest.x = CSMath::min((int)(bp.x + _camera->pickRange()), _terrain->width() - 1);
        bounds.dest.y = CSMath::min((int)(bp.y + _camera->pickRange()), _terrain->height() - 1);
            
        static const IPoint quadOffsets[] = {
            IPoint(0, 0),
            IPoint(1, 0),
            IPoint(0, 1),
            IPoint(1, 1)
        };
            
        IPoint p;
        for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
            for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                CSVector3 positions[4];
                for (int i = 0; i < 4; i++) {
                    IPoint np = p + quadOffsets[i];
                    positions[i] = CSVector3(np.x, np.y, _terrain->altitude(np)) * (float)TileSize;
                }
                CSVector3 pp;
                if (pickRay.intersects(positions[0], positions[1], positions[2], pp) || pickRay.intersects(positions[1], positions[3], positions[2], pp)) {
                    float d2 = capture.position().distanceSquared(pp);
                    if (d2 < distance2) {
                        point = pp;
                    }
                }
            }
        }
    }
    result.x = point.x / TileSize;
    result.y = point.y / TileSize;

	if (result.x < fixed::Zero) {
        result.x = fixed::Zero;
    }
    else if (result.x >= _terrain->width()) {
        result.x = _terrain->width() - fixed::Epsilon;
    }
    if (result.y < fixed::Zero) {
        result.y = fixed::Zero;
    }
    else if (result.y >= _terrain->height()) {
        result.y = _terrain->height() - fixed::Epsilon;
    }
    return true;
}

Object* Map::targetImpl(const CSVector2& vp, float range, const byte* condition) {
    AssertOnMainThread();
    
    CSRay ray;
    synchronized(_camera, GameLockMap) {
        ray = _camera->capture().pickRay(vp);
    }
    Object* target = NULL;
    float distance = FloatMax;
    
    const Force* vision = this->vision();
    
    synchronized(_phaseLock, GameLockMap) {
        foreach(Unit*, obj, _units) {
            if (obj->isVisible(vision)) {
                CSVector3 center = obj->worldPoint(ObjectLayerMiddle);
                CSBoundingSphere sphere(center, ((float)obj->collider() + range) * TileSize);
                CSVector3 pos;
                
                if (ray.intersects(sphere, pos)) {
                    float d = CSVector3::cross(ray.direction, center - ray.position).lengthSquared();
                    
                    if (d < distance && (condition ? obj->match(_force, condition) : obj->isAlive())) {
                        target = obj;
                        distance = d;
                    }
                }
            }
        }
        if (!condition) {
            foreach(Resource*, obj, _resources) {
                if (obj->isVisible(vision) && obj->isDisplaying(vision)) {
                    CSVector3 center = obj->worldPoint(ObjectLayerMiddle);
                    CSBoundingSphere sphere(center, ((float)obj->collider() + range) * TileSize);
                    CSVector3 pos;
                    
                    if (ray.intersects(sphere, pos)) {
                        float d = CSVector3::cross(ray.direction, center - ray.position).lengthSquared();
                        
                        if (d < distance) {
                            target = obj;
                            distance = d;
                        }
                    }
                }
            }
        }
        if (target) {
            target->retain();
            target->autorelease();
        }
    }
    return target;
}

CSArray<Object>* Map::targetsImpl(const CSVector2 &vp, float range, const byte* condition) {
    AssertOnMainThread();
    
    CSRay ray;
    synchronized(_camera, GameLockMap) {
        ray = _camera->capture().pickRay(vp);
    }
    
    CSArray<Object>* targets = CSArray<Object>::array();
    
    const Force* vision = this->vision();
    
    synchronized(_phaseLock, GameLockMap) {
        foreach(Unit*, obj, _units) {
            if (obj->isVisible(vision)) {
                CSBoundingSphere sphere(obj->worldPoint(ObjectLayerMiddle), ((float)obj->collider() + range) * TileSize);
                float d;
                
                if (ray.intersects(sphere, d) && (condition ? obj->match(_force, condition) : obj->isAlive())) {
                    targets->addObject(obj);
                }
            }
        }
        if (!condition) {
            foreach(Resource*, obj, _resources) {
                if (obj->isVisible(vision) && obj->isDisplaying(vision)) {
                    CSBoundingSphere sphere(obj->worldPoint(ObjectLayerMiddle), ((float)obj->collider() + range) * TileSize);
                    float d;
                    
                    if (ray.intersects(sphere, d)) {
                        targets->addObject(obj);
                    }
                }
            }
        }
    }
    return targets;
}

CSArray<Object>* Map::targetsWithRangeImpl(const CSVector2& vp, fixed range, const byte* condition) {
    AssertOnMainThread();
    
    CSArray<Object>* targets = CSArray<Object>::array();
    
    FPoint origin;
    
    if (convertViewToMapSpace(vp, origin, true)) {
        IBounds bounds(origin, range, true);
        
        IPoint p;
        
        const Force* vision = this->vision();
        
        synchronized(_markLock, GameLockMap) {
            _mark++;

            for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
					synchronized(_phaseLock, GameLockMap) {
						for (TileObject* current = tileImpl(p).objects(); current; current = current->next()) {
							if (current->object()->mark(_mark) && current->object()->compareType(ObjectMaskUnit) && current->object()->isVisible(vision)) {
								Unit* target = static_cast<Unit*>(current->object());

								if (condition ? target->match(_force, condition) : target->isAlive()) {
									fixed d2 = origin.distanceSquared(target->point());
									fixed c = target->collider() + range;

									if (d2 <= c * c) {
										targets->addObject(target);
									}
								}
							}
						}
					}
                }
            }
        }
    }
    return targets;
}

CSArray<Object>* Map::targetsWithAngleImpl(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed angle, const byte* condition) {
    AssertOnMainThread();
    
    CSArray<Object>* targets = CSArray<Object>::array();
    
    FPoint origin;
    
    if (convertViewToMapSpace(vp, origin, true)) {
        const Force* vision = this->vision();

        synchronized(_markLock, GameLockMap) {
            _mark++;
            
            foreach(const Unit*, unit, units) {
                if (unit->point() != origin) {
                    FPoint p0, p1;
                    p0 = p1 = origin - unit->point();
                    p0.rotate(angle / 2);
                    p1.rotate(-angle / 2);
                    p0 += unit->point();
                    p1 += unit->point();

					fixed r = range + unit->collider();
                    
                    IBounds bounds(unit->point(), r, true);
                    
                    if (bounds.size() < _units->count()) {
                        IPoint p;
                        
                        for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                            for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
								synchronized(_phaseLock, GameLockMap) {
									for (TileObject* current = tileImpl(p).objects(); current; current = current->next()) {
										Object* obj = current->object();
										if (obj->mark(_mark) && obj->compareType(ObjectMaskUnit) && obj->isVisible(vision)) {
											fixed d2 = obj->point().distanceSquared(unit->point());
											fixed c = r + obj->collider();
											if (d2 <= c * c && obj->point().between(unit->point(), p0, p1)) {
												Unit* target = static_cast<Unit*>(obj);

												if (condition ? target->match(_force, condition) : target->isAlive()) {
													targets->addObject(target);
												}
											}
										}
									}
								}
                            }
                        }
                    }
                    else {
						synchronized(_phaseLock, GameLockMap) {
							foreach(Unit*, target, _units) {
								if (target->mark(_mark) && target->isVisible(vision)) {
									fixed d2 = target->point().distanceSquared(unit->point());
									fixed c = r + target->collider();
									if (d2 <= c * c && target->point().between(unit->point(), p0, p1) && (condition ? target->match(_force, condition) : target->isAlive())) {
										targets->addObject(target);
									}
								}
							}
						}
                    }
                }
            }
        }
    }
    return targets;
}

CSArray<Object>* Map::targetsWithLineImpl(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed thickness, const byte* condition) {
    AssertOnMainThread();
    
    CSArray<Object>* targets = CSArray<Object>::array();
    
    FPoint origin;
    
    if (convertViewToMapSpace(vp, origin, true)) {
        const Force* vision = this->vision();
        
		synchronized(_markLock, GameLockMap) {
			_mark++;
			
			foreach(const Unit*, unit, units) {
				if (unit->point() != origin) {
					FPoint n = FPoint::normalize(origin - unit->point());
					FPoint p0 = unit->point() + n * thickness;
					FPoint p1 = unit->point() + n * (range + unit->collider());

					synchronized(_phaseLock, GameLockMap) {
						foreach(Unit*, target, _units) {
							if (target->mark(_mark) && target->isVisible(vision)) {
								fixed c = thickness + target->collider();
								if (target->point().distanceSquaredToLine(p0, p1) <= c * c && (condition ? target->match(_force, condition) : target->isAlive())) {
									targets->addObject(target);
								}
							}
						}
					}
				}
			}
		}
    }
    return targets;
}

