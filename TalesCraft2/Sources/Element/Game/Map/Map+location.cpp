//
//  Map+location.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 6..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "FogBuilding.h"
#include "FogResource.h"

#define FindPathPerFrame    30

void Map::locate(Object* obj) {
	IBounds bounds = obj->bounds();

	synchronized(_phaseLock, GameLockMap) {
		IPoint p;
		for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
			for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
				if (tileImpl(p).locate(obj)) {
					_minimapPoints->addObject(p);
				}
			}
		}
	}
	if (obj->compareType(ObjectMaskLandmark) && obj->collision()) {
		AssertOnUpdateThread();

		synchronized(_navMeshLock, GameLockMap) {
			_navMesh->locateObstacle(obj);
		}
	}
}
void Map::unlocate(Object* obj) {
	IBounds bounds = obj->bounds();

	synchronized(_phaseLock, GameLockMap) {
		IPoint p;
		for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
			for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
				if (tileImpl(p).unlocate(obj)) {
					_minimapPoints->addObject(p);
				}
			}
		}
	}
	if (obj->compareType(ObjectMaskLandmark) && obj->collision()) {
		AssertOnUpdateThread();

		synchronized(_navMeshLock, GameLockMap) {
			_navMesh->unlocateObstacle(obj);
		}
	}
}

bool Map::relocate(Object* obj, bool vision, ObjectType type, fixed collider, bool collision) {
	if (locatable(obj, type, obj->point(), collider, NULL, MapLocationEnabled) == MapLocationUnabled) {
		return false;
	}
    if (obj->compareType(ObjectMaskLandmark)) {
		AssertOnUpdateThread();

        if (!Object::compareType(type, ObjectMaskLandmark)) {
            if (obj->collision()) {
				synchronized(_navMeshLock, GameLockMap) {
					_navMesh->unlocateObstacle(obj);
				}
            }
        }
        else if (collider != obj->collider() || collision != obj->collision()) {
            if (obj->collision()) {
				synchronized(_navMeshLock, GameLockMap) {
					_navMesh->unlocateObstacle(obj);
				}
            }
            if (collision) {
				synchronized(_navMeshLock, GameLockMap) {
					_navMesh->locateObstacle(obj);
				}
            }
        }
    }
    else if (Object::compareType(type, ObjectMaskLandmark) && collision) {
		AssertOnUpdateThread();

		synchronized(_navMeshLock, GameLockMap) {
			_navMesh->locateObstacle(obj);
		}
    }
    IBounds pb = obj->bounds();
    IBounds nb(obj->point(), collider, true);
    bool flag = pb != nb;
        
    if (vision || flag) {
		synchronized(_phaseLock, GameLockMap) {
			IPoint p;
			for (p.y = pb.src.y; p.y <= pb.dest.y; p.y++) {
				for (p.x = pb.src.x; p.x <= pb.dest.x; p.x++) {
					Tile& tile = this->tileImpl(p);

					if (tile.relocate(obj)) {
						_minimapPoints->addObject(p);
					}
					if (flag) {
						tile.unlocate(obj);
					}
				}
			}
			if (flag) {
				for (p.y = nb.src.y; p.y <= nb.dest.y; p.y++) {
					for (p.x = nb.src.x; p.x <= nb.dest.x; p.x++) {
						Tile& tile = this->tileImpl(p);

						if (tile.relocate(obj)) {
							_minimapPoints->addObject(p);
						}
						tile.locate(obj);
					}
				}
			}
		}
    }
    return true;
}

void Map::tiledPosition(fixed collider, FPoint& point) {
    int c = CSMath::ceil(collider * 2);
    point.x = CSMath::floor(point.x);
    point.y = CSMath::floor(point.y);
    if (c & 1) {
        point.x += fixed::Half;
        point.y += fixed::Half;
    }
}

