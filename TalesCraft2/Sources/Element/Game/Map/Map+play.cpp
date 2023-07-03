//
//  Map+play.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "FogBuilding.h"

#define VisionFrameDivisionUnitFactor	50
#define VisionFrameDivisionMax          4

static const fixed BlockVisionAltitude(fixed::Raw, 98304);

bool Map::isVisible(const IBounds& bounds) const {
    CSAssert(bounds.src.x >= 0 && bounds.src.y >= 0 && bounds.dest.x < _terrain->width() && bounds.dest.y < _terrain->height(), "out of bounds");
    
    IPoint p;
    for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
        for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
            if (tile(p).isVisible()) {
                return true;
            }
        }
    }
    return false;
}
bool Map::isVisible(const IBounds& bounds, int alliance) const {
    CSAssert(bounds.src.x >= 0 && bounds.src.y >= 0 && bounds.dest.x < _terrain->width() && bounds.dest.y < _terrain->height(), "out of bounds");
    
    IPoint p;
    for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
        for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
            if (tile(p).isVisible(alliance)) {
                return true;
            }
        }
    }
    return false;
}
bool Map::isDetected(const IBounds& bounds, int alliance) const {
    CSAssert(bounds.src.x >= 0 && bounds.src.y >= 0 && bounds.dest.x < _terrain->width() && bounds.dest.y < _terrain->height(), "out of bounds");
    
    IPoint p;
    for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
        for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
            if (tile(p).isDetected(alliance)) {
                return true;
            }
        }
    }
    return false;
}

bool Map::isSpawnable(const FPoint& point, const Force* force) const {
    synchronized(_phaseLock, GameLockMap) {
        foreach(const Unit*, unit, _units) {
            if (unit->force() == force && unit->compareType(ObjectMaskBase)) {
                if (CSMath::abs(unit->point().x - point.x) <= SpawnWidth && CSMath::abs(unit->point().y - point.y) <= SpawnHeight) return true;
            }
        }
    }
    return false;
}

void Map::resetVision() {
	synchronized(_phaseLock, GameLockMap) {		//비전 업데이트가 일부만 되서 이상하게 보이지 않게 처리
		IPoint p;
		for (p.y = 0; p.y < _terrain->height(); p.y++) {
			for (p.x = 0; p.x < _terrain->width(); p.x++) {
				Tile& tile = this->tileImpl(p);
				if (tile.updateMinimap()) {
					_minimapPoints->addObject(p);
				}
				if (tile.updateVision()) {
					_visionPoints->addObject(p);
				}
			}
        }
    }
}

void Map::changeVision(const Object* obj, const FPoint& point, fixed range, bool detecting, int alliance, bool visible, bool lock) {
    bool bushVisible;
    
    fixed originAltitude = fixed::Max;
    
    if (obj) {
        range += obj->collider();
        
        if (obj->compareType(ObjectMaskAir)) {
            bushVisible = false;
        }
        else {
            bushVisible = tile(point).attribute() == TileAttributeBush;
            if (obj->compareType(ObjectMaskUnit)) {
                originAltitude = _terrain->altitude(point);
            }
        }
    }
    else {
        bushVisible = tile(point).attribute() == TileAttributeBush;
    }
    fixed range2 = range * range;
    fixed edge2 = range > fixed::One ? (range - fixed::One) * (range - fixed::One) : fixed::Zero;
    
    IBounds bounds(point, range, true);

#ifdef CS_ASSERT_DEBUG
	if (!lock) _phaseLock.assertOnActive();		//중복락 회피
#endif
    
    synchronized(_phaseLock, GameLockMap, lock) {	//비전 업데이트가 일부만 되서 이상하게 보이지 않게 처리
		IPoint p;
		for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
            for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                Tile& tile = this->tileImpl(p);
                
                switch (tile.attribute()) {
                    case TileAttributeBush:
                        if (!bushVisible) {
                            continue;
                        }
                        break;
                    case TileAttributeHidden:
                        continue;
                }
                fixed d2 = point.distanceSquared(FPoint(p.x + fixed::Half, p.y + fixed::Half));
                
                if (d2 < range2) {
                    fixed altitude = tile.altitude();
                    fixed rate;
                    if (originAltitude < altitude - BlockVisionAltitude) {
                        continue;
                    }
                    else if (originAltitude < altitude) {
                        static const fixed ReverseBlockVisionAltitude = fixed::One / BlockVisionAltitude;
                        
                        rate = fixed::One - (altitude - originAltitude) * ReverseBlockVisionAltitude;
                    }
                    else {
                        rate = fixed::One;
                    }
                    if (d2 > edge2) {
                        rate *= range - CSMath::sqrt(d2);
                    }
                    Tile::ChangeVisionReturn rtn = tile.changeVision(alliance, detecting, visible, rate);
                    if (rtn.minimapUpdated) {
                        _minimapPoints->addObject(p);
                    }
                    if (rtn.visionUpdated) {
                        _visionPoints->addObject(p);
                    }
                }
            }
        }
    }
}

void Map::changeMinimap(const Object* obj) {
    IBounds bounds = obj->bounds();

    synchronized(_phaseLock, GameLockMap) {
        IPoint p;
        for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
            for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                if (tileImpl(p).updateMinimap()) {
                    _minimapPoints->addObject(p);
                }
            }
        }
    }
}

