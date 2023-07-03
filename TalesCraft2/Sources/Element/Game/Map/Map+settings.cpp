//
//  Map+settings.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 10. 16..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "Destination.h"

#include "Sound.h"

#include "StrokeEffect.h"

//============================================================================================================
Map::Setting::Thread::Stack::Stack(int func) : func(func), ip(0), prev(NULL) {
    
}

Map::Setting::Thread::Thread(int threadId, int func) : threadId(threadId), current(new Stack(func)), waiting(fixed::Zero), units(new CSArray<Unit>()) {

}

Map::Setting::Thread::~Thread() {
    while (current) {
        Stack* prev = current->prev;
        delete current;
        current = prev;
    }
    units->release();
}

Map::Setting::Setting() : threadId(0), threads(new CSArray<Thread*>()), voiceEnabled(true), soundEnabled(true) {
    threads->addObject(new Thread(0, 0));
    memset(timers, 0, sizeof(timers));
    memset(values, 0, sizeof(values));
}

Map::Setting::~Setting() {
    foreach(Thread*, thread, threads) {
        delete thread;
    }
    threads->release();
}

//============================================================================================================
ActionCode Map::action() const {
    if (isSingle() && _mode.context.single.action) {
        synchronized(_controlLock, GameLockMap) {
            if (_mode.context.single.action && !_mode.context.single.action->passed()) {
                return _mode.context.single.action->code();
            }
        }
    }
    return ActionNone;
}

void Map::passAction(ActionCode code, const CSArray<Unit>* units, const CommandIndex* command, const Object* target) {
    if (isSingle() && _mode.context.single.action) {
        synchronized(_controlLock, GameLockMap) {
            if (_mode.context.single.action && _mode.context.single.action->pass(code, units, command, target)) {
                _mode.context.single.actionRestriction = _mode.context.single.action->restriction();
            }
        }
    }
}

static void mergeAbilityRevision(CSArray<AbilityRevision>*& abilities, CSArray<ushort>* changeAbilityDefaults, CSArray<AbilityRevision>* changeAbilities) {
    if (changeAbilityDefaults && abilities) {
        foreach(ushort, index, changeAbilityDefaults) {
            int i = 0;
            while (i < abilities->count()) {
                const AbilityRevision& otherAbility = abilities->objectAtIndex(i);
                if (otherAbility.index == index) {
                    abilities->removeObjectAtIndex(i);
                }
                else {
                    i++;
                }
            }
        }
        if (!abilities->count()) CSObject::release(abilities);
    }
    if (changeAbilities) {
        if (abilities) {
            foreach(AbilityRevision&, newAbility, changeAbilities) {
                bool isNew = true;
                foreach(AbilityRevision&, otherAbility, abilities) {
                    if (otherAbility.index == newAbility.index && otherAbility.conditionLength == newAbility.conditionLength && (!otherAbility.condition || memcmp(otherAbility.condition, newAbility.condition, otherAbility.conditionLength) == 0)) {
                        
                        if (newAbility.value >= fixed::Zero) otherAbility.value = newAbility.value;
                        otherAbility.diff += newAbility.diff;
                        
                        isNew = false;
                        break;
                    }
                }
                if (isNew) {
                    abilities->addObject(newAbility);
                }
            }
        }
        else {
            abilities = new CSArray<AbilityRevision>(changeAbilities, false);
        }
    }
}

static void addAbilityRevision(CSArray<AbilityRevision>* abilities, int index, const byte* condition, fixed value, fixed diff) {
    AbilityRevision* ability = NULL;
    
    int conditionLength = condition ? UnitBase::matchLength(condition) : 0;
    
    foreach(AbilityRevision&, otherAbility, abilities) {
        if (otherAbility.index == index && otherAbility.conditionLength == conditionLength && (!condition || memcmp(condition, otherAbility.condition, conditionLength) == 0)) {
            ability = &otherAbility;
        }
    }
    if (!ability) {
        ability = &abilities->addObject();
        ability->index = index;
        ability->conditionLength = conditionLength;
        ability->condition = condition;
        ability->value = -fixed::One;
        ability->diff = fixed::Zero;
    }
    if (value >= fixed::Zero) ability->value = value;
    ability->diff += diff;
}

Map::UnitSetting::UnitSetting(const CSArray<Force>* forces, const byte* condition) :
    forces(CSObject::retain(forces)),
    condition(condition),
    growups(NULL),
    name(NULL),
    abilities(NULL),
	scaleBase(fixed::One),
	scaleUp(fixed::Zero)
{
    
}

Map::UnitSetting::~UnitSetting() {
    forces->release();
    CSObject::release(growups);
    CSObject::release(name);
    CSObject::release(abilities);
}

typename Map::UnitSetting& Map::addUnitSetting(const CSArray<Force>* forces, const byte* condition) {
    if (_unitSettings) {
        int len = UnitBase::matchLength(condition);

        foreach(UnitSetting&, setting, _unitSettings) {
            if (len == UnitBase::matchLength(setting.condition) && memcmp(setting.condition, condition, len) == 0) {
                if (forces->count() == setting.forces->count()) {
                    bool flag = true;
                    for (int i = 0; i < forces->count(); i++) {
                        if (forces->objectAtIndex(i) != setting.forces->objectAtIndex(i)) {
                            flag = false;
                            break;
                        }
                    }
                    if (flag) {
                        return setting;
                    }
                }
            }
        }
    }
    else {
        _unitSettings = new CSArray<UnitSetting>();
    }
    UnitSetting& setting = _unitSettings->addObject();
    new (&setting) UnitSetting(forces, condition);
    return setting;
}

bool Map::matchUnitSetting(const CSArray<Force> *forces, const byte *condition, const Unit *target) {
    if (forces->count()) {
        foreach(const Force*, force, forces) {
            if (target->match(force, condition)) {
                return true;
            }
        }
    }
    else if (target->match((const ForceBase*)NULL, condition)) {
        return true;
    }
    return false;
}

void Map::applyUnitSetting(Unit* unit) {
    foreach(const UnitSetting&, setting, _unitSettings) {
        if (matchUnitSetting(setting.forces, setting.condition, unit)) {
            foreach (const Growup*, growup, setting.growups) {
                unit->addGrowup(growup);
            }
            if (setting.name) {
                unit->rename(setting.name);
            }
            if (setting.abilities) {
                synchronized(unit, GameLockObject) {
                    mergeAbilityRevision(unit->abilities(), NULL, setting.abilities);
                }
            }
			if (setting.scaleBase != fixed::One || setting.scaleUp != fixed::Zero) {
				unit->setScale(setting.scaleBase, setting.scaleUp);
			}
        }
    }
}

