//
//  Unit+action.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Unit.h"

#include "Map.h"

#include "Destination.h"
#include "Frame.h"

#define MoveNearFrameDivision   20
#define WatchFrameDivision      20
#define PathRetryFrame          40

void Unit::setWatching(const void *trigger, Unit *target) {
    if (_watchings) {
        foreach(Watching&, watching, _watchings) {
            if (watching.trigger == trigger) {
                retain(watching.target, target);
                return;
            }
        }
    }
    else {
        _watchings = new CSArray<Watching>(2);
    }
    {
        Watching& watching = _watchings->addObject();
        watching.trigger = trigger;
        watching.target = retain(target);
    }
}

bool Unit::getWatching(const void* trigger, Unit*& target) {
    foreach(Watching&, watching, _watchings) {
        if (watching.trigger == trigger) {
            target = watching.target;
            return true;
        }
    }
    return false;
}

void Unit::releaseWatchings() {
    if (_watchings) {
        foreach(const Watching&, watching, _watchings) {
            if (watching.target) watching.target->release();
        }
        _watchings->release();
        _watchings = NULL;
    }
}

void Unit::updateWatchings() {
    if (_watchings && (Map::sharedMap()->frame() + key) % WatchFrameDivision == 0) {
        releaseWatchings();
    }
}

bool Unit::isWatchable(const Unit* target) const {
    return !_watchingExclusions || !_watchingExclusions->containsObjectIdenticalTo(target->key);
}

void Unit::atMotion(const byte*& cursor) {
    int op = readByte(cursor);
    
    switch (op) {
        case 2:
            {
                AnimationIndex index(cursor);
                float rate = readAbilityFloat(cursor).value(this);
                bool rewind = readBoolean(cursor);
                
                setMotion(index, rate, rewind, false);
            }
            break;
		case 1:
			stopMotion(false);
			break;
		case 0:
			removeMotion(false);
			break;
    }
    _action->ip++;
}
void Unit::atBaseMotion(const byte*& cursor) {
    int index = readByte(cursor);
    
    setBaseMotion(index, false);
    
    _action->ip++;
}
void Unit::atCancelBaseMotion(const byte*& cursor) {
	int from = readByte(cursor);
	int to = readByte(cursor);

	cancelBaseMotion(from, to);

	_action->ip++;
}
void Unit::atChangeBaseMotion(const byte *&cursor) {
    int from = readByte(cursor);
    int to = readByte(cursor);
    _motions[from] = to;
    _action->ip++;
}
void Unit::atProgressMotion(const byte *&cursor) {
    int count = readLength(cursor);
    
    if (_building >= BuildingTransform) {
        int index = CSMath::min((int)(count * _progress / _data->progress), count - 1);
        
        setBaseMotion(cursor[index], false);
    }
    cursor += count;
    _action->ip++;
}
bool Unit::atTurn(const byte*& cursor) {
    int dir = readByte(cursor);
    int ip1 = readShort(cursor);
    
    switch (dir) {
        case 12:
            if (!_target || _target == this) {
                _action->ip++;
                return false;
            }
            dir = direction(_target->point());
            break;
        case 13:
            dir = (_motion.dir + 1) % 12;
            break;
        case 14:
            dir = (_motion.dir + 11) % 12;
            break;
    }
    if (turnMotion(dir)) {
        _action->ip++;
        return false;
    }
    else {
        _action->ip = ip1;
        interruptAction();
        return true;
    }
}
void Unit::atEffect(const byte*& cursor) {
    int toTarget = readByte(cursor);
    AnimationIndex index(cursor);
    index = _data->effectIndex(skin, index);
    bool attach = readBoolean(cursor);
	bool ui = readBoolean(cursor);
	int slot = readByte(cursor);

	if (slot < 0) {
        AnimationEffect::effect(index, this, toTarget ? _target : NULL, toTarget >> 1, attach, ui);
    }
    else {
		AnimationEffect*& effect = _effects[slot];

		if (effect) {
			if (effect->index() == index) {
				effect->setDestination(toTarget && _target ? _target : this);
				goto exit;
			}
			else {
				effect->dispose();
				effect->release();
			}
		}
		effect = retain(AnimationEffect::effect(index, this, toTarget ? _target : NULL, toTarget >> 1, attach, ui));
	}
exit:
    _action->ip++;
}
void Unit::atRemoveEffect(const byte*& cursor) {
    int slot = readByte(cursor);
    int op = readByte(cursor);
    
	AnimationEffect*& effect = _effects[slot];

    if (effect) {
		switch (op) {
			case 1:
				effect->stop();
				break;
			case 2:
				effect->dispose();
				break;
		}
		effect->release();
		effect = NULL;
    }

    _action->ip++;
}
void Unit::atEffectRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
	bool nearbyTo = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AnimationIndex index(cursor);
    index = _data->effectIndex(skin, index);
    bool attach = readBoolean(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    Map::sharedMap()->effectWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, index, nearbyTo, attach);
    
    _action->ip++;
}
void Unit::atEffectAngle(const byte*& cursor) {
    bool nearbyTo = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AnimationIndex index(cursor);
    index = _data->effectIndex(skin, index);
    bool attach = readBoolean(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->effectWithAngle(this, this, _target, range, angle, condition, index, nearbyTo, attach);
    }
    _action->ip++;
}
void Unit::atEffectLine(const byte*& cursor) {
    bool nearbyTo = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AnimationIndex index(cursor);
    index = _data->effectIndex(skin, index);
    bool attach = readBoolean(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->effectWithLine(this, this, _target, range, thickness, condition, index, nearbyTo, attach);
    }
    _action->ip++;
}

void Unit::atAttackWatch(const byte*& cursor) {
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    stopBuilding(StopBuildingTarget);
    
    if (!_force) {
        retain(_target, _originTarget);
        
        _action->ip = ip2;
        return;
    }
    bool moveable = ability(AbilityMoveSpeed) != fixed::Zero;
    
    if (_originTarget) {
        if (!_originTarget->isVisible(_force)) {
            changeAction();
            return;
        }
        if (_originTarget->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_originTarget);
            
            if ((!target->force() || target->force()->alliance() != _force->alliance()) && isWatchable(target)) {
                if (target->isAlive()) {
                    if (ability(AbilityAttack, fixed::Zero, target)) {
                        fixed d2 = _point.distanceSquared(target->_point);
                        fixed c = ability(AbilityAttackBlind, fixed::Zero, target);
                        if (c) {
                            c += _collider + target->_collider;
                            if (d2 < c * c) {
                                retain(_target, target);
                                _action->ip = ip2;
                                return;
                            }
                        }
                        c = _collider + target->_collider + ability(AbilityAttackRange, fixed::Zero, target) + EpsilonCollider;
                        
                        if (d2 <= c * c) {
                            retain(_target, target);
                            _action->ip++;
                        }
                        else if (moveable) {
                            retain(_target, target);
                            _action->ip = ip1;
                        }
                        else {
                            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandAttackFail)));
                            
                            changeAction();
                        }
                    }
                    else {
                        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandAttackFail)));
                        
                        changeAction();
                    }
                }
                else {
                    changeAction();
                }
                return;
            }
        }
    }
    if (_aggro && ability(AbilityAttack, fixed::Zero, _aggro) && isWatchable(_aggro)) {
        bool visible = _aggro->isVisible(_force);
        fixed d2 = _point.distanceSquared(_aggro->_point);
        fixed c = ability(AbilityAttackBlind, fixed::Zero, _aggro);
        if (c) {
            c += _collider + _aggro->_collider;
            if (d2 < c * c) {
                goto watch1;
            }
        }
        if (visible) {
            c = _collider + _aggro->_collider + ability(AbilityAttackRange, fixed::Zero, _aggro) + EpsilonCollider;

            if (d2 <= c * c) {
                retain(_target, _aggro);
                _action->ip++;
                return;
            }
        }
        if (moveable) {
            retain(_target, _aggro);
            _action->ip = ip1;
            return;
        }
    }
watch1:
    {
        Unit* target;
        if (getWatching(cursor, target)) {
            if (target) {
                if (target->compareType(ObjectMaskUnit) && target->isAlive() && target->isVisible(_force) && 
					(!target->force() || target->force()->alliance() != _force->alliance()) && ability(AbilityAttack, fixed::Zero, target) && 
                    isWatchable(target))
				{
                    fixed d2 = _point.distanceSquared(target->_point);
                    fixed c = ability(AbilityAttackBlind, fixed::Zero, target);
                    if (c) {
                        c += _collider + target->_collider;
                        if (d2 < c * c) {
                            goto watch2;
                        }
                    }
                    retain(_target, target);
                    
                    c = _collider + target->_collider + ability(AbilityAttackRange, fixed::Zero, target) + EpsilonCollider;
                    
                    if (d2 <= c * c) {
                        _action->ip++;
                    }
                    else {
                        _action->ip = ip1;
                    }
                    return;
                }
            }
            else {
                retain(_target, _originTarget);
                _action->ip = ip2;
                return;
            }
        }
    }
watch2:
    {
        Map::WatchReturn rtn = Map::sharedMap()->attackWatch(this, _watchingExclusions);
        
        setWatching(cursor, rtn.target);
        
        if (rtn.target) {
            retain(_target, rtn.target);
            
            _action->ip = rtn.inside ? _action->ip + 1 : ip1;
        }
        else {
            retain(_target, _originTarget);
            
            _action->ip = ip2;
        }
    }
}