Unit* Map::findRefinery(const FPoint& point) {
    synchronized(_phaseLock, GameLockMap) {
        for (TileObject* current = tileImpl(point).objects(); current; current = current->next()) {
            if (current->object()->compareType(ObjectMaskRefinery) && current->object()->point() == point) {
                return static_cast<Unit*>(current->object());
            }
        }
    }
    return NULL;
}
Resource* Map::findGas(const FPoint& point) {
    synchronized(_phaseLock, GameLockMap) {
        for (TileObject* current = tileImpl(point).objects(); current; current = current->next()) {
            if (current->object()->compareType(ObjectMaskGas) && current->object()->point() == point) {
                return static_cast<Resource*>(current->object());
            }
        }
    }
    return NULL;
}

Resource* Map::findResource(const Unit* unit, int kind, bool inArea) {
	AssertOnUpdateThread();

	fixed minDistanceToArea = fixed::Max;

	Resource* resource = NULL;

	if (inArea) {
		const Area* nearArea = NULL;
		foreach(const Area&, area, _areas) {
			fixed d = _navMesh->distance(unit->point(), area.point);
			if (d < minDistanceToArea) {
				nearArea = &area;
				minDistanceToArea = d;
			}
		}
		if (nearArea) {
			bool hasBase = false;
			synchronized(_phaseLock, GameLockMap) {
				for (const TileObject* current = tile(nearArea->point).objects(); current; current = current->next()) {
					if (current->object()->compareType(ObjectMaskBase)) {
						const Unit* base = static_cast<const Unit*>(current->object());
						if (base->force() == unit->force()) {
							hasBase = true;
							break;
						}
					}
				}
			}
			if (hasBase) {
				fixed heuristic = fixed::Max;

				foreach(Resource*, r, _resources) {
					if (r->kind == kind && nearArea->contains(r->point()) && r->isGatherabled(unit->force())) {
						fixed h = unit->point().distanceSquared(r->point()) + (r->gatherings() << 16);

						if (h < heuristic) {
							heuristic = h;
							resource = r;
						}
					}
				}
			}
		}
	}
	else {
		int maxBaseOrder = 1;

		foreach(const Area&, area, _areas) {
			int baseOrder = 1;
			synchronized(_phaseLock, GameLockMap) {
				for (const TileObject* current = tile(area.point).objects(); current; current = current->next()) {
					if (current->object()->compareType(ObjectMaskBase)) {
						const Unit* base = static_cast<const Unit*>(current->object());
						if (base->force() && unit->force()) {
							if (base->force() == unit->force()) {
								if (base->building() <= BuildingTransform) {
									baseOrder = 2;
								}
							}
							else if (base->force()->alliance() != unit->force()->alliance()) {
								baseOrder = 0;
							}
						}
						break;
					}
				}
			}
			if (baseOrder >= maxBaseOrder) {
				bool first = true;

				fixed heuristic = fixed::Max;

				foreach(Resource*, r, _resources) {
					if (r->kind == kind && area.contains(r->point()) && r->isGatherabled(unit->force())) {
						if (first) {
							fixed d = _navMesh->distance(unit->point(), area.point);
							if (d == fixed::Max || (baseOrder == maxBaseOrder && d >= minDistanceToArea)) break;
							minDistanceToArea = d;
							maxBaseOrder = baseOrder;
							first = false;
						}
						fixed h = unit->point().distanceSquared(r->point()) + (r->gatherings() << 16);

						if (h < heuristic) {
							heuristic = h;
							resource = r;
						}
					}
				}
			}
		}
	}

	return resource;
}

Resource* Map::findResource(const Unit* unit, const Resource* originResource) {
    AssertOnUpdateThread();
    
    Resource* resource = NULL;
    
    fixed heuristic = fixed::Max;
    
	foreach(Resource*, r, _resources) {
		if (r->kind == originResource->kind && r->group() == originResource->group() && r->isGatherabled(unit->force())) {
			fixed h = unit->point().distanceSquared(r->point()) + (r->gatherings() << 16);

			if (h < heuristic) {
				heuristic = h;
				resource = r;
			}
		}
	}
    return resource;
}

bool Map::isResourceRemaining(const Resource* originResource) {
	AssertOnUpdateThread();

	if (originResource->amount) return true;

	foreach(Resource*, r, _resources) {
		if (r->kind == originResource->kind && r->group() == originResource->group() && r->amount) {
			return true;
		}
	}
	return false;
}

Unit* Map::findBase(const Unit* unit) {
    AssertOnUpdateThread();
    
    Unit* base = NULL;
    
    fixed heuristic = fixed::Max;
    
    foreach(Unit*, u, _units) {
        if (u->compareType(ObjectMaskBase) && u->force() == unit->force() && u->isLocated() && u->building() <= BuildingTransform) {
            fixed h = _navMesh->distance(unit->point(), u->point());
            
            if (h < heuristic) {
                heuristic = h;
                base = u;
            }
        }
    }
    return base;
}

typename Map::GatheringStateReturn Map::findGatheringState(const Unit* unit, int kind) {
    GatheringStateReturn rtn = {};
    if (!unit->force()) {
        return rtn;
    }
    const Area* area = findArea(unit);
    
    if (!area) {
        return rtn;
    }
	synchronized_to_update_thread(_phaseLock, GameLockMap) {
        foreach(const Resource*, resource, _resources) {
            if (resource->kind == kind && area->contains(resource->point()) && resource->isGatherabled(unit->force())) {
                rtn.current += resource->gatherings();
                rtn.max += resource->data().gatherings;
            }
        }
    }
    return rtn;
}