MapLocation Map::locatable(const Object* obj, const FPoint& point, const Force* vision) const {
    return locatable(CSArray<const Object*>::arrayWithObjects(&obj, 1), obj->type(), point, obj->collider(), vision, obj->collision() ? MapLocationCollided : MapLocationEnabled);
}
MapLocation Map::locatable(const Object* obj, ObjectType type, fixed collider, const Force* vision) const {
    return locatable(CSArray<const Object*>::arrayWithObjects(&obj, 1), type, obj->point(), collider, vision, obj->collision() ? MapLocationCollided : MapLocationEnabled);
}
MapLocation Map::locatable(const Object* obj, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
    return locatable(CSArray<const Object*>::arrayWithObjects(&obj, 1), type, point, collider, vision, duplicate);
}
MapLocation Map::locatable(ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
    MapLocation rtn;
    synchronized(_markLock, GameLockMap) {
        _mark++;
        rtn = locatableImpl(NULL, type, point, collider, vision, duplicate);
    }
    return rtn;
}
MapLocation Map::locatable(CSArray<const Object*>* collisions, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
    MapLocation rtn;
    synchronized(_markLock, GameLockMap) {
        _mark++;
        foreach(const Object*, obj, collisions) {
            obj->mark(_mark);
        }
        collisions->removeAllObjects();
        
        rtn = locatableImpl(collisions, type, point, collider, vision, duplicate);
    }
    return rtn;
}
MapLocation Map::locatableImpl(CSArray<const Object*>* collisions, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
	_markLock.assertOnActive();
    
    bool landmark = Object::compareType(type, ObjectMaskLandmark);
    
    if (landmark) {
        if (Object::compareType(type, ObjectMaskBase)) {
            bool enabled = false;
            foreach(const Area&, area, _areas) {
                if (area.point == point) {
                    enabled = true;
                    break;
                }
            }
            if (!enabled) {
                return MapLocationUnabled;
            }
        }
        IBounds bounds(point, collider + fixed::One, false);
        
        if (bounds.src.x < 0 || bounds.dest.x >= _terrain->width() || bounds.src.y < 0 || bounds.dest.y >= _terrain->height()) {
            return MapLocationUnabled;
        }
        IPoint p;
        for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
            for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                if (tile(p).attribute() < TileAttributeBuild) {
                    return MapLocationUnabled;
                }
            }
        }
    }
    else {
        if (Object::compareType(type, ObjectMaskUnit)) {
            const Terrain::Thumbnail& thumbnail = _terrain->thumbnail();
            
            if (point.x < thumbnail.left || point.x >= _terrain->width() - thumbnail.right || point.y < thumbnail.top || point.y >= _terrain->height() - thumbnail.bottom) {
                return MapLocationUnabled;
            }
        }
        else if (point.x < fixed::Zero || point.x >= _terrain->width() || point.y < fixed::Zero || point.y >= _terrain->height()) {
            return MapLocationUnabled;
        }
        if (Object::compareType(type, ObjectMaskPath)) {
			synchronized_to_update_thread(_navMeshLock, GameLockMap) {
				if (!_navMesh->locatable(point)) {
					return MapLocationUnabled;
				}
			}
        }
        else if (Object::compareType(type, ObjectMaskRefinery)) {
            MapLocation rtn = MapLocationUnabled;
            
            static const uint ObjectMaskGasAndFogGas = ObjectMaskGas | ObjectMaskFogGas;
            static const uint ObjectMaskRefineryAndFogBuilding = ObjectMaskRefinery | ObjectMaskFogBuilding;
            
			synchronized(_phaseLock, GameLockMap) {
				for (const TileObject* current = tile(point).objects(); current; current = current->next()) {
					const Object* obj = current->object();
					if (obj->compareType(ObjectMaskGasAndFogGas)) {
						if (obj->point() == point && obj->isVisible(vision)) {
							rtn = MapLocationEnabled;
						}
					}
					else if (obj->compareType(ObjectMaskRefineryAndFogBuilding) && obj->isVisible(vision)) {
						rtn = MapLocationUnabled;
						break;
					}
				}
			}
            return rtn;
        }
    }
    
    uint mask;
    
    if (Object::compareType(type, ObjectMaskUndergroundCollision)) {
        mask = ObjectMaskUndergroundCollision;
    }
    else if (Object::compareType(type, ObjectMaskGroundCollision)) {
        mask = ObjectMaskGroundCollision;
        if (vision) mask |= ObjectMaskFog;
    }
    else if (Object::compareType(type, ObjectMaskAirCollision)) {
        mask = ObjectMaskAirCollision;
    }
    else return MapLocationEnabled;
    
    if (vision) {
        mask |= ObjectMaskFrame;
    }
    if (duplicate == MapLocationEnabled) {
        mask &= ~ObjectMaskMove;
        
        if (!mask) {
            return MapLocationEnabled;
        }
    }

    MapLocation rtn = MapLocationEnabled;
    
    IPoint p;
    
    IBounds bounds(point, collider, true);
    
    for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
        for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
			synchronized(_phaseLock, GameLockMap) {
				for (const TileObject* current = tile(p).objects(); current; current = current->next()) {
					const Object* obj = current->object();

					if (obj->mark(_mark) && obj->compareType(mask) && (duplicate == MapLocationUnabled || obj->collision()) && obj->isVisible(vision)) {
						fixed c = obj->collider() + collider;

						bool grid = false;

						if (landmark) {
							static const uint ObjectMaskLandmarkAndFog = ObjectMaskLandmark | ObjectMaskFog;

							if (obj->compareType(ObjectMaskLandmarkAndFog)) {
								grid = true;
							}
							else if (obj->compareType(ObjectMaskFrame)) {
								const Frame* frame = static_cast<const Frame*>(obj);

								if (Object::compareType(frame->data().type, ObjectMaskLandmark)) {
									grid = true;
								}
							}
						}
						if (grid ? CSMath::abs(obj->point().x - point.x) < c && CSMath::abs(obj->point().y - point.y) < c : obj->point().distanceSquared(point) < c * c) {
							if (collisions) {
								rtn = MapLocationCollided;

								collisions->addObject(obj);
							}
							else {
								return MapLocationCollided;
							}
						}
					}
				}
            }
        }
    }
    return rtn;
}