void Unit::atAttack(const byte*& cursor) {
    AbilityValue<fixed> rate = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
    if (_target && _target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(_target);
        
        _revision += attack(target, rate.value(this, target), time);
    }
    _action->ip++;
}
void Unit::atAttackRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    _revision += Map::sharedMap()->attackWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, rate, attenuation, time);
    
    _action->ip++;
}
void Unit::atAttackAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    if (_target) {
        range += _collider;
        
        _revision += Map::sharedMap()->attackWithAngle(this, this, _target, range, angle, condition, rate, attenuation, time);
    }
    _action->ip++;
}
void Unit::atAttackLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    if (_target) {
        range += _collider;
        
        _revision += Map::sharedMap()->attackWithLine(this, this, _target, range, thickness, condition, rate, attenuation, time);
    }
    _action->ip++;
}

void Unit::atWatch(const byte*& cursor) {
    AbilityValue<fixed> range = readAbilityFixed(cursor);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    stopBuilding(StopBuildingTarget);
    
    bool moveable = ability(AbilityMoveSpeed) != fixed::Zero;
    
    if (_originTarget) {
        if (!_originTarget->isVisible(_force)) {
            changeAction();
            return;
        }
        if (_originTarget->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_originTarget);
            
            if (target->match(this, condition) && isWatchable(target)) {
                fixed d2 = _point.distanceSquared(target->_point);
                fixed c = _collider + target->_collider + range.value(this, target) + EpsilonCollider;
                
                if (d2 <= c * c) {
                    retain(_target, target);
                    _action->ip++;
                }
                else if (moveable) {
                    retain(_target, target);
                    _action->ip = ip1;
                }
                else {
					const ImageIndex& icon = skinData() ? skinData()->smallIcon : ImageIndex::None;

                    Map::sharedMap()->pushMessage(_force, Message::message(icon, Asset::sharedAsset()->string(MessageSetCommand, MessageCommandInvalidTarget)));
                    
                    changeAction();
                }
                return;
            }
        }
    }
    if (_aggro && _aggro->match(this, condition) && isWatchable(_aggro)) {
        if (_aggro->isVisible(_force)) {
            fixed d2 = _point.distanceSquared(_aggro->_point);
            fixed c = _collider + _aggro->_collider + range.value(this, _aggro) + EpsilonCollider;

            if (d2 <= c * c) {
                retain(_target, _aggro);
                _action->ip++;
                return;
            }
        }
        if (moveable) {
            retain(_target, _aggro);
            _action->ip = ip1;
            return;
        }
    }
    {
        Unit* target;
        if (getWatching(cursor, target)) {
            if (target) {
                if (target->compareType(ObjectMaskUnit) && target->isVisible(_force) && target->match(this, condition) && isWatchable(target)) {
                    retain(_target, target);
                    
                    fixed d2 = _point.distanceSquared(target->_point);
                    fixed c = _collider + target->_collider + range.value(this, target) + EpsilonCollider;
                    
                    if (d2 <= c * c) {
                        _action->ip++;
                    }
                    else {
                        _action->ip = ip1;
                    }
                    return;
                }
            }
            else {
                retain(_target, _originTarget);
                _action->ip = ip2;
                return;
            }
        }
    }

    Map::WatchReturn rtn = Map::sharedMap()->watch(this, this, condition, range, _watchingExclusions);
    
    setWatching(cursor, rtn.target);
    
    if (rtn.target) {
        retain(_target, rtn.target);
        _action->ip = rtn.inside ? _action->ip + 1 : ip1;
    }
    else {
        retain(_target, _originTarget);
        _action->ip = ip2;
    }
}
void Unit::atWatchExclude(const byte*& cursor) {
	switch (readByte(cursor)) {
		case 0:
			if (_watchingExclusions) _watchingExclusions->removeAllObjects();
			break;
		case 1:
			if (_target && _target->compareType(ObjectMaskUnit)) {
				uint key = static_cast<Unit*>(_target)->key;
				if (!_watchingExclusions) _watchingExclusions = new CSArray<uint>(4);
				else if (_watchingExclusions->containsObjectIdenticalTo(key)) break;
				_watchingExclusions->addObject(key);
			}
			break;
		case 2:
			if (_watchingExclusions && _target && _target->compareType(ObjectMaskUnit)) {
				uint key = static_cast<Unit*>(_target)->key;
				_watchingExclusions->removeObjectIdenticalTo(key);
			}
			break;
	}
	_action->ip++;
}
void Unit::atCooltime(const byte*& cursor) {
    int index = readByte(cursor);
    AbilityValue<fixed> cooltime = readAbilityFixed(cursor);
    bool reverse = readBoolean(cursor);
    
    fixed cooltimeValue = cooltime.value(this, _target && _target->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(_target) : NULL);
    
    if (reverse) {
        if (cooltimeValue) cooltimeValue = fixed::One / cooltimeValue;
    }
    _cooltimes[index].remaining = _cooltimes[index].initial = cooltimeValue;

	_action->ip++;
}
bool Unit::atIsCooltime(const byte*& cursor) {
    int index = readByte(cursor);
    int type = readByte(cursor);
    int ip1 = readShort(cursor);
    
    if (!_cooltimes[index].remaining) {
        _action->ip++;
    }
    else {
        _action->ip = ip1;
        
        switch (type) {
            case 2:
                interruptAction();
            case 1:
                return true;
        }
    }
    return false;
}
void Unit::atShowCooltime(const byte*& cursor) {
    int index = readByte(cursor);
    bool on = readBoolean(cursor);
    
    _cooltimes[index].displaying = on;
    
    _action->ip++;
}
void Unit::atCompare(const byte*& cursor) {
    int op = readByte(cursor);
    fixed value = readAbilityFixed(cursor).value(this);
    int ip1 = readShort(cursor);
    
    _action->ip = compare(op, value) ? _action->ip + 1 : ip1;
}
void Unit::atRevise(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    
    int op = readByte(cursor);
    
    AbilityValue<fixed> value = readAbilityFixed(cursor);
    
    fixed time = readFixed(cursor);
    
    if (toTarget) {
        if (_target && _target->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_target);
            
            target->revise(op, value.value(this, target), time, this);
        }
    }
    else {
        revise(op, value.value(this), time, this);
    }
    _action->ip++;
}
void Unit::atReviseRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int op = readByte(cursor);
    AbilityValue<fixed> value = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    _revision += Map::sharedMap()->reviseWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, op, value, attenuation, time);
    
    _action->ip++;
}
void Unit::atReviseAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int op = readByte(cursor);
    AbilityValue<fixed> value = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        _revision += Map::sharedMap()->reviseWithAngle(this, this, _target, range, angle, condition, op, value, attenuation, time);
    }
    _action->ip++;
}
void Unit::atReviseLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int op = readByte(cursor);
    AbilityValue<fixed> value = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        _revision += Map::sharedMap()->reviseWithLine(this, this, _target, range, thickness, condition, op, value, attenuation, time);
    }
    _action->ip++;
}
void Unit::atReviseReset(const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            _revision = fixed::Zero;
            break;
        case 1:
            _damage = fixed::Zero;
			_damageCount = 0;
            break;
    }
    _action->ip++;
}
void Unit::atResurrect(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    AbilityValue<fixed> value = readAbilityFixed(cursor);
    
    if (toTarget) {
        if (_target && _target->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_target);
            
            target->resurrect(value.value(this, target));
        }
    }
    else {
        resurrect(value.value(this));
    }
    _action->ip++;
}
void Unit::atIsLevel(const byte *&cursor) {
    int level = readShort(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = this->level >= level ? _action->ip + 1 : ip1;
}
void Unit::atHasUnit(const byte*& cursor) {
    UnitIndex index(cursor);
    bool alive = readBoolean(cursor);
    int unitCount = readByte(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = _force && _force->unitCount(index, alive) >= unitCount ? _action->ip + 1 : ip1;
}
void Unit::atHasUnits(const byte*& cursor) {
    int count = readLength(cursor);
    UnitIndex* indices = (UnitIndex*)alloca(count * sizeof(UnitIndex));
    for (int i = 0; i < count; i++) {
        indices[i] = UnitIndex(cursor);
    }
    bool alive = readBoolean(cursor);
    int unitCount = readByte(cursor);
    int ip1 = readShort(cursor);
    
    if (_force) {
        for (int i = 0; i < count; i++) {
            unitCount -= _force->unitCount(indices[i], alive);
        }
        if (unitCount <= 0) {
            _action->ip++;
            return;
        }
    }
    _action->ip = ip1;
}
void Unit::atHasRune(const byte*& cursor) {
    RuneIndex index(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = _force && _force->runeForIndex(index) && _force->isTrained(index) ? _action->ip + 1 : ip1;
}
void Unit::atIsDamaged(const byte*& cursor) {
    int ip1 = readShort(cursor);
    _action->ip = _damageCount ? _action->ip + 1 : ip1;
}
void Unit::atIsState(const byte*& cursor) {
    int state = readByte(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = _state == state ? _action->ip + 1 : ip1;
}
void Unit::atChangeState(const byte*& cursor) {
    int state = readByte(cursor);
    
    if (readBoolean(cursor)) {
        _action->ip++;

        Action* stack = new Action();
        stack->trigger = _data->actionTriggers->objectAtIndex(state);
        stack->ip = 0;
        stack->loop = 0;
        stack->prev = _action;
        _action = stack;
    }
    else {
        _state = state;
        if (_state == UnitStateDestroy) {
			if (_alive == AliveYes) _alive = AliveNo;
            deactivate();
            changeAction();
        }
        else {
			if (_alive == AliveNo && health()) _alive = AliveYes;
            _action->ip++;
        }
    }
}
void Unit::atChangeType(const byte*& cursor) {
    ObjectType type = (ObjectType)readByte(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = setType(type) ? _action->ip + 1 : ip1;
}
void Unit::atChangeBusy(const byte*& cursor) {
	_busy = readByte(cursor);
	_action->ip++;
}
void Unit::atChangeTarget(const byte*& cursor) {
	bool origin = readBoolean(cursor);

    stopBuilding(StopBuildingTarget);
    
    switch (readByte(cursor)) {
        case 0:
            release(_target);
			break;
        case 1:
            retain(_target, _originTarget);
            break;
        case 2:
            retain(_target, _parent);
            break;
        case 3:
            retain(_target, _aggro);
            break;
		case 4:
			if (_damageOrigins) {
				if (_damageOrigins->count()) {
					uint key = _damageOrigins->lastObject();

					_damageOrigins->removeLastObject();

					Unit* target = Map::sharedMap()->findUnit(key);

					retain(_target, target);
				}
				else {
					release(_target);
				}
			}
			else {
				release(_target);
				_damageOrigins = new CSArray<uint>();
			}
			break;
		case 5:
			release(_target);
			release(_damageOrigins);
			break;
        case 6:
            if (_target) {
                Destination* destination = new Destination(_force);
                destination->locate(_target->point());
                _target->release();
                _target = destination;
            }
            break;
        case 7:
            if (_target) {
                Destination* destination = new Destination(_force);
                destination->locate(_target->facingPoint(_point));
                _target->release();
                _target = destination;
            }
            break;
        case 8:
			{
				fixed range = readAbilityFixed(cursor).value(this);
				bool approach = readBoolean(cursor);

				if (_target) {
					FPoint diff = _target->point() - _point;
					fixed length = diff.length();
					if (approach && length <= _collider + _target->collider()) {
						release(_target);
					}
					else if (approach && length <= _collider + _target->collider() + range) {
						diff.normalize();
						diff *= _collider + _target->collider();

						Destination* destination = new Destination(_force);
						destination->locate(_target->point() - diff);
						_target->release();
						_target = destination;
					}
					else {
						diff.normalize();
						diff *= range + _collider;

						Destination* destination = new Destination(_force);
						destination->locate(_point + diff);
						_target->release();
						_target = destination;
					}
				}
			}
            break;
        case 9:
			{
				fixed rotation = CSMath::degreesToRadians(readFixed(cursor));

				if (_target) {
					FPoint p = _target->point() - _point;
					p.rotate(rotation);
					p += _point;

					Destination* destination = new Destination(_force);
					destination->locate(p);
					_target->release();
					_target = destination;
				}
			}
            break;
        case 10:
            {
                fixed range = readAbilityFixed(cursor).value(this);
                fixed thickness = readAbilityFixed(cursor).value(this);
                const byte* condition = cursor;
                cursor += matchLength(condition);

                if (_target) {
                    range += _collider;

                    FPoint p0, p1;
                    fixed distance = fixed::Max;
                    Unit* selectedTarget = NULL;
                    foreach(Unit*, target, Map::sharedMap()->unitsWithLine(this, this, _target, range, thickness, condition, p0, p1)) {
                        fixed d = _point.distance(target->point());
                        if (d < distance) {
                            distance = d;
                            selectedTarget = target;
                        }
                    }
                    retain(_target, selectedTarget);
                }
            }
            break;
        case 11:
            if (_originPoint) {
                Destination* destination = new Destination(_force);
                destination->locate(*_originPoint);
                _target->release();
                _target = destination;
            }
            break;
        case 12:
            if (_target && _target->compareType(ObjectMaskUnit)) {
                retain(_target, static_cast<Unit*>(_target)->_target);
            }
            break;
            
    }
	if (origin) {
		retain(_originTarget, _target);
	}
    _action->ip++;
}
void Unit::atHasTarget(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = _target ? _action->ip + 1 : ip1;
}
void Unit::atIsTarget(const byte*& cursor) {
    bool visible = readBoolean(cursor);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int ip1 = readShort(cursor);
    
    _action->ip = _target && _target->compareType(ObjectMaskUnit) && (visible ? _target->isVisible(_force) : _target->isLocated()) && static_cast<const Unit*>(_target)->match(this, condition) ? _action->ip + 1 : ip1;
}
void Unit::atIsBuffed(const byte*& cursor) {
    BuffIndex index(cursor);
    fixed stack = readFixed(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = isBuffed(index, stack) ? _action->ip + 1 : ip1;
}
void Unit::atBuff(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    BuffIndex index(cursor);
	int timeOp = readByte(cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cursor);
    
    if (toTarget) {
        if (_target && _target->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_target);
            
            target->buff(index, timeOp, time.value(this, target), stack.value(this, target), this);
        }
    }
    else {
        buff(index, timeOp, time.value(this), stack.value(this), this);
    }
    _action->ip++;
}
void Unit::atBuffRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    BuffIndex index(cursor);
	int timeOp = readByte(cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    Map::sharedMap()->buffWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, index, timeOp, time, stack);
    
    _action->ip++;
}
void Unit::atBuffAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    BuffIndex index(cursor);
	int timeOp = readByte(cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->buffWithAngle(this, this, _target, range, angle, condition, index, timeOp, time, stack);
    }
    _action->ip++;
}
void Unit::atBuffLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    BuffIndex index(cursor);
	int timeOp = readByte(cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->buffWithLine(this, this, _target, range, thickness, condition, index, timeOp, time, stack);
    }
    _action->ip++;
}
void Unit::atCloak(const byte*& cursor) {
    _cloaking = readBoolean(cursor);
    Map::sharedMap()->changeMinimap(this);
    _action->ip++;
}
void Unit::atDetect(const byte*& cursor) {
    setDetecting(readBoolean(cursor));
    _action->ip++;
}
void Unit::atIsAmbush(const byte*& cursor) {
	int ip1 = readShort(cursor);

	_action->ip = isHiding() ? _action->ip + 1 : ip1;
}
bool Unit::atMove(const byte*& cursor) {
    bool useRange = readBoolean(cursor);
    fixed range = fixed::Zero;
    if (useRange) {
        AbilityValue<fixed> rangeValue = readAbilityFixed(cursor);
        if (_target) {
            range = rangeValue.value(this, _target->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(_target) : NULL);
        }
    }
    int ip1 = readShort(cursor);
    
    MoveLog(this, "=========================================");
    MoveLog(this, "atMove start (position:%.2f, %.2f) (delay:%d)", (float)_point.x, (float)_point.y, _pathDelay);
    
    if (!_target || _target == this) {
        _action->ip++;
        return false;
    }

    const FPoint& dp = _target->point();

	const FPoint& np = !useRange && _path && !_path->isComplete() ? _path->lastPoint() : dp;

	{
		fixed c = _collider + _target->collider() + range + EpsilonCollider;

		if (!useRange && (Map::sharedMap()->frame() + key) % MoveNearFrameDivision == 0) {
			c += Map::sharedMap()->moveableDistance(this, _target, np);
		}

		MoveLog(this, "atMove target (target:%.2f, %.2f) (near:%.2f, %.2f) (collider:%.2f)", (float)dp.x, (float)dp.y, (float)np.x, (float)np.y, (float)c);

		if (_point.distanceSquared(np) <= c * c && (!useRange || !_force || Map::sharedMap()->isVisible(_target->bounds(), _force->alliance()))) {
			if (_path && !_path->isComplete()) {
				const ImageIndex& icon = skinData() ? skinData()->smallIcon : ImageIndex::None;

				Map::sharedMap()->pushMessage(_force, Message::message(icon, Asset::sharedAsset()->string(MessageSetCommand, MessageCommandMoveUnabled)));
			}
			release(_path);
			_movement.dir = 0;
			_pathDelay = 0;

			_action->ip++;

			MoveLog(this, "atMove complete");

			return false;
		}
	}

    fixed move = ability(AbilityMoveSpeed) * Map::sharedMap()->frameDelayFixed();
    
    if (move) {
        if (!_path && !_pathDelay) {
            Map::FindPathReturn rtn = Map::sharedMap()->findPath(this, _target);
            if (rtn.path) {
                _path = rtn.path;
                _path->retain();
            }
            if (!rtn.retry) {
                _pathDelay = PathRetryFrame;
            }
            MoveLog(this, "atMove find path (path:%d) (retry:%d)", _path ? (_path->isComplete() + 1) : 0, rtn.retry);
        }
        if (_path) {
            int dir;
            const FPoint& p = _path->point(_point, np, true, &dir);

            MoveLog(this, "atMove move start (position:%.2f, %.2f) (index:%d) (dir:%d)", (float)p.x, (float)p.y, _path->pointIndex(), dir);
            
            turnMotion(direction(p));
            
            if (!Map::sharedMap()->move(this, _target, p, move, dir)) {
				Map::sharedMap()->flock(this, move, MapLocationEnabled);
                _path->rewind();
            }
            
            ProgressPathReturn progress = Map::sharedMap()->progressPath(this, _target, _path);
            
            MoveLog(this, "atMove move end (progress:%d) (remaining:%d)", progress, _path->pointCount());

            switch (progress) {
                case ProgressPathSuccess:
                    _pathDelay = PathRetryFrame;
                    break;
                case ProgressPathFail:
                    if (_path->isComplete()) _pathDelay = 0;
					release(_path);
					break;
                case ProgressPathOut:
                    if (!_pathDelay) {
                        release(_path);
                    }
                    break;
            }
        }
        else {
            Map::sharedMap()->flock(this, move, MapLocationEnabled);
        }
    }
    _action->ip = ip1;
    interruptAction();
    return true;
}
bool Unit::atFlock(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    fixed move = ability(AbilityMoveSpeed) * Map::sharedMap()->frameDelayFixed();
    
    if (move) {
        if (Map::sharedMap()->flock(this, move, MapLocationUnabled)) {
            _action->ip = ip1;
            interruptAction();
            return true;
        }
    }
    _action->ip++;
    return false;
}
void Unit::atCollision(const byte*& cursor) {
    setCollision(readBoolean(cursor));
    _action->ip++;
}
void Unit::atPoint(const byte*& cursor) {
    if (!_originPoint) {
        _originPoint = new FPoint();
    }
    *_originPoint = _point;
    _action->ip++;
}
bool Unit::atEscape(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (_target) {
        if (toTarget) {
            if (_target->compareType(ObjectMaskUnit) && _target->isLocated()) {
                Unit* target = static_cast<Unit*>(_target);

                target->escape(_point, time.value(this, target));
            }
        }
        else {
            escape(_target->point(), time.value(this));
        }
    }
    
    _action->ip++;
    
	return commitEscaping();
}
bool Unit::atEscapeRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    Map::sharedMap()->escapeWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, time);
    
    _action->ip++;
    
	return commitEscaping();
}
bool Unit::atEscapeAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->escapeWithAngle(this, this, _target, range, angle, condition, time);
    }
    
    _action->ip++;
    
	return commitEscaping();
}
bool Unit::atEscapeLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->escapeWithLine(this, this, _target, range, thickness, condition, time);
    }
    
    _action->ip++;
    
	return commitEscaping();
}
bool Unit::atJump(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    AbilityValue<fixed> distance = readAbilityFixed(cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
    if (_target) {
        if (toTarget) {
            if (_target->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(_target);
                target->jump(_point, distance.value(this, target) + _collider, altitude, time, pushing);
            }
        }
        else {
            jump(_target->point(), distance.value(this) + _target->collider(), altitude, time, pushing);
        }
    }
    _action->ip++;
    
    return commitJumping();
}
bool Unit::atJumpRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> distance = readAbilityFixed(cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    Map::sharedMap()->jumpWithRange(this, this, toTarget ? _target : NULL, (toTarget >> 1) & 1, range, condition, distance, altitude, time, pushing, toTarget >> 2);
    
    _action->ip++;
    
	return commitJumping();
}
bool Unit::atJumpAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> distance = readAbilityFixed(cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->jumpWithAngle(this, this, _target, range, angle, condition, distance, altitude, time, pushing);
    }
    
    _action->ip++;
    
	return commitJumping();
}
bool Unit::atJumpLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> distance = readAbilityFixed(cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->jumpWithLine(this, this, _target, range, thickness, condition, distance, altitude, time, pushing);
    }
    
    _action->ip++;
    
	return commitJumping();
}