const Area* Map::findArea(const Unit* unit) {
    const Area* nearArea = NULL;
    
    fixed distance2 = fixed::Max;
    
	foreach(const Area&, area, _areas) {
		synchronized(_phaseLock, GameLockMap) {
            for (const TileObject* current = tile(area.point).objects(); current; current = current->next()) {
                if (current->object()->compareType(ObjectMaskBase)) {
                    const Unit* base = static_cast<const Unit*>(current->object());
                    if (base->force() == unit->force() && base->building() <= BuildingTransform) {
                        fixed d2 = area.point.distanceSquared(unit->point());
                        if (d2 <= area.frange2 && d2 < distance2) {
                            nearArea = &area;
                            distance2 = d2;
                        }
                        break;
                    }
                }
            }
        }
    }
    return nearArea;
}

int Map::unitCount(const Force* force, const UnitIndex& index) const {
    int count = 0;
    synchronized_to_update_thread(_phaseLock, GameLockMap) {
        foreach(const Unit*, unit, _units) {
            if (unit->force() == force) {
                if (unit->building() == BuildingTransform) {
                    if (unit->originIndex() == index) {
                        count++;
                    }
                }
                else if (unit->building() <= BuildingTargetProgress) {
                    if (unit->index() == index) {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

CSDictionary<UnitIndex, int>* Map::unitCounts(const Force* force, uint typeMask) const {
	CSDictionary<UnitIndex, int>* counts = CSDictionary<UnitIndex, int>::dictionary();

	synchronized_to_update_thread(_phaseLock, GameLockMap) {
		foreach(const Unit*, unit, _units) {
			if (unit->force() == force && unit->compareType(typeMask)) {
				UnitIndex index;
				if (unit->building() == BuildingTransform) index = unit->originIndex();
				else if (unit->building() <= BuildingTargetProgress) index = unit->index();
				else continue;

				int* count = counts->tryGetObjectForKey(index);
				if (!count) counts->setObject(index, 1);
				else (*count)++;
			}
		}
	}
	return counts;
}


const Unit* Map::findUnit(const Force* force, const UnitIndex& index) const {
	synchronized_to_update_thread(_phaseLock, GameLockMap) {
		foreach(const Unit*, unit, _units) {
			if (unit->force() == force) {
				if (unit->building() == BuildingTransform) {
					if (unit->originIndex() == index) {
						unit->retain();
						unit->autorelease();
						return unit;
					}
				}
				else if (unit->building() <= BuildingTargetProgress) {
					if (unit->index() == index) {
						unit->retain();
						unit->autorelease();
						return unit;
					}
				}
			}
		}
	}
	return NULL;
}

bool Map::findUnitIndex(uint key, uint& result) const {
    AssertOnUpdateThread();
    
    int s = 0;
    int e = _units->count();
    
    for (; ; ) {
        if (s >= e) {
            result = e;
            return false;
        }
        int index = (s + e) / 2;
        
        const Unit* otherUnit = _units->objectAtIndex(index);
        
        if (otherUnit->key == key) {
            result = index;
            return true;
        }
        else if (otherUnit->key < key) {
            s = index + 1;
        }
        else {
            e = index;
        }
    }
}
bool Map::findResourceIndex(uint key, uint& result) const {
    AssertOnUpdateThread();
    
    int s = 0;
    int e = _resources->count();
    
    for (; ; ) {
        if (s >= e) {
            result = e;
            return false;
        }
        int index = (s + e) / 2;
        
        const Resource* otherResource = _resources->objectAtIndex(index);
        
        if (otherResource->key == key) {
            result = index;
            return true;
        }
        else if (otherResource->key < key) {
            s = index + 1;
        }
        else {
            e = index;
        }
    }
}
Unit* Map::findUnit(uint key) {
    uint index;

    if (findUnitIndex(key, index)) {
        return _units->objectAtIndex(index);
    }
    CSWarnLog("unknown unit:%d", key);
    return NULL;
}
Resource* Map::findResource(uint key) {
    uint index;
    
    if (findResourceIndex(key, index)) {
        return _resources->objectAtIndex(index);
    }
    CSWarnLog("unknown resource:%d", key);
    return NULL;
}
void Map::registerObject(Object* obj) {
    synchronized(_phaseLock, GameLockMap) {
        if (!_objects->containsObjectIdenticalTo(obj)) {
            _objects->addObject(obj);
        }
    }
}
void Map::registerUnit(Unit* unit) {
    uint index;
    if (!findUnitIndex(unit->key, index)) {
        synchronized(_phaseLock, GameLockMap) {
            _units->insertObject(index, unit);
        }
    }
}
void Map::unregisterUnit(Unit* unit) {
    uint index;
    if (findUnitIndex(unit->key, index)) {
        synchronized(_phaseLock, GameLockMap) {
            _units->removeObjectAtIndex(index);
        }
    }
}
void Map::registerMinimapUnit(Unit* unit) {
	synchronized(_minimapUnits, GameLockMap) {
		if (!_minimapUnits->containsObjectIdenticalTo(unit)) {
			_minimapUnits->addObject(unit);
		}
	}
}
void Map::unregisterMinimapUnit(Unit* unit) {
	synchronized(_minimapUnits, GameLockMap) {
		_minimapUnits->removeObjectIdenticalTo(unit);
	}
}
void Map::registerResource(Resource* resource) {
    uint index;
    if (!findResourceIndex(resource->key, index)) {
        synchronized(_phaseLock, GameLockMap) {
            _resources->insertObject(index, resource);
        }
    }
}
void Map::unregisterResource(Resource* resource) {
    uint index;
    if (findResourceIndex(resource->key, index)) {
        synchronized(_phaseLock, GameLockMap) {
            _resources->removeObjectAtIndex(index);
        }
    }
}
void Map::registerFogBuilding(FogBuilding* fogBuilding) {
    synchronized(_phaseLock, GameLockMap) {
        if (!_fogBuildings->containsObjectIdenticalTo(fogBuilding)) {
            _fogBuildings->addObject(fogBuilding);
        }
    }
}
void Map::unregisterFogBuilding(FogBuilding* fogBuilding) {
    synchronized(_phaseLock, GameLockMap) {
        _fogBuildings->removeObjectIdenticalTo(fogBuilding);
    }
}

void Map::effectToAll(UnitBaseGame* src, const byte* condition, const AnimationIndex& index, bool attach) {
    foreach(Unit*, unit, _units) {
        if (unit->isLocated() && unit->match(src, src->force(), condition)) {
            AnimationEffect::effect(index, unit, NULL, false, attach, false);
        }
    }
}
fixed Map::attackToAll(UnitBaseGame* src, const byte* condition, const AbilityValue<fixed>& rate, fixed time) {
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, _units) {
        if (unit->isLocated() && unit->match(src, condition)) {
            revision += src->attack(unit, rate.value(src, unit), time);
        }
    }
    return revision;
}
fixed Map::reviseToAll(UnitBaseGame* src, const byte* condition, int op, const AbilityValue<fixed>& value, fixed time) {
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, _units) {
        if (unit->isLocated() && unit->match(src, condition)) {
            revision += unit->revise(op, value.value(src, unit), time, src);
        }
    }
    return revision;
}
void Map::buffToAll(UnitBaseGame* src, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack) {
    foreach(Unit*, unit, _units) {
        if (unit->isLocated() && unit->match(src, condition)) {
            unit->buff(index, timeOp, time.value(src, unit), stack.value(src, unit), src);
        }
    }
}
void Map::freezeToAll(UnitBaseGame* src, const byte* condition, const AbilityValue<fixed>& time) {
    foreach(Unit*, unit, _units) {
        if (unit->isLocated() && unit->match(src, condition)) {
            unit->freeze(time.value(src, unit));
        }
    }
}

static FPoint pointOfUnitsWithRange(const FPoint& point, const Object* target, bool nearby) {
    return target ? (nearby ? target->facingPoint(point) : target->point()) : point;
}

CSArray<Unit>* Map::unitsWithRange(const UnitBaseGame* match, const Object* origin, const FPoint& point, fixed range, const byte* condition) {
    AssertOnUpdateThread();
    
    CSArray<Unit>* units = CSArray<Unit>::array();
    
	const Force* matchForce = match ? match->force() : NULL;
	const Unit* matchUnit = origin->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(origin) : NULL;

    IBounds bounds(point, range, true);
    
    if (bounds.size() < _units->count()) {
        IPoint p;
        
        synchronized(_markLock, GameLockMap) {
            _mark++;
            
            for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
					synchronized(_phaseLock, GameLockMap) {
						for (TileObject* current = tileImpl(p).objects(); current; current = current->next()) {
							Object* obj = current->object();
							if (obj->mark(_mark) && obj->compareType(ObjectMaskUnit)) {
								fixed d2 = obj->point().distanceSquared(point);
								fixed c = range + obj->collider();
								if (d2 <= c * c) {
									Unit* unit = static_cast<Unit*>(obj);

									if (unit->match(matchUnit, matchForce, condition)) {
										units->addObject(unit);
									}
								}
							}
						}
					}
                }
            }
        }
    }
    else {
        foreach (Unit*, unit, _units) {
            if (unit->isLocated()) {
                fixed d2 = unit->point().distanceSquared(point);
                fixed c = range + unit->collider();
                if (d2 <= c * c && unit->match(matchUnit, matchForce, condition)) {
                    units->addObject(unit);
                }
            }
        }
    }
    return units;
}

void Map::effectWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach) {
    foreach(Unit*, unit, unitsWithRange(src, origin, pointOfUnitsWithRange(origin->point(), target, nearby), range, condition)) {
        AnimationEffect::effect(index, origin, unit, nearbyTo, attach, false);
    }
}
fixed Map::attackWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time) {
    FPoint point = pointOfUnitsWithRange(origin->point(), target, nearby);
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithRange(src, origin, point, range, condition)) {
        fixed r = rate.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distance(point) - unit->collider();

            if (d >= range) {
                r *= a;
            }
            else if (d > fixed::Zero) {
                r *= CSMath::lerp(fixed::One, a, d / range);
            }
        }
        revision += src->attack(unit, r, time);
    }
    return revision;
}
fixed Map::reviseWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time) {
    FPoint point = pointOfUnitsWithRange(origin->point(), target, nearby);
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithRange(src, origin, point, range, condition)) {
        fixed v = value.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distance(point) - unit->collider();
            
            if (d >= range) {
                v *= a;
            }
            else if (d > fixed::Zero) {
                v *= CSMath::lerp(fixed::One, a, d / range);
            }
        }
        revision += unit->revise(op, v, time, src);
    }
    return revision;
}
void Map::buffWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack) {
    foreach(Unit*, unit, unitsWithRange(src, origin, pointOfUnitsWithRange(origin->point(), target, nearby), range, condition)) {
        unit->buff(index, timeOp, time.value(src, unit), stack.value(src, unit), src);
    }
}
void Map::escapeWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& time) {
    foreach(Unit*, unit, unitsWithRange(src, origin, pointOfUnitsWithRange(origin->point(), target, nearby), range, condition)) {
        unit->escape(origin->point(), time.value(src, unit));
    }
}
void Map::jumpWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing, bool forward) {
    FPoint point = pointOfUnitsWithRange(origin->point(), target, nearby);
    fixed fd = target == NULL || forward ? origin->collider() : fixed::Zero;
    foreach(Unit*, unit, unitsWithRange(src, origin, point, range, condition)) {
        fixed d = fd + distance.value(src, unit);
        unit->jump(forward ? origin->point() : point, d, altitude, time, pushing);
    }
}
void Map::warpWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, bool vision) {
    if (!vision || !src->force() || tile(IPoint(origin->point())).isVisible(src->force()->alliance())) {
        foreach(Unit*, unit, unitsWithRange(src, origin, pointOfUnitsWithRange(origin->point(), target, nearby), range, condition)) {
            unit->warp(origin->point());
        }
    }
}
void Map::freezeWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& time) {
    foreach(Unit*, unit, unitsWithRange(src, origin, pointOfUnitsWithRange(origin->point(), target, nearby), range, condition)) {
        unit->freeze(time.value(src, unit));
    }
}

