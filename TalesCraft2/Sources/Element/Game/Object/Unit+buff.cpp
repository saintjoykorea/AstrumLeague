//
//  Unit+buff.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Unit.h"

#include "Map.h"

#include "Destination.h"

AnimationIndex Unit::btAnimationIndex(Buff* buff, const byte*& cursor) {
	int indexCount = readLength(cursor);
	CSAssert(indexCount >= 1, "invalid data");

	if (indexCount > 1) {
		AnimationIndex* indices = (AnimationIndex*)alloca(indexCount * sizeof(AnimationIndex));
		for (int i = 0; i < indexCount; i++) {
			indices[i] = AnimationIndex(cursor);
		}
		const BuffData& data = Asset::sharedAsset()->buff(buff->index);

		return indices[CSMath::min((int)(indexCount * buff->stack / data.maxStack), indexCount - 1)];
	}
	else {
		return AnimationIndex(cursor);
	}
}

void Unit::btMotion(Buff* buff, const byte*& cursor) {
    int op = readByte(cursor);
    
    switch (op) {
		case 3:
			if (buff->motion) {
				_motion.buff--;
				buff->motion = false;
			}
			break;
        case 2:
            {
                AnimationIndex index = btAnimationIndex(buff, cursor);
                float rate = readAbilityFloat(buff, cursor).value(buff->origin, this);
                bool rewind = readBoolean(cursor);
                
				if (!buff->motion) {
					_motion.buff++;
					buff->motion = true;
				}
                setMotion(index, rate, rewind, true);
            }
            break;
        case 1:
			if (!buff->motion) {
				_motion.buff++;
				buff->motion = true;
			}
			stopMotion(true);
            break;
        case 0:
			if (!buff->motion) {
				_motion.buff++;
				buff->motion = true;
			}
			removeMotion(true);
            break;
    }
    buff->ip++;
}
void Unit::btBaseMotion(Buff* buff, const byte*& cursor) {
    int index = readByte(cursor);
	if (!buff->motion) {
		_motion.buff++;
		buff->motion = true;
	}
	setBaseMotion(index, true);
    buff->ip++;
}
void Unit::btChangeBaseMotion(Buff* buff, const byte *&cursor) {
	CSAssert(buff->origin == this, "invalid operation");
    int from = readByte(cursor);
    int to = readByte(cursor);
    _motions[from] = to;
    buff->ip++;
}
void Unit::btEffect(Buff* buff, const byte*& cursor) {
    int toTarget = readByte(cursor);
    AnimationIndex index = btAnimationIndex(buff, cursor);
    if (buff->origin) index = buff->origin->data().effectIndex(buff->origin->skin, index);
    bool attach = readBoolean(cursor);
    int slot = readByte(cursor);

    if (!toTarget || buff->target) {
		if (slot < 0) {
			AnimationEffect::effect(index, this, toTarget ? buff->target : NULL, toTarget >> 1, attach, false);
		}
		else {
			AnimationEffect*& effect = buff->effects[slot];

			if (effect) {
				if (effect->index() == index) {
					effect->setDestination(toTarget && buff->target ? buff->target : this);
					goto exit;
				}
				else {
					effect->dispose();
					effect->release();
				}
			}
			effect = retain(AnimationEffect::effect(index, this, toTarget ? buff->target : NULL, toTarget >> 1, attach, false));
		}
    }
exit:
    buff->ip++;
}
void Unit::btRemoveEffect(Buff* buff, const byte*& cursor) {
    int slot = readByte(cursor);
	int op = readByte(cursor);
    
	AnimationEffect*& effect = buff->effects[slot];

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
    buff->ip++;
}
void Unit::btEffectRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
	bool nearbyTo = readBoolean(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AnimationIndex index = btAnimationIndex(buff, cursor);
    if (buff->origin) index = buff->origin->data().effectIndex(buff->origin->skin, index);
    bool attach = readBoolean(cursor);
    
	if (!toTarget) {
		range += collider();
	}

	Map::sharedMap()->effectWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, index, nearbyTo, attach);

    buff->ip++;
}
void Unit::btEffectAngle(Buff* buff, const byte*& cursor) {
	bool nearbyTo = readBoolean(cursor);
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AnimationIndex index = btAnimationIndex(buff, cursor);
	if (buff->origin) index = buff->origin->data().effectIndex(buff->origin->skin, index);
	bool attach = readBoolean(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->effectWithAngle(buff->origin, this, buff->target, range, angle, condition, index, nearbyTo, attach);
	}
	buff->ip++;
}
void Unit::btEffectLine(Buff* buff, const byte*& cursor) {
	bool nearbyTo = readBoolean(cursor);
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AnimationIndex index = btAnimationIndex(buff, cursor);
	if (buff->origin) index = buff->origin->data().effectIndex(buff->origin->skin, index);
	bool attach = readBoolean(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->effectWithLine(buff->origin, this, buff->target, range, thickness, condition, index, nearbyTo, attach);
	}
	buff->ip++;
}
void Unit::btAttack(Buff* buff, const byte*& cursor) {
	bool toTarget = readBoolean(cursor);
    fixed rate = readAbilityFixed(buff, cursor).value(buff->origin, this);
    fixed time = readFixed(cursor);

	if (buff->origin) {
		Unit* target = toTarget ? (buff->target && buff->target->compareType(ObjectMaskUnit) ? static_cast<Unit*>(buff->target) : NULL) : this;

		if (target) {
			buff->revision += buff->origin->attack(target, rate, time);
		}
	}
    
    buff->ip++;
}
void Unit::btAttackRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(buff, cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
	if (buff->origin) {
		if (!toTarget) {
			range += collider();
		}
		buff->revision += Map::sharedMap()->attackWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, rate, attenuation, time);
	}
    buff->ip++;
}
void Unit::btAttackAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> rate = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
	fixed time = readFixed(cursor);
	if (buff->origin && buff->target) {
		range += collider();

		buff->revision += Map::sharedMap()->attackWithAngle(buff->origin, this, buff->target, range, angle, condition, rate, attenuation, time);
	}
	buff->ip++;
}
void Unit::btAttackLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> rate = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
	fixed time = readFixed(cursor);
	if (buff->origin && buff->target) {
		range += collider();

		buff->revision += Map::sharedMap()->attackWithLine(buff->origin, this, buff->target, range, thickness, condition, rate, attenuation, time);
	}
	buff->ip++;
}
void Unit::btWatch(Buff* buff, const byte*& cursor) {
	AbilityValue<fixed> range = readAbilityFixed(cursor);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	int ip1 = readShort(cursor);

	UnitBaseGame* origin = buff->origin ? buff->origin : this;

	Unit* target;
	if (getWatching(cursor, target)) {
		if (target) {
			if (target->compareType(ObjectMaskUnit) && target->isVisible(origin->force()) &&
				_point.distance(target->point()) <= range.value(origin, target) + collider() + target->collider() &&
				target->match(origin, condition))
			{
				retain(buff->target, target);
				buff->ip++;
				return;
			}
		}
		else {
			release(buff->target);
			buff->ip = ip1;
			return;
		}
	}

	Map::WatchReturn rtn = Map::sharedMap()->watch(origin, this, condition, range);
	target = rtn.inside ? rtn.target : NULL;
	setWatching(cursor, target);
	retain(buff->target, target);

	buff->ip = target ? buff->ip + 1 : ip1;
}
void Unit::btCooltime(Buff* buff, const byte*& cursor) {
	bool toTarget = readBoolean(cursor);
    int index = readByte(cursor);
    fixed cooltime = readAbilityFixed(buff, cursor).value(buff->origin, this);
    bool reverse = readBoolean(cursor);
    
    if (reverse) {
        if (cooltime) cooltime = fixed::One / cooltime;
    }
    else {
        buff->cooltimes[index] = cooltime;
    }
	if (toTarget) {
		CSAssert(buff->origin == this, "invalid operation");
		_cooltimes[index].remaining = _cooltimes[index].initial = cooltime;
	}
	else {
		buff->cooltimes[index] = cooltime;
	}
    buff->ip++;
}
bool Unit::btIsCooltime(Buff* buff, const byte*& cursor) {
    int index = readByte(cursor);
    bool next = readBoolean(cursor);
    int ip1 = readShort(cursor);
    
    if (!buff->cooltimes[index]) {
        buff->ip++;
    }
    else {
        buff->ip = ip1;
        
        if (next) {
            return true;
        }
    }
    return false;
}
void Unit::btCompare(Buff* buff, const byte*& cursor) {
    bool toOrigin = readBoolean(cursor);
    int op = readByte(cursor);
    fixed value = readAbilityFixed(buff, cursor).value(buff->origin, this);
    int ip1 = readShort(cursor);
    
    bool flag = toOrigin ? buff->origin && buff->origin->compare(op, value) : compare(op, value);

    buff->ip = flag ? buff->ip + 1 : ip1;
}
void Unit::btRevise(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    int op = readByte(cursor);
	AbilityValue<fixed> value = readAbilityFixed(buff, cursor);
    fixed time = readFixed(cursor);
    
	if (toTarget) {
		if (buff->target && buff->target->compareType(ObjectMaskUnit)) {
			Unit* target = static_cast<Unit*>(buff->target);

			buff->revision += target->revise(op, value.value(buff->origin, target), time, buff->origin);
		}
	}
	else {
		buff->revision += revise(op, value.value(buff->origin, this), time, buff->origin);
	}

    buff->ip++;
}
void Unit::btReviseRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    int op = readByte(cursor);
    AbilityValue<fixed> value = readAbilityFixed(buff, cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
    fixed time = readFixed(cursor);
    
	if (!toTarget) {
		range += collider();
	}

	buff->revision += Map::sharedMap()->reviseWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, op, value, attenuation, time);
    
    buff->ip++;
}
void Unit::btReviseAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	int op = readByte(cursor);
	AbilityValue<fixed> value = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
	fixed time = readFixed(cursor);

	if (buff->target) {
		range += collider();

		buff->revision += Map::sharedMap()->reviseWithAngle(buff->origin, this, buff->target, range, angle, condition, op, value, attenuation, time);
	}
	buff->ip++;
}
void Unit::btReviseLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	int op = readByte(cursor);
	AbilityValue<fixed> value = readAbilityFixed(cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cursor);
	fixed time = readFixed(cursor);

	if (buff->target) {
		range += collider();

		buff->revision += Map::sharedMap()->reviseWithLine(buff->origin, this, buff->target, range, thickness, condition, op, value, attenuation, time);
	}
	buff->ip++;
}
void Unit::btReviseReset(Buff* buff, const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            buff->revision = fixed::Zero;
            break;
        case 1:
            buff->damage = fixed::Zero;
			buff->damageCount = 0;
            break;
    }
    buff->ip++;
}
void Unit::btIsLevel(Unit::Buff *buff, const byte *&cursor) {
    int level = readShort(cursor);
    int ip1 = readShort(cursor);
    
    buff->ip = buff->origin && buff->origin->level >= level ? buff->ip + 1 : ip1;
}
void Unit::btHasUnit(Buff* buff, const byte*& cursor) {
    UnitIndex index(cursor);
    bool alive = readBoolean(cursor);
    int unitCount = readByte(cursor);
    int ip1 = readShort(cursor);
    
    const Force* force = buff->origin ? buff->origin->force() : NULL;
    
    buff->ip = force && force->unitCount(index, alive) >= unitCount ? buff->ip + 1 : ip1;
}
void Unit::btHasUnits(Buff* buff, const byte*& cursor) {
    int count = readLength(cursor);
    UnitIndex* indices = (UnitIndex*)alloca(count * sizeof(UnitIndex));
    for (int i = 0; i < count; i++) {
        indices[i] = UnitIndex(cursor);
    }
    bool alive = readBoolean(cursor);
    int unitCount = readByte(cursor);
    int ip1 = readShort(cursor);
    
	const Force* force = buff->origin ? buff->origin->force() : NULL;
    
    if (force) {
        for (int i = 0; i < count; i++) {
            unitCount -= force->unitCount(indices[i], alive);
        }
        if (unitCount <= 0) {
            buff->ip++;
            return;
        }
    }
    buff->ip = ip1;
}
void Unit::btHasRune(Buff* buff, const byte*& cursor) {
    RuneIndex index(cursor);
    int ip1 = readShort(cursor);
    
	const Force* force = buff->origin ? buff->origin->force() : NULL;
    
    buff->ip = force && force->runeForIndex(index) && force->isTrained(index) ? buff->ip + 1 : ip1;
}
void Unit::btIsDamaged(Buff* buff, const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    buff->ip = buff->damageCount ? buff->ip + 1 : ip1;
}
void Unit::btIsState(Buff* buff, const byte*& cursor) {
    bool toOrigin = readBoolean(cursor);
    int state = readByte(cursor);
    int ip1 = readShort(cursor);
    
    buff->ip = (toOrigin ? (buff->origin && buff->origin->state() == state) : (_state == state)) ? buff->ip + 1 : ip1;
}
void Unit::btChangeState(Buff* buff, const byte*& cursor) {
	bool toOrigin = readBoolean(cursor);
	int state = readByte(cursor);

	Unit* target = toOrigin ? (buff->origin ? buff->origin->asUnit() : NULL) : this;

	if (target && target->_state != UnitStateDestroy) {
		target->_state = state;
		if (state == UnitStateDestroy) {
			if (target->_alive == AliveYes) target->_alive = AliveNo;
			target->deactivate();
		}
		else {
			target->_nextActions->removeAllObjects();
		}
		target->changeAction();
	}

	buff->ip++;
}
void Unit::btIsType(Buff* buff, const byte*& cursor) {
	bool toOrigin = readBoolean(cursor);
	uint type = readInt(cursor);
	int ip1 = readShort(cursor);

	Unit* target = toOrigin ? (buff->origin ? buff->origin->asUnit() : NULL) : this;

	buff->ip = target && target->compareType(type) ? buff->ip + 1 : ip1;
}
void Unit::btChangeTarget(Buff* buff, const byte*& cursor) {
	switch (readByte(cursor)) {
		case 0:
			release(buff->target);
			break;
		case 1:
			retain(buff->target, buff->origin ? buff->origin->asUnit() : NULL);
			break;
		case 2:
			retain(buff->target, _parent);
			break;
		case 3:
			retain(buff->target, _aggro);
			break;
		case 4:
			if (buff->damageOrigins) {
				if (buff->damageOrigins->count()) {
					uint key = buff->damageOrigins->lastObject();

					buff->damageOrigins->removeLastObject();

					Unit* target = Map::sharedMap()->findUnit(key);

					retain(buff->target, target);
				}
				else {
					release(buff->target);
				}
			}
			else {
				release(buff->target);
				buff->damageOrigins = new CSArray<uint>();
			}
			break;
		case 5:
			release(buff->target);
			release(buff->damageOrigins);
			break;
		case 6:
			if (buff->target) {
				Destination* destination = new Destination(_force);
				destination->locate(buff->target->point());
				buff->target->release();
				buff->target = destination;
			}
			break;
		case 7:
			if (buff->target) {
				Destination* destination = new Destination(_force);
				destination->locate(buff->target->facingPoint(_point));
				buff->target->release();
				buff->target = destination;
			}
			break;
		case 8:
			{
				fixed range = readAbilityFixed(cursor).value(this);
				bool approach = readBoolean(cursor);

				if (buff->target) {
					FPoint diff = buff->target->point() - _point;
					fixed length = diff.length();
					if (approach && length <= collider() + buff->target->collider()) {
						release(buff->target);
					}
					else if (approach && length <= collider() + buff->target->collider() + range) {
						diff.normalize();
						diff *= collider() + buff->target->collider();

						Destination* destination = retain(Destination::destination(_force));
						destination->locate(buff->target->point() - diff);
						buff->target->release();
						buff->target = destination;
					}
					else {
						diff.normalize();
						diff *= range + collider();

						Destination* destination = new Destination(_force);
						destination->locate(_point + diff);
						buff->target->release();
						buff->target = destination;
					}
				}
			}
			break;
		case 9:
			{
				fixed rotation = CSMath::degreesToRadians(readFixed(cursor));

				if (buff->target) {
					FPoint p = buff->target->point() - _point;
					p.rotate(rotation);
					p += _point;

					Destination* destination = new Destination(_force);
					destination->locate(p);
					buff->target->release();
					buff->target = destination;
				}
			}
			break;
		case 10:
			{
				fixed range = readAbilityFixed(cursor).value(buff->origin, this);
				fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
				const byte* condition = cursor;
				cursor += matchLength(condition);

				if (buff->target) {
					range += _collider;

					FPoint p0, p1;
					fixed distance = fixed::Max;
					Unit* selectedTarget = NULL;
					foreach(Unit*, target, Map::sharedMap()->unitsWithLine(buff->origin, this, buff->target, range, thickness, condition, p0, p1)) {
						fixed d = _point.distance(target->point());
						if (d < distance) {
							distance = d;
							selectedTarget = target;
						}
					}
					retain(buff->target, selectedTarget);
				}
			}
			break;
	}
	buff->ip++;
}
void Unit::btHasTarget(Buff* buff, const byte*& cursor) {
	int ip1 = readShort(cursor);

	buff->ip = buff->target ? buff->ip + 1 : ip1;
}
void Unit::btIsTarget(Buff* buff, const byte*& cursor) {
	bool visible = readBoolean(cursor);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	int ip1 = readShort(cursor);

	buff->ip = buff->target && buff->target->compareType(ObjectMaskUnit) && (visible ? buff->target->isVisible(_force) : buff->target->isLocated()) && static_cast<const Unit*>(buff->target)->match(this, condition) ? buff->ip + 1 : ip1;
}
void Unit::btIsBuffed(Buff* buff, const byte*& cursor) {
    bool toOrigin = readBoolean(cursor);
    BuffIndex index(cursor);
    fixed stack = readFixed(cursor);
    int ip1 = readShort(cursor);

    buff->ip = (toOrigin ? (buff->origin && buff->origin->isBuffed(index, stack)) : isBuffed(index, stack)) ? buff->ip + 1 : ip1;
}
void Unit::btBuff(Buff* buff, const byte*& cursor) {
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
    AbilityValue<fixed> stack = readAbilityFixed(buff, cursor);
    
	if (toTarget) {
		if (buff->target && buff->target->compareType(ObjectMaskUnit)) {
			Unit* target = static_cast<Unit*>(buff->target);

			target->buff(index, timeOp, time.value(buff->origin, target), stack.value(buff->origin, target), buff->origin);
		}
	}
	else {
		this->buff(index, timeOp, time.value(buff->origin, this), stack.value(buff->origin, this), buff->origin);
	}

    buff->ip++;
}
void Unit::btBuffRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
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
    AbilityValue<fixed> stack = readAbilityFixed(buff, cursor);
    
	if (!toTarget) {
		range += collider();
	}
    
    Map::sharedMap()->buffWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, index, timeOp, time, stack);

	buff->ip++;
}
void Unit::btBuffAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
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

	if (buff->target) {
		range += collider();

		Map::sharedMap()->buffWithAngle(buff->origin, this, buff->target, range, angle, condition, index, timeOp, time, stack);
	}
	buff->ip++;
}
void Unit::btBuffLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
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

	if (buff->target) {
		range += collider();

		Map::sharedMap()->buffWithLine(buff->origin, this, buff->target, range, thickness, condition, index, timeOp, time, stack);
	}
	buff->ip++;
}
void Unit::btCloak(Buff* buff, const byte*& cursor) {
    buff->cloaking = readBoolean(cursor);
	Map::sharedMap()->changeMinimap(this);
    buff->ip++;
}
void Unit::btDetect(Buff* buff, const byte*& cursor) {
    setDetecting(readBoolean(cursor), buff);
    buff->ip++;
}
void Unit::btIsAmbush(Buff* buff, const byte*& cursor) {
	bool toOrigin = readBoolean(cursor);
	int ip1 = readShort(cursor);

	Unit* target = toOrigin ? (buff->origin ? buff->origin->asUnit() : NULL) : this;

	buff->ip = target && target->isHiding() ? buff->ip + 1 : ip1;
}
void Unit::btEscape(Buff* buff, const byte*& cursor) {
    bool toTarget = readByte(cursor);
	AbilityValue<fixed> time = readAbilityFixed(buff, cursor);

    if (buff->target) {
        if (toTarget) {
			if (buff->target->compareType(ObjectMaskUnit) && buff->target->isLocated()) {
				Unit* target = static_cast<Unit*>(buff->target);

				target->escape(_point, time.value(buff->origin, target));
			}
        }
        else {
            escape(buff->target->point(), time.value(buff->origin, this));
        }
    }
    buff->ip++;
}
void Unit::btEscapeRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(buff, cursor);
    
	if (!toTarget) {
		range += collider();
	}
    
	Map::sharedMap()->escapeWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, time);

	buff->ip++;
}
void Unit::btEscapeAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> time = readAbilityFixed(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->escapeWithAngle(buff->origin, this, buff->target, range, angle, condition, time);
	}

	buff->ip++;
}
void Unit::btEscapeLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> time = readAbilityFixed(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->escapeWithLine(buff->origin, this, buff->target, range, thickness, condition, time);
	}

	buff->ip++;
}
void Unit::btJump(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
	AbilityValue<fixed> distance = readAbilityFixed(buff, cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
	if (buff->target) {
		if (toTarget) {
			if (buff->target->compareType(ObjectMaskUnit) && buff->target->isLocated()) {
				Unit* target = static_cast<Unit*>(buff->target);

				target->jump(_point, distance.value(buff->origin, target) + collider(), altitude, time, pushing);
			}
		}
		else {
			jump(buff->target->point(), distance.value(buff->origin, this) + buff->target->collider(), altitude, time, pushing);
		}
	}
    buff->ip++;
}
void Unit::btJumpRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> distance = readAbilityFixed(buff, cursor);
    float altitude = readFloat(cursor);
    fixed time = readFixed(cursor);
    bool pushing = readBoolean(cursor);
    
	if (!toTarget) {
		range += collider();
	}

	Map::sharedMap()->jumpWithRange(buff->origin, this, toTarget ? buff->target : NULL, (toTarget >> 1) & 1, range, condition, distance, altitude, time, pushing, toTarget >> 2);

	buff->ip++;
}
void Unit::btJumpAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> distance = readAbilityFixed(cursor);
	float altitude = readFloat(cursor);
	fixed time = readFixed(cursor);
	bool pushing = readBoolean(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->jumpWithAngle(buff->origin, this, buff->target, range, angle, condition, distance, altitude, time, pushing);
	}

	buff->ip++;
}
void Unit::btJumpLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> distance = readAbilityFixed(cursor);
	float altitude = readFloat(cursor);
	fixed time = readFixed(cursor);
	bool pushing = readBoolean(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->jumpWithLine(buff->origin, this, buff->target, range, thickness, condition, distance, altitude, time, pushing);
	}

	buff->ip++;
}
void Unit::btFreeze(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
	AbilityValue<fixed> time = readAbilityFixed(buff, cursor);
    
    if (toTarget) {
        if (buff->target && buff->target->compareType(ObjectMaskUnit)) {
			Unit* target = static_cast<Unit*>(buff->target);

			target->freeze(time.value(buff->origin, target));
        }
    }
    else {
        freeze(time.value(buff->origin, this));
    }
    buff->ip++;
}
void Unit::btFreezeRange(Buff* buff, const byte*& cursor) {
	int toTarget = readByte(cursor);
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    AbilityValue<fixed> time = readAbilityFixed(buff, cursor);
    
	if (!toTarget) {
		range += collider();
	}
    
    Map::sharedMap()->freezeWithRange(buff->origin, this, toTarget ? buff->target : NULL, toTarget >> 1, range, condition, time);

	buff->ip++;
}
void Unit::btFreezeAngle(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed angle = CSMath::degreesToRadians(readAbilityFixed(cursor).value(buff->origin, this));
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> time = readAbilityFixed(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->freezeWithAngle(buff->origin, this, buff->target, range, angle, condition, time);
	}

	buff->ip++;
}
void Unit::btFreezeLine(Buff* buff, const byte*& cursor) {
	fixed range = readAbilityFixed(cursor).value(buff->origin, this);
	fixed thickness = readAbilityFixed(cursor).value(buff->origin, this);
	const byte* condition = cursor;
	cursor += matchLength(condition);
	AbilityValue<fixed> time = readAbilityFixed(cursor);

	if (buff->target) {
		range += collider();

		Map::sharedMap()->freezeWithLine(buff->origin, this, buff->target, range, thickness, condition, time);
	}

	buff->ip++;
}
void Unit::btCreate(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
	bool useResource = readBoolean(cursor);
	UnitIndex index = readRandomUnitIndex(cursor);
    int ip1 = readShort(cursor);
    
	if (!buff->origin) return;

	FPoint p;

    if (toTarget) {
        if (buff->target) {
            p = buff->target->point();
        }
        else {
            buff->ip = ip1;
            return;
        }
    }
    else {
        p = _point;
    }
    
	const UnitData& data = Asset::sharedAsset()->unit(index);

    if (Map::sharedMap()->locatablePosition(data.type, p, data.collider, NULL, MapLocationCollided) != MapLocationUnabled) {
		Force* force = buff->origin->force();
		
		if (force) {
			if (useResource) {
				for (int i = 0; i < 2; i++) {
					if (force->resource(i) < data.resources[i]) {
						Map::sharedMap()->pushMessage(force, Message::message(buff->origin->smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughMineral + i)));
						Map::sharedMap()->announce(_force, (AnnounceIndex)(AnnounceNotEnoughMineral + i));
						buff->ip = ip1;
						return;
					}
				}
				if (data.population && force->population() + data.population > _force->supply()) {
					Map::sharedMap()->pushMessage(force, Message::message(buff->origin->smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughSupply)));
					buff->ip = ip1;
					return;
				}
				force->reviseResource(0, (fixed)-data.resources[0]);
				force->reviseResource(1, (fixed)-data.resources[1]);
			}
        }
        Unit* unit = Unit::unit(force, index, 0, 0, buff->origin->asUnit());
        unit->_originTarget = retain(buff->target);
        unit->_target = retain(buff->target);
		unit->locate(p);
		Map::sharedMap()->voice(VoiceSpawn, unit);
        buff->ip++;
    }
    else {
        buff->ip = ip1;
    }
}
void Unit::btTransform(Buff* buff, const byte*& cursor) {
	UnitIndex index = readRandomUnitIndex(cursor);
    int ip1 = readShort(cursor);
    
    buff->ip = setIndex(index) ? buff->ip + 1 : ip1;
}
void Unit::btRetransform(Buff* buff, const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    buff->ip = resetIndex() ? buff->ip + 1 : ip1;
}
void Unit::btAttach(Buff* buff, const byte*& cursor) {
    bool ally = readBoolean(cursor);
	bool toLevel = readBoolean(cursor);

    Force* force = ally && buff->origin ? buff->origin->force() : NULL;

    setForce(force, toLevel && buff->origin ? CSMath::min(buff->origin->level, level) : level);
    
    buff->ip++;
}
void Unit::btDetach(Buff* buff, const byte*& cursor) {
    resetForce();
    
    buff->ip++;
}
void Unit::btIsBuilding(Buff* buff, const byte*& cursor) {
    bool toOrigin = readBoolean(cursor);
    int building = readByte(cursor);
    int ip1 = readShort(cursor);
    
    buff->ip = toOrigin ? (buff->origin && (building & (1 << buff->origin->building()))) : (building & (1 << _building)) ? buff->ip + 1 : ip1;
}
void Unit::btRandom(Buff* buff, const byte*& cursor) {
    fixed rate = readAbilityFixed(buff, cursor).value(buff->origin, this);
    int ip1 = readShort(cursor);
    
    buff->ip = Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One) <= rate ? buff->ip + 1 : ip1;
}
void Unit::btWatchRandom(Buff* buff, const byte*& cursor) {
    fixed range = readAbilityFixed(buff, cursor).value(buff->origin, this);
    const byte* condition = cursor;
    cursor += matchLength(condition);
    fixed rate = readAbilityFixed(buff, cursor).value(buff->origin, this);
    int ip1 = readShort(cursor);
    
    rate *= Map::sharedMap()->count(buff->origin, this, condition, range);

	buff->ip = Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One) <= rate ? buff->ip + 1 : ip1;
}
void Unit::btChildrenCount(Buff* buff, const byte*& cursor) {
	const byte* condition = cursor;
	cursor += matchLength(condition);
    int count = readAbilityByte(buff, cursor).value(buff->origin, this);
    int ip1 = readShort(cursor);
    
	const Unit* unit = buff->origin ? buff->origin->asUnit() : NULL;

	int matching = 0;
	if (unit) {
		foreach(const Unit*, child, unit->_children) {
			if (child->match(unit, condition)) matching++;
		}
	}
	
	buff->ip = matching >= count ? buff->ip + 1 : ip1;
}
void Unit::btVision(Unit::Buff *buff, const byte *&cursor) {
    bool on = readBoolean(cursor);
    
    if (on != buff->vision) {
        buff->vision = on;
        if (buff->originForce) {
            Map::sharedMap()->changeVision(this, _visionPoint, _sight, false, buff->originForce->alliance(), on);
        }
    }
    buff->ip++;
}
void Unit::btResurrect(Unit::Buff *buff, const byte *&cursor) {
	bool on = readBoolean(cursor);

	if (on != buff->resurrecting) {
		buff->resurrecting = on;
		if (on) _resurrecting++;
		else _resurrecting--;
	}
	buff->ip++;
}
void Unit::btEvent(Buff* buff, const byte*& cursor) {
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
    buff->ip++;
}
void Unit::btSound(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
	int soundCount = readLength(cursor);
	const byte* soundCursor = cursor + (randInt(0, soundCount - 1) * sizeof(SoundIndex));
	SoundIndex soundIndex(soundCursor);
	cursor += soundCount * sizeof(SoundIndex);
    float volume = readFloat(cursor);
	bool essential = readBoolean(cursor);
    if (toTarget) {
        if (buff->target) {
            Map::sharedMap()->sound(soundIndex, buff->target, volume, essential);
        }
    }
    else {
        Map::sharedMap()->sound(soundIndex, this, volume, essential);
    }
    buff->ip++;
}
void Unit::btAnnounce(Buff* buff, const byte*& cursor) {
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
	buff->ip++;
}
void Unit::btVibrate(Buff* buff, const byte*& cursor) {
    bool toTarget = readBoolean(cursor);
    int level = readByte(cursor);
    if (toTarget) {
        Map::sharedMap()->vibrate(level, buff->target);
    }
    else {
        Map::sharedMap()->vibrate(level, this);
    }
    buff->ip++;
}
void Unit::btMessage(Buff* buff, const byte*& cursor) {
    int target = readByte(cursor);
    
    MessageIndex index(cursor);
    
    const CSString* msg = Asset::sharedAsset()->string(index);

	const SkinData* skin = buff->origin->skinData();
	const ImageIndex& icon = skin ? skin->smallIcon : ImageIndex::None;
    
    switch (target) {
        case 0:
            Map::sharedMap()->pushMessage(_force, Message::message(icon, msg));
            break;
        case 1:
            Map::sharedMap()->pushMessage(_force, true, Message::message(icon, msg));
            break;
        case 2:
            Map::sharedMap()->pushMessage(_force, false, Message::message(icon, msg));
            break;
        default:
            Map::sharedMap()->pushMessage(Message::message(icon, msg));
            break;
    }
    buff->ip++;
}
void Unit::btStack(Buff* buff, const byte*& cursor) {
    fixed stack = readAbilityFixed(buff, cursor).value(buff->origin, this);
    
    switch(readByte(cursor)) {
        case 0:
            buff->stack = stack;
            break;
        case 1:
            buff->stack += stack;
            break;
        case 2:
            buff->stack -= stack;
            break;
        case 3:
            buff->stack *= stack;
            break;
        case 4:
            if (stack) buff->stack /= stack;
            break;
    }
    buff->stack = CSMath::clamp(buff->stack, fixed::Zero, Asset::sharedAsset()->buff(buff->index).maxStack);
    
    buff->ip++;
}
void Unit::btIsRemaining(Buff* buff, const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    buff->ip = buff->remaining ? buff->ip + 1 : ip1;
}
void Unit::btLoop(Buff* buff, const byte*& cursor) {
    int loop = readAbilityByte(buff, cursor).value(buff->origin, this);
    int ip1 = readShort(cursor);
    
    if (buff->loop < loop) {
        buff->loop++;
        buff->ip++;
    }
    else {
        buff->loop = 0;
        buff->ip = ip1;
    }
}
void Unit::btWait(Buff* buff, const byte*& cursor) {
    fixed time = readAbilityFixed(buff, cursor).value(buff->origin, this);
    bool reverse = readBoolean(cursor);
    
    buff->ip++;
    if (time) {
        if (reverse) {
            time = fixed::One / time;
        }
        buff->waiting = time;
    }
}
bool Unit::btStop(Buff* buff, const byte*& cursor) {
    bool next = readBoolean(cursor);
    int ip1 = readShort(cursor);
    
    buff->ip = ip1;
    
    return next;
}