void Unit::atWarpTo(const byte*& cursor) {
    bool vision = readBoolean(cursor);
    int ip1 = readShort(cursor);
    
    if (_target) {
        FPoint point = _target->point();
        
        if ((!vision || !_force || Map::sharedMap()->isVisible(bounds(point), _force->alliance())) && warp(point)) {
            _action->ip++;
            return;
        }
    }
    _action->ip = ip1;
}
void Unit::atWarpFrom(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (_target && _target->compareType(ObjectMaskUnit) && _target->isLocated()) {
        FPoint point = _point;
        
        if (static_cast<Unit*>(_target)->warp(point)) {
            _action->ip++;
            return;
        }
    }
    _action->ip = ip1;
}
void Unit::atWarpRangeTo(const byte*& cursor) {
    bool nearby = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    bool vision = readBoolean(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->warpWithRange(this, _target, this, nearby, range, condition, vision);
    }
    _action->ip++;
}
void Unit::atWarpRangeFrom(const byte*& cursor) {
    bool nearby = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    
    if (_target) {
        Map::sharedMap()->warpWithRange(this, this, _target, nearby, range, condition, false);
    }
    _action->ip++;
}
bool Unit::atFreeze(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    _action->ip++;
    
    if (toTarget) {
        if (_target && _target->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_target);
            
            target->freeze(time.value(this, target));
        }
    }
    else {
        freeze(time.value(this));
    }
	return commitFreezing();
}
bool Unit::atFreezeRange(const byte*& cursor) {
    int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (!toTarget) {
        range += _collider;
    }
    Map::sharedMap()->freezeWithRange(this, this, toTarget ? _target : NULL, toTarget >> 1, range, condition, time);
    
    _action->ip++;
    
	return commitFreezing();
}
bool Unit::atFreezeAngle(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(this));
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->freezeWithAngle(this, this, _target, range, angle, condition, time);
    }
    
    _action->ip++;
    
	return commitFreezing();
}
bool Unit::atFreezeLine(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    fixed thickness = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    
    if (_target) {
        range += _collider;
        
        Map::sharedMap()->freezeWithLine(this, this, _target, range, thickness, condition, time);
    }
    
    _action->ip++;
    
	return commitFreezing();
}