CSArray<Unit>* Map::unitsWithAngle(const UnitBaseGame* match, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition) {
    AssertOnUpdateThread();
    
    CSArray<Unit>* units = CSArray<Unit>::array();
    
	const FPoint& point = origin->point();

    if (target->point() != point) {
		const Force* matchForce = match ? match->force() : NULL;
		const Unit* matchUnit = origin->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(origin) : NULL;

        FPoint p0, p1;
        p0 = p1 = target->point() - point;
        p0.rotate(angle / 2);
        p1.rotate(-angle / 2);
        p0 += point;
        p1 += point;
        
        IBounds bounds(point, range, true);
        
        if (bounds.size() < _units->count()) {
            IPoint p;
            
            synchronized(_markLock, GameLockMap) {
                _mark++;
                
                for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                    for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
						synchronized(_phaseLock, GameLockMap) {
							for (TileObject* current = tileImpl(p).objects(); current; current = current->next()) {
								Object* obj = current->object();
								if (obj->mark(_mark) && obj->compareType(ObjectMaskUnit)) {
									fixed d2 = obj->point().distanceSquared(point);
									fixed c = range + obj->collider();
									if (d2 <= c * c && obj->point().between(point, p0, p1)) {
										Unit* unit = static_cast<Unit*>(obj);

										if (unit->match(matchUnit, matchForce, condition)) {
											units->addObject(unit);
										}
									}
								}
							}
						}
                    }
                }
            }
        }
        else {
            foreach (Unit*, unit, _units) {
                if (unit->isLocated()) {
                    fixed d2 = unit->point().distanceSquared(point);
                    fixed c = range + unit->collider();
                    if (d2 <= c * c && unit->point().between(point, p0, p1) && unit->match(matchUnit, matchForce, condition)) {
                        units->addObject(unit);
                    }
                }
            }
        }
    }
    return units;
}