MapLocation Map::locatablePosition(const Object* obj, FPoint& point, const Force* vision) const {
    return locatablePosition(CSArray<const Object*>::arrayWithObjects(&obj, 1), obj->type(), point, obj->collider(), vision, obj->collision() ? MapLocationCollided : MapLocationEnabled);
}

MapLocation Map::locatablePosition(const Object* obj, FPoint& point, const Force* vision, MapLocation duplicate) const {
	return locatablePosition(CSArray<const Object*>::arrayWithObjects(&obj, 1), obj->type(), point, obj->collider(), vision, duplicate);
}

MapLocation Map::locatablePosition(const Object* obj, ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
	return locatablePosition(CSArray<const Object*>::arrayWithObjects(&obj, 1), type, point, collider, vision, duplicate);
}

MapLocation Map::locatablePosition(ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
	return locatablePosition(CSArray<const Object*>::array(), type, point, collider, vision, duplicate);
}

MapLocation Map::locatablePosition(CSArray<const Object*>* collisions, ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const {
    struct Entry {
        FPoint point;
        fixed distance2;
        int lastCollisionCount;
        bool enabled;
        
        inline Entry(int lastCollisionCount, const FPoint& point, fixed distance2, bool enabled) : lastCollisionCount(lastCollisionCount), point(point), distance2(distance2), enabled(enabled) {
        
        }
    };
    int firstCollisionCount = collisions->count();
    
    bool landmark = Object::compareType(type, ObjectMaskLandmark);
    bool tiled = Object::compareType(type, ObjectMaskTiled);
    
    CSArray<Entry>* entries = new CSArray<Entry>();
    if (tiled) {
        tiledPosition(collider, point);
    }
    new (&entries->addObject()) Entry(firstCollisionCount, point, fixed::Zero, true);
    
    MapLocation rtn = MapLocationEnabled;
    
    enum PositionState {
        PositionStateFirst,
        PositionStateUnabled,
        PositionStateEnabled
    };
    PositionState state = PositionStateFirst;
    
    do {
        const Entry& e = entries->lastObject();
            
        if (state != PositionStateEnabled || e.enabled) {
			int lastCollisionCount = collisions->count();
			MapLocation loc;
			synchronized(_markLock, GameLockMap) {
				_mark++;
				foreach(const Object*, obj, collisions) {
					obj->mark(_mark);
				}
				loc = locatableImpl(collisions, type, e.point, collider, vision, duplicate);
			}
            switch (loc) {
                case MapLocationUnabled:
                    if (state == PositionStateFirst) {
                        rtn = MapLocationCollided;
                            
                        static const fixed FirstUnabledRange = (fixed)3;
                            
                        fixed lx = point.x - FirstUnabledRange;
                        fixed rx = point.x + FirstUnabledRange;
                        fixed ty = point.y - FirstUnabledRange;
                        fixed by = point.y + FirstUnabledRange;
                            
                        FPoint np;
                            
                        for (np.y = ty; np.y <= by; np.y += fixed::One) {
                            for (np.x = lx; np.x <= rx; np.x += fixed::One) {
                                if (np != point) {
                                    fixed d2 = np.distanceSquared(point);
                                        
                                    int i;
                                    for (i = 0; i < entries->count() - 1; i++) {
                                        if (d2 > entries->objectAtIndex(i).distance2) {
                                            break;
                                        }
                                    }
                                    new (&entries->insertObject(i)) Entry(firstCollisionCount, np, d2, false);
                                }
                            }
                        }
                        state = PositionStateUnabled;
                    }
                    break;
                case MapLocationCollided:
                    state = PositionStateEnabled;
                        
                    rtn = MapLocationCollided;

					{
						FPoint ep = e.point;

						for (int i = lastCollisionCount; i < collisions->count(); i++) {
							const Object* src = collisions->objectAtIndex(i);

							fixed c = src->collider() + collider + EpsilonCollider;
							if (tiled) {
								c += fixed::One;
							}
							int npsc;

							const FPoint& sp = src->point();

							FPoint nps[8];

							if (landmark && src->compareType(ObjectMaskLandmark)) {     //건물 주변 지점은 slabId가 다를 것임, 0 유지
								nps[0] = FPoint(sp.x - c, ep.y);
								nps[1] = FPoint(sp.x + c, ep.y);
								nps[2] = FPoint(ep.x, sp.y - c);
								nps[3] = FPoint(ep.x, sp.y + c);
								npsc = 4;
							}
							else {
								FPoint diff = point - sp;
								if (diff.x || diff.y) {
									diff.normalize();
								}
								else {
									diff.y = fixed::One;
								}
								diff *= c;

								static const fixed cosq = CSMath::cos(fixed::PiOverFour);
								static const fixed sinq = CSMath::sin(fixed::PiOverFour);

								for (int j = 0; j < 8; j++) {
									nps[j] = sp;
									nps[j] += diff;     //slabId 캐시적중율

									fixed rx = diff.x * cosq - diff.y * sinq;
									fixed ry = diff.y * cosq + diff.x * sinq;
									diff.x = rx;
									diff.y = ry;
								}
								npsc = 8;
							}
							if (tiled) {
								for (int j = 0; j < npsc; j++) {
									tiledPosition(collider, nps[j]);
								}
							}
							for (int j = 0; j < npsc; j++) {
								const FPoint& np = nps[j];

								bool flag = true;

								for (int k = collisions->count() - 1; k >= firstCollisionCount; k--) {
									if (i != k) {
										const Object* dest = collisions->objectAtIndex(k);

										c = dest->collider() + collider;

										if (landmark && dest->compareType(ObjectMaskLandmark)) {
											if (CSMath::abs(np.x - dest->point().x) < c && CSMath::abs(np.y - dest->point().y) < c) {
												flag = false;
												break;
											}
										}
										else {
											fixed d2 = np.distanceSquared(dest->point());

											if (d2 < c * c) {
												flag = false;
												break;
											}
										}
									}
								}
								if (flag) {
									fixed d2 = np.distanceSquared(point);

									int k;
									for (k = 0; k < entries->count() - 1; k++) {
										if (d2 > entries->objectAtIndex(k).distance2) {
											break;
										}
									}
									new (&entries->insertObject(k)) Entry(collisions->count(), np, d2, true);
								}
							}
						}
					}
                    break;
                case MapLocationEnabled:
                    state = PositionStateEnabled;
                    {
                        bool flag = true;
                            
                        for (int k = e.lastCollisionCount; k < collisions->count(); k++) {
                            const Object* dest = collisions->objectAtIndex(k);
                                
                            fixed c = dest->collider() + collider;
                                
                            if (landmark && dest->compareType(ObjectMaskLandmark)) {
                                if (CSMath::abs(e.point.x - dest->point().x) < c && CSMath::abs(e.point.y - dest->point().y) < c) {
                                    flag = false;
                                    break;
                                }
                            }
                            else {
                                fixed d2 = e.point.distanceSquared(dest->point());
                                    
                                if (d2 < c * c) {
                                    flag = false;
                                    break;
                                }
                            }
                        }
                        if (flag) {
                            point = e.point;
                            goto exit;
                        }
                    }
                    break;
                        
            }
        }
        entries->removeLastObject();
    } while (entries->count());
    
    rtn = MapLocationUnabled;
exit:
    for (int i = 0; i < firstCollisionCount; i++) {
        collisions->removeObjectAtIndex(0);
    }
    entries->release();
    return rtn;
}
/*
static CSArray<FPoint>* nearOffsets(fixed range) {
	CSArray<FPoint>* offsets = CSArray<FPoint>::array();
	FPoint p;
	for (p.y = -range; p.y <= range; p.y += fixed::One) {
		for (p.x = -range; p.x <= range; p.x += fixed::One) {
			if (p != FPoint::Zero) {
				fixed d2 = p.lengthSquared();

				int i;
				for (i = 0; i < (int)offsets->count() - 1; i++) {
					if (d2 <= offsets->objectAtIndex(i).lengthSquared()) {
						break;
					}
				}
				offsets->insertObject(i, p);
			}
		}
	}
	return offsets;
}
*/
fixed Map::moveableDistance(const Object* obj, const Object* target, const FPoint& dest) const {
    uint mask;
    if (obj->compareType(ObjectMaskUndergroundCollision)) {
        mask = ObjectMaskUndergroundCollision;
    }
    else if (obj->compareType(ObjectMaskGroundCollision)) {
        mask = ObjectMaskGroundCollision;
    }
    else if (obj->compareType(ObjectMaskAirCollision)) {
        mask = ObjectMaskAirCollision;
    }
    else {
		return fixed::Zero;
    }
    if (!obj->collision()) {
        mask &= ~ObjectMaskMove;
            
        if (!mask) {
			return fixed::Zero;
        }
    }

	CSArray<const Object*>* collisions = new CSArray<const Object*>();
	fixed collider = obj->collider() + target->collider();
	fixed distance = fixed::Zero;
	fixed extent = fixed::Zero;
    bool first = true;
    IBounds prevBounds;
        
	synchronized(_markLock, GameLockMap) {
		_mark++;
        obj->mark(_mark);
		target->mark(_mark);

        for (; ; ) {
            fixed range = distance + collider;
            
            IBounds bounds(dest, range, true);
            
            bool collided = false;
            
            {
                int i = 0;
                while (i < collisions->count()) {
                    const Object* target = collisions->objectAtIndex(i);
                    fixed d2 = target->point().distanceSquared(dest);
                    fixed c = target->collider();
                    fixed r = range + c;
                    
                    if (d2 < r * r) {
                        collisions->removeObjectAtIndex(i);
                        extent += c * c * 4;
                        collided = true;
                    }
                    else {
                        i++;
                    }
                }
            }
            if (first || prevBounds != bounds) {
                IPoint p;
                for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                    for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                        if (first || !prevBounds.contains(p)) {
							synchronized(_phaseLock, GameLockMap) {
								for (const TileObject* current = tile(p).objects(); current; current = current->next()) {
									const Object* curobj = current->object();

									if (curobj->mark(_mark) && curobj->compareType(mask) && curobj->collision()) {
										fixed d2 = curobj->point().distanceSquared(dest);
										fixed c = curobj->collider();
										fixed r = range + c;

										if (d2 < r * r) {
											extent += c * c * 4;
											collided = true;
										}
										else {
											collisions->addObject(curobj);
										}
									}
								}
							}
                        }
                    }
                }
                prevBounds = bounds;
                first = false;
            }
                
            if (collided) {
                distance = CSMath::sqrt(extent / fixed::Pi);
            }
            else {
                break;
            }
        }
        collisions->release();
    }

    return distance;
}