bool Unit::atBuild(const byte*& cursor) {
    int type = readByte(cursor);    //0:complete 1:independent 2:dependent 3:reserve
    int ip1 = readShort(cursor);
    
    if (_target) {
        if (_target->compareType(ObjectMaskFrame)) {
            Frame* target = static_cast<Frame*>(_target);
            
            const UnitData& data = target->data();
            
            if (type == 3) {
                if (_building == BuildingNone) {
                    if (_force) {
                        for (int i = 0; i < 2; i++) {
                            if (_force->resource(i) < data.resources[i]) {
                                Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughMineral + i)));
								Map::sharedMap()->announce(_force, (AnnounceIndex)(AnnounceNotEnoughMineral + i));
                                _action->ip = ip1;
                                return false;
                            }
                        }
                        if (data.population && _force->population() + data.population > _force->supply()) {
                            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughSupply)));
                            _action->ip = ip1;
                            return false;
                        }
                        _force->reviseResource(0, (fixed)-data.resources[0]);
                        _force->reviseResource(1, (fixed)-data.resources[1]);
                        _force->revisePopulation(data.population);
                    }
                    _building = BuildingTargetReady;
                    _action->ip++;
                    return false;
                }
            }
            else if (_building == BuildingTargetReady) {
                MapLocation rtn = Map::sharedMap()->buildable(this, data.type, _target->point(), data.collider, NULL, CommandBuildTargetAll);
                
                switch (rtn) {
                    case MapLocationCollided:
                        interruptAction();
                        return true;
                    case MapLocationUnabled:
                        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandBuildUnabled)));
                        _action->ip = ip1;
                        return false;
                }
                if (_force) {
                    _force->revisePopulation(-data.population);
                }
                Unit* unit = Unit::unit(_force, target->index, 0, 0, this);
				
				target->unlocate();

				unit->locate(target->point());
				Map::sharedMap()->voice(VoiceSpawn, unit);
                
                release(_originTarget);
                retain(_target, unit);
                
                switch (type) {
                    case 1:
                        unit->startBuilding(StartBuildingIndependent);
                        break;
                    case 2:
                        unit->startBuilding(StartBuildingDependent);
                        _building = BuildingTargetProgress;
                        break;
                }
                _action->ip++;
                return false;
            }
            
        }
        else if (_target->compareType(ObjectMaskUnit)) {
            Unit* target = static_cast<Unit*>(_target);
            
            if (target->force() == _force) {
                switch (type) {
                    case 2:
                        if (_building == BuildingTargetReady && target->resumeBuilding()) {
                            _building = BuildingTargetProgress;
                            _action->ip++;
                            return false;
                        }
                        break;
                    case 3:
                        if (_building == BuildingNone && target->reserveBuilding()) {
                            _building = BuildingTargetReady;
                            _action->ip++;
                            return false;
                        }
                        break;
                }
            }
        }
    }
    _action->ip = ip1;
    return false;
}
void Unit::atCreate(const byte*& cursor) {
    int toTarget = readByte(cursor);
	bool useResource = readBoolean(cursor);
	UnitIndex index = readRandomUnitIndex(cursor);
    int ip1 = readShort(cursor);
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    FPoint p = _point;
    switch (toTarget) {
        case 1:
            if (_target) {
                p = _target->point();
            }
            break;
        case 2:
            if (_target) {
                p = _target->facingPoint(_point);
            }
            break;
    }
    if (Map::sharedMap()->locatablePosition(data.type, p, data.collider, NULL, MapLocationEnabled) != MapLocationUnabled) {
		if (_force) {
			if (useResource) {
				for (int i = 0; i < 2; i++) {
					if (_force->resource(i) < data.resources[i]) {
						Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughMineral + i)));
						Map::sharedMap()->announce(_force, (AnnounceIndex)(AnnounceNotEnoughMineral + i));
						_action->ip = ip1;
						return;
					}
				}
				if (data.population && _force->population() + data.population > _force->supply()) {
					Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughSupply)));
					_action->ip = ip1;
					return;
				}
				_force->reviseResource(0, (fixed)-data.resources[0]);
				_force->reviseResource(1, (fixed)-data.resources[1]);
			}
        }
        Unit* unit = Unit::unit(_force, index, 0, 0, this);
        unit->_originTarget = retain(_target);
        unit->_target = retain(_target);
		unit->locate(p);
		Map::sharedMap()->voice(VoiceSpawn, unit);
        _action->ip++;
    }
    else {
        _action->ip = ip1;
    }
}
void Unit::atTransform(const byte*& cursor) {
	UnitIndex index = readRandomUnitIndex(cursor);
    int ip1 = readShort(cursor);
    
    int type = readByte(cursor);

    switch (type) {
        case 0:
            if (setIndex(index)) {
                return;
            }
            break;
        case 1:
        case 2:
            {
                const UnitData& data = Asset::sharedAsset()->unit(index);
                
                if (_force) {
                    for (int i = 0; i < 2; i++) {
                        if (_force->resource(i) < data.resources[i]) {
                            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughMineral + i)));
							Map::sharedMap()->announce(_force, (AnnounceIndex)(AnnounceNotEnoughMineral + i));
                            _action->ip = ip1;
                            return;
                        }
                    }
                    int populationDiff = data.population - _data->population;
                    if (populationDiff > 0 && _force->population() + populationDiff > _force->supply()) {
                        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughSupply)));
                        _action->ip = ip1;
                        return;
                    }
                }
                if (type == 1) {
                    _action->ip++;
                    return;
                }
                else {
                    _originState = readByte(cursor);

                    if (setIndex(index)) {
                        if (_force) {
                            _force->reviseResource(0, (fixed)-data.resources[0]);
                            _force->reviseResource(1, (fixed)-data.resources[1]);
                        }
                        if (data.progress) startBuilding(StartBuildingTransform);
                        return;
                    }
                }
            }
            break;
        case 3:
            if (_target && _target->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(_target);
                
                if (target->setIndex(index)) {
                    _action->ip++;
                    return;
                }
            }
            break;
    }
    _action->ip = ip1;
}
void Unit::atCancel(const byte *&cursor) {
    int type = readByte(cursor);    //0:self complete 1:self transform 2:target complete
    int ip1 = readShort(cursor);
    
    switch (type) {
        case 0:
            if (resetIndex()) {
                return;
            }
            break;
        case 1:
            if (stopBuilding(StopBuildingCancel)) {
                _action->ip++;
                return;
            }
            break;
        case 2:
            if (_target && _target->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(_target);
                
                if (target->resetIndex()) {
                    _action->ip++;
                    return;
                }
            }
            break;
            
    }
    _action->ip = ip1;
}
void Unit::atCopy(const byte*& cursor) {
    if (_target && _target->compareType(ObjectMaskUnit)) {
        const Unit* target = static_cast<const Unit*>(_target);
        
        if (setIndex(target->index())) {
            return;
        }
    }
    _action->ip++;
}
void Unit::atAttachTarget(const byte*& cursor) {
    bool ally = readByte(cursor);
	bool toLevel = readBoolean(cursor);
    
    if (_target && _target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(_target);
        
        target->setForce(ally ? _force : NULL, toLevel ? level : CSMath::min(target->level, level));
    }
    _action->ip++;
}
void Unit::atDetachTarget(const byte*& cursor) {
    if (_target && _target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(_target);
        
        target->resetForce();
    }
    _action->ip++;
}
void Unit::atAttach(const byte*& cursor) {
	if (_target && _target->compareType(ObjectMaskUnit)) {
		Unit* target = static_cast<Unit*>(_target);

		setForce(target->_force);
	}
	_action->ip++;
}
void Unit::atDetach(const byte*& cursor) {
	resetForce();
	_action->ip++;
}
void Unit::atIsTransporting(const byte *&cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = _transporting && _transporting->crews->count() ? _action->ip + 1 : ip1;
}
void Unit::atFindCall(const byte *&cursor) {
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int ip1 = readShort(cursor);
    
	struct Target {
		Unit* unit;
		fixed d2;
	};
	CSArray<Target>* targets = new CSArray<Target>();
    foreach(const Unit*, unit, Map::sharedMap()->units()) {
        if (unit->isLocated() && unit->match(this, condition)) {
			fixed d2 = unit->_point.distanceSquared(_point);
			
			int i;
			for (i = 0; i < targets->count(); i++) {
				if (d2 < targets->objectAtIndex(i).d2) {
					break;
				}
			}
			Target& target = targets->insertObject(i);
			target.unit = const_cast<Unit*>(unit);
			target.d2 = d2;
        }
    }
	foreach(Target&, target, targets) {
		if (target.unit->sendCall(this, false) != SendCallReturnNone) {
			stopBuilding(StopBuildingTarget);

			retain(_target, target.unit);
			_action->ip++;

			targets->release();
			return;
		}
	}
	targets->release();
    _action->ip = ip1;
}
bool Unit::atSendCall(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (_target && _target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(_target);
        
        switch (target->sendCall(this, true)) {
            case SendCallReturnNear:
                unlocate();
				_nextActions->removeAllObjects();
                changeAction();
                return true;
            case SendCallReturnFar:
                _action->ip++;
                return false;
        }
    }
    _action->ip = ip1;
    return false;
}
void Unit::atReceiveCall(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (_transporting && _transporting->follows->count()) {
        stopBuilding(StopBuildingTarget);
        
        retain(_target, _transporting->follows->lastObject());
        
        _action->ip++;
    }
    else {
        _action->ip = ip1;
    }
}
void Unit::atUnload(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (_transporting && _transporting->crews->count()) {
        Unit* crew = _transporting->crews->lastObject();
        
        FPoint point = _point;
        
        if (Map::sharedMap()->locatablePosition(crew, point, NULL) != MapLocationUnabled) {
            crew->locate(point);
            
            synchronized(this, GameLockObject) {
                _transporting->crews->removeLastObject();
                
                if (!_transporting->crews->count() && !_transporting->follows->count()) {
                    _transporting->crews->release();
                    _transporting->follows->release();
                    delete _transporting;
                    _transporting = NULL;
                }
            }
            _dirtyui = true;
            _action->ip++;
            return;
        }
        else {
            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandMoveUnabled)));
        }
    }
    _action->ip = ip1;
}
void Unit::atIsResource(const byte*& cursor) {
    int kind = readByte(cursor);
    bool gather = readBoolean(cursor);
    int ip1 = readShort(cursor);
    
    if (_target) {
        if (kind == 0) {
            if (_target->compareType(ObjectMaskMineral)) {
                _action->ip++;
                return;
            }
        }
        else if (kind == 1) {
            if (gather) {
                if (_target->compareType(ObjectMaskRefinery)) {
                    const Unit* mine = static_cast<const Unit*>(_target);
                    
                    if (mine->_force == _force && mine->_located && mine->_building <= BuildingTransform) {
                        _action->ip++;
                        return;
                    }
                }
                else if (_target->compareType(ObjectMaskGas)) {
                    const Unit* mine = Map::sharedMap()->findRefinery(_target->point());
                    
                    if (mine && mine->_force == _force && mine->_building <= BuildingTransform) {
                        _action->ip++;
                        return;
                    }
                }
            }
            else {
                if (_target->compareType(ObjectMaskGas)) {
                    _action->ip++;
                    return;
                }
            }
        }
    }
    _action->ip = ip1;
}
void Unit::atFindResource(const byte*& cursor) {
    int kind = readByte(cursor);
	bool inArea = readBoolean(cursor);
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    stopBuilding(StopBuildingTarget);
    
    Resource* resource = NULL;
    
    if (_originTarget) {
        if (kind == 0) {
            if (_originTarget->compareType(ObjectMaskMineral)) {
                Resource* r = static_cast<Resource*>(_originTarget);
                if (r->isGatherabled(_force)) {
                    resource = r;
                }
            }
        }
        else if (kind == 1) {
            if (_originTarget->compareType(ObjectMaskRefinery)) {
                Resource* r = Map::sharedMap()->findGas(_originTarget->point());
                
                if (r->isGatherabled(_force)) {
                    resource = r;
                }
            }
            else if (_originTarget->compareType(ObjectMaskGas)) {
                Resource* r = static_cast<Resource*>(_originTarget);
                if (r->isGatherabled(_force)) {
                    resource = r;
                }
            }
        }
    }
    if (!resource) {
        if (_gathering && _gathering->resource && _gathering->resource->kind == kind) {
            if (_gathering->resource->isGatherabled(_force)) {
                resource = _gathering->resource;
            }
            else {
                resource = Map::sharedMap()->findResource(this, _gathering->resource);
            }
        }
        else {
            resource = Map::sharedMap()->findResource(this, kind, inArea);
        }
    }
    if (resource) {
        synchronized(this, GameLockObject) {
            if (!_gathering) {
                _gathering = new Gathering();
                memset(_gathering, 0, sizeof(Gathering));
            }
            else if (_gathering->resource) {
                if (_gathering->occupied) {
                    _gathering->resource->occupy(false);
                }
                _gathering->resource->endGathering();
            }
            retain(_gathering->resource, resource);
        }
        retain(_target, resource);
        _action->ip = resource->gatherings() < resource->data().gatherings ? _action->ip + 1 : ip1;
        resource->startGathering();
    }
    else {
        release(_target);
        _action->ip = ip2;
    }
}
bool Unit::atOccupyResource(const byte*& cursor) {
	fixed waiting = readFixed(cursor);
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    stopBuilding(StopBuildingTarget);
    
    release(_originTarget);
    
    if (_gathering && _gathering->resource) {
        if (_gathering->resource->isGatherabled(_force)) {
			if (!_gathering->resource->occupied()) {
				_gathering->waiting = 0;
				_gathering->resource->occupy(true);
				_gathering->occupied = true;
				_action->ip++;
				return false;
			}
			else if (++_gathering->waiting < (int)(waiting * Map::sharedMap()->framePerSecond())) {
				interruptAction();
				return true;
			}
        }
        
		_gathering->waiting = 0;

        Resource* resource = Map::sharedMap()->findResource(this, _gathering->resource);
            
        if (resource) {
            _gathering->resource->endGathering();
            retain(_gathering->resource, resource);
            _gathering->resource->startGathering();
                
            if (_target == resource) {
                interruptAction();
                return true;
            }
            else {
                retain(_target, resource);
                _action->ip = ip1;
                return false;
            }
        }
    }
    release(_target);
    _action->ip = ip2;
    return false;
}
void Unit::atGatherResource(const byte*& cursor) {
    fixed amount = readAbilityFixed(cursor).value(this);
    int ip1 = readShort(cursor);

    if (_gathering && _gathering->resource && _gathering->occupied) {
        _gathering->occupied = false;
        _gathering->resource->occupy(false);
        _gathering->cargoKind = _gathering->resource->kind;
        _gathering->cargo = _gathering->resource->gather(amount);
        if (_force) {
            _force->gatherResource(_gathering->cargoKind, _gathering->cargo);
        }
		if (!Map::sharedMap()->isResourceRemaining(_gathering->resource)) {
			Map::sharedMap()->announce(_force, (AnnounceIndex)(AnnounceRunOutMineral + _gathering->resource->kind));
		}
        _action->ip++;
    }
    else {
        _action->ip = ip1;
    }
}
bool Unit::atFindBase() {
    stopBuilding(StopBuildingTarget);
    
    Unit* base = NULL;
    
    if (_originTarget && _originTarget->compareType(ObjectMaskBase)) {
        Unit* b = static_cast<Unit*>(_originTarget);
            
        if (b->_force == _force && b->_located && b->_building <= BuildingTransform) {
            base = b;
        }
    }
    if (!base) {
        base = Map::sharedMap()->findBase(this);
    }
    if (base) {
        if (!_gathering) {
            synchronized(this, GameLockObject) {
                _gathering = new Gathering();
                memset(_gathering, 0, sizeof(Gathering));
            }
        }
        retain(_gathering->base, base);
        retain(_target, base);
        return true;
    }
    else {
        if (_gathering) {
            release(_gathering->base);
        }
        release(_target);
        return false;
    }
}
void Unit::atFindBase(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = atFindBase() ? _action->ip + 1 : ip1;
}
void Unit::atReturnCargo(const byte*& cursor) {
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    release(_originTarget);
    
    if (_gathering && _gathering->cargo) {
        if (_gathering->base && _gathering->base->compareType(ObjectMaskBase) && _gathering->base->_force == _force && _gathering->base->_located && _gathering->base->_building <= BuildingTransform) {
            if (_force) {
                _force->reviseResource(_gathering->cargoKind, _gathering->cargo);
            }
            _gathering->cargo = fixed::Zero;
            _action->ip++;
        }
        else if (atFindBase()) {
            _action->ip = ip1;
        }
        else _action->ip = ip2;
    }
    else _action->ip++;
}
void Unit::atHasCargo(const byte*& cursor) {
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    _action->ip = _gathering && _gathering->cargo ? (_gathering->cargoKind == 0 ? _action->ip + 1 : ip1) : ip2;
}
void Unit::atReturnGathering(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = Map::sharedMap()->findArea(this) ? _action->ip + 1 : ip1;
}
void Unit::atRefineryEnabled(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (compareType(ObjectMaskRefinery)) {
        Resource* resource = Map::sharedMap()->findGas(_point);
        
        if (resource && resource->remaining()) {
            _action->ip++;
            return;
        }
    }
    _action->ip = ip1;
}
void Unit::atRefineryOccupied(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (compareType(ObjectMaskRefinery)) {
        Resource* resource = Map::sharedMap()->findGas(_point);
        
        if (resource && resource->occupied()) {
            _action->ip++;
            return;
        }
    }
    _action->ip = ip1;
}
void Unit::atIsBuilding(const byte*& cursor) {
    int building = readByte(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = (building & (1 << _building)) ? _action->ip + 1 : ip1;
}
void Unit::atIsProducing(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = _producing && _producing->count ? _action->ip + 1 : ip1;
}
void Unit::atIsTraining(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    _action->ip = _training ? _action->ip + 1 : ip1;
}
void Unit::atIsAlliance(const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    const Force* playerForce = Map::sharedMap()->force();
    
    if (playerForce && _force && _force->alliance() == playerForce->alliance()) {
        _action->ip++;
    }
    else {
        _action->ip = ip1;
    }
}
void Unit::atNavigate(const byte*& cursor) {
    Navigating navigating = (Navigating)readByte(cursor);
    
    if (_navigating != navigating) {
        if (compareType(ObjectMaskUnit)) {
            Map::sharedMap()->resetNavigation(this, navigating);
        }
        _navigating = navigating;
    }
    _action->ip++;
}
void Unit::atSupply(const byte*& cursor) {
    int supply = readByte(cursor);
    
    if (_supply < supply) {
        if (_force) {
            _force->reviseSupply(supply - _supply);
        }
        _supply = supply;
    }
    _action->ip++;
}
void Unit::atAdvance(const byte *&cursor) {
    stopBuilding(StopBuildingTarget);
    
    FPoint p;
    if (Map::sharedMap()->targetToAdvance(this, p)) {
        if (_originTarget && _originTarget->point() == p) {
            changeAction();
        }
        else {
            Destination* target = Destination::destination(_force);
            target->locate(p);
            retain(_originTarget, target);
            retain(_target, target);
            _action->ip++;
        }
    }
    else {
        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandAdvanceFail)));
        
        changeAction();
    }
}
void Unit::atRetreat(const byte *&cursor) {
    stopBuilding(StopBuildingTarget);
    
    FPoint p;
    if (Map::sharedMap()->targetToRetreat(this, p)) {
        Destination* target = Destination::destination(_force);
        target->locate(p);
        retain(_originTarget, target);
        retain(_target, target);
        _action->ip++;
    }
    else {
        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandRetreatFail)));
        
        changeAction();
    }
}
void Unit::atRandom(const byte*& cursor) {
    fixed rate = readAbilityFixed(cursor).value(this);
    int ip1 = readShort(cursor);
    
    _action->ip = Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One) <= rate ? _action->ip + 1 : ip1;
}
void Unit::atWatchRandom(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    fixed range = readAbilityFixed(cursor).value(this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    fixed rate = readAbilityFixed(cursor).value(this);
    int ip1 = readShort(cursor);
    
    if (!toTarget || _target) {
        _action->ip = Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One) <= rate * Map::sharedMap()->count(this, toTarget ? _target : this, condition, range) ? _action->ip + 1 : ip1;
    }
    else {
        _action->ip = ip1;
    }
}
void Unit::atChildrenCount(const byte*& cursor) {
	const byte* condition = cursor;
	cursor += matchLength(condition);
    int count = readAbilityByte(cursor).value(this);
    int ip1 = readShort(cursor);
    
	int matching = 0;
	foreach(const Unit*, child, _children) {
		if (child->match(this, condition)) matching++;
	}
    _action->ip = matching >= count ? _action->ip + 1 : ip1;
}
void Unit::atChildrenTarget(const byte*& cursor) {
	const byte* condition = cursor;
	cursor += matchLength(condition);

    foreach(Unit*, child, _children) {
		if (child->match(this, condition)) {
			retain(child->_originTarget, _target);
		}
    }
    _action->ip++;
}
void Unit::atChildrenCommand(const byte*& cursor) {
	const byte* condition = cursor;
	cursor += matchLength(condition);
	int state = readByte(cursor);
	CommandStack cs = (CommandStack)readByte(cursor);

	foreach(Unit*, child, _children) {
		if (child->match(this, condition)) {
			child->commandAction(cs, CommandIndex::None, child->data().actionTriggers->objectAtIndex(state), _target, true);
		}
	}
	_action->ip++;
}
void Unit::atParentInRange(const byte*& cursor) {
    fixed range = readAbilityFixed(cursor).value(this);
    int ip1 = readShort(cursor);
    int ip2 = readShort(cursor);
    
    if (_parent && _parent->_located) {
        if (_point.distanceSquared(_parent->_point) <= range * range) {
            _action->ip++;
        }
        else {
            _action->ip = ip1;
        }
    }
    else {
        _action->ip = ip2;
    }
}
void Unit::atDisplayRange(const byte*& cursor) {
    bool visible = readBoolean(cursor);
    
    if (visible) {
        if (!_rangeDisplayValue) {
            _rangeDisplayValue = new AbilityValue<float>();
        }
        *_rangeDisplayValue = readAbilityFloat(cursor);
    }
    else {
        if (_rangeDisplayValue) {
            delete _rangeDisplayValue;
            _rangeDisplayValue = NULL;
        }
    }
    _action->ip++;
}
void Unit::atTile(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    int mask = readByte(cursor);
    int ip1 = readShort(cursor);
    
    int attr;
    
    if (toTarget) {
        if (_target) {
            attr = Map::sharedMap()->tile(_target->point()).attribute();
        }
        else {
            _action->ip = ip1;
            return;
        }
    }
    else {
        attr = Map::sharedMap()->tile(_point).attribute();
    }
    _action->ip = (1 << attr) & mask ? _action->ip + 1 : ip1;
}
void Unit::atEvent(const byte*& cursor) {
    int target = readByte(cursor);
    const CSColor& color = _force ? _force->allianceColor() : Asset::neutralColor;
    
    static const AnimationIndex animation(AnimationSetGame, AnimationGameMinimapEvent);
    
    switch (target) {
        case 0:
            Map::sharedMap()->event(_force, animation, color, _point);
            break;
        case 1:
            Map::sharedMap()->event(_force, true, animation, color, _point);
            break;
        case 2:
            Map::sharedMap()->event(_force, false, animation, color, _point);
            break;
        default:
            Map::sharedMap()->event(animation, color, _point);
            break;
    }
    _action->ip++;
}
void Unit::atSound(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
	int soundCount = readLength(cursor);
	const byte* soundCursor = cursor + (randInt(0, soundCount - 1) * sizeof(SoundIndex));
	SoundIndex soundIndex(soundCursor);
	cursor += soundCount * sizeof(SoundIndex);
    float volume = readFloat(cursor);
    bool essential = readBoolean(cursor);
    
    if (toTarget) {
        if (_target) {
            Map::sharedMap()->sound(soundIndex, _target, volume, essential);
        }
    }
    else {
        Map::sharedMap()->sound(soundIndex, this, volume, essential);
    }
    _action->ip++;
}
void Unit::atAnnounce(const byte*& cursor) {
	int target = readByte(cursor);
	AnnounceIndex index = (AnnounceIndex)readByte(cursor);

	switch (target) {
		case 0:
			Map::sharedMap()->announce(_force, index);
			break;
		case 1:
			Map::sharedMap()->announce(_force, true, index);
			break;
		case 2:
			Map::sharedMap()->announce(_force, false, index);
			break;
		default:
			Map::sharedMap()->announce(index);
			break;
	}
	_action->ip++;
}
void Unit::atVibrate(const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    int level = readByte(cursor);
    
    if (toTarget) {
        if (_target) {
            Map::sharedMap()->vibrate(level, _target);
        }
    }
    else {
        Map::sharedMap()->vibrate(level, this);
    }
    
    _action->ip++;
}
void Unit::atMessage(const byte*& cursor) {
    int target = readByte(cursor);
    
    MessageIndex index(cursor);
    
    const CSString* msg = Asset::sharedAsset()->string(index);
    
    switch (target) {
        case 0:
            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), msg));
            break;
        case 1:
            Map::sharedMap()->pushMessage(_force, true, Message::message(smallIcon(), msg));
            break;
        case 2:
            Map::sharedMap()->pushMessage(_force, false, Message::message(smallIcon(), msg));
            break;
        default:
            Map::sharedMap()->pushMessage(Message::message(smallIcon(), msg));
            break;
    }
    _action->ip++;
}
void Unit::atLoop(const byte*& cursor) {
    int loop = readAbilityByte(cursor).value(this);
    int ip1 = readShort(cursor);
    
    if (_action->loop < loop) {
        _action->loop++;
        _action->ip++;
    }
    else {
        _action->loop = 0;
        _action->ip = ip1;
    }
}
void Unit::atWaitReserved(const byte *&cursor) {
    _waitingReserved = fixed::Zero;
    _waitingReservedOn = true;
    _action->ip++;
}
bool Unit::atWait(const byte*& cursor) {
    AbilityValue<fixed> time = readAbilityFixed(cursor);
    bool distance = readBoolean(cursor);
    bool reverse = readBoolean(cursor);
    bool reserved = readBoolean(cursor);
    bool interrupt = readBoolean(cursor);
    
    _action->ip++;
    
    fixed timeValue = time.value(this, _target && _target->compareType(ObjectMaskUnit) ? static_cast<const Unit*>(_target) : NULL);

    if (!timeValue) {
        return false;
    }
    if (reverse) {
        timeValue = fixed::One / timeValue;
    }
    if (distance) {
        if (!_target) {
            return false;
        }
        timeValue *= _point.distance(_target->point());
    }
    if (reserved && _waitingReservedOn) {
        timeValue -= _waitingReserved;
        _waitingReserved = fixed::Zero;
        _waitingReservedOn = false;
    }
    if (timeValue > fixed::Zero) {
        _waiting = timeValue;
        _waitingState = interrupt ? WaitingStateInterrupt : WaitingStateStop;
        return true;
    }
    return false;
}
bool Unit::atStop(const byte*& cursor) {
    int type = readByte(cursor);
    int ip1 = readShort(cursor);
    
    _action->ip = ip1;
    
    switch(type) {
        case 2:
            interruptAction();
        case 1:
            return true;
    }
    return false;
}