//============================================================================================================
int Map::mtReadByte(const byte*& cursor) {
    switch(readByte(cursor)) {
        case 1:
            {
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                return _setting->values[valueIndex];
            }
        case 2:
			{
				int min = readByte(cursor);
				int max = readByte(cursor);
				return _random->nextInt(min, max);
			}
        case 3:
            {
                int count = readLength(cursor);
                const byte* vraw = cursor;
                cursor += count;
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                int offset = _setting->values[valueIndex];
                CSAssert(offset >= 0 && offset < count, "invalid operation");
                vraw += offset;
                return *vraw;
            }
    }
    return readByte(cursor);
}
int Map::mtReadShort(const byte*& cursor) {
    switch(readByte(cursor)) {
        case 1:
            {
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                return _setting->values[valueIndex];
            }
        case 2:
			{
				int min = readShort(cursor);
				int max = readShort(cursor);
				return _random->nextInt(min, max);
			}
        case 3:
            {
                int count = readLength(cursor);
                const byte* vraw = cursor;
                cursor += count * sizeof(short);
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                int offset = _setting->values[valueIndex];
                CSAssert(offset >= 0 && offset < count, "invalid operation");
                vraw += offset * sizeof(short);
                return bytesToShort(vraw);
            }
    }
    return readShort(cursor);
}
int Map::mtReadInt(const byte*& cursor) {
    switch(readByte(cursor)) {
        case 1:
            {
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                return _setting->values[valueIndex];
            }
        case 2:
			{
				int min = readInt(cursor);
				int max = readInt(cursor);
				return _random->nextInt(min, max);
			}
        case 3:
            {
                int count = readLength(cursor);
                const byte* vraw = cursor;
                cursor += count * sizeof(int);
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                int offset = _setting->values[valueIndex];
                CSAssert(offset >= 0 && offset < count, "invalid operation");
                vraw += offset * sizeof(int);
                return bytesToInt(vraw);
            }
    }
    return readInt(cursor);
}
fixed Map::mtReadFixed(const byte*& cursor, Setting::Thread* thread) {
    switch(readByte(cursor)) {
        case 1:
            {
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                return _setting->values[valueIndex];
            }
        case 2:
			{
				fixed min = readFixed(cursor);
				fixed max = readFixed(cursor);
				return _random->nextFixed(min, max);
			}
        case 3:
            {
                int count = readLength(cursor);
                const byte* vraw = cursor;
                cursor += count * sizeof(fixed);
                int valueIndex = readShort(cursor);
                CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
                int offset = _setting->values[valueIndex];
                CSAssert(offset >= 0 && offset < count, "invalid operation");
                vraw += offset * sizeof(fixed);
                return bytesToFixed(vraw);
            }
        case 4:
            {
                CSArray<Force>* forces = mtReadForces(cursor);
                
                int op = readByte(cursor);
                
                fixed v = fixed::Zero;
                
                switch (op) {
                    case 0:
                        {
                            int kind = readByte(cursor);
                            foreach (const Force*, force, forces) {
                                v += force->resource(kind);
                            }
                        }
                        break;
                    case 1:
                        foreach (const Force*, force, forces) {
                            v += force->population();
                        }
                        break;
                    case 2:
                        foreach (const Force*, force, forces) {
                            v += force->supply();
                        }
                        break;
                }
                return v;
            }
        case 5:
            {
                CSArray<Unit>* units = mtReadUnits(cursor, thread);
                
                int op = readByte(cursor);
                
                fixed v = fixed::Zero;
                
                switch (op) {
                    case 0:
                        return (fixed)units->count();
                    case 1:
                        {
                            int abt = readShort(cursor);
                            foreach(Unit*, unit, units) {
                                v += unit->ability(abt);
                            }
                        }
                        break;
                    case 2:
                        foreach(Unit*, unit, units) {
                            v += unit->health();
                        }
                        break;
                    case 3:
                        foreach(Unit*, unit, units) {
                            v += unit->mana();
                        }
                        break;
                }
                return v;
            }
    }
    return readFixed(cursor);
}
AssetIndex2 Map::mtReadAssetIndex2(const byte*& cursor) {
    AssetIndex2 v;
    if (readBoolean(cursor)) {
        int count = readLength(cursor);
        const byte* vraw = cursor;
        cursor += count * sizeof(AssetIndex2);
        int valueIndex = readShort(cursor);
        CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
        int offset = _setting->values[valueIndex];
        CSAssert(offset >= 0 && offset < count, "invalid operation");
        vraw += offset * sizeof(AssetIndex2);
        v = AssetIndex2(vraw);
    }
    else {
        v = AssetIndex2(cursor);
    }
    return v;
}
const CSLocaleString* Map::mtReadLocaleString(const byte*& cursor) {
    const CSLocaleString* str;
    if (readBoolean(cursor)) {
        int index0 = readShort(cursor);
        int index1 = readShort(cursor);
        
        str = Asset::sharedAsset()->strings->objectAtIndex(index0)->objectAtIndex(index1);
    }
    else {
        str = readLocaleString(cursor);
    }
    return str;
}
FPoint Map::mtReadPoint(const byte*& cursor, Setting::Thread* thread) {
	FPoint point = _points->objectAtIndex(mtReadShort(cursor));

	switch (readByte(cursor)) {
		case 1:
			{
				fixed v = mtReadFixed(cursor, thread);
				fixed x = _random->nextFixed(-v, v);
				fixed y = CSMath::sqrt(v * v - x * x);
				point.x = CSMath::clamp(point.x + x, fixed::Zero, _terrain->width() - EpsilonCollider);
				point.y = CSMath::clamp(point.y + y, fixed::Zero, _terrain->height() - EpsilonCollider);
			}
			break;
		case 2:
			{
				fixed w = mtReadFixed(cursor, thread);
				fixed h = mtReadFixed(cursor, thread);
				fixed x = _random->nextFixed(-w, w);
				fixed y = _random->nextFixed(-h, h);
				point.x = CSMath::clamp(point.x + x, fixed::Zero, _terrain->width() - EpsilonCollider);
				point.y = CSMath::clamp(point.y + y, fixed::Zero, _terrain->height() - EpsilonCollider);
			}
			break;
	}
	return point;
}
CSArray<FPoint>* Map::mtReadPoints(const byte*& cursor, Setting::Thread* thread) {
	CSArray<FPoint>* points = CSArray<FPoint>::array();

	switch (readByte(cursor)) {
		case 0:
			{
				int count = readLength(cursor);

				for (int i = 0; i < count; i++) {
					points->addObject(_points->objectAtIndex(readShort(cursor)));
				}
			}
			break;
		case 1:
			{
				FPoint center = mtReadPoint(cursor, thread);
				fixed v = mtReadFixed(cursor, thread);
				fixed x = _random->nextFixed(-v, v);
				fixed y = CSMath::sqrt(v * v - x * x);
				int count = mtReadInt(cursor);

				for (int i = 0; i < count; i++) {
					FPoint& point = points->addObject();
					point.x = CSMath::clamp(center.x + x, fixed::Zero, _terrain->width() - EpsilonCollider);
					point.y = CSMath::clamp(center.y + y, fixed::Zero, _terrain->height() - EpsilonCollider);
				}
			}
			break;
		case 2:
			{
				FPoint center = mtReadPoint(cursor, thread);
				fixed w = mtReadFixed(cursor, thread);
				fixed h = mtReadFixed(cursor, thread);
				fixed x = _random->nextFixed(-w, w);
				fixed y = _random->nextFixed(-h, h);
				int count = mtReadInt(cursor);

				for (int i = 0; i < count; i++) {
					FPoint& point = points->addObject();
					point.x = CSMath::clamp(center.x + x, fixed::Zero, _terrain->width() - EpsilonCollider);
					point.y = CSMath::clamp(center.y + y, fixed::Zero, _terrain->height() - EpsilonCollider);
				}
			}
			break;
	}
	return points;
}
CSArray<Squad*>* Map::mtReadSquads(const byte*& cursor) {
    CSArray<Squad*>* squads = CSArray<Squad*>::array();
    int squadCount = readLength(cursor);
    for (int i = 0; i < squadCount; i++) {
        int squadIndex = mtReadShort(cursor);
        foreach(Squad&, squad, _squads->objectAtIndex(squadIndex)) {
            squads->addObject(&squad);
        }
    }
    return squads;
}
CSArray<Unit>* Map::mtReadUnits(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units;
    switch (readByte(cursor)) {
        case 0:
            {
                const CSArray<Force>* forces = mtReadForces(cursor);
                const byte* condition = cursor;
                cursor += UnitBase::matchLength(condition);
                
                units = CSArray<Unit>::array();
                foreach(Unit*, unit, _units) {
                    if (matchUnitSetting(forces, condition, unit)) {
                        units->addObject(unit);
                    }
                }
            }
            break;
        case 1:
            {
                CSArray<Squad*>* squads = mtReadSquads(cursor);
                units = CSArray<Unit>::array();
                foreach(const Squad*, squad, squads) {
                    units->addObjectsFromArray(squad->units);
                }
            }
            break;
        default:
            units = thread->units;
            break;
    }
    return units;
}
CSArray<Resource>* Map::mtReadResources(const byte*& cursor, Setting::Thread* thread) {
	CSArray<Resource>* resources = CSArray<Resource>::array();
	switch (readByte(cursor)) {
		case 0:
			{
				AssetIndex2 index(cursor);
				foreach(Resource*, resource, _resources) {
					if (resource->kind == 0 && resource->index == index) {
						resources->addObject(resource);
						break;
					}
				}
			}
			break;
		case 1:
			{
				AssetIndex2 index(cursor);
				foreach(Resource*, resource, _resources) {
					if (resource->kind == 1 && resource->index == index) {
						resources->addObject(resource);
						break;
					}
				}
			}
			break;
		case 2:
			{
				int group = readShort(cursor);
				foreach(Resource*, resource, _resources) {
					if (resource->kind == 0 && resource->group() == group) {
						resources->addObject(resource);
					}
				}
			}
			break;
		case 3:
			{
				int group = readShort(cursor);
				foreach(Resource*, resource, _resources) {
					if (resource->kind == 1 && resource->group() == group) {
						resources->addObject(resource);
					}
				}
			}
			break;
	}
	return resources;
}
CSArray<Force>* Map::mtReadForces(const byte*& cursor) {
    CSArray<Force>* forces = CSArray<Force>::array();
    
    switch (readByte(cursor)) {
        case 0:
            for (int i = 0; i < MaxPlayers; i++) {
                if (_forces[i]) {
                    forces->addObject(_forces[i]);
                }
            }
            break;
        case 1:
            {
                int alliance = mtReadByte(cursor);
                for (int i = 0; i < MaxPlayers; i++) {
                    if (_forces[i] && _forces[i]->alliance() == alliance) {
                        forces->addObject(_forces[i]);
                    }
                }
            }
            break;
        case 2:
            {
                int forceCount = readLength(cursor);
                for (int i = 0; i < forceCount; i++) {
                    int forceIndex = mtReadByte(cursor);
                    CSAssert(forceIndex >= 0 && forceIndex < MaxPlayers, "invalid state");
                    
                    if (_forces[forceIndex]) {
                        CSAssert(!forces->containsObjectIdenticalTo(_forces[forceIndex]), "invalid state");
                        
                        int j = 0;
                        while (j < forces->count()) {
                            const Force* other = forces->objectAtIndex(j);
                            if (other->index == forceIndex) {
                                j = -1;
                                break;
                            }
                            else if (other->index > forceIndex) {
                                break;
                            }
                            j++;
                        }
                        if (j != - 1) forces->insertObject(j, _forces[forceIndex]);
                    }
                }
            }
            break;
    }
    return forces;
}

bool Map::mtReadPlaying(const byte*& cursor) {
    CSArray<Force>* forces = mtReadForces(cursor);
    bool includeObserver = readBoolean(cursor);
    
    return _playing == PlayingParticipate ? forces->containsObjectIdenticalTo(_force) : includeObserver;
}
//============================================================================================================

void Map::mtMeleeStart(const byte*& cursor, Setting::Thread* thread) {
    int count = readLength(cursor);
    
    for (int i = 0; i < count; i++) {
        UnitIndex index(cursor);
        FPoint np(cursor);
        for (int j = 0; j < MaxPlayers; j++) {
            if (_forces[j]) {
                Unit* unit = Unit::unit(_forces[j], index, 0, 0, NULL);

                FPoint rnp = np;
                rnp.rotate(_forces[j]->pointDir * fixed::PiOverTwo);
                FPoint p = _forces[j]->point + rnp;
                
                if (unit->compareType(ObjectMaskBase)) {
                    fixed distance = fixed::Max;
                    FPoint bp = p;
                    foreach(const Area&, area, _areas) {
                        bool enabled = true;
                        
                        synchronized(_phaseLock, GameLockMap) {
                            for (const TileObject* current = tile(area.point).objects(); current; current = current->next()) {
                                if (current->object()->compareType(ObjectMaskBase)) {
                                    enabled = false;
                                    break;
                                }
                            }
                        }
                        if (enabled) {
                            fixed d = area.point.distanceSquared(p);
                            if (d < distance) {
                                bp = area.point;
                                distance = d;
                            }
                        }
                    }
                    p = bp;
                }
                else if (unit->compareType(ObjectMaskRefinery)) {
                    fixed distance = fixed::Max;
                    FPoint bp = p;
                    
                    foreach(const Resource*, resource, _resources) {
                        if (resource->kind == 1) {
                            bool enabled = true;
                                
							synchronized(_phaseLock, GameLockMap) {
								for (const TileObject* current = tile(resource->point()).objects(); current; current = current->next()) {
									if (current->object()->compareType(ObjectMaskRefinery)) {
										enabled = false;
										break;
									}
								}
							}
                            if (enabled) {
                                fixed d = resource->point().distanceSquared(p);
                                if (d < distance) {
                                    bp = resource->point();
                                    distance = d;
                                }
                            }
                        }
                    }
                    p = bp;
                }
                if (Map::sharedMap()->locatablePosition(unit->type(), p, unit->collider(), NULL, MapLocationEnabled) == MapLocationUnabled) {
                    p = _forces[j]->point;
                }
                unit->locate(p);
            }
        }
    }
    fixed resource0 = (fixed)readShort(cursor);
	fixed resource1 = (fixed)readShort(cursor);
    for (int i = 0; i < MaxPlayers; i++) {
        if (_forces[i]) {
            _forces[i]->reviseResource(0, resource0);
            _forces[i]->reviseResource(1, resource1);
        }
    }
    thread->current->ip++;
}