enum BuffTriggerOp {
    BT_Motion,
    BT_BaseMotion,
    BT_ChangeBaseMotion,
    BT_Effect,
    BT_RemoveEffect,
    BT_EffectRange,
	BT_EffectAngle,
	BT_EffectLine,
    BT_Attack,
    BT_AttackRange,
	BT_AttackAngle,
	BT_AttackLine,
	BT_Watch,
    BT_Cooltime,
    BT_IsCooltime,
    BT_Compare,
    BT_Revise,
    BT_ReviseRange,
	BT_ReviseAngle,
	BT_ReviseLine,
    BT_ReviseReset,
    BT_IsLevel,
    BT_HasUnit,
    BT_HasUnits,
    BT_HasRune,
    BT_IsDamaged,
    BT_IsState,
	BT_ChangeState,
	BT_IsType,
	BT_ChangeTarget,
	BT_HasTarget,
	BT_IsTarget,
    BT_IsBuffed,
    BT_Buff,
    BT_BuffRange,
	BT_BuffAngle,
	BT_BuffLine,
    BT_Cloak,
    BT_Detect,
	BT_IsAmbush,
	BT_Escape,
    BT_EscapeRange,
	BT_EscapeAngle,
	BT_EscapeLine,
    BT_Jump,
    BT_JumpRange,
	BT_JumpAngle,
	BT_JumpLine,
    BT_Freeze,
    BT_FreezeRange,
	BT_FreezeAngle,
	BT_FreezeLine,
    BT_Create,
    BT_Transform,
    BT_Retransform,
    BT_Attach,
    BT_Detach,
    BT_IsBuilding,
    BT_Random,
    BT_WatchRandom,
    BT_ChildrenCount,
    BT_Vision,
	BT_Resurrect,
    BT_Event,
    BT_Sound,
	BT_Announce,
    BT_Vibrate,
    BT_Message,
    BT_Stack,
    BT_IsRemaining,
    BT_Loop,
    BT_Wait,
    BT_Stop
};