MapLocation Map::buildable(const Object* obj, ObjectType type, const FPoint& point, fixed collider, const Force* vision, CommandBuildTarget target) const {
    switch (target) {
        case CommandBuildTargetVisible:
            if (!vision || !isVisible(IBounds(point, collider, true), vision->alliance())) return MapLocationUnabled;
            break;
        case CommandBuildTargetSpawn:
            if (!vision || !isSpawnable(point, vision)) return MapLocationUnabled;
            break;
    }

    CSArray<const Object*>* collisions = CSArray<const Object*>::array();
    collisions->addObject(obj);
    
    MapLocation rtn = Map::sharedMap()->locatable(collisions, type, point, collider, vision, MapLocationCollided);
    
    switch (rtn) {
        case MapLocationUnabled:
            return MapLocationUnabled;
        case MapLocationCollided:
			{
				MapLocation loc = MapLocationEnabled;

				foreach(const Object*, other, collisions) {
					if (other->compareType(ObjectMaskUnit)) {
						const Unit* unit = static_cast<const Unit*>(other);
						if (unit->target() == obj && unit->building() == BuildingTargetReady) {
							continue;
						}
					}
					if (!other->compareType(ObjectMaskMove)) {
						return MapLocationUnabled;
					}
					loc = MapLocationCollided;
				}

				return loc;
			}
    }
    return MapLocationEnabled;
}