void Map::mtMeleePlay(const byte*& cursor, Setting::Thread* thread) {
#ifdef UseBenchMarkTest
	for (int i = 0; i < MaxPlayers; i++) {
		Force* force = _forces[i];

		if (force && force->population() < force->supply()) {
			const CSArray<Card>* cards = force->cards();

			const Card* card = cards->objectAtIndex(_random->nextInt(0, cards->count() - 1));

			const UnitData& data = card->data();

			if (Object::compareType(data.type, ObjectMaskAttacker)) {
				Unit *unit = Unit::unit(force, card->index(), 1, 0, NULL);

				FPoint p = force->point;
				if (Map::sharedMap()->locatablePosition(unit->type(), p, unit->collider(), NULL, MapLocationUnabled) == MapLocationUnabled) {
					p = force->point;
				}
				unit->locate(p);

				fixed e = fixed::Zero;
				unit->command(CommandIndex(1, 1), NULL, e, 0, 1, false);
			}
		}
	}
#endif
	switch (_mode.originCode) {
		case GameModeTestOffline:
		case GameModeTestOnline:
			if (_playing == PlayingParticipate) {
				bool alone = true;
				for (int i = 0; i < MaxPlayers; i++) {
					Force* force = _forces[i];
					if (force && force->alliance() != _force->alliance()) {
						alone = false;
					}
				}
				if (alone) {
					goto fin;
				}
			}
			break;
	}
	{
		bool alives[MaxAlliances] = {};
		int aliveCount = 0;
		foreach(const Unit*, unit, _units) {
			if (unit->compareType(ObjectMaskBase) && unit->force() && unit->force()->isAlive() && !alives[unit->force()->alliance()]) {
				alives[unit->force()->alliance()] = true;
				aliveCount++;
			}
		}
		if (aliveCount == 0) {
			for (int i = 0; i < MaxPlayers; i++) {
				Force* force = _forces[i];
				if (force) {
					force->result = GameResultDraw;
				}
			}
			Sound::stop(SoundControlGameBgm);
			_running = false;
			_state = MapStateEnd;
		}
		else {
			for (int i = 0; i < MaxPlayers; i++) {
				Force* force = _forces[i];
				if (force) {
					if (alives[force->alliance()]) {
						if (aliveCount == 1) force->result = GameResultVictory;
					}
					else force->result = GameResultDefeat;
				}
			}
			if (_playing == PlayingParticipate) {
				if (!alives[_force->alliance()]) {
					Sound::stop(SoundControlGameBgm);
					_running = false;
					_state = MapStateDefeat;
				}
				else if (aliveCount == 1) {
					Sound::stop(SoundControlGameBgm);
					_running = false;
					_state = MapStateVictory;
				}
			}
			else {
				if (aliveCount == 1) {
					Sound::stop(SoundControlGameBgm);
					_running = false;
					_state = MapStateEnd;
				}
			}
		}
	}
fin:
	thread->current->ip++;
}

void Map::mtHasForce(const byte*& cursor, Setting::Thread* thread) {
	CSArray<Force>* forces = mtReadForces(cursor);
	bool melee = readBoolean(cursor);
	int ip1 = readShort(cursor);

	bool alive = false;
	foreach(const Force*, force, forces) {
		if (force->result == GameResultNone && force->isAlive()) {
			alive = true;
			break;
		}
	}
	if (alive && melee) {
		alive = false;
		foreach(const Unit*, unit, _units) {
			if (unit->compareType(ObjectMaskBase) && forces->containsObjectIdenticalTo(unit->force())) {
				alive = true;
				break;
			}
		}
	}
	thread->current->ip = alive ? thread->current->ip + 1 : ip1;
}