enum ActionTriggerOp {
    AT_Motion,
    AT_BaseMotion,
	AT_CancelBaseMotion,
    AT_ChangeBaseMotion,
    AT_ProgressMotion,
    AT_Turn,
    AT_Effect,
    AT_RemoveEffect,
    AT_EffectRange,
    AT_EffectAngle,
    AT_EffectLine,
    AT_AttackWatch,
    AT_Attack,
    AT_AttackRange,
    AT_AttackAngle,
    AT_AttackLine,
    AT_Watch,
	AT_WatchExclude,
    AT_Cooltime,
    AT_IsCooltime,
    AT_ShowCooltime,
    AT_Compare,
    AT_Revise,
    AT_ReviseRange,
    AT_ReviseAngle,
    AT_ReviseLine,
    AT_ReviseReset,
    AT_Resurrect,
    AT_IsLevel,
    AT_HasUnit,
    AT_HasUnits,
    AT_HasRune,
    AT_IsDamaged,
    AT_IsState,
    AT_ChangeState,
    AT_ChangeType,
	AT_ChangeBusy,
    AT_ChangeTarget,
    AT_HasTarget,
    AT_IsTarget,
    AT_IsBuffed,
    AT_Buff,
    AT_BuffRange,
    AT_BuffAngle,
    AT_BuffLine,
    AT_Cloak,
    AT_Detect,
	AT_IsAmbush,
	AT_Move,
    AT_Flock,
    AT_Collision,
    AT_Point,
    AT_Escape,
    AT_EscapeRange,
    AT_EscapeAngle,
    AT_EscapeLine,
    AT_Jump,
    AT_JumpRange,
    AT_JumpAngle,
    AT_JumpLine,
    AT_WarpTo,
    AT_WarpFrom,
    AT_WarpRangeTo,
    AT_WarpRangeFrom,
    AT_Freeze,
    AT_FreezeRange,
    AT_FreezeAngle,
    AT_FreezeLine,
    AT_Build,
    AT_Create,
    AT_Transform,
    AT_Cancel,
    AT_Copy,
	AT_AttachTarget,
	AT_DetachTarget,
    AT_Attach,
    AT_Detach,
    AT_IsTransporting,
    AT_FindCall,
    AT_SendCall,
    AT_ReceiveCall,
    AT_Unload,
    AT_IsResource,
    AT_FindResource,
    AT_OccupyResource,
    AT_GatherResource,
    AT_FindBase,
    AT_ReturnCargo,
    AT_HasCargo,
    AT_ReturnGathering,
    AT_RefineryEnabled,
    AT_RefineryOccupied,
    AT_IsBuilding,
    AT_IsProducing,
    AT_IsTraining,
    AT_IsAlliance,
    AT_Navigate,
    AT_Supply,
    AT_Advance,
    AT_Retreat,
    AT_Random,
    AT_WatchRandom,
    AT_ChildrenCount,
    AT_ChildrenTarget,
	AT_ChildrenCommand,
    AT_ParentInRange,
    AT_DisplayRange,
    AT_Tile,
    AT_Event,
    AT_Sound,
	AT_Announce,
    AT_Vibrate,
    AT_Message,
    AT_Loop,
    AT_WaitReserved,
    AT_Wait,
    AT_Stop
};