void Map::effectWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach) {
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        AnimationEffect::effect(index, origin, unit, nearbyTo, attach, false);
    }
}
fixed Map::attackWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time) {
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        fixed r = rate.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distance(origin->point()) - unit->collider() - origin->collider();
            
            if (d >= range) {
                r *= a;
            }
            else if (d > fixed::Zero) {
                r *= CSMath::lerp(fixed::One, a, d / range);
            }
        }
        revision += src->attack(unit, r, time);
    }
    return revision;
}
fixed Map::reviseWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time) {
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        fixed v = value.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distance(origin->point()) - unit->collider() - origin->collider();
            
            if (d >= range) {
                v *= a;
            }
            else if (d > fixed::Zero) {
                v *= CSMath::lerp(fixed::One, a, d / range);
            }
        }
        revision += unit->revise(op, v, time, src);
    }
    return revision;
}
void Map::buffWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack) {
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        unit->buff(index, timeOp, time.value(src, unit), stack.value(src, unit), src);
    }
}
void Map::escapeWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& time) {
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        unit->escape(origin->point(), time.value(src, unit));
    }
}
void Map::jumpWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing) {
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        unit->jump(origin->point(), distance.value(src, unit) + origin->collider(), altitude, time, pushing);
    }
}
void Map::freezeWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& time) {
    foreach(Unit*, unit, unitsWithAngle(src, origin, target, range, angle, condition)) {
        unit->freeze(time.value(src, unit));
    }
}