void Map::mtCall(const byte*& cursor, Setting::Thread* thread) {
    int func = mtReadShort(cursor);
    CSAssert(func >= 0 && func < _triggers->count(), "invalid operation");
    bool isNew = readBoolean(cursor);
    
    thread->current->ip++;
    
    if (isNew) {
        _setting->threads->addObject(new Setting::Thread(++_setting->threadId, func));
    }
    else {
        Setting::Thread::Stack* stack = new Setting::Thread::Stack(func);
        stack->prev = thread->current;
        thread->current = stack;
    }
}
void Map::mtValue(const byte*& cursor, Setting::Thread* thread) {
    int valueIndex = readShort(cursor);
    CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
    fixed value = mtReadFixed(cursor, thread);
    int combineOp = readByte(cursor);
    
    switch (combineOp) {
        case 0:
            _setting->values[valueIndex] = value;
            break;
        case 1:
            _setting->values[valueIndex] += value;
            break;
        case 2:
            _setting->values[valueIndex] -= value;
            break;
        case 3:
            _setting->values[valueIndex] *= value;
            break;
        case 4:
            if (_setting->values[valueIndex] > value) {
                _setting->values[valueIndex] = value;
            }
            break;
        case 5:
            if (_setting->values[valueIndex] < value) {
                _setting->values[valueIndex] = value;
            }
            break;
    }
    thread->current->ip++;
}
bool Map::mtCompareValue(const byte*& cursor, Setting::Thread* thread) {
    int valueIndex = readShort(cursor);
    CSAssert(valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
    fixed value = mtReadFixed(cursor, thread);
    int compareOp = readByte(cursor);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    bool flag = false;
    
    switch (compareOp) {
        case 0:
            flag = _setting->values[valueIndex] == value;
            break;
        case 1:
            flag = _setting->values[valueIndex] <= value;
            break;
        case 2:
            flag = _setting->values[valueIndex] < value;
            break;
        case 3:
            flag = _setting->values[valueIndex] >= value;
            break;
        case 4:
            flag = _setting->values[valueIndex] > value;
            break;
    }
    if (flag) {
        thread->current->ip++;
        return false;
    }
    else {
        thread->current->ip = ip1;
        return interrupt;
    }
}
void Map::mtResource(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    int kind = readByte(cursor);
    fixed amount = (fixed)mtReadShort(cursor);
    
    foreach(Force*, force, forces) {
        force->reviseResource(kind, amount);
    }
    thread->current->ip++;
}
void Map::mtSupply(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    int amount = mtReadShort(cursor);
    
    foreach(Force*, force, forces) {
        force->reviseSupply(amount);
    }
    thread->current->ip++;
}
void Map::mtMaxSupply(const byte*& cursor, Setting::Thread* thread) {
	CSArray<Force>* forces = mtReadForces(cursor);
	int amount = mtReadShort(cursor);

	foreach(Force*, force, forces) {
		force->setMaxSupply(amount);
	}
	thread->current->ip++;
}
bool Map::mtHasResource(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    int kind = readByte(cursor);
    int amount = mtReadShort(cursor);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    foreach(Force*, force, forces) {
        if (force->resource(kind) < amount) {
            thread->current->ip = ip1;
            return interrupt;
        }
    }
    thread->current->ip++;
    return false;
}
bool Map::mtHasPopulation(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    int amount = mtReadShort(cursor);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    foreach(Force*, force, forces) {
        if (force->population() < amount) {
            thread->current->ip = ip1;
            return interrupt;
        }
    }
    thread->current->ip++;
    return false;
}
bool Map::mtHasRune(const byte*& cursor, Setting::Thread* thread) {
	CSArray<Force>* forces = mtReadForces(cursor);
	RuneIndex index = mtReadAssetIndex2(cursor);
	int ip1 = readShort(cursor);
	bool interrupt = readBoolean(cursor);

	foreach(Force*, force, forces) {
		if (!force->isTrained(index)) {
			thread->current->ip = ip1;
			return interrupt;
		}
	}
	thread->current->ip++;
	return false;
}
void Map::mtPoint(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    FPoint point = mtReadPoint(cursor, thread);
    foreach(Force*, force, forces) {
        force->point = point;
    }
    thread->current->ip++;
}
void Map::mtAppear(const Squad& squad, const FPoint* point) {
    FPoint p;
    if (point) p = *point;
    else if (squad.point) p = *squad.point;
    else if (squad.force) p = squad.force->point;
    else return;

	if (squad.range) {
		fixed r = _random->nextFixed(fixed::Zero, squad.range);
		FPoint dir;
		dir.x = _random->nextFixed(-r, r);
		dir.y = CSMath::sqrt(r * r - dir.x * dir.x);
		if (_random->nextInt(0, 1)) {
			dir.y = -dir.y;
		}
		p += dir;
	}
    
    const UnitData& data = Asset::sharedAsset()->unit(squad.index);
    if (locatablePosition(data.type, p, data.collider, NULL, MapLocationCollided) != MapLocationUnabled) {
        Unit* unit = Unit::unit(squad.force, squad.index, squad.level, squad.skin, NULL);
        foreach (const Growup*, growup, squad.growups) {
            unit->addGrowup(growup);
        }
        if (squad.name) {
            unit->rename(squad.name);
        }
        if (squad.abilities) {
            synchronized(unit, GameLockObject) {
                mergeAbilityRevision(unit->abilities(), NULL, squad.abilities);
            }
        }
		if (squad.scaleBase != fixed::One || squad.scaleUp != fixed::Zero) {
			unit->setScale(squad.scaleBase, squad.scaleUp);
		}
		unit->locate(p);

		voice(VoiceSpawn, unit);

        squad.units->addObject(unit);
    }
}
void Map::mtAppear(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Squad*>* squads = mtReadSquads(cursor);
    int count = readByte(cursor);
    bool duplicate = readBoolean(cursor);
    
    FPoint* point = NULL;
    switch (readByte(cursor)) {
        case 1:
            point = new FPoint(mtReadPoint(cursor, thread));
            break;
        case 2:
            {
                point = new FPoint(fixed::Zero, fixed::Zero);
                
                const CSArray<Unit>* units = mtReadUnits(cursor, thread);
                
                int count = 0;
                foreach(const Unit*, unit, units) {
                    if (unit->isLocated()) {
                        *point += unit->point();
                        count++;
                    }
                }
                if (count) {
                    *point /= count;
                }
                else {
                    delete point;
                    point = NULL;
                }
            }
            break;
    }
    if (!count) {
        foreach(const Squad*, squad, squads) {
            if ((!squad->units->count() || duplicate) && (point || squad->point)) {
                mtAppear(*squad, point);
            }
        }
    }
    else {
        for (int i = 0; i < count; i++) {
            int subSquadIndex = _random->nextInt(0, squads->count() - 1);
            
            if (!duplicate) {
                for (int i = 0; i < squads->count(); i++) {
                    if (!squads->objectAtIndex(subSquadIndex)->units->count()) {
                        goto appear;
                    }
                    subSquadIndex = (subSquadIndex + 1) % squads->count();
                }
                goto exit;
            }
appear:
            const Squad* squad = squads->objectAtIndex(subSquadIndex);
            
            if (point || squad->point) {
                mtAppear(*squad, point);
            }
        }
    }
exit:
    if (point) {
        delete point;
    }
    thread->current->ip++;
}
void Map::mtLeave(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    bool vanish = readBoolean(cursor);
    
    foreach(Unit*, unit, units) {
        unit->destroy();
        
        if (vanish) {
            unit->unlocate();
        }
    }
    thread->current->ip++;
}
bool Map::mtIsAlive(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    int count = mtReadShort(cursor);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (units->count() >= count) {
        thread->current->ip++;
        return false;
    }
    else {
        thread->current->ip = ip1;
        return interrupt;
    }
}
bool Map::mtIsDestroyed(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (!units->count()) {
        thread->current->ip++;
        return false;
    }
    else {
        thread->current->ip = ip1;
        return interrupt;
    }
}
bool Map::mtIsTarget(const byte*& cursor, Setting::Thread* thread) {
	const CSArray<Force>* forces = mtReadForces(cursor);
	const CSArray<Squad*>* squads = mtReadSquads(cursor);
	const byte* condition = cursor;
	cursor += UnitBase::matchLength(condition);
	int count = mtReadShort(cursor);
	int ip1 = readShort(cursor);
	bool interrupt = readBoolean(cursor);

	int matchingCount = 0;
	bool flag = true;
	foreach(const Squad*, squad, squads) {
		foreach(const Unit*, unit, squad->units) {
			if (matchUnitSetting(forces, condition, unit)) {
				matchingCount++;
			}
			else {
				flag = false;
			}
		}
	}
	if (count ? matchingCount >= count : flag) {
		thread->current->ip++;
		return false;
	}
	else {
		thread->current->ip = ip1;
		return interrupt;
	}
}
bool Map::mtIsEncountered(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    if (units == thread->units) {
        units = CSArray<Unit>::arrayWithArray(thread->units, false);
    }
    thread->units->removeAllObjects();
    
    if (readBoolean(cursor)) {
        CSArray<Unit>* targets = mtReadUnits(cursor, thread);
        fixed distance = readFixed(cursor);
        
        foreach(Unit*, unit, units) {
            if (unit->isLocated()) {
                fixed collider = unit->collider();
                
                foreach(Unit*, otherUnit, targets) {
                    if (otherUnit->isLocated()) {
                        fixed d2 = unit->point().distanceSquared(otherUnit->point());
                        fixed c = distance + collider + otherUnit->collider() + EpsilonCollider;
                        if (d2 <= c * c) {
                            thread->units->addObject(unit);
                        }
                    }
                }
            }
        }
    }
    else {
        FPoint point = mtReadPoint(cursor, thread);
        fixed distance = readFixed(cursor);
        
        foreach(Unit*, unit, units) {
            if (unit->isLocated()) {
                fixed d2 = unit->point().distanceSquared(point);
                fixed c = distance + unit->collider() + EpsilonCollider;
                if (d2 <= c * c) {
                    thread->units->addObject(unit);
                }
            }
        }
    }
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (thread->units->count()) {
        thread->current->ip++;
        return false;
    }
    else {
        thread->current->ip = ip1;
        return interrupt;
    }
}
void Map::mtCommand(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    
    CommandIndex key(cursor);
    
    Object* target = NULL;
    FPoint* point = NULL;
    
    switch (readByte(cursor)) {
        case 1:
            target = Destination::destination(_force);
            target->locate(mtReadPoint(cursor, thread));
            break;
        case 2:
            point = new FPoint(mtReadPoint(cursor, thread));
            break;
        case 3:
            {
                CSArray<Unit>* targets = mtReadUnits(cursor, thread);
                foreach(Unit*, unit, targets) {
                    if (unit->isLocated()) {
                        target = unit;
                        break;
                    }
                }
                if (!target) {
                    thread->current->ip++;
                    return;
                }
            }
            break;
		case 4:
			{
				CSArray<Resource>* targets = mtReadResources(cursor, thread);
				foreach(Resource*, resource, targets) {
					if (resource->isLocated()) {
						target = resource;
						break;
					}
				}
				if (!target) {
					thread->current->ip++;
					return;
				}
			}
			break;
    }
    commandImpl(units, key, target, point, 0, CommandCommitRun);
    
    if (point) {
        delete point;
    }
    thread->current->ip++;
}
void Map::mtTransform(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    
    if (readBoolean(cursor)) {
        UnitIndex index = mtReadAssetIndex2(cursor);
        foreach(Unit*, unit, units) {
            unit->setIndex(index);
        }
    }
    else {
        foreach(Unit*, unit, units) {
            unit->resetIndex();
        }
    }
    thread->current->ip++;
}
void Map::mtBuff(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    BuffIndex index = mtReadAssetIndex2(cursor);
	int timeOp = readByte(cursor);
    fixed time;
    switch (readByte(cursor)) {
        case 1:
		case 2:
		case 3:
            time = mtReadFixed(cursor, thread);
            break;
    }
    fixed stack = mtReadFixed(cursor, thread);
    
    foreach(Unit*, unit, units) {
        unit->buff(index, timeOp, time, stack, NULL);
    }
    thread->current->ip++;
}
void Map::mtSpawn(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    UnitIndex index = mtReadAssetIndex2(cursor);
    int op = readByte(cursor);
    
    switch (op) {
        case 0:
            {
                Object* target = NULL;
                switch (readByte(cursor)) {
                    case 1:
                        target = Destination::destination(_force);
                        target->locate(mtReadPoint(cursor, thread));
                        break;
                    case 2:
                        {
                            CSArray<Unit>* targets = mtReadUnits(cursor, thread);
                            foreach(Unit*, unit, targets) {
                                if (unit->isLocated()) {
                                    target = unit;
                                    break;
                                }
                            }
                            if (!target) {
                                thread->current->ip++;
                                return;
                            }
                        }
                        break;
					case 3:
						{
							CSArray<Resource>* targets = mtReadResources(cursor, thread);
							foreach(Resource*, resource, targets) {
								if (resource->isLocated()) {
									target = resource;
									break;
								}
							}
							if (!target) {
								thread->current->ip++;
								return;
							}
						}
						break;
                }
                foreach(Force*, force, forces) {
                    Spawn* spawn = force->spawn(index);
                    if (spawn) {
                        spawn->run(target, true);
                    }
                }
            }
            break;
        case 1:
            foreach(Force*, force, forces) {
                Spawn* spawn = force->spawn(index);
                if (spawn) {
                    spawn->activate();
                }
            }
            break;
    }
    thread->current->ip++;
}
void Map::mtPlayer(const byte *&cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    
    int op = readByte(cursor);
    
    switch (op) {
        case 0:
            {
                int level = mtReadShort(cursor);
                foreach(Force*, force, forces) {
                    force->baseUnitLevel = level;
                }
            }
            break;
        case 1:
            {
                UnitIndex index = mtReadAssetIndex2(cursor);
                int level = mtReadShort(cursor);
                int skin = readShort(cursor);
                foreach(Force*, force, forces) {
                    force->addCard(Card::card(force, index, level, skin));
                }
            }
            break;
		case 2:
			{
				int length = readLength(cursor);
				const byte* vraw = cursor;
				cursor += length * (sizeof(UnitIndex) + 2);
				int level = mtReadShort(cursor);
				int count = mtReadByte(cursor);

				foreach(Force*, force, forces) {
					for (int i = 0; i < count; i++) {
						int offset = _random->nextInt(0, length - 1);
						for (int j = 0; j < length; j++) {
							const byte* cvraw = vraw + (((offset + j) % length) * (sizeof(UnitIndex) + 2));
							UnitIndex index = UnitIndex(cvraw);
							if (Asset::sharedAsset()->openCards->containsObject(index) && !force->cardForIndex(index)) {
								int skin = readShort(cvraw);
								force->addCard(Card::card(force, index, level, skin));
								break;
							}
						}
					}
				}
			}
			break;
        case 3:
            {
                UnitIndex index = mtReadAssetIndex2(cursor);
                foreach(Force*, force, forces) {
                    force->removeCard(index);
                }
            }
            break;
        case 4:
            foreach(Force*, force, forces) {
                force->removeAllCards();
            }
            break;
        case 5:
            foreach(Force*, force, forces) {
                force->resetCards();
            }
            break;
        case 6:
            {
                RuneIndex index = mtReadAssetIndex2(cursor);
                int level = mtReadShort(cursor);
                foreach(Force*, force, forces) {
                    force->addRune(Rune::rune(force, index, level));
                }
            }
            break;
		case 7:
			{
				int length = readLength(cursor);
				const byte* vraw = cursor;
				cursor += length * sizeof(RuneIndex);
				int level = mtReadShort(cursor);
				int count = mtReadByte(cursor);

				foreach(Force*, force, forces) {
					for (int i = 0; i < count; i++) {
						int offset = _random->nextInt(0, length - 1);
						for (int j = 0; j < length; j++) {
							const byte* cvraw = vraw + (((offset + j) % length) * (sizeof(RuneIndex)));
							RuneIndex index = RuneIndex(cvraw);
							if (Asset::sharedAsset()->openRunes->containsObject(index) && !force->runeForIndex(index)) {
								force->addRune(Rune::rune(force, index, level));
								break;
							}
						}
					}
				}
			}
			break;
        case 8:
            {
                RuneIndex index = mtReadAssetIndex2(cursor);
                foreach(Force*, force, forces) {
                    force->removeRune(index);
                }
            }
            break;
        case 9:
            foreach(Force*, force, forces) {
                force->removeAllRunes();
            }
            break;
        case 10:
            foreach(Force*, force, forces) {
                force->resetRunes();
            }
            break;
		case 11:
			{
				RuneIndex index = mtReadAssetIndex2(cursor);
				foreach(Force*, force, forces) {
					force->completeTraining(index);
				}
			}
			break;
        case 12:
            {
                GameControl control = (GameControl)readByte(cursor);
                foreach(Force*, force, forces) {
                    force->setControl(control);
                }
            }
            break;
    }
    thread->current->ip++;
}

void Map::mtUnit(const byte *&cursor, Setting::Thread *thread) {
    bool nameChanged = readBoolean(cursor);
    const CSString* name = NULL;
    if (nameChanged) {
        name = mtReadLocaleString(cursor)->value();
    }
    int abilityCount = readLength(cursor);
    CSArray<ushort>* abilityDefaults = NULL;
    CSArray<AbilityRevision>* abilities = NULL;
    for (int i = 0; i < abilityCount; i++) {
        int index = readShort(cursor);
        switch (readByte(cursor)) {
            case 0:
                if (!abilityDefaults) abilityDefaults = CSArray<ushort>::array();
                abilityDefaults->addObject(index);
                break;
            case 1:
                if (!abilities) abilities = CSArray<AbilityRevision>::array();
                addAbilityRevision(abilities, index, NULL, mtReadFixed(cursor, thread), fixed::Zero);
                break;
            case 2:
                if (!abilities) abilities = CSArray<AbilityRevision>::array();
                addAbilityRevision(abilities, index, NULL, -fixed::One, mtReadFixed(cursor, thread));
                break;
            case 3:
                {
                    if (!abilities) abilities = CSArray<AbilityRevision>::array();
                    fixed value = mtReadFixed(cursor, thread);
                    const byte* condition = cursor;
                    cursor += UnitBase::matchLength(condition);
                    addAbilityRevision(abilities, index, condition, value, fixed::Zero);
                }
                break;
            case 4:
                {
                    if (!abilities) abilities = CSArray<AbilityRevision>::array();
                    fixed diff = mtReadFixed(cursor, thread);
                    const byte* condition = cursor;
                    cursor += UnitBase::matchLength(condition);
                    addAbilityRevision(abilities, index, condition, -fixed::One, diff);
                }
                break;
        }
    }
	bool scaleChanged = readBoolean(cursor);
	fixed scaleBase;
	fixed scaleUp;
	if (scaleChanged) {
		scaleBase = mtReadFixed(cursor, thread);
		scaleUp = mtReadFixed(cursor, thread);
	}
	else {
		scaleBase = fixed::One;
		scaleUp = fixed::Zero;
	}
    switch (readByte(cursor)) {
        case 0:
            {
                const CSArray<Force>* forces = mtReadForces(cursor);
                const byte* condition = cursor;
                cursor += UnitBase::matchLength(condition);
                
                UnitSetting& setting = addUnitSetting(forces, condition);
                if (nameChanged) {
                    CSObject::retain(setting.name, name);
                }
                mergeAbilityRevision(setting.abilities, abilityDefaults, abilities);
				if (scaleChanged) {
					setting.scaleBase = scaleBase;
					setting.scaleUp = scaleUp;
				}
                foreach(Unit*, unit, _units) {
                    if (matchUnitSetting(forces, condition, unit)) {
                        if (nameChanged) {
                            unit->rename(name);
                        }
                        synchronized(unit, GameLockObject) {
                            mergeAbilityRevision(unit->abilities(), abilityDefaults, abilities);
                        }
						if (scaleChanged) {
							unit->setScale(scaleBase, scaleUp);
						}
                    }
                }
				for (int i = 0; i < MaxPlayers; i++) {
					Force* force = _forces[i];
					if (force) {
						for (int j = 0; j < force->spawnCount(); j++) {
							Spawn* spawn = force->spawn(j);
							if (nameChanged) {
								spawn->rename(name);
							}
							synchronized(spawn, GameLockObject) {
								mergeAbilityRevision(spawn->abilities(), abilityDefaults, abilities);
							}
						}
					}
				}
            }
            break;
        case 1:
            {
                CSArray<Squad*>* squads = mtReadSquads(cursor);
                
                foreach (Squad*, squad, squads) {
                    if (nameChanged) {
                        CSObject::retain(squad->name, name);
                    }
                    mergeAbilityRevision(squad->abilities, abilityDefaults, abilities);
                    
                    foreach(Unit*, unit, squad->units) {
                        if (nameChanged) {
                            unit->rename(name);
                        }
                        synchronized(unit, GameLockObject) {
                            mergeAbilityRevision(unit->abilities(), abilityDefaults, abilities);
                        }
						if (scaleChanged) {
							unit->setScale(scaleBase, scaleUp);
						}
                    }
                }
            }
            break;
        case 2:
            CSArray<Unit>* units = mtReadUnits(cursor, thread);
            
            foreach(Unit*, unit, units) {
                if (nameChanged) {
                    unit->rename(name);
                }
                synchronized(unit, GameLockObject) {
                    mergeAbilityRevision(unit->abilities(), abilityDefaults, abilities);
                }
				if (scaleChanged) {
					unit->setScale(scaleBase, scaleUp);
				}
            }
            break;
    }
    thread->current->ip++;
}
void Map::mtRevise(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    int op = readByte(cursor);
    fixed value = mtReadFixed(cursor, thread);
    
    foreach(Unit*, unit, units) {
        unit->revise(op, value, fixed::Zero, NULL);
    }
    thread->current->ip++;
}
void Map::mtWarp(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    FPoint point = mtReadPoint(cursor, thread);
    
    foreach(Unit*, unit, units) {
        FPoint p = point;
        if (Map::locatablePosition(unit, p, NULL) != MapLocationUnabled) {
            unit->locate(p);
        }
    }
    thread->current->ip++;
}
void Map::mtVision(const byte*& cursor, Setting::Thread* thread) {
    FPoint point = mtReadPoint(cursor, thread);
    fixed range = (fixed)readByte(cursor);
    int alliance = mtReadByte(cursor);
    CSAssert(alliance >= 0 && alliance < MaxAlliances, "invalid operation");
    bool visible = readBoolean(cursor);
    
    changeVision(NULL, point, range, false, alliance, visible);
    
    thread->current->ip++;
}
void Map::mtState(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Force>* forces = mtReadForces(cursor);
    bool includeObserver = readBoolean(cursor);
    MapState state = (MapState)readByte(cursor);

    switch (state) {
        case MapStateVictory:
            foreach(Force*, force, forces) {
                force->result = GameResultVictory;
            }
            break;
        case MapStateDefeat:
            foreach(Force*, force, forces) {
                force->result = GameResultDefeat;
            }
            break;
        case MapStateEnd:
            foreach(Force*, force, forces) {
                force->result = GameResultDraw;
            }
            break;
    }
    bool flag = _playing == PlayingParticipate ? forces->containsObjectIdenticalTo(_force) : includeObserver;
    
    if (flag) {
        _state = state;
        
        switch (state) {
            case MapStateVictory:
            case MapStateDefeat:
            case MapStateEnd:
                Sound::stop(SoundControlGameBgm);
                _running = false;
				break;
        }
    }
    thread->current->ip++;
}
void Map::mtActivate(const byte*& cursor, Setting::Thread* thread) {
    _running = readBoolean(cursor);
    thread->current->ip++;
}
void Map::mtEffect(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Object>* targets = NULL;
	switch (readByte(cursor)) {
		case 0:
			{
				CSArray<FPoint>* points = mtReadPoints(cursor, thread);
				targets = CSArray<Object>::arrayWithCapacity(points->count());
				foreach(const FPoint&, p, points) {
					Object* target = Destination::destination(_force);
					target->locate(p);
					targets->addObject(target);
				}
			}
			break;
		case 1:
			targets = reinterpret_cast<CSArray<Object>*>(mtReadUnits(cursor, thread));
			break;
		case 2:
			targets = reinterpret_cast<CSArray<Object>*>(mtReadResources(cursor, thread));
			break;
	}
    AnimationIndex index(cursor);
    bool attach = readBoolean(cursor);
    int slot = readByte(cursor);
    int count = readByte(cursor);
    
    if (slot < 0 || !_effects[slot]->count()) {
        int currentCount = 0;
        foreach(const Object*, target, targets) {
            if (target->isLocated() || attach) {
                AnimationEffect* effect = AnimationEffect::effect(index, target, NULL, false, attach, false);
                if (slot >= 0) {
                    _effects[slot]->addObject(effect);
                }
                if (count && ++currentCount >= count) {
                    break;
                }
            }
        }
    }
    thread->current->ip++;
}
void Map::mtRemoveEffect(const byte*& cursor, Setting::Thread* thread) {
    int slot = readByte(cursor);
    bool disposing = readBoolean(cursor);
    foreach(AnimationEffect*, effect, _effects[slot]) {
        if (disposing) {
            effect->dispose();
        }
        else {
            effect->stop();
        }
    }
    _effects[slot]->removeAllObjects();
    thread->current->ip++;
}
void Map::mtTimer(const byte*& cursor, Setting::Thread* thread) {
    int timerIndex = mtReadByte(cursor);
    CSAssert(timerIndex >= 0 && timerIndex < countof(_setting->timers), "invalid operation");
    int op = readByte(cursor);
    
	Setting::Timer& timer = _setting->timers[timerIndex];

    switch (op) {
        case 0:
			{
				fixed v = mtReadFixed(cursor, thread);
				if (readBoolean(cursor) || !timer.set) {
					timer.remaining = CSMath::max(timer.remaining + v, fixed::Zero);
					timer.set = true;
				}
			}
            break;
        case 1:
			{
				fixed v = mtReadFixed(cursor, thread);
				if (readBoolean(cursor) || !timer.set) {
					timer.remaining = CSMath::max(v, fixed::Zero);
					timer.set = true;
				}
			}
            break;
        case 2:
			timer.set = false;
            break;
        case 3:
			timer.set = true;
            break;
    }
    thread->current->ip++;
}
bool Map::mtIsTimeout(const byte*& cursor, Setting::Thread* thread) {
    int timerIndex = mtReadByte(cursor);
    CSAssert(timerIndex >= 0 && timerIndex < countof(_setting->timers), "invalid operation");
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
	Setting::Timer& timer = _setting->timers[timerIndex];

    if (timer.set && !timer.remaining) {
		timer.set = false;
        thread->current->ip++;
        return false;
    }
    else {
        thread->current->ip = ip1;
        return interrupt;
    }
}
void Map::mtShowTimer(const byte*& cursor, Setting::Thread* thread) {
    bool show = readBoolean(cursor);
    
    if (show) {
        int timerIndex = mtReadByte(cursor);
        CSAssert(timerIndex >= 0 && timerIndex < countof(_setting->timers), "invalid operation");
        _timer = &_setting->timers[timerIndex].remaining;
    }
    else {
        _timer = NULL;
    }
    thread->current->ip++;
}
void Map::mtSpeech(const byte*& cursor, Setting::Thread* thread) {
    ImageIndex portrait(cursor);
    const CSString* name = mtReadLocaleString(cursor)->value();
    const CSString* msg = mtReadLocaleString(cursor)->value();
    int valueCount = readLength(cursor);
    msg = settingValueString(msg, cursor, valueCount);
    cursor += valueCount * 2;
    synchronized(_controlLock, GameLockMap) {
        _speech = new Speech(portrait, name, msg);
    }
    thread->current->ip++;
}
void Map::mtSpeechBubble(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    const CSString* msg = mtReadLocaleString(cursor)->value();
    int valueCount = readLength(cursor);
    msg = settingValueString(msg, cursor, valueCount);
    cursor += valueCount * 2;
    if (units->count()) {
        int index = randInt(0, units->count() - 1);
        units->objectAtIndex(index)->addSpeech(msg);
    }
    thread->current->ip++;
}
bool Map::mtScroll(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    FPoint point;

	switch (readByte(cursor)) {
		case 0:
			point = mtReadPoint(cursor, thread);
			break;
		case 1:
			{
				CSArray<Unit>* units = mtReadUnits(cursor, thread);
				uint count = 0;
				point = FPoint(fixed::Zero, fixed::Zero);
				foreach(const Unit*, unit, units) {
					if (unit->isLocated()) {
						point += unit->point();
						count++;
					}
				}
				if (!count) {
					cursor += 9;
					thread->current->ip++;
					return false;
				}
				point /= count;
			}
			break;
		case 2:
			{
				CSArray<Resource>* resources = mtReadResources(cursor, thread);
				uint count = 0;
				point = FPoint(fixed::Zero, fixed::Zero);
				foreach(const Resource*, resource, resources) {
					if (resource->isLocated()) {
						point += resource->point();
						count++;
					}
				}
				if (!count) {
					cursor += 9;
					thread->current->ip++;
					return false;
				}
				point /= count;
			}
			break;
	}

    fixed time = readFixed(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (playing) {
        _camera->setScroll(convertMapToFlatWorldSpace(point), time);
    }

    thread->current->ip++;
    if (interrupt) {
        thread->waiting = time;
		thread->skip = false;
        return true;
    }
    return false;
}
bool Map::mtZoom(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    float zoom = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (playing) {
        _camera->setZoom(zoom, time);
    }
    
    thread->current->ip++;
    if (interrupt) {
        thread->waiting = time;
		thread->skip = false;
        return true;
    }
    return false;
}
void Map::mtEvent(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    
    static const AnimationIndex animation(AnimationSetGame, AnimationGameMinimapEvent);
    
	switch (readByte(cursor)) {
		case 0:
			foreach(const FPoint&, p, mtReadPoints(cursor, thread)) {
				if (playing) event(animation, Asset::neutralColor, p);
			}
			break;
		case 1:
			foreach(const Unit*, unit, mtReadUnits(cursor, thread)) {
				if (playing && unit->isLocated()) {
					event(_force, animation, Asset::neutralColor, unit->point());
				}
			}
			break;
		case 2:
			foreach(const Resource*, resource, mtReadResources(cursor, thread)) {
				if (playing && resource->isLocated()) {
					event(_force, animation, Asset::neutralColor, resource->point());
				}
			}
			break;
	}
    thread->current->ip++;
}
bool Map::mtColor(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    CSColor color(cursor, true);
    fixed time = readFixed(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (playing) {
        _color.time = time;
        if (time) {
            _color.delta = (color - _color.current) / (float)time;
        }
        else {
            _color.current = color;
        }
    }
    thread->current->ip++;
    if (interrupt && time) {
        thread->waiting = time;
		thread->skip = false;
        return true;
    }
    return false;
}
void Map::mtSound(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    
    int op = readByte(cursor);
    
    switch (op) {
        case 0:
            {
                int soundCount = readLength(cursor);
                const byte* soundCursor = cursor + (randInt(0, soundCount - 1) * sizeof(SoundIndex));
                SoundIndex soundIndex(soundCursor);
                cursor += soundCount * sizeof(SoundIndex);
                float volume = readFloat(cursor);
                if (playing) {
                    Sound::stop(SoundControlGameBgm);
                    Sound::play(SoundControlGameBgm, soundIndex, SoundPlayBgm, volume);
                }
            }
            break;
        case 1:
            if (playing) {
                Sound::stop(SoundControlGameBgm);
            }
            break;
        case 2:
            {
				int soundCount = readLength(cursor);
				const byte* soundCursor = cursor + (randInt(0, soundCount - 1) * sizeof(SoundIndex));
				SoundIndex soundIndex(soundCursor);
				cursor += soundCount * sizeof(SoundIndex);
                float volume = readFloat(cursor);
                if (playing) {
                    Sound::play(SoundControlEffect, soundIndex, SoundPlayEffectEssential, volume);
                }
            }
            break;
		case 3:
			_setting->soundEnabled = readBoolean(cursor);
			break;
		case 4:
			_setting->voiceEnabled = readBoolean(cursor);
			break;
    }
    thread->current->ip++;
}
void Map::mtAnnounce(const byte*& cursor, Setting::Thread* thread) {
	bool playing = mtReadPlaying(cursor);

	AnnounceIndex index = (AnnounceIndex)readByte(cursor);

	if (playing) {
		announce(index);
	}

	thread->current->ip++;
}
void Map::mtVibrate(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    int level = readByte(cursor);
    
    if (playing) {
        vibrate(level, NULL);
    }
    
    thread->current->ip++;
}
void Map::mtMessage(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    const CSString* msg = mtReadLocaleString(cursor)->value();
    int valueCount = readLength(cursor);
    msg = settingValueString(msg, cursor, valueCount);
    cursor += valueCount * 2;
    if (playing) {
        pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), msg, true));
    }
    thread->current->ip++;
}
void Map::mtMission(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    uint key = readByte(cursor);
    synchronized(_missions, GameLockMap) {
        if (readBoolean(cursor)) {
            Mission* mission = Mission::mission(key, cursor);
            if (playing) {
                for (int i = 0; i < _missions->count(); i++) {
                    const Mission* otherMission = _missions->objectAtIndex(i);
                    if (otherMission->key == key) {
                        mission->confirm = otherMission->confirm;
                        mission->replaced = true;
                        _missions->removeObjectAtIndex(i);
                        break;
                    }
                }
                _missions->addObject(mission);
            }
        }
        else {
            if (playing) {
                for (int i = 0; i < _missions->count(); i++) {
                    Mission* mission = _missions->objectAtIndex(i);
                    if (mission->key == key) {
                        _missions->removeObjectAtIndex(i);
                        break;
                    }
                }
            }
        }
    }
    thread->current->ip++;
}
void Map::mtControl(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    MapControl control = (MapControl)readByte(cursor);
    MapControlState state = (MapControlState)readByte(cursor);
    
    switch(control) {
        case MapControlSelect:
            if (readBoolean(cursor)) {
                CSArray<Unit>* units = mtReadUnits(cursor, thread);
                if (playing) {
					synchronized(_controlLock, GameLockMap) {
						foreach(const Unit*, unit, units) {
							_control.selectionStates->setObject(unit->key, state);
						}
                    }
                }
            }
            else {
                if (playing) {
					synchronized(_controlLock, GameLockMap) {
						_control.states[MapControlSelect] = state;
						_control.selectionStates->removeAllObjects();
					}
                }
            }
            break;
        case MapControlCommand:
            switch (readByte(cursor)) {
                case 0:
                    if (playing) {
						synchronized(_controlLock, GameLockMap) {
							_control.states[MapControlCommand] = state;
							_control.commandStates->removeAllObjects();
						}
                    }
                    break;
                case 1:
                    {
                        CommandIndex index(cursor);
                        if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.commandStates->setObject(index, MapControlStateCommand(state, NULL));
							}
                        }
                    }
                    break;
                case 2:
                    {
                        CommandIndex index(cursor);
						FPoint target = mtReadPoint(cursor, thread);
                        fixed range = readFixed(cursor);
                        if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.commandStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(target, range)));
							}
                        }
                    }
                    break;
				case 3:
					{
						CommandIndex index(cursor);
						CSArray<Unit>* targets = mtReadUnits(cursor, thread);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.commandStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(reinterpret_cast<CSArray<Object>*>(targets))));
							}
						}
					}
					break;
				case 4:
					{
						CommandIndex index(cursor);
						CSArray<Resource>* targets = mtReadResources(cursor, thread);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.commandStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(reinterpret_cast<CSArray<Object>*>(targets))));
							}
						}
					}
					break;
            }
            break;
        case MapControlSpawn:
            switch (readByte(cursor)) {
                case 0:
                    if (playing) {
						synchronized(_controlLock, GameLockMap) {
							_control.states[MapControlSpawn] = state;
							_control.spawnStates->removeAllObjects();
						}
                    }
                    break;
				case 1:
					{
						UnitIndex index(cursor);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.spawnStates->setObject(index, MapControlStateCommand(state, NULL));
							}
						}
					}
					break;
				case 2:
					{
						UnitIndex index(cursor);
						FPoint target = mtReadPoint(cursor, thread);
						fixed range = readFixed(cursor);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.spawnStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(target, range)));
							}
						}
					}
					break;
				case 3:
					{
						UnitIndex index(cursor);
						CSArray<Unit>* targets = mtReadUnits(cursor, thread);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.spawnStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(reinterpret_cast<CSArray<Object>*>(targets))));
							}
						}
					}
					break;
				case 4:
					{
						UnitIndex index(cursor);
						CSArray<Resource>* targets = mtReadResources(cursor, thread);
						if (playing) {
							synchronized(_controlLock, GameLockMap) {
								_control.spawnStates->setObject(index, MapControlStateCommand(state, ActionTarget::target(reinterpret_cast<CSArray<Object>*>(targets))));
							}
						}
					}
					break;
            }
            break;
        default:
            if (playing) {
				synchronized(_controlLock, GameLockMap) {
					_control.states[control] = state;
				}
            }
            break;
    }
    thread->current->ip++;
}
void Map::mtAttach(const byte*& cursor, Setting::Thread* thread) {
    CSArray<Unit>* units = mtReadUnits(cursor, thread);
    int type = readByte(cursor);
    
    switch(type) {
        case 0:
            {
                Force* force = mtReadForces(cursor)->lastObject();
                foreach(Unit*, unit, units) {
                    unit->setForce(force);
                }
            }
            break;
        case 1:
            foreach(Unit*, unit, units) {
                unit->resetForce();
            }
            break;
    }
    thread->current->ip++;
}
void Map::mtGrowup(const byte*& cursor, Setting::Thread* thread) {
    switch (readByte(cursor)) {
        case 0:
            {
                const CSArray<Force>* forces = mtReadForces(cursor);
                const byte* condition = cursor;
                cursor += UnitBase::matchLength(condition);
                Growup* growup = Growup::growup(cursor);
                
                UnitSetting& setting = addUnitSetting(forces, condition);
                if (!setting.growups) setting.growups = new CSArray<const Growup>();
                setting.growups->addObject(growup);
                
                foreach(Unit*, unit, _units) {
                    if (matchUnitSetting(forces, condition, unit)) {
                        unit->addGrowup(growup);
                    }
                }
            }
            break;
        case 1:
            {
                CSArray<Squad*>* squads = mtReadSquads(cursor);
                Growup* growup = Growup::growup(cursor);
                foreach(Squad*, squad, squads) {
                    if (!squad->growups) squad->growups = new CSArray<const Growup>();
                    squad->growups->addObject(growup);
                    
                    foreach(Unit*, unit, squad->units) {
                        unit->addGrowup(growup);
                    }
                }
            }
            break;
        case 2:
            {
                CSArray<Unit>* units = mtReadUnits(cursor, thread);
                Growup* growup = Growup::growup(cursor);
                foreach(Unit*, unit, units) {
                    unit->addGrowup(growup);
                }
            }
            break;
        case 3:
            {
                CSArray<Unit>* units = mtReadUnits(cursor, thread);
                int exp = mtReadShort(cursor);
                foreach(Unit*, unit, units) {
                    unit->obtainExp(exp);
                }
            }
            break;
    }
    thread->current->ip++;
}
void Map::mtAttain(const byte*& cursor, Setting::Thread* thread) {
    int attainIndex = readByte(cursor);
    CSAssert(attainIndex >= 0 && attainIndex < 3, "invalid operation");

	_attainOrders[attainIndex] = (MapAttainOrder)readByte(cursor);
    
    int forceCount = readByte(cursor);
    for (int i = 0; i < forceCount; i++) {
        int forceIndex = readByte(cursor);
        int valueIndex = readShort(cursor);
        CSAssert(forceIndex >= 0 && forceIndex < MaxPlayers && valueIndex >= 0 && valueIndex < countof(_setting->values), "invalid operation");
        
        if (_forces[forceIndex]) {
            _forces[forceIndex]->setAttainValue(attainIndex, &_setting->values[valueIndex]);
        }
    }

    thread->current->ip++;
}
void Map::mtSelect(const byte*& cursor, Setting::Thread *thread) {
    CSArray<Unit>* units = readBoolean(cursor) ? mtReadUnits(cursor, thread) : NULL;
    
    if (_playing == PlayingParticipate) {
        synchronized(_selectionLock, GameLockMap) {
            _selection.groupedSelections->removeAllObjects();
            _selection.expandedSelections->removeAllObjects();
            _selection.selections = _selection.expandedSelections;
            CSObject::release(_selection.target);
            _selection.following = false;
            _selection.mask = 0;
            
            uint typeMask = ObjectMaskBuilding;
            foreach(Unit*, unit, units) {
                if (unit->force() == _force && unit->compareType(ObjectMaskWeapon)) {
                    typeMask = ObjectMaskWeapon;
                    break;
                }
            }

            foreach(Unit*, unit, units) {
                if (unit->force() == _force && unit->compareType(typeMask)) {
                    int i;
                    
                    bool isNew = true;
                    
                    int order = unit->data().order;
                    
                    for (i = 0; i < _selection.groupedSelections->count(); i++) {
                        CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(i);
                        
                        const Unit* selection = selections->objectAtIndex(0);
                        
                        if (selection->index() == unit->index()) {
                            if (!selections->containsObjectIdenticalTo(unit)) {
                                selections->addObject(unit);
                            }
                            isNew = false;
                            break;
                        }
                        else if (selection->data().order < order) {
                            break;
                        }
                    }
                    if (isNew) {
                        CSArray<Unit>* selections = new CSArray<Unit>();
                        selections->addObject(unit);
                        _selection.groupedSelections->insertObject(i, selections);
                        selections->release();
                    }
                    _selection.expandedSelections->addObject(unit);
                    StrokeEffect::effect(unit, EffectDisplayVanish);
                }
            }
        }
    }
    thread->current->ip++;
}
bool Map::mtIsSelected(const byte*& cursor, Setting::Thread* thread) {
	CSArray<Unit>* units = mtReadUnits(cursor, thread);
	bool all = readBoolean(cursor);
	int ip1 = readShort(cursor);
	bool interrupt = readBoolean(cursor);

	if (_playing == PlayingParticipate) {
		uint count = 0;

		synchronized(_selectionLock, GameLockMap) {
			foreach(const Unit*, unit, units) {
				if (_selection.selections->containsObjectIdenticalTo(unit)) {
					count++;
				}
			}
		}
		if (all ? count == units->count() : count) {
			thread->current->ip++;
			return false;
		}
	}
	thread->current->ip = ip1;
	return interrupt;
}
bool Map::mtAction(const byte*& cursor, Setting::Thread* thread) {
    ActionCode code = (ActionCode)readByte(cursor);
    
    CSArray<Unit>* units = readBoolean(cursor) ? mtReadUnits(cursor, thread) : NULL;
    const CommandIndex* command = readBoolean(cursor) ? new CommandIndex(cursor) : NULL;
    const ActionTarget* target = NULL;
    switch (readByte(cursor)) {
        case 1:
			{
				FPoint point = mtReadPoint(cursor, thread);
				fixed range = readFixed(cursor);
				target = ActionTarget::target(point, range);
			}
            break;
        case 2:
            target = ActionTarget::target(reinterpret_cast<CSArray<Object>*>(mtReadUnits(cursor, thread)));
            break;
		case 3:
			target = ActionTarget::target(reinterpret_cast<CSArray<Object>*>(mtReadResources(cursor, thread)));
			break;
    }
    bool all = readBoolean(cursor);
    uint restriction = readByte(cursor);
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    
    if (isSingle()) {
        if (_mode.context.single.action) {
			if (thread->current->func != _mode.context.single.action->func || thread->current->ip != _mode.context.single.action->ip) {
				CSErrorLog("duplicate action:%d-%d / %d-%d", _mode.context.single.action->func, _mode.context.single.action->ip, thread->current->func, thread->current->ip);
				abort();
			}

            if (command) {
                delete command;
            }
            synchronized(_controlLock, GameLockMap) {
                if (_mode.context.single.action->passed()) {
                    delete _mode.context.single.action;
                    _mode.context.single.action = NULL;
                    thread->current->ip++;
                    return false;
                }
            }
        }
        else {
            synchronized(_controlLock, GameLockMap) {
                _mode.context.single.action = new Action(thread->current->func, thread->current->ip, code, units, command, target, all, restriction);
            }
        }
    }
    else {
        if (command) {
            delete command;
        }
    }
    thread->current->ip = ip1;
    return interrupt;
}
void Map::mtActionFinish(const byte*& cursor, Setting::Thread* thread) {
    if (isSingle()) {
        synchronized(_controlLock, GameLockMap) {
			if (_mode.context.single.action) {
				delete _mode.context.single.action;
				_mode.context.single.action = NULL;
			}
            _mode.context.single.actionRestriction = 0;
        }
    }
    thread->current->ip++;
}
void Map::mtAnimation(const byte*& cursor, Setting::Thread* thread) {
    bool playing = mtReadPlaying(cursor);
    int key = readByte(cursor);
    int op = readByte(cursor);
    
    synchronized(_animations, GameLockMap) {
        switch (op) {
            case 0:
                if (playing) {
                    _animations->removeObject(key);
                }
                break;
            case 1:
                if (playing && !_animations->containsKey(key)) {
                    new (&_animations->setObject(key)) GameAnimation(cursor);
                }
                else {
                    GameAnimation temp(cursor);
                }
                break;
            case 2:
                if (playing && _animations->containsKey(key)) {
                    GameAnimation& animation = _animations->objectForKey(key);
                    animation.animation->stop();
                }
                break;
        }
    }
    thread->current->ip++;
}
void Map::mtLevel(const byte*& cursor, Setting::Thread* thread) {
    int level = readShort(cursor);
    int op = readByte(cursor);
    int ip1 = readShort(cursor);
    
    int originLevel = _index.indices[3];
    bool flag = false;
    switch(op) {
        case 0:
            flag = originLevel == level;
            break;
        case 1:
            flag = originLevel <= level;
            break;
        case 2:
            flag = originLevel < level;
            break;
        case 3:
            flag = originLevel >= level;
            break;
        case 4:
            flag = originLevel > level;
            break;
    }
    thread->current->ip = flag ? thread->current->ip + 1 : ip1;
}
void Map::mtWait(const byte*& cursor, Setting::Thread* thread) {
	thread->skip = readBoolean(cursor);
	thread->waiting = readFixed(cursor);
    thread->current->ip++;
}
bool Map::mtStop(const byte*& cursor, Setting::Thread* thread) {
    int ip1 = readShort(cursor);
    bool interrupt = readBoolean(cursor);
    thread->current->ip = ip1;
    return interrupt;
}
void Map::mtSkipReady(const byte*& cursor, Setting::Thread* thread) {
	bool skipReady = readBoolean(cursor);
	if (isSingle()) _mode.context.single.skipReady = skipReady;
	thread->current->ip++;
}