bool Map::buildingPosition(const Force* force, const UnitIndex& index, const Area& area, FPoint& point) const {
    AssertOnUpdateThread();
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    if (Object::compareType(data.type, ObjectMaskBase)) {
        if (locatable(data.type, area.point, data.collider, NULL, MapLocationEnabled) == MapLocationEnabled) {
            point = area.point;
            return true;
        }
    }
    else if (Object::compareType(data.type, ObjectMaskRefinery)) {
        const FPoint* p = NULL;
        fixed distance2 = area.frange2;
        foreach(const Resource*, resource, _resources) {
            if (resource->kind == 1) {
                fixed d2 = resource->point().distanceSquared(area.point);
                if (d2 <= distance2 && locatable(data.type, resource->point(), data.collider, NULL, MapLocationEnabled) == MapLocationEnabled) {
                    p = &resource->point();
                    distance2 = d2;
                }
            }
        }
        if (p) {
            point = *p;
            return true;
        }
    }
    else {
        const CSArray<FPoint>* ps;
        if (Object::compareType(data.type, ObjectMaskBuilding)) {
            ps = area.buildingPoints;
        }
        else {
            ps = area.rallyPoints;
        }
        if (ps) {
            FPoint p = ps->objectAtIndex(_random->nextInt(0, ps->count() - 1));
            if (locatablePosition(data.type, p, data.collider, NULL, MapLocationEnabled) != MapLocationUnabled) {
                point = p;
                return true;
            }
        }
    }
    return false;
}