#ifdef UseActionTriggerTest
static const char* ActionTriggerOpNames[] = {
    "AT_Motion",
    "AT_BaseMotion",
	"AT_CancelBaseMotion",
    "AT_ChangeBaseMotion",
    "AT_ProgressMotion",
    "AT_Turn",
    "AT_Effect",
    "AT_RemoveEffect",
    "AT_EffectRange",
    "AT_EffectAngle",
    "AT_EffectLine",
    "AT_AttackWatch",
    "AT_Attack",
    "AT_AttackRange",
    "AT_AttackAngle",
    "AT_AttackLine",
    "AT_Watch",
	"AT_WatchExclude",
    "AT_Cooltime",
    "AT_IsCooltime",
    "AT_ShowCooltime",
    "AT_Compare",
    "AT_Revise",
    "AT_ReviseRange",
    "AT_ReviseAngle",
    "AT_ReviseLine",
    "AT_ReviseReset",
    "AT_Resurrect",
    "AT_IsLevel",
    "AT_HasUnit",
    "AT_HasUnits",
    "AT_HasRune",
    "AT_IsDamaged",
    "AT_IsState",
    "AT_ChangeState",
    "AT_ChangeType",
	"AT_ChangeBusy",
    "AT_ChangeTarget",
    "AT_HasTarget",
    "AT_IsTarget",
    "AT_IsBuffed",
    "AT_Buff",
    "AT_BuffRange",
    "AT_BuffAngle",
    "AT_BuffLine",
    "AT_Cloak",
    "AT_Detect",
	"AT_IsAmbush",
	"AT_Move",
    "AT_Flock",
    "AT_Collision",
    "AT_Point",
    "AT_Escape",
    "AT_EscapeRange",
    "AT_EscapeAngle",
    "AT_EscapeLine",
    "AT_Jump",
    "AT_JumpRange",
    "AT_JumpAngle",
    "AT_JumpLine",
    "AT_WarpTo",
    "AT_WarpFrom",
    "AT_WarpRangeTo",
    "AT_WarpRangeFrom",
    "AT_Freeze",
    "AT_FreezeRange",
    "AT_FreezeAngle",
    "AT_FreezeLine",
    "AT_Build",
    "AT_Create",
    "AT_Transform",
    "AT_Cancel",
    "AT_Copy",
	"AT_AttachTarget",
	"AT_DetachTarget",
    "AT_Attach",
    "AT_Detach",
    "AT_IsTransporting",
    "AT_FindCall",
    "AT_SendCall",
    "AT_ReceiveCall",
    "AT_Unload",
    "AT_IsResource",
    "AT_FindResource",
    "AT_OccupyResource",
    "AT_GatherResource",
    "AT_FindBase",
    "AT_ReturnCargo",
    "AT_HasCargo",
    "AT_ReturnGathering",
    "AT_RefineryEnabled",
    "AT_RefineryOccupied",
    "AT_IsBuilding",
    "AT_IsProducing",
    "AT_IsTraining",
    "AT_IsAlliance",
    "AT_Navigate",
    "AT_Supply",
    "AT_Advance",
    "AT_Retreat",
    "AT_Random",
    "AT_WatchRandom",
    "AT_ChildrenCount",
    "AT_ChildrenTarget",
	"AT_ChildrenCommand",
    "AT_ParentInRange",
    "AT_DisplayRange",
    "AT_Tile",
    "AT_Event",
    "AT_Sound",
	"AT_Announce",
    "AT_Vibrate",
    "AT_Message",
    "AT_Loop",
    "AT_WaitReserved",
    "AT_Wait",
    "AT_Stop"
};
#endif