#ifdef UseBuffTriggerTest
static const char* BuffTriggerOpNames[] = {
    "BT_Motion",
    "BT_BaseMotion",
    "BT_ChangeBaseMotion",
    "BT_Effect",
    "BT_RemoveEffect",
    "BT_EffectRange",
	"BT_EffectAngle",
	"BT_EffectLine",
    "BT_Attack",
    "BT_AttackRange",
	"BT_AttackAngle",
	"BT_AttackLine",
	"BT_Watch",
    "BT_Cooltime",
    "BT_IsCooltime",
    "BT_Compare",
    "BT_Revise",
    "BT_ReviseRange",
	"BT_ReviseAngle",
	"BT_ReviseLine",
    "BT_ReviseReset",
    "BT_IsLevel",
    "BT_HasUnit",
    "BT_HasUnits",
    "BT_HasRune",
    "BT_IsDamaged",
    "BT_IsState",
	"BT_ChangeState",
	"BT_IsType",
	"BT_ChangeTarget",
	"BT_HasTarget",
	"BT_IsTarget",
    "BT_IsBuffed",
    "BT_Buff",
    "BT_BuffRange",
	"BT_BuffAngle",
	"BT_BuffLine",
    "BT_Cloak",
    "BT_Detect",
	"BT_IsAmbush",
    "BT_Escape",
    "BT_EscapeRange",
	"BT_EscapeAngle",
	"BT_EscapeLine",
    "BT_Jump",
    "BT_JumpRange",
	"BT_JumpAngle",
	"BT_JumpLine",
    "BT_Freeze",
    "BT_FreezeRange",
	"BT_FreezeAngle",
	"BT_FreezeLine",
    "BT_Create",
    "BT_Transform",
    "BT_Retransform",
    "BT_Attach",
    "BT_Detach",
    "BT_IsBuilding",
    "BT_Random",
    "BT_WatchRandom",
    "BT_ChildrenCount",
    "BT_Vision",
	"BT_Resurrect",
    "BT_Event",
    "BT_Sound",
	"BT_Announce",
    "BT_Vibrate",
    "BT_Message",
    "BT_Stack",
    "BT_IsRemaining",
    "BT_Loop",
    "BT_Wait",
    "BT_Stop"
};
#endif