bool Map::hasPath(const FPoint &p0, const FPoint &p1) const {
	synchronized_to_update_thread(_navMeshLock, GameLockMap) {
		return _navMesh->hasPath(p0, p1);
	}
	return false;
}

ProgressPathReturn Map::progressPath(const Object *obj, const Object *target, Path* path) const {
    AssertOnUpdateThread();
    
    if (obj->compareType(ObjectMaskPath)) {
        return _navMesh->progressPath(obj, target, path);
    }
    path->clear();
    return obj->compareType(ObjectMaskMove) ? ProgressPathSuccess : ProgressPathFail;
}

typename Map::FindPathReturn Map::findPath(const Object* obj, const Object* target) const {
    AssertOnUpdateThread();
    
    FindPathReturn rtn;
    
    if (obj->compareType(ObjectMaskPath)) {
        if (_findPathCount >= FindPathPerFrame) {
            rtn.path = NULL;
            rtn.retry = true;
        }
        else {
            _findPathCount++;
            rtn.path = _navMesh->findPath(obj, target);
            rtn.retry = false;
        }
        return rtn;
    }
    else{
        if (obj->compareType(ObjectMaskMove)) {
            rtn.path = Path::path(0, target->point(), 0, true);
            rtn.retry = false;
        }
        else {
            rtn.path = NULL;
            rtn.retry = false;
        }
    }
    return rtn;
}