CSArray<Unit>* Map::unitsWithLine(const UnitBaseGame* match, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, FPoint& p0, FPoint& p1) {
    AssertOnUpdateThread();
    
    CSArray<Unit>* units = CSArray<Unit>::array();
    
	const FPoint& point = origin->point();

    if (point != target->point()) {
		const Force* matchForce = match ? match->force() : NULL;
		const Unit* matchUnit = origin->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(origin) : NULL;

        FPoint n = FPoint::normalize(target->point() - point);
        p0 = point + n * thickness;
        p1 = point + n * range;
        
        foreach (Unit*, unit, _units) {
            if (unit->isLocated()) {
                fixed c = thickness + unit->collider();

                if (unit->point().distanceSquaredToLine(p0, p1) <= c * c && unit->match(matchUnit, matchForce, condition)) {
                    units->addObject(unit);
                }
            }
        }
    }
    return units;
}

void Map::effectWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach) {
    FPoint p0, p1;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        AnimationEffect::effect(index, origin, unit, nearbyTo, attach, false);
    }
}
fixed Map::attackWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time) {
    FPoint p0, p1;
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        fixed r = rate.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distanceToLine(p0, p1) - unit->collider();
            
            if (d >= thickness) {
                r *= a;
            }
            else if (d > fixed::Zero) {
                r *= CSMath::lerp(fixed::One, a, d / thickness);
            }
        }
        revision += src->attack(unit, r, time);
    }
    return revision;
}
fixed Map::reviseWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time) {
    FPoint p0, p1;
    fixed revision = fixed::Zero;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        fixed v = value.value(src, unit);
		fixed a = attenuation.value(src, unit);
        if (a != fixed::One) {
            fixed d = unit->point().distanceToLine(p0, p1) - unit->collider();
            
            if (d >= thickness) {
                v *= a;
            }
            else if (d > fixed::Zero) {
                v *= CSMath::lerp(fixed::One, a, d / thickness);
            }
        }
        revision += unit->revise(op, v, time, src);
    }
    return revision;
}
void Map::buffWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack) {
    FPoint p0, p1;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        unit->buff(index, timeOp, time.value(src, unit), stack.value(src, unit), src);
    }
}
void Map::escapeWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& time) {
    FPoint p0, p1;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        unit->escape(origin->point(), time.value(src, unit));
    }
}
void Map::jumpWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing) {
    FPoint p0, p1;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        unit->jump(origin->point(), distance.value(src, unit) + origin->collider(), altitude, time, pushing);
    }
}
void Map::freezeWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& time) {
    FPoint p0, p1;
    foreach(Unit*, unit, unitsWithLine(src, origin, target, range, thickness, condition, p0, p1)) {
        unit->freeze(time.value(src, unit));
    }
}

Map::WatchReturn Map::attackWatch(const Unit* unit, const CSArray<uint>* exclusions) {
    AssertOnUpdateThread();
    
    WatchReturn rtn = {NULL, false};
    
    if (!unit->force()) {
        return rtn;
    }
    int alliance = unit->force()->alliance();
    
    fixed maxRange = CSMath::max(unit->maxAbility(AbilityAttackRange), unit->sight());
    fixed collider = unit->collider();
    fixed distance = fixed::Max;
    bool moveable = unit->ability(AbilityMoveSpeed) != fixed::Zero;
    bool first = false;
    
    IBounds bounds(unit->point(), collider + maxRange, true);
    
    if (bounds.size() < _units->count()) {
        IPoint p;
        
        synchronized(_markLock, GameLockMap) {
            _mark++;
            
            for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                    Tile& tile = this->tileImpl(p);
                    
                    if (tile.isVisible(alliance)) {
						synchronized(_phaseLock, GameLockMap) {
							for (TileObject* current = tile.objects(); current; current = current->next()) {
								if (current->object()->mark(_mark) && current->object()->compareType(ObjectMaskUnit)) {
									Unit* target = static_cast<Unit*>(current->object());

									if (target->force() && target->force()->alliance() != alliance && target->isAlive() && target->isDetected(alliance) && 
										unit->ability(AbilityAttack, fixed::Zero, target) && (!exclusions || !exclusions->containsObjectIdenticalTo(target->key))) 
									{
										fixed d2 = unit->point().distanceSquared(target->point());
										fixed targetCollider = target->collider();
										fixed c = collider + targetCollider + maxRange + EpsilonCollider;

										if (d2 <= c * c) {
											c = unit->ability(AbilityAttackBlind, fixed::Zero, target);
											if (c) {
												c += collider + targetCollider;
												if (d2 < c * c) {
													continue;
												}
											}
											bool replace = false;
											if (moveable) {
												if (target->compareType(ObjectMaskWatchFirst)) {
													replace = !first;
													first = true;
												}
												else if (first) {
													continue;
												}
											}
											fixed d = CSMath::sqrt(d2) - collider - targetCollider - unit->ability(AbilityAttackRange, fixed::Zero, target);

											if (replace || d < distance) {
												rtn.target = target;
												distance = d;
											}
										}
									}
								}
							}
						}
                    }
                }
            }
        }
    }
    else {
        foreach(Unit*, target, _units) {
            if (target->force() && target->force()->alliance() != alliance && target->isAlive() && target->isVisible(alliance) && 
				unit->ability(AbilityAttack, fixed::Zero, target) && (!exclusions || !exclusions->containsObjectIdenticalTo(target->key)))
			{
                fixed d2 = unit->point().distanceSquared(target->point());
                fixed targetCollider = target->collider();
                fixed c = collider + targetCollider + maxRange + EpsilonCollider;
                
                if (d2 <= c * c) {
                    c = unit->ability(AbilityAttackBlind, fixed::Zero, target);
                    if (c) {
                        c += collider + targetCollider;
                        if (d2 < c * c) {
                            continue;
                        }
                    }
                    bool replace = false;
                    if (moveable) {
                        if (target->compareType(ObjectMaskWatchFirst)) {
                            replace = !first;
                            first = true;
                        }
                        else if (first) {
                            continue;
                        }
                    }
                    fixed d = CSMath::sqrt(d2) - collider - targetCollider - unit->ability(AbilityAttackRange, fixed::Zero, target);
                    
                    if (replace || d < distance) {
                        rtn.target = target;
                        distance = d;
                    }
                }
            }
        }
    }

    rtn.inside = distance <= EpsilonCollider;

    return rtn;
}