void Map::mtIsSkipped(const byte*& cursor, Setting::Thread* thread) {
	int ip1 = readShort(cursor);

	thread->current->ip = isSingle() && _mode.context.single.skipCommit ? thread->current->ip + 1 : ip1;
}

enum MapTriggerOp {
	MT_MeleeStart,
	MT_MeleePlay,
	MT_HasForce,
	MT_Call,
	MT_Value,
	MT_CompareValue,
	MT_Resource,
	MT_Supply,
	MT_MaxSupply,
	MT_HasResource,
	MT_HasPopulation,
	MT_HasRune,
	MT_Point,
	MT_Appear,
	MT_Leave,
	MT_IsAlive,
	MT_IsDestroyed,
	MT_IsTarget,
	MT_IsEncountered,
	MT_Command,
	MT_Transform,
	MT_Buff,
	MT_Spawn,
	MT_Player,
	MT_Unit,
	MT_Revise,
	MT_Warp,
	MT_Vision,
	MT_State,
	MT_Activate,
	MT_Effect,
	MT_RemoveEffect,
	MT_Timer,
	MT_IsTimeout,
	MT_ShowTimer,
	MT_Speech,
	MT_SpeechBubble,
	MT_Scroll,
	MT_Zoom,
	MT_Event,
	MT_Color,
	MT_Sound,
	MT_Announce,
	MT_Vibrate,
	MT_Message,
	MT_Mission,
	MT_Control,
	MT_Attach,
	MT_Growup,
	MT_Attain,
	MT_Select,
	MT_IsSelected,
	MT_Action,
	MT_ActionFinish,
	MT_Animation,
	MT_Level,
	MT_Wait,
	MT_Stop,
	MT_SkipReady,
	MT_IsSkipped
};