bool Unit::updateBuff(Buff* buff) {
    if (buff->remaining && buff->remaining < fixed::Max) {
        buff->remaining = CSMath::max(buff->remaining - Map::sharedMap()->frameDelayFixed(), fixed::Zero);
    }
    if (buff->waiting) {
        buff->waiting -= Map::sharedMap()->frameDelayFixed();
        if (buff->waiting > fixed::Zero) {
            return true;
        }
        buff->waiting = fixed::Zero;
    }
    for (int i = 0; i < CooltimeCount; i++) {
        buff->cooltimes[i] = CSMath::max(buff->cooltimes[i] - Map::sharedMap()->frameDelayFixed(), fixed::Zero);
    }

    const CSArray<byte, 2>* buffTrigger = Asset::sharedAsset()->buff(buff->index).trigger;
    
    if (buffTrigger) {
        while(buff->ip < buffTrigger->count()) {
            const CSArray<byte, 1>* trigger = buffTrigger->objectAtIndex(buff->ip);
            
            const byte* cursor = trigger->pointer();
            
            BuffTriggerOp opCode = (BuffTriggerOp)readByte(cursor);
            
#ifdef UseBuffTriggerTest
            UnitLog(this, "buff trigger (frame:%d) (index:%d, %d) (ip:%d) (%s)", Map::sharedMap()->frame(), buff->index.indices[0], buff->index.indices[1], buff->ip, BuffTriggerOpNames[opCode]);
#endif
            switch(opCode) {
                case BT_Motion:
                    btMotion(buff, cursor);
                    break;
                case BT_BaseMotion:
                    btBaseMotion(buff, cursor);
                    break;
                case BT_ChangeBaseMotion:
                    btChangeBaseMotion(buff, cursor);
                    break;
                case BT_Effect:
                    btEffect(buff, cursor);
                    break;
				case BT_RemoveEffect:
                    btRemoveEffect(buff, cursor);
                    break;
                case BT_EffectRange:
                    btEffectRange(buff, cursor);
                    break;
				case BT_EffectAngle:
					btEffectAngle(buff, cursor);
					break;
				case BT_EffectLine:
					btEffectLine(buff, cursor);
					break;
                case BT_Attack:
                    btAttack(buff, cursor);
                    break;
                case BT_AttackRange:
                    btAttackRange(buff, cursor);
                    break;
				case BT_AttackAngle:
					btAttackAngle(buff, cursor);
					break;
				case BT_AttackLine:
					btAttackLine(buff, cursor);
					break;
				case BT_Watch:
					btWatch(buff, cursor);
					break;
                case BT_Cooltime:
                    btCooltime(buff, cursor);
                    break;
                case BT_IsCooltime:
                    if (btIsCooltime(buff, cursor)) {
                        return true;
                    }
                    break;
                case BT_Compare:
                    btCompare(buff, cursor);
                    break;
                case BT_Revise:
                    btRevise(buff, cursor);
                    break;
                case BT_ReviseRange:
                    btReviseRange(buff, cursor);
                    break;
				case BT_ReviseAngle:
					btReviseAngle(buff, cursor);
					break;
				case BT_ReviseLine:
					btReviseLine(buff, cursor);
					break;
                case BT_ReviseReset:
                    btReviseReset(buff, cursor);
                    break;
                case BT_IsLevel:
                    btIsLevel(buff, cursor);
                    break;
                case BT_HasUnit:
                    btHasUnit(buff, cursor);
                    break;
                case BT_HasUnits:
                    btHasUnits(buff, cursor);
                    break;
                case BT_HasRune:
                    btHasRune(buff, cursor);
                    break;
                case BT_IsDamaged:
                    btIsDamaged(buff, cursor);
                    break;
                case BT_IsState:
                    btIsState(buff, cursor);
                    break;
				case BT_ChangeState:
					btChangeState(buff, cursor);
					break;
				case BT_IsType:
					btIsType(buff, cursor);
					break;
				case BT_ChangeTarget:
					btChangeTarget(buff, cursor);
					break;
				case BT_HasTarget:
					btHasTarget(buff, cursor);
					break;
				case BT_IsTarget:
					btIsTarget(buff, cursor);
					break;
                case BT_IsBuffed:
                    btIsBuffed(buff, cursor);
                    break;
                case BT_Buff:
                    btBuff(buff, cursor);
                    break;
                case BT_BuffRange:
                    btBuffRange(buff, cursor);
                    break;
				case BT_BuffAngle:
					btBuffAngle(buff, cursor);
					break;
				case BT_BuffLine:
					btBuffLine(buff, cursor);
					break;
                case BT_Cloak:
                    btCloak(buff, cursor);
                    break;
                case BT_Detect:
                    btDetect(buff, cursor);
                    break;
				case BT_IsAmbush:
					btIsAmbush(buff, cursor);
					break;
                case BT_Escape:
                    btEscape(buff, cursor);
                    break;
                case BT_EscapeRange:
                    btEscapeRange(buff, cursor);
                    break;
                case BT_Jump:
                    btJump(buff, cursor);
                    break;
                case BT_JumpRange:
                    btJumpRange(buff, cursor);
                    break;
				case BT_JumpAngle:
					btJumpAngle(buff, cursor);
					break;
				case BT_JumpLine:
					btJumpLine(buff, cursor);
					break;
                case BT_Freeze:
                    btFreeze(buff, cursor);
                    break;
                case BT_FreezeRange:
                    btFreezeRange(buff, cursor);
                    break;
				case BT_FreezeAngle:
					btFreezeAngle(buff, cursor);
					break;
				case BT_FreezeLine:
					btFreezeLine(buff, cursor);
					break;
                case BT_Create:
                    btCreate(buff, cursor);
                    break;
                case BT_Transform:
                    btTransform(buff, cursor);
                    break;
                case BT_Retransform:
                    btRetransform(buff, cursor);
                    break;
                case BT_Attach:
                    btAttach(buff, cursor);
                    break;
                case BT_Detach:
                    btDetach(buff, cursor);
                    break;
                case BT_IsBuilding:
                    btIsBuilding(buff, cursor);
                    break;
                case BT_Random:
                    btRandom(buff, cursor);
                    break;
                case BT_WatchRandom:
                    btWatchRandom(buff, cursor);
                    break;
                case BT_ChildrenCount:
                    btChildrenCount(buff, cursor);
                    break;
                case BT_Vision:
                    btVision(buff, cursor);
                    break;
				case BT_Resurrect:
					btResurrect(buff, cursor);
					break;
                case BT_Event:
                    btEvent(buff, cursor);
                    break;
                case BT_Sound:
                    btSound(buff, cursor);
                    break;
				case BT_Announce:
					btAnnounce(buff, cursor);
					break;
                case BT_Vibrate:
                    btVibrate(buff, cursor);
                    break;
                case BT_Message:
                    btMessage(buff, cursor);
                    break;
                case BT_Stack:
                    btStack(buff, cursor);
                    break;
                case BT_IsRemaining:
                    btIsRemaining(buff, cursor);
                    break;
                case BT_Loop:
                    btLoop(buff, cursor);
                    break;
                case BT_Wait:
                    btWait(buff, cursor);
                    return true;
                case BT_Stop:
                    if (btStop(buff, cursor)) {
                        return true;
                    }
                    break;
            }
            
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "buff trigger error:%d,%d,%d", opCode, pos, len);
#endif
        }
    }
    return buff->remaining;
}