Map::WatchReturn Map::watch(const UnitBaseGame* src, const Unit* unit, const byte* condition, const AbilityValue<fixed>& range, const CSArray<uint>* exclusions) {
    AssertOnUpdateThread();
    
    WatchReturn rtn = {NULL, false};
    
    fixed maxRange = CSMath::max(range.maxValue(src), unit->sight());
    fixed collider = unit->collider();
    fixed distance = fixed::Max;
    bool moveable = unit->ability(AbilityMoveSpeed) != fixed::Zero;
    bool first = false;
    
    IBounds bounds(unit->point(), collider + maxRange, true);
    
    if (bounds.size() < _units->count()) {
        IPoint p;
        
        synchronized(_markLock, GameLockMap) {
            _mark++;
            
            for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
                    Tile& tile = this->tileImpl(p);
                    
                    if (!src->force() || tile.isVisible(src->force()->alliance())) {
						synchronized(_phaseLock, GameLockMap) {
							for (TileObject* current = tile.objects(); current; current = current->next()) {
								if (current->object()->mark(_mark) && current->object()->compareType(ObjectMaskUnit)) {
									Unit* target = static_cast<Unit*>(current->object());

									if (target->isDetected(src->force()) && target->match(unit, src->force(), condition) && (!exclusions || !exclusions->containsObjectIdenticalTo(target->key))) {
										fixed d2 = unit->point().distanceSquared(target->point());
										fixed targetCollider = target->collider();
										fixed c = collider + targetCollider + maxRange + EpsilonCollider;

										if (d2 <= c * c) {
											bool replace = false;
											if (moveable) {
												if (target->compareType(ObjectMaskWatchFirst)) {
													replace = !first;
													first = true;
												}
												else if (first) {
													continue;
												}
											}
											fixed d = CSMath::sqrt(d2) - collider - targetCollider - range.value(src, target);

											if (replace || d < distance) {
												rtn.target = target;
												distance = d;
											}
										}
									}
								}
							}
						}
                    }
                }
            }
        }
    }
    else {
        foreach(Unit*, target, _units) {
            if (target->isVisible(src->force()) && target->match(unit, src->force(), condition) && (!exclusions || !exclusions->containsObjectIdenticalTo(target->key))) {
                fixed d2 = unit->point().distanceSquared(target->point());
                fixed targetCollider = target->collider();
                fixed c = collider + targetCollider + maxRange + EpsilonCollider;
                
                if (d2 <= c * c) {
                    bool replace = false;
                    if (moveable) {
                        if (target->compareType(ObjectMaskWatchFirst)) {
                            replace = !first;
                            first = true;
                        }
                        else if (first) {
                            continue;
                        }
                    }
                    fixed d = CSMath::sqrt(d2) - collider - targetCollider - range.value(src, target);
                    
                    if (replace || d < distance) {
                        rtn.target = target;
                        distance = d;
                    }
                }
            }
        }
    }

    rtn.inside = distance <= EpsilonCollider;

    return rtn;
}

uint Map::count(const UnitBase* src, const Object* target, const byte* condition, fixed range) const {
    AssertOnUpdateThread();
    
    uint count = 0;
    
    IBounds bounds(target->point(), range + target->collider(), true);
    
    if (bounds.size() < _units->count()) {
        IPoint p;
        
        synchronized(_markLock, GameLockMap) {
            _mark++;
            
            for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
                for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
					synchronized(_phaseLock, GameLockMap) {
						for (const TileObject* current = tile(p).objects(); current; current = current->next()) {
							if (current->object()->mark(_mark) && current->object()->compareType(ObjectMaskUnit)) {
								const Unit* dest = static_cast<const Unit*>(current->object());

								if (dest->match(src, condition)) {
									fixed d2 = target->point().distanceSquared(dest->point());
									fixed c = dest->collider() + range;

									if (d2 <= c * c) {
										count++;
									}
								}
							}
						}
                    }
                }
            }
        }
    }
    else {
        foreach (Unit*, dest, _units) {
            if (dest->isLocated() && dest->match(src, condition)) {
                fixed d2 = target->point().distanceSquared(dest->point());
                fixed c = dest->collider() + range;
                
                if (d2 <= c * c) {
                    count++;
                }
            }
        }
    }
    return count;
}