#ifdef UseMapTriggerTest
static const char* MapTriggerOpNames[] = {
    "MT_MeleeStart",
    "MT_MeleePlay",
	"MT_HasForce",
    "MT_Call",
    "MT_Value",
    "MT_CompareValue",
    "MT_Resource",
    "MT_Supply",
	"MT_MaxSupply",
    "MT_HasResource",
    "MT_HasPopulation",
	"MT_HasRune",
    "MT_Point",
    "MT_Appear",
    "MT_Leave",
    "MT_IsAlive",
    "MT_IsDestroyed",
	"MT_IsTarget",
    "MT_IsEncountered",
    "MT_Command",
    "MT_Transform",
    "MT_Buff",
    "MT_Spawn",
    "MT_Player",
    "MT_Unit",
    "MT_Revise",
    "MT_Warp",
    "MT_Vision",
    "MT_State",
    "MT_Activate",
    "MT_Effect",
    "MT_RemoveEffect",
    "MT_Timer",
    "MT_IsTimeout",
    "MT_ShowTimer",
    "MT_Speech",
    "MT_SpeechBubble",
    "MT_Scroll",
    "MT_Zoom",
    "MT_Event",
    "MT_Color",
    "MT_Sound",
	"MT_Announce",
    "MT_Vibrate",
    "MT_Message",
    "MT_Mission",
    "MT_Control",
    "MT_Attach",
    "MT_Growup",
    "MT_Attain",
    "MT_Select",
	"MT_IsSelected",
    "MT_Action",
    "MT_ActionFinish",
    "MT_Animation",
    "MT_Level",
    "MT_Wait",
    "MT_Stop",
	"MT_SkipReady",
	"MT_IsSkipped"
};
#endif