bool Unit::updateAction() {
    while(_action) {
        while(_action->trigger && _action->ip < _action->trigger->count()) {
            const CSArray<byte, 1>* trigger = _action->trigger->objectAtIndex(_action->ip);
            
            const byte* cursor = trigger->pointer();
            
            ActionTriggerOp opCode = (ActionTriggerOp)readByte(cursor);
            
#ifdef UseActionTriggerTest
            UnitLog(this, "action trigger (frame:%d) (state:%d, %d) (ip:%d) (%s)", Map::sharedMap()->frame(), _state, _commandExec, _action->ip, ActionTriggerOpNames[opCode]);
#endif
            bool stop = false;
            
            switch(opCode) {
                case AT_Motion:
                    atMotion(cursor);
                    break;
                case AT_BaseMotion:
                    atBaseMotion(cursor);
                    break;
				case AT_CancelBaseMotion:
					atCancelBaseMotion(cursor);
					break;
                case AT_ChangeBaseMotion:
                    atChangeBaseMotion(cursor);
                    break;
                case AT_ProgressMotion:
                    atProgressMotion(cursor);
                    break;
                case AT_Turn:
                    if (atTurn(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Effect:
                    atEffect(cursor);
                    break;
                case AT_RemoveEffect:
                    atRemoveEffect(cursor);
                    break;
                case AT_EffectRange:
                    atEffectRange(cursor);
                    break;
                case AT_EffectAngle:
                    atEffectAngle(cursor);
                    break;
                case AT_EffectLine:
                    atEffectLine(cursor);
                    break;
                case AT_AttackWatch:
                    atAttackWatch(cursor);
                    break;
                case AT_Attack:
                    atAttack(cursor);
                    break;
                case AT_AttackRange:
                    atAttackRange(cursor);
                    break;
                case AT_AttackAngle:
                    atAttackAngle(cursor);
                    break;
                case AT_AttackLine:
                    atAttackLine(cursor);
                    break;
                case AT_Watch:
                    atWatch(cursor);
                    break;
				case AT_WatchExclude:
					atWatchExclude(cursor);
					break;
                case AT_Cooltime:
                    atCooltime(cursor);
                    break;
                case AT_IsCooltime:
                    if (atIsCooltime(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_ShowCooltime:
                    atShowCooltime(cursor);
                    break;
                case AT_Compare:
                    atCompare(cursor);
                    break;
                case AT_Revise:
                    atRevise(cursor);
                    break;
                case AT_ReviseRange:
                    atReviseRange(cursor);
                    break;
                case AT_ReviseAngle:
                    atReviseAngle(cursor);
                    break;
                case AT_ReviseLine:
                    atReviseLine(cursor);
                    break;
                case AT_ReviseReset:
                    atReviseReset(cursor);
                    break;
                case AT_Resurrect:
                    atResurrect(cursor);
                    break;
                case AT_IsLevel:
                    atIsLevel(cursor);
                    break;
                case AT_HasUnit:
                    atHasUnit(cursor);
                    break;
                case AT_HasUnits:
                    atHasUnits(cursor);
                    break;
                case AT_HasRune:
                    atHasRune(cursor);
                    break;
                case AT_IsDamaged:
                    atIsDamaged(cursor);
                    break;
                case AT_IsState:
                    atIsState(cursor);
                    break;
                case AT_ChangeState:
                    atChangeState(cursor);
                    break;
                case AT_ChangeType:
                    atChangeType(cursor);
                    break;
				case AT_ChangeBusy:
					atChangeBusy(cursor);
					break;
                case AT_ChangeTarget:
                    atChangeTarget(cursor);
                    break;
                case AT_HasTarget:
                    atHasTarget(cursor);
                    break;
                case AT_IsTarget:
                    atIsTarget(cursor);
                    break;
                case AT_IsBuffed:
                    atIsBuffed(cursor);
                    break;
                case AT_Buff:
                    atBuff(cursor);
                    break;
                case AT_BuffRange:
                    atBuffRange(cursor);
                    break;
                case AT_BuffAngle:
                    atBuffAngle(cursor);
                    break;
                case AT_BuffLine:
                    atBuffLine(cursor);
                    break;
                case AT_Cloak:
                    atCloak(cursor);
                    break;
                case AT_Detect:
                    atDetect(cursor);
                    break;
				case AT_IsAmbush:
					atIsAmbush(cursor);
					break;
				case AT_Move:
                    if (atMove(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Flock:
                    if (atFlock(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Collision:
                    atCollision(cursor);
                    break;
                case AT_Point:
                    atPoint(cursor);
                    break;
                case AT_Escape:
                    if (atEscape(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_EscapeRange:
                    if (atEscapeRange(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_EscapeAngle:
                    if (atEscapeAngle(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_EscapeLine:
                    if (atEscapeLine(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Jump:
                    if (atJump(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_JumpRange:
                    if (atJumpRange(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_JumpAngle:
                    if (atJumpAngle(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_JumpLine:
                    if (atJumpLine(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_WarpTo:
                    atWarpTo(cursor);
                    break;
                case AT_WarpFrom:
                    atWarpFrom(cursor);
                    break;
                case AT_WarpRangeTo:
                    atWarpRangeTo(cursor);
                    break;
                case AT_WarpRangeFrom:
                    atWarpRangeFrom(cursor);
                    break;
                case AT_Freeze:
                    if (atFreeze(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_FreezeRange:
                    if (atFreezeRange(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_FreezeAngle:
                    if (atFreezeAngle(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_FreezeLine:
                    if (atFreezeLine(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Build:
                    if (atBuild(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Create:
                    atCreate(cursor);
                    break;
                case AT_Transform:
                    atTransform(cursor);
                    break;
                case AT_Cancel:
                    atCancel(cursor);
                    break;
                case AT_Copy:
                    atCopy(cursor);
                    break;
				case AT_AttachTarget:
					atAttachTarget(cursor);
					break;
				case AT_DetachTarget:
					atDetachTarget(cursor);
					break;
                case AT_Attach:
                    atAttach(cursor);
                    break;
                case AT_Detach:
                    atDetach(cursor);
                    break;
                case AT_IsTransporting:
                    atIsTransporting(cursor);
                    break;
                case AT_FindCall:
                    atFindCall(cursor);
                    break;
                case AT_SendCall:
                    if (atSendCall(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_ReceiveCall:
                    atReceiveCall(cursor);
                    break;
                case AT_Unload:
                    atUnload(cursor);
                    break;
                case AT_IsResource:
                    atIsResource(cursor);
                    break;
                case AT_FindResource:
                    atFindResource(cursor);
                    break;
                case AT_OccupyResource:
                    if (atOccupyResource(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_GatherResource:
                    atGatherResource(cursor);
                    break;
                case AT_FindBase:
                    atFindBase(cursor);
                    break;
                case AT_ReturnCargo:
                    atReturnCargo(cursor);
                    break;
                case AT_HasCargo:
                    atHasCargo(cursor);
                    break;
                case AT_ReturnGathering:
                    atReturnGathering(cursor);
                    break;
                case AT_RefineryEnabled:
                    atRefineryEnabled(cursor);
                    break;
                case AT_RefineryOccupied:
                    atRefineryOccupied(cursor);
                    break;
                case AT_IsBuilding:
                    atIsBuilding(cursor);
                    break;
                case AT_IsProducing:
                    atIsProducing(cursor);
                    break;
                case AT_IsTraining:
                    atIsTraining(cursor);
                    break;
                case AT_IsAlliance:
                    atIsAlliance(cursor);
                    break;
                case AT_Navigate:
                    atNavigate(cursor);
                    break;
                case AT_Supply:
                    atSupply(cursor);
                    break;
                case AT_Advance:
                    atAdvance(cursor);
                    break;
                case AT_Retreat:
                    atRetreat(cursor);
                    break;
                case AT_Random:
                    atRandom(cursor);
                    break;
                case AT_WatchRandom:
                    atWatchRandom(cursor);
                    break;
                case AT_ChildrenCount:
                    atChildrenCount(cursor);
                    break;
                case AT_ChildrenTarget:
                    atChildrenTarget(cursor);
                    break;
				case AT_ChildrenCommand:
					atChildrenCommand(cursor);
					break;
                case AT_ParentInRange:
                    atParentInRange(cursor);
                    break;
                case AT_DisplayRange:
                    atDisplayRange(cursor);
                    break;
                case AT_Tile:
                    atTile(cursor);
                    break;
                case AT_Event:
                    atEvent(cursor);
                    break;
                case AT_Sound:
                    atSound(cursor);
                    break;
				case AT_Announce:
					atAnnounce(cursor);
					break;
                case AT_Vibrate:
                    atVibrate(cursor);
                    break;
                case AT_Message:
                    atMessage(cursor);
                    break;
                case AT_Loop:
                    atLoop(cursor);
                    break;
                case AT_WaitReserved:
                    atWaitReserved(cursor);
                    break;
                case AT_Wait:
                    if (atWait(cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Stop:
                    if (atStop(cursor)) {
                        stop = true;
                    }
                    break;
            }
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "action trigger error:%d,%d,%d", opCode, pos, len);
#endif
            if (stop) {
                return true;
            }
        }
        Action* prev = _action->prev;
        delete _action;
        _action = prev;
    }
    return false;
}