bool Map::targetToAdvance(const Unit* unit, FPoint& p) const {
    AssertOnUpdateThread();
    
    if (!unit->force()) {
        return false;
    }
    fixed distance2 = fixed::Max;
    
    int alliance = unit->force()->alliance();
    
	const FPoint& sp = unit->point();

	{
		bool attackable = false;

		foreach(const Unit*, target, _units) {
			if (target->force() && target->force()->alliance() != alliance && target->isAlive()) {      //for performance
				const FPoint& tp = target->point();

				fixed d2 = sp.distanceSquared(tp);

				bool a = unit->ability(AbilityAttack, fixed::Zero, target) != fixed::Zero;

				if (attackable) {
					if (!a || d2 >= distance2) continue;
				}
				else {
					if (!a && d2 >= distance2) continue;
				}
				if (target->isVisible(alliance) && (unit->compareType(ObjectMaskAir) || target->compareType(ObjectMaskAir) || hasPath(sp, tp))) {
					if (a) attackable = true;
					distance2 = d2;
					p = tp;
				}
			}
		}
	}
    foreach(const FogBuilding*, target, _fogBuildings) {
        if (target->isLocated() && target->force && target->alliance == alliance) {
			const FPoint& tp = target->point();

            fixed d2 = sp.distanceSquared(tp);
            
            if (d2 < distance2 && (unit->compareType(ObjectMaskAir) || hasPath(sp, tp))) {
                distance2 = d2;
                p = tp;
            }
        }
    }
    
    if (distance2 == fixed::Max) {
        for (int i = 0; i < MaxPlayers; i++) {
            const Force* force = _forces[i];
            if (force && force->pointFixed && force->alliance() != alliance && force->result == GameResultNone) {
				const FPoint& tp = force->point;

                fixed d2 = sp.distanceSquared(tp);
                
                if (d2 < distance2 && (unit->compareType(ObjectMaskAir) || hasPath(sp, tp))) {
                    distance2 = d2;
                    p = tp;
                }
            }
        }
    }
    return distance2 != fixed::Max;
}
bool Map::targetToRetreat(const Unit* unit, FPoint& p) const {
    AssertOnUpdateThread();
    
    if (!unit->force()) {
        return false;
    }
    CSArray<const Unit*>* enemies = new CSArray<const Unit*>();
    int alliance = unit->force()->alliance();
    
    foreach(const Unit*, target, _units) {
        if (target->isLocated() && target->force() && target->force()->alliance() != alliance) {
            enemies->addObject(target);
        }
    }

    fixed distance2 = fixed::Max;
    
    static const fixed SafeRange2 = (fixed)81;     //9x9
    
	const FPoint& sp = unit->point();

    foreach(const Unit*, target, _units) {
        if (target->isLocated() && target->compareType(ObjectMaskBuilding) && target->force() == unit->force()) {
			const FPoint& tp = target->point();

			fixed d2 = sp.distanceSquared(tp);

			if (d2 < distance2 && (unit->compareType(ObjectMaskAir) || hasPath(sp, tp))) {
				bool safe = true;
				foreach(const Unit*, enemy, enemies) {
					if (tp.distanceSquared(enemy->point()) < SafeRange2) {
						safe = false;
						break;
					}
				}
				if (safe) {
					distance2 = d2;
					p = tp;
				}
			}
        }
    }
    if (distance2 == fixed::Max) {
        for (int i = 0; i < MaxPlayers; i++) {
            if (_forces[i] && _forces[i]->alliance() == alliance) {
                const FPoint& tp = _forces[i]->point;

				fixed d2 = sp.distanceSquared(tp);

				if (d2 < distance2 && (unit->compareType(ObjectMaskAir) || hasPath(sp, tp))) {
					bool safe = true;
					foreach(const Unit*, enemy, enemies) {
						if (tp.distanceSquared(enemy->point()) < SafeRange2) {
							safe = false;
							break;
						}
					}
					if (safe) {
						distance2 = d2;
						p = tp;
					}
				}
            }
        }
    }
    enemies->release();
    
    return distance2 != fixed::Max;
}

void Map::removeSquad(const Unit* unit) {
    AssertOnUpdateThread();
    
    foreach(CSArray<Squad>*, squads, _squads) {
        foreach(Squad&, squad, squads) {
            squad.units->removeObjectIdenticalTo(unit);
        }
    }
}

void Map::updateObjects(bool running) {
	uint count = _objects->count();

	uint i = 0;
    
    while (i < count) {
        Object* obj = _objects->objectAtIndex(i);
        
        if (obj->update(running)) {
            i++;
        }
        else {
            obj->dispose();
            synchronized(_phaseLock, GameLockMap) {
                _objects->removeObjectAtIndex(i);
            }
            count--;
        }
    }
    _findPathCount = 0;
	_visionFrameDivision = CSMath::clamp((int)_units->count() / VisionFrameDivisionUnitFactor + 1, 1, VisionFrameDivisionMax);
}

void Map::updateForces() {
    for (int i = 0; i < MaxPlayers; i++) {
        Force* force = _forces[i];
        if (force) {
            force->update();
#ifndef UseObserverPlayTest
            if (force->ai()) updateAI(force);
#endif
        }
    }
}