typename Map::UpdateSettingReturn Map::updateSetting(Setting::Thread* thread, bool skip) {
    thread->units->removeAllObjects();
    
    if (thread->waiting) {
		if (!thread->skip || !skip) {
			thread->waiting -= _latency.frameDelayFixed;
			if (thread->waiting > fixed::Zero) {
				return thread->skip ? UpdateSettingReturnSkip : UpdateSettingReturnNone;
			}
		}
        thread->waiting = fixed::Zero;
		thread->skip = false;
    }
    while (thread->current) {
        const CSArray<byte, 2>* triggerSet;
        
        while ((triggerSet = _triggers->objectAtIndex(thread->current->func)) && thread->current->ip < triggerSet->count()) {
            const CSArray<byte, 1>* trigger = triggerSet->objectAtIndex(thread->current->ip);
            
            const byte* cursor = trigger->pointer();
            
            MapTriggerOp opCode = (MapTriggerOp)readByte(cursor);
            
#ifdef UseMapTriggerTest
            CSLog("map trigger (frame:%d) (thread:%d) (func:%d) (ip:%d) (%s)", _frame, thread->threadId, thread->current->func, thread->current->ip, MapTriggerOpNames[opCode]);
#endif
            switch(opCode) {
                case MT_MeleeStart:
                    mtMeleeStart(cursor, thread);
                    break;
                case MT_MeleePlay:
					mtMeleePlay(cursor, thread);
                    break;
				case MT_HasForce:
					mtHasForce(cursor, thread);
					break;
                case MT_Call:
                    mtCall(cursor, thread);
                    break;
                case MT_Value:
                    mtValue(cursor, thread);
                    break;
                case MT_CompareValue:
                    if (mtCompareValue(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_Resource:
                    mtResource(cursor, thread);
                    break;
                case MT_Supply:
                    mtSupply(cursor, thread);
                    break;
				case MT_MaxSupply:
					mtMaxSupply(cursor, thread);
					break;
                case MT_HasResource:
                    if (mtHasResource(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_HasPopulation:
                    if (mtHasPopulation(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
				case MT_HasRune:
					if (mtHasRune(cursor, thread)) {
						return UpdateSettingReturnNone;
					}
					break;
                case MT_Point:
                    mtPoint(cursor, thread);
                    break;
                case MT_Appear:
                    mtAppear(cursor, thread);
                    break;
                case MT_Leave:
                    mtLeave(cursor, thread);
                    break;
                case MT_IsAlive:
                    if (mtIsAlive(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_IsDestroyed:
                    if (mtIsDestroyed(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
				case MT_IsTarget:
					if (mtIsTarget(cursor, thread)) {
						return UpdateSettingReturnNone;
					}
					break;
                case MT_IsEncountered:
                    if (mtIsEncountered(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_Command:
                    mtCommand(cursor, thread);
                    break;
                case MT_Transform:
                    mtTransform(cursor, thread);
                    break;
                case MT_Buff:
                    mtBuff(cursor, thread);
                    break;
                case MT_Spawn:
                    mtSpawn(cursor, thread);
                    break;
                case MT_Player:
                    mtPlayer(cursor, thread);
                    break;
                case MT_Unit:
                    mtUnit(cursor, thread);
                    break;
                case MT_Revise:
                    mtRevise(cursor, thread);
                    break;
                case MT_Warp:
                    mtWarp(cursor, thread);
                    break;
                case MT_Vision:
                    mtVision(cursor, thread);
                    break;
                case MT_State:
					mtState(cursor, thread);
                    break;
                case MT_Activate:
                    mtActivate(cursor, thread);
                    break;
                case MT_Effect:
                    mtEffect(cursor, thread);
                    break;
                case MT_RemoveEffect:
                    mtRemoveEffect(cursor, thread);
                    break;
                case MT_Timer:
                    mtTimer(cursor, thread);
                    break;
                case MT_IsTimeout:
                    if (mtIsTimeout(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_ShowTimer:
                    mtShowTimer(cursor, thread);
                    break;
                case MT_Speech:
                    mtSpeech(cursor, thread);
                    return UpdateSettingReturnInterrupt;
                case MT_SpeechBubble:
                    mtSpeechBubble(cursor, thread);
                    break;
                case MT_Scroll:
                    if (mtScroll(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_Zoom:
                    if (mtZoom(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_Event:
                    mtEvent(cursor, thread);
                    break;
                case MT_Color:
                    if (mtColor(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_Sound:
                    mtSound(cursor, thread);
                    break;
				case MT_Announce:
					mtAnnounce(cursor, thread);
					break;
                case MT_Vibrate:
                    mtVibrate(cursor, thread);
                    break;
                case MT_Message:
                    mtMessage(cursor, thread);
                    break;
                case MT_Mission:
                    mtMission(cursor, thread);
                    break;
                case MT_Control:
                    mtControl(cursor, thread);
                    break;
                case MT_Attach:
                    mtAttach(cursor, thread);
                    break;
                case MT_Growup:
                    mtGrowup(cursor, thread);
                    break;
                case MT_Attain:
                    mtAttain(cursor, thread);
                    break;
                case MT_Select:
                    mtSelect(cursor, thread);
                    break;
				case MT_IsSelected:
					if (mtIsSelected(cursor, thread)) {
						return UpdateSettingReturnNone;
					}
					break;
				case MT_Action:
                    if (mtAction(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
                case MT_ActionFinish:
                    mtActionFinish(cursor, thread);
                    break;
                case MT_Animation:
                    mtAnimation(cursor, thread);
                    break;
                case MT_Level:
                    mtLevel(cursor, thread);
                    break;
                case MT_Wait:
                    mtWait(cursor, thread);
                    return UpdateSettingReturnNone;
                case MT_Stop:
                    if (mtStop(cursor, thread)) {
                        return UpdateSettingReturnNone;
                    }
                    break;
				case MT_SkipReady:
					mtSkipReady(cursor, thread);
					break;
				case MT_IsSkipped:
					mtIsSkipped(cursor, thread);
					break;
            }
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "map trigger error:%d,%d,%d", opCode, pos, len);
#endif
        }
        Setting::Thread::Stack* prev = thread->current->prev;
        delete thread->current;
        thread->current = prev;
    }
    return UpdateSettingReturnEnd;
}

void Map::updateSetting() {
	if (!_triggers) {
		return;
	}
	switch (_state) {
		case MapStateVictory:
		case MapStateDefeat:
		case MapStateEnd:
		case MapStateInterrupt:
			return;
	}
	for (int i = 0; i < countof(_setting->timers); i++) {
		Setting::Timer& timer = _setting->timers[i];
		if (timer.set && timer.remaining) {
			timer.remaining = CSMath::max(timer.remaining - _latency.frameDelayFixed, fixed::Zero);
		}
	}
	if (_speech) {
		if (_speech->confirm) {
			synchronized(_controlLock, GameLockMap) {
				_speech->release();
				_speech = NULL;
			}
		}
		else {
			return;
		}
	}

	_playTime += _latency.frameDelayFloat;

	updateGraph();

	bool skipReady = false;
	bool skipCommit = _setting->skipCommit;
	bool giveup = _playing == PlayingParticipate && !_force->isAlive();
        
    int i = 0;
    while (i < _setting->threads->count()) {
        switch (updateSetting(_setting->threads->objectAtIndex(i), skipCommit)) {
            case UpdateSettingReturnSkip:
                skipReady = true;
            case UpdateSettingReturnNone:
                i++;
                break;
            case UpdateSettingReturnEnd:
                _setting->threads->removeObjectAtIndex(i);
                break;
            default:
#ifdef UseAutoPerformanceTest
                switch (_state) {
                    case MapStateVictory:
                    case MapStateDefeat:
                    case MapStateEnd:
					case MapStateInterrupt:
                        CSLog("performance test:%f(%" PRId64 ") update:%f lock:%f", CSThread::mainThread()->timeTotal(), CSThread::mainThread()->timeSequence(), _latency.updateAccumLatency, CSLock::totalWaitingTime());
                        CSLog("- map lock:%f", CSLock::waitingTime(GameLockMap));
                        CSLog("- object lock:%f", CSLock::waitingTime(GameLockObject));
                        CSLog("- effect lock:%f", CSLock::waitingTime(GameLockEffect));
                        CSLog("- force lock:%f", CSLock::waitingTime(GameLockForce));
                        break;
                }
#endif
                goto fin;
        }
    }
fin:
	_setting->skipReady = skipReady;
	_setting->skipCommit = false;

	if (giveup) {
		switch (_state) {
			case MapStateNone:
			case MapStateAuto:
				Sound::stop(SoundControlGameBgm);
				_running = false;
				_force->result = GameResultDefeat;
				_state = MapStateDefeat;
				break;
		}
	}
}