bool Map::move(Object* obj, const Object* target, const FPoint& dest, fixed distance, int dir) {
    if (!obj->compareType(ObjectMaskMove)) {
        return false;
    }
    Movement* movement = obj->movement();
    if (!movement) {
        return false;
    }
    fixed collider = obj->collider();

    const FPoint& sp = obj->point();
    FPoint diff = dest - sp;
    FPoint dp;
    
    if (diff.length() > distance) {
        diff.normalize();
        diff *= distance;
        dp = sp;  //slabId 캐시적중율
        dp += diff;
    }
    else {
        dp = dest;
    }
    
    MoveLog(obj, "move start (position:%.2f, %.2f) (target:%.2f, %.2f)", (float)sp.x, (float)sp.y, (float)dp.x, (float)dp.y);
    
    MapLocation duplicate = obj->collision() ? MapLocationCollided : MapLocationEnabled;
    
    CSArray<const Object*>* collisions = new CSArray<const Object*>();
    collisions->addObject(obj);
    switch (locatable(collisions, obj->type(), dp, collider, NULL, duplicate)) {
        case MapLocationEnabled:
            collisions->release();
            obj->locate(dp);
            movement->dir = 0;
            MoveLog(obj, "move enabled");
            return true;
        case MapLocationUnabled:
			collisions->release();
			movement->dir = 0;
			MoveLog(obj, "move unabled");
			return false;

    }
    fixed collisionDistance = distance;
    
    if (target && collisions->containsObjectIdenticalTo(target)) {
        fixed distanceToTarget = sp.distance(target->point()) - collider - target->collider();
        
        if (distanceToTarget <= distance) {
            collisionDistance = distanceToTarget;
            collisions->release();
            movement->dir = 0;
            MoveLog(obj, "move complete near");
            goto rtn;
        }
        else {
            collisions->removeObjectIdenticalTo(target);
        }
    }
    {
        fixed comp0 = sp.distance(dest) - collider;
        
        CSArray<const Object*>* nextCollisions = new CSArray<const Object*>();
        fixed rd2 = fixed::Zero;
        FPoint rp;
        const Object* rounder = NULL;
        bool rounding = false;
        
        MoveLog(obj, "move rounding start (direction:%d comparison:%.2f)", movement->dir, (float)comp0);
        
        foreach(const Object*, other, collisions) {
            if (obj->compareType(ObjectMaskWeapon) && other->compareType(ObjectMaskWeapon)) {
                const Unit* unit = static_cast<const Unit*>(obj);
                const Unit* otherUnit = static_cast<const Unit*>(other);
                
                if ((!unit->force() || !otherUnit->force() || unit->force()->alliance() == otherUnit->force()->alliance()) && unit->data().weight > otherUnit->data().weight) {
                    continue;
                }
            }
            fixed c = other->collider();
            fixed d = sp.distance(other->point());
            fixed comp1 = other->point().distance(dest) - c;
            MoveLog(obj, "move rounding check (distance:%.2f comparison:%.2f)", (float)d, (float)comp1);
            
            if (comp0 > comp1 && (c <= collider || d > c - collider)) {
                //목표점에 더 멀리 있는 것은 선회대상이 아님, 회전이동으로 생긴 새로운 콜리전이 이 조건에 해당되지 않으면 선회방향을 바꿔야 됨
                //타겟오브젝트에 갖혀 있는 경우에도 선회대상이 아님, 내가 근접지점까지 가더라도 목표점에 더 가까워질 수 없음
                
                MoveLog(obj, "move rounding check 1");
                
                c += collider;
                
                if (d && c + distance > d) {         //충돌은 했지만 거리는 더 먼 경우(계산오차), d가 0인 경우는 라운딩 무브하지 않고 직진
                    MoveLog(obj, "move rounding check 2");
                    
                    if (movement->round(other)) {
                        MoveLog(obj, "move rounding check 3");
                        
                        fixed nr = CSMath::min(c, d);
                        fixed a = CSMath::acos((d * d + distance * distance - nr * nr) / (distance * 2 * d));
                        
                        if (movement->dir) {
                            a *= movement->dir;
                        }
                        else if (dir && !other->compareType(ObjectMaskLandmark)) {
                            movement->dir = dir;        //충돌한 오브젝트가 길찾기에 포함되어 있지 않은 오브젝트를 대상으로만 유도되는 선회방향으로 회전, 다른 오브젝트와 겹쳐서 들어갈 경우는 오류가 있지만 무시
                            a *= movement->dir;
                        }
                        else {
                            FPoint p0, p1;
                            
                            p0 = p1 = other->point() - sp;
                            
                            p0.rotate(a);
                            p0.normalize();
                            p0 *= distance;
                            p0 += obj->point();
                            
                            p1.rotate(-a);
                            p1.normalize();
                            p1 *= distance;
                            p1 += obj->point();
                            
                            if (p0.distanceSquared(dest) > p1.distanceSquared(dest)) {
                                a = -a;
                                movement->dir = -1;
                            }
                            else {
                                movement->dir = 1;
                            }
                        }
                        FPoint nextDiff = other->point() - sp;
                        nextDiff.rotate(a);
                        nextDiff.normalize();
                        nextDiff *= distance;
                        
                        FPoint np = sp;
                        np += nextDiff;     //slabId 캐시적중율
                        
                        fixed nd2 = np.distanceSquared(dest);
                        
                        //콜리전들에서 가장 먼 선회지점은 이동가능해야 성공
                        if (nd2 > rd2) {
                            rd2 = nd2;
                            
                            rounding = true;     //가장 먼지점인데 실패한다면 방향전환
                            rounder = NULL;
                            
                            nextCollisions->removeAllObjects();
                            nextCollisions->addObject(obj);
                            if (target) nextCollisions->addObject(target);
                            nextCollisions->addObjectsFromArray(collisions);
                            switch (locatable(nextCollisions, obj->type(), np, collider, NULL, duplicate)) {
                                case MapLocationCollided:
                                    MoveLog(obj, "move rounding check 4");
                                    collisions->addObjectsFromArray(nextCollisions);
                                    break;
                                case MapLocationEnabled:
                                    MoveLog(obj, "move rounding check 5");
                                    rounder = other;    //계산할 수 있는 가장 먼 지점에서 새로운 콜리전 없이 성공한다면 선회이동성공
                                    rp = np;
                                    break;
                            }
                        }
                    }
                    d -= c;
                    
                    if (d < collisionDistance) {
                        collisionDistance = d;
                    }
                }
            }
        }
        nextCollisions->release();
        collisions->release();
        
        if (rounding) {
            if (rounder) {
                Movement* rounderMovement = const_cast<Object*>(rounder)->movement();
                if (rounderMovement) {
                    rounderMovement->hold(obj);
                }
                obj->locate(rp);
                MoveLog(obj, "move rounding success (direction:%d) (position:%.2f, %.2f)", movement->dir, (float)rp.x, (float)rp.y);
                return true;
            }
            else {
                movement->dir = -movement->dir;
                MoveLog(obj, "move rounding fail (direction:%d)", movement->dir);
            }
        }
    }
rtn:
    if (collisionDistance == distance) {
        obj->locate(dp);
        movement->dir = 0;
        MoveLog(obj, "move ignoring collisions");
        return true;
    }
    else if (collisionDistance > fixed::Zero) {
        diff.normalize();
        diff *= collisionDistance;
        dp = sp;
        dp += diff;     //slabId 캐시적중율
        obj->locate(dp);
        MoveLog(obj, "move collided (distance:%.2f) (position:%.2f, %.2f)", (float)collisionDistance, (float)dp.x, (float)dp.y);
        return true;
    }
    MoveLog(obj, "move collided stuck");
    return false;
}

bool Map::flock(Object* obj, fixed distance, MapLocation duplicate) {
    FPoint point = obj->point();
    
    if (locatablePosition(obj, point, NULL, duplicate) == MapLocationCollided) {
        FPoint diff = point - obj->point();
        if (diff.length() > distance) {
            diff.normalize();
            diff *= distance;
            point = obj->point() + diff;        //point 의 slabId는 유지됨
        }
        obj->locate(point);
        return true;
    }
    return false;
}
