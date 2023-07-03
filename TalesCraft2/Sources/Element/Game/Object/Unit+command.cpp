//
//  Unit+command.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Unit.h"

#include "Map.h"

#include "Frame.h"

void Unit::ctEffect(CommandTriggerState& cts) {
    int toTarget = readByte(cts.cursor);
    AnimationIndex index(cts.cursor);
    bool attach = readBoolean(cts.cursor);
	bool ui = readBoolean(cts.cursor);

    if (cts.commit) {
        index = _data->effectIndex(skin, index);

        AnimationEffect::effect(index, this, toTarget ? *cts.target : NULL, toTarget >> 1, attach, ui);
    }
    
    cts.ip++;
}
void Unit::ctEffectRange(CommandTriggerState& cts) {
    int toTarget = readByte(cts.cursor);
	bool nearbyTo = readBoolean(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AnimationIndex index(cts.cursor);
    bool attach = readBoolean(cts.cursor);
    
    if (cts.commit) {
        index = _data->effectIndex(skin, index);

        fixed rangeValue = range.value(this);
        if (!toTarget) {
            rangeValue += collider();
        }
        Map::sharedMap()->effectWithRange(this, this, toTarget ? *cts.target : NULL, toTarget >> 1, rangeValue, condition, index, nearbyTo, attach);
    }
    
    cts.ip++;
}
void Unit::ctEffectAngle(CommandTriggerState& cts) {
    bool nearbyTo = readBoolean(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> angle = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AnimationIndex index(cts.cursor);
    bool attach = readBoolean(cts.cursor);
    
    if (cts.commit && *cts.target) {
        index = _data->effectIndex(skin, index);

        fixed rangeValue = range.value(this) + collider();
        fixed angleValue = CSMath::degreesToRadians(angle.value(this));
        
        Map::sharedMap()->effectWithAngle(this, this, *cts.target, rangeValue, angleValue, condition, index, nearbyTo, attach);
    }
    
    cts.ip++;
}
void Unit::ctEffectLine(CommandTriggerState& cts) {
    bool nearbyTo = readBoolean(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> thickness = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AnimationIndex index(cts.cursor);
    bool attach = readBoolean(cts.cursor);
    
    if (cts.commit && *cts.target) {
        index = _data->effectIndex(skin, index);

        fixed rangeValue = range.value(this) + collider();
        fixed thicknessValue = thickness.value(this);
        
        Map::sharedMap()->effectWithLine(this, this, *cts.target, rangeValue, thicknessValue, condition, index, nearbyTo, attach);
    }
    
    cts.ip++;
}
void Unit::ctAttack(CommandTriggerState& cts) {
    AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit && *cts.target && (*cts.target)->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(*cts.target);
        
        attack(target, rate.value(this, target), time);
    }
    cts.ip++;
}
void Unit::ctAttackRange(CommandTriggerState& cts) {
    int toTarget = readByte(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit) {
        fixed rangeValue = range.value(this);
        if (!toTarget) {
            rangeValue += collider();
        }
        Map::sharedMap()->attackWithRange(this, this, toTarget ? *cts.target : NULL, toTarget >> 1, rangeValue, condition, rate, attenuation, time);
    }
    cts.ip++;
}
void Unit::ctAttackAngle(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> angle = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit && *cts.target) {
        fixed rangeValue = range.value(this) + collider();
        fixed angleValue = CSMath::degreesToRadians(angle.value(this));

        Map::sharedMap()->attackWithAngle(this, this, *cts.target, rangeValue, angleValue, condition, rate, attenuation, time);
    }
    cts.ip++;
}
void Unit::ctAttackLine(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> thickness = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit && *cts.target) {
        fixed rangeValue = range.value(this) + collider();
        fixed thicknessValue = thickness.value(this);
        
        Map::sharedMap()->attackWithLine(this, this, *cts.target, rangeValue, thicknessValue, condition, rate, attenuation, time);
    }
    cts.ip++;
}
void Unit::ctCompare(CommandTriggerState& cts) {
    int op = readByte(cts.cursor);
    fixed value = readAbilityFixed(cts.cursor).value(this);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = compare(op, value) ? cts.ip + 1 : ip1;
}
void Unit::ctRevise(CommandTriggerState& cts) {
    bool toTarget = readBoolean(cts.cursor);
    int op = readByte(cts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit) {
        if (toTarget) {
            if (*cts.target && (*cts.target)->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(*cts.target);
                
                target->revise(op, value.value(this, target), time, this);
            }
        }
        else {
            revise(op, value.value(this), time, this);
        }
    }
    
    cts.ip++;
}
void Unit::ctReviseRange(CommandTriggerState& cts) {
    int toTarget = readByte(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    int op = readByte(cts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit) {
        fixed rangeValue = range.value(this);
        if (!toTarget) {
            rangeValue += collider();
        }
        Map::sharedMap()->reviseWithRange(this, this, toTarget ? *cts.target : NULL, toTarget >> 1, rangeValue, condition, op, value, attenuation, time);
    }
    cts.ip++;
}
void Unit::ctReviseAngle(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> angle = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    int op = readByte(cts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit && *cts.target) {
        fixed rangeValue = range.value(this) + collider();
        fixed angleValue = CSMath::degreesToRadians(angle.value(this));
        
        Map::sharedMap()->reviseWithAngle(this, this, *cts.target, rangeValue, angleValue, condition, op, value, attenuation, time);
    }
    
    cts.ip++;
}
void Unit::ctReviseLine(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> thickness = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    int op = readByte(cts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(cts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(cts.cursor);
    fixed time = readFixed(cts.cursor);
    
    if (cts.commit && *cts.target) {
        fixed rangeValue = range.value(this) + collider();
        fixed thicknessValue = thickness.value(this);
        
        Map::sharedMap()->reviseWithLine(this, this, *cts.target, rangeValue, thicknessValue, condition, op, value, attenuation, time);
    }
    
    cts.ip++;
}
void Unit::ctIsLevel(CommandTriggerState& cts) {
    int level = readShort(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = this->level >= level ? cts.ip + 1 : ip1;
}
void Unit::ctHasUnit(CommandTriggerState& cts) {
    UnitIndex index(cts.cursor);
    bool alive = readBoolean(cts.cursor);
    int unitCount = readByte(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    const Force* force = _force;
    
    cts.ip = force && force->unitCount(index, alive) >= unitCount ? cts.ip + 1 : ip1;
}
void Unit::ctHasUnits(CommandTriggerState& cts) {
    int count = readLength(cts.cursor);
    UnitIndex* indices = (UnitIndex*)alloca(count * sizeof(UnitIndex));
    for (int i = 0; i < count; i++) {
        indices[i] = UnitIndex(cts.cursor);
    }
    bool alive = readBoolean(cts.cursor);
    int unitCount = readByte(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    const Force* force = _force;
    
    if (force) {
        for (int i = 0; i < count; i++) {
            unitCount -= force->unitCount(indices[i], alive);
        }
        if (unitCount <= 0) {
            cts.ip++;
            return;
        }
    }
    cts.ip = ip1;
}
void Unit::ctHasTier(CommandTriggerState& cts) {
	int tier = readByte(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    const Force* force = _force;

	if (force) {
		foreach(const Rune*, rune, force->runes()) {
			if (rune->data().tier >= tier) {
				cts.ip++;
				return;
			}
		}
	}
    cts.ip = ip1;
}
void Unit::ctHasRune(CommandTriggerState& cts) {
    RuneIndex index(cts.cursor);
    int ip1 = readShort(cts.cursor);
	int ip2 = readShort(cts.cursor);
    
    const Force* force = _force;

	cts.ip = force && force->runeForIndex(index) ? (force->isTrained(index) ? cts.ip + 1 : ip1) : ip2;
}
void Unit::ctIsState(CommandTriggerState& cts) {
    int state = readByte(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = _state == state ? cts.ip + 1 : ip1;
}
void Unit::ctChangeState(CommandTriggerState& cts) {
    int state = readByte(cts.cursor);
    if (cts.commit) {
        _state = state;
        if (_state == UnitStateDestroy) {
            deactivate();
            changeAction();
        }
    }
    cts.ip++;
}
void Unit::ctMaintainTarget(CommandTriggerState &cts) {
    Object* target = readBoolean(cts.cursor) ? _originTarget : _target;
    int ip1 = readShort(cts.cursor);
    if (target) {
        *cts.target = target;
        cts.ip++;
    }
    else {
        cts.ip = ip1;
    }
}
void Unit::ctHasTarget(CommandTriggerState& cts) {
    int ip1 = readShort(cts.cursor);
    
    cts.ip = *cts.target ? cts.ip + 1 : ip1;
}
void Unit::ctIsTarget(CommandTriggerState& cts) {
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = *cts.target && (*cts.target)->compareType(ObjectMaskUnit) && (*cts.target)->isLocated() && static_cast<const Unit*>(*cts.target)->match(this, condition) ? cts.ip + 1 : ip1;
}
void Unit::ctIsBuffed(CommandTriggerState& cts) {
    BuffIndex index(cts.cursor);
    fixed stack = readFixed(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = isBuffed(index, stack) ? cts.ip + 1 : ip1;
}
void Unit::ctBuff(CommandTriggerState& cts) {
    bool toTarget = readBoolean(cts.cursor);
    BuffIndex index(cts.cursor);
	int timeOp = readByte(cts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cts.cursor);
    
    if (cts.commit) {
        if (toTarget) {
            if (*cts.target && (*cts.target)->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(*cts.target);
                
                target->buff(index, timeOp, time.value(this, target), stack.value(this, target), this);
            }
        }
        else {
            buff(index, timeOp, time.value(this), stack.value(this), this);
        }
    }
    
    cts.ip++;
}
void Unit::ctBuffRange(CommandTriggerState& cts) {
    int toTarget = readByte(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    BuffIndex index(cts.cursor);
	int timeOp = readByte(cts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cts.cursor);
    
    if (cts.commit) {
        fixed rangeValue = range.value(this);
        if (!toTarget) {
            rangeValue += collider();
        }
        Map::sharedMap()->buffWithRange(this, this, toTarget ? *cts.target : NULL, toTarget >> 1, rangeValue, condition, index, timeOp, time, stack);
    }
    cts.ip++;
}
void Unit::ctBuffAngle(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> angle = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    BuffIndex index(cts.cursor);
	int timeOp = readByte(cts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cts.cursor);
    
    if (cts.commit && *cts.target) {
        fixed rangeValue = range.value(this) + collider();
        fixed angleValue = CSMath::degreesToRadians(angle.value(this));

        Map::sharedMap()->buffWithAngle(this, this, *cts.target, rangeValue, angleValue, condition, index, timeOp, time, stack);
    }
    cts.ip++;
}
void Unit::ctBuffLine(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<fixed> thickness = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    cts.cursor += matchLength(condition);
    BuffIndex index(cts.cursor);
	int timeOp = readByte(cts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(cts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(cts.cursor);
    
    if (cts.commit) {
        fixed rangeValue = range.value(this) + collider();
        fixed thicknessValue = thickness.value(this);
        
        Map::sharedMap()->buffWithLine(this, this, *cts.target, rangeValue, thicknessValue, condition, index, timeOp, time, stack);
    }
    cts.ip++;
}
void Unit::ctCloak(CommandTriggerState& cts) {
    bool cloaking = readBoolean(cts.cursor);
    
    if (cts.commit) {
        _cloaking = cloaking;
    }
    cts.ip++;
}
void Unit::ctDetect(CommandTriggerState& cts) {
    bool detecting = readBoolean(cts.cursor);
    
    if (cts.commit) {
        setDetecting(detecting);
    }
    cts.ip++;
}
void Unit::ctIsAmbush(CommandTriggerState& cts) {
	int ip1 = readShort(cts.cursor);

	cts.ip = isHiding() ? cts.ip + 1 : ip1;
}
void Unit::ctIsCallable(CommandTriggerState &cts) {
	const byte* condition = cts.cursor;
	cts.cursor += matchLength(condition);
    int ip1 = readShort(cts.cursor);
	synchronized_to_update_thread(Map::sharedMap()->phaseLock(), GameLockMap) {
		foreach(const Unit*, unit, Map::sharedMap()->units()) {
			if (unit->isLocated() && unit->match(this, condition) && const_cast<Unit*>(unit)->sendCall(this, false) != SendCallReturnNone) {
				cts.ip++;
				return;
			}
		}
	}
    cts.ip = ip1;
}
void Unit::ctIsTransporting(CommandTriggerState &cts) {
    int ip1 = readShort(cts.cursor);
    
    bool flag = false;
    if (_transporting) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_transporting && _transporting->crews->count()) {
                flag = true;
            }
        }
    }
    cts.ip = flag ? cts.ip + 1 : ip1;
}
void Unit::ctIsResource(CommandTriggerState &cts) {
    int kind = readByte(cts.cursor);
    bool gather = readBoolean(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    if (*cts.target) {
        if (kind == 0) {
            if ((*cts.target)->compareType(ObjectMaskMineral)) {
                cts.ip++;
                return;
            }
        }
        else if (kind == 1) {
            if (gather) {
                if ((*cts.target)->compareType(ObjectMaskRefinery)) {
                    const Unit* mine = static_cast<const Unit*>(*cts.target);
                    
                    if (mine->_force == _force && mine->_building <= BuildingTransform) {
                        cts.ip++;
                        return;
                    }
                }
                else if ((*cts.target)->compareType(ObjectMaskGas)) {
                    const Unit* mine = Map::sharedMap()->findRefinery((*cts.target)->point());
                    
                    if (mine && mine->_force == _force && mine->_building <= BuildingTransform) {
                        cts.ip++;
                        return;
                    }
                }
            }
            else {
                if ((*cts.target)->compareType(ObjectMaskGas)) {
                    cts.ip++;
                    return;
                }
            }
        }
    }
    cts.ip = ip1;
}
void Unit::ctIsBuilding(CommandTriggerState& cts) {
    int building = readByte(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = (building & (1 << _building)) ? cts.ip + 1 : ip1;
}
void Unit::ctIsProducing(CommandTriggerState& cts) {
    int ip1 = readShort(cts.cursor);
    
    bool flag = false;
    if (_producing) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_producing && _producing->count) {
                flag = true;
            }
        }
    }
    cts.ip = flag ? cts.ip + 1 : ip1;
}
void Unit::ctIsTraining(CommandTriggerState& cts) {
    int ip1 = readShort(cts.cursor);
    
    cts.ip = _training ? cts.ip + 1 : ip1;
}
void Unit::ctRandom(CommandTriggerState& cts) {
    fixed rate = readAbilityFixed(cts.cursor).value(this);
    int ip1 = readShort(cts.cursor);
    
    cts.ip = !cts.commit || Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One) <= rate ? cts.ip + 1 : ip1;
}
void Unit::ctWatchRandom(CommandTriggerState& cts) {
    bool toTarget = readBoolean(cts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    const byte* condition = cts.cursor;
    AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);
    int ip1 = readShort(cts.cursor);
    
    if (cts.commit && (!toTarget || *cts.target)) {
        cts.ip = Map::sharedMap()->random()->nextFixed(fixed::Zero, fixed::One) <= rate.value(this) * Map::sharedMap()->count(this, toTarget ? *cts.target : NULL, condition, range.value(this)) ? cts.ip + 1 : ip1;
    }
    else {
        cts.ip = ip1;
    }
}
void Unit::ctChildrenCount(CommandTriggerState& cts) {
	const byte* condition = cts.cursor;
	cts.cursor += matchLength(condition);
	int count = readAbilityByte(cts.cursor).value(this);
    int ip1 = readShort(cts.cursor);

	int matching = 0;
	if (_children) {
		synchronized_to_update_thread(this, GameLockObject) {
			foreach(const Unit*, child, _children) {
				if (child->match(this, condition)) matching++;
			}
		}
	}
    cts.ip = matching >= count ? cts.ip + 1 : ip1;
}
void Unit::ctEvent(CommandTriggerState& cts) {
    int target = readBoolean(cts.cursor);
    
    if (cts.commit) {
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
    }
    cts.ip++;
}
void Unit::ctSound(CommandTriggerState& cts) {
    bool toTarget = readBoolean(cts.cursor);
	int soundCount = readLength(cts.cursor);
	const byte* soundCursor = cts.cursor;
	cts.cursor += soundCount * sizeof(SoundIndex);
    float volume = readFloat(cts.cursor);
	bool essential = readBoolean(cts.cursor);
    if (cts.commit) {
		soundCursor += (randInt(0, soundCount - 1) * sizeof(SoundIndex));
		SoundIndex soundIndex(soundCursor);

        if (toTarget) {
            if (*cts.target) {
                Map::sharedMap()->sound(soundIndex, *cts.target, volume, essential);
            }
        }
        else {
            Map::sharedMap()->sound(soundIndex, this, volume, essential);
        }
    }
    cts.ip++;
}
void Unit::ctAnnounce(CommandTriggerState& cts) {
	int target = readByte(cts.cursor);
	AnnounceIndex index = (AnnounceIndex)readByte(cts.cursor);

	if (cts.commit) {
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
	}
	cts.ip++;
}
void Unit::ctVibrate(CommandTriggerState& cts) {
    bool toTarget = readBoolean(cts.cursor);
    int level = readByte(cts.cursor);
    
    if (cts.commit) {
        if (toTarget) {
            if (*cts.target) {
                Map::sharedMap()->vibrate(level, *cts.target);
            }
        }
        else {
            Map::sharedMap()->vibrate(level, this);
        }
    }
    cts.ip++;
    
}
void Unit::ctMessage(CommandTriggerState &cts) {
    int target = readByte(cts.cursor);
    
    MessageIndex index(cts.cursor);
    
    if (cts.commit) {
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
    }
    cts.ip++;
}
bool Unit::ctAttackEstimation(CommandTriggerState& cts) {
	AbilityValue<fixed> rate = readAbilityFixed(cts.cursor);

	bool enabled;

	if (cts.commit) {
		enabled = false;

		if (*cts.target && (*cts.target)->compareType(ObjectMaskUnit)) {
			const Unit* target = static_cast<const Unit*>(*cts.target);

			if (*cts.estimation <= target->health()) {
				enabled = true;
			}

			fixed damage = ability(AbilityAttack, fixed::Zero, target) * rate.value(target);

			*cts.estimation += damage;
		}
	}
	else {
		enabled = true;
	}
	if (enabled) {
		cts.ip++;
		return false;
	}
	else {
		cts.rtn.state = CommandStateEmpty;
		return true;
	}
}
bool Unit::ctReviseEstimation(CommandTriggerState& cts) {
	int op = readByte(cts.cursor);
	AbilityValue<fixed> value = readAbilityFixed(cts.cursor);

	bool enabled;

	if (cts.commit) {
		enabled = false;

		if (*cts.target && (*cts.target)->compareType(ObjectMaskUnit)) {
			const Unit* target = static_cast<const Unit*>(*cts.target);

			switch (op) {
				case 0:
					if (*cts.estimation <= target->health()) {
						enabled = true;
					}
					break;
				case 1:
					if (*cts.estimation <= target->mana()) {
						enabled = true;
					}
					break;
				case 2:
					if (target->_force && *cts.estimation <= target->_force->resource(0)) {
						enabled = true;
					}
					break;
				case 3:
					if (target->_force && *cts.estimation <= target->_force->resource(1)) {
						enabled = true;
					}
					break;
			}
			*cts.estimation += value.value(this, target);
		}
	}
	else {
		enabled = true;
	}
	if (enabled) {
		cts.ip++;
		return false;
	}
	else {
		cts.rtn.state = CommandStateEmpty;
		return true;
	}
}
void Unit::ctIsSelecting(CommandTriggerState& cts) {
	int selectionFlag = readByte(cts.cursor);
	int ip1 = readShort(cts.cursor);

	cts.ip = (cts.selectionFlag & selectionFlag) ? ip1 : cts.ip + 1;
}
void Unit::ctIsCommanding(CommandTriggerState& cts) {
	CommandIndex command(cts.cursor);
	int ip1 = readShort(cts.cursor);

	cts.ip = _command == command ? cts.ip + 1 : ip1;
}
void Unit::ctEmpty(CommandTriggerState &cts) {
    cts.rtn.state = CommandStateEmpty;
}
void Unit::ctUnabled(CommandTriggerState& cts) {
    cts.rtn.state = CommandStateUnabled;
    cts.rtn.parameter.unabled.msg = MessageIndex(cts.cursor);
	cts.rtn.parameter.unabled.announce = (AnnounceIndex)readByte(cts.cursor);
}
bool Unit::ctTarget(CommandTriggerState& cts) {
    AbilityValue<fixed> distance = readAbilityFixed(cts.cursor);
    const byte* condition = NULL;
    if (readBoolean(cts.cursor)) {
        condition = cts.cursor;
        cts.cursor += matchLength(condition);
    }
    AnimationIndex destAnimation(cts.cursor);
	AnimationIndex sourceAnimation(cts.cursor);
    int effect = readByte(cts.cursor);
    bool all = readBoolean(cts.cursor);
    
    if (*cts.target) {
        if (condition == NULL || ((*cts.target)->compareType(ObjectMaskUnit) && (*cts.target)->isLocated() && static_cast<const Unit*>(*cts.target)->match(this, condition))) {
            cts.ip++;
            return false;
        }
        else {
            cts.rtn.state = CommandStateUnabled;
            cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
			cts.rtn.parameter.unabled.announce = AnnounceNone;
            return true;
        }
    }
    else if (cts.commit) {
        if (condition) {
            *cts.target = Map::sharedMap()->watch(this, this, condition, distance).target;
            
            if (*cts.target) {
                cts.ip++;
                return false;
            }
            else {
                cts.rtn.state = CommandStateUnabled;
                cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
				cts.rtn.parameter.unabled.announce = AnnounceNone;
                return true;
            }
        }
        else {
            cts.ip++;
            return false;
        }
    }
    else {
        cts.rtn.state = CommandStateTarget;
        cts.rtn.parameter.target.distance = distance.value(this);
        cts.rtn.parameter.target.condition = condition;
        cts.rtn.parameter.target.destAnimation = destAnimation;
		cts.rtn.parameter.target.sourceAnimation = sourceAnimation;
        cts.rtn.parameter.target.effect = effect;
        cts.rtn.parameter.target.all = all;
        return true;
    }
}
bool Unit::ctRange(CommandTriggerState& cts) {
    AbilityValue<fixed> distance = readAbilityFixed(cts.cursor);
    AbilityValue<float> range = readAbilityFloat(cts.cursor);
    const byte* condition = NULL;
    if (readBoolean(cts.cursor)) {
        condition = cts.cursor;
        cts.cursor += matchLength(condition);
    }
	AnimationIndex destAnimation(cts.cursor);
	AnimationIndex sourceAnimation(cts.cursor);
    int effect = readByte(cts.cursor);
    bool all = readBoolean(cts.cursor);
    
    if (*cts.target) {
        cts.ip++;
        return false;
    }
    else if (cts.commit) {
        if (condition) {
            *cts.target = Map::sharedMap()->watch(this, this, condition, distance).target;
            
            if (*cts.target) {
                cts.ip++;
                return false;
            }
        }
        cts.rtn.state = CommandStateUnabled;
        cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
        return true;
    }
    else {
        cts.rtn.state = CommandStateRange;
        cts.rtn.parameter.range.distance = distance.value(this);
        cts.rtn.parameter.range.condition = condition;
        cts.rtn.parameter.range.range = range.value(this);
		cts.rtn.parameter.range.destAnimation = destAnimation;
		cts.rtn.parameter.range.sourceAnimation = sourceAnimation;
        cts.rtn.parameter.range.effect = effect;
        cts.rtn.parameter.range.all = all;
        return true;
    }
}
bool Unit::ctAngle(CommandTriggerState& cts) {
	AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<float> angle = readAbilityFloat(cts.cursor);
    
    const byte* condition = NULL;
    if (readBoolean(cts.cursor)) {
        condition = cts.cursor;
        cts.cursor += matchLength(condition);
    }
	AnimationIndex destAnimation(cts.cursor);
	AnimationIndex sourceAnimation(cts.cursor);
    int effect = readByte(cts.cursor);
    bool all = readBoolean(cts.cursor);
    
    if (*cts.target) {
		fixed d = _point.distance((*cts.target)->point()) - collider() - (*cts.target)->collider();

		if (d <= range.value(this)) {
			cts.ip++;
			return false;
		}
		cts.rtn.state = CommandStateUnabled;
		cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
		return true;
    }
    else if (cts.commit) {
        if (condition) {
			Map::WatchReturn watch = Map::sharedMap()->watch(this, this, condition, range);

			if (watch.inside) {
	            *cts.target = watch.target;
                cts.ip++;
                return false;
            }
        }
        cts.rtn.state = CommandStateUnabled;
        cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
        return true;
    }
    else {
        cts.rtn.state = CommandStateAngle;
        cts.rtn.parameter.angle.condition = condition;
        cts.rtn.parameter.angle.range = range.value(this);
        cts.rtn.parameter.angle.angle = CSMath::degreesToRadians(angle.value(this));
		cts.rtn.parameter.angle.destAnimation = destAnimation;
		cts.rtn.parameter.angle.sourceAnimation = sourceAnimation;
		cts.rtn.parameter.angle.effect = effect;
        cts.rtn.parameter.angle.all = all;
        return true;
    }
}
bool Unit::ctLine(CommandTriggerState& cts) {
    AbilityValue<fixed> range = readAbilityFixed(cts.cursor);
    AbilityValue<float> thickness = readAbilityFloat(cts.cursor);
    
    const byte* condition = NULL;
    if (readBoolean(cts.cursor)) {
        condition = cts.cursor;
        cts.cursor += matchLength(condition);
    }
	AnimationIndex destAnimation(cts.cursor);
	AnimationIndex sourceAnimation(cts.cursor);
    int effect = readByte(cts.cursor);
    bool all = readBoolean(cts.cursor);
    
	if (*cts.target) {
		fixed d = _point.distance((*cts.target)->point()) - collider() - (*cts.target)->collider();

		if (d <= range.value(this)) {
			cts.ip++;
			return false;
		}
		cts.rtn.state = CommandStateUnabled;
		cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
		return true;
	}
	else if (cts.commit) {
		if (condition) {
			Map::WatchReturn watch = Map::sharedMap()->watch(this, this, condition, range);

			if (watch.inside) {
				*cts.target = watch.target;
				cts.ip++;
				return false;
			}
		}
		cts.rtn.state = CommandStateUnabled;
		cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
		return true;
	}
    else {
        cts.rtn.state = CommandStateLine;
        cts.rtn.parameter.line.condition = condition;
        cts.rtn.parameter.line.range = range.value(this);
        cts.rtn.parameter.line.thickness = thickness.value(this);
		cts.rtn.parameter.line.destAnimation = destAnimation;
		cts.rtn.parameter.line.sourceAnimation = sourceAnimation;
		cts.rtn.parameter.line.effect = effect;
        cts.rtn.parameter.line.all = all;
        return true;
    }
}
void Unit::ctRallyPoint(CommandTriggerState& cts) {
    if (*cts.target) {
        if (cts.commit) {
            synchronized(this, GameLockObject) {
                if (*cts.target != this) {
                    if (!_producing) {
                        _producing = new Producing();
                        memset(_producing, 0, sizeof(Producing));
                    }
                    retain(_producing->rallyPoint, *cts.target);
                }
                else {
                    if (_producing) {
                        release(_producing->rallyPoint);
                        
                        if (_producing->count == 0) {
                            delete _producing;
                            _producing = NULL;
                        }
                    }
                }
            }
        }
        cts.rtn.state = CommandStateEnabled;
        cts.rtn.parameter.enabled.stack = CommandStackRewind;
        cts.rtn.parameter.enabled.all = true;
    }
    else if (cts.commit) {
        cts.rtn.state = CommandStateUnabled;
        cts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		cts.rtn.parameter.unabled.announce = AnnounceNone;
    }
    else {
        cts.rtn.state = CommandStateTarget;
        cts.rtn.parameter.target.distance = 0.0f;
        cts.rtn.parameter.target.condition = NULL;
		cts.rtn.parameter.target.destAnimation = AnimationIndex::None;
        cts.rtn.parameter.target.sourceAnimation = AnimationIndex(AnimationSetGame, AnimationGameRallyCommand);
        cts.rtn.parameter.target.effect = CommandEffectLine_3 | CommandEffectStroke | CommandEffectBlink;
        cts.rtn.parameter.target.all = true;
    }
}
void Unit::ctProduce(CommandTriggerState& cts) {
    UnitIndex index(cts.cursor);
    
    bool unabled = readBoolean(cts.cursor);
    
    if (unabled) {
        cts.rtn.state = CommandStateProduceUnabled;
        cts.rtn.parameter.produce.index = index;
        cts.rtn.parameter.produce.msg = MessageIndex(cts.cursor);
		cts.rtn.parameter.unabled.announce = (AnnounceIndex)readByte(cts.cursor);
        return;
    }
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    const Force* force = _force;
    
    if (force) {
        for (int i = 0; i < 2; i++) {
            if (force->resource(i) < data.resources[i]) {
                cts.rtn.state = CommandStateProduceUnabled;
                cts.rtn.parameter.produce.index = index;
                cts.rtn.parameter.produce.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughMineral + i);
				cts.rtn.parameter.produce.announce = (AnnounceIndex)(AnnounceNotEnoughMineral + i);
                return;
            }
        }
    }
    if (_producing) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_producing && _producing->count == MaxWorking) {
                cts.rtn.state = CommandStateProduceUnabled;
                cts.rtn.parameter.produce.index = index;
                cts.rtn.parameter.produce.msg = MessageIndex(MessageSetCommand, MessageCommandProduceFull);
				cts.rtn.parameter.produce.announce = AnnounceNone;
                return;
            }
        }
    }
    if (cts.commit) {
        synchronized(this, GameLockObject) {
            if (!_producing) {
                _producing = new Producing();
                memset(_producing, 0, sizeof(Producing));
            }
            else if (_producing->count) {
                int remaining = 0;
                for (int i = 0; i < _producing->count; i++) {
                    remaining += Asset::sharedAsset()->unit(_producing->indices[i]).progress;
                }
                remaining -= (int)_producing->progress;
                if (cts.commit <= remaining) {
                    cts.rtn.parameter.retry.remaining = remaining;
                    cts.rtn.state = CommandStateRetry;
                    return;
                }
            }
            _producing->indices[_producing->count++] = index;
        }
        if (_force) {
            _force->reviseResource(0, (fixed)-data.resources[0]);
            _force->reviseResource(1, (fixed)-data.resources[1]);
        }
        if (_producing->count == 1) {
            _dirtyui = true;
        }
        cts.rtn.state = CommandStateEnabled;
        cts.rtn.parameter.enabled.stack = CommandStackRewind;
        cts.rtn.parameter.enabled.all = false;
    }
    else {
        cts.rtn.parameter.produce.index = index;
        cts.rtn.state = CommandStateProduce;
    }
}
void Unit::ctTrain(CommandTriggerState& cts) {
    RuneIndex index(cts.cursor);
    
    bool unabled = readBoolean(cts.cursor);
    
	const Force* force = _force;

	if (!force) {
		cts.rtn.state = CommandStateEmpty;
		return;
	}
    if (unabled) {
        cts.rtn.state = CommandStateTrainUnabled;
        cts.rtn.parameter.train.index = index;
        cts.rtn.parameter.train.msg = MessageIndex(cts.cursor);
		cts.rtn.parameter.train.announce = (AnnounceIndex)readByte(cts.cursor);
		cts.rtn.parameter.train.progress = 0;
		return;
    }
    
	switch (force->trainingState(index)) {
		case TrainingStateNone:
			cts.rtn.state = CommandStateEmpty;
			return;
		case TrainingStateProgress:
			cts.rtn.state = CommandStateTrainUnabled;
			cts.rtn.parameter.train.index = index;
			cts.rtn.parameter.train.msg = MessageIndex::None;
			cts.rtn.parameter.train.announce = AnnounceNone;
			cts.rtn.parameter.train.progress = 0;

			if (_training) {
				synchronized_to_update_thread(this, GameLockObject) {
					if (_training && _training->indices[0] == index) {
						cts.rtn.parameter.train.progress = _training->progress;
					}
				}
			}
			return;
		case TrainingStateComplete:
			cts.rtn.state = CommandStateTrainComplete;
			cts.rtn.parameter.train.index = index;
			cts.rtn.parameter.train.msg = MessageIndex::None;
			cts.rtn.parameter.train.announce = AnnounceNone;
			cts.rtn.parameter.train.progress = 0;
			return;
	}

	const RuneData& data = Asset::sharedAsset()->rune(index);

    for (int i = 0; i < 2; i++) {
        if (force->resource(i) < data.resources[i]) {
            cts.rtn.state = CommandStateTrainUnabled;
            cts.rtn.parameter.train.index = index;
            cts.rtn.parameter.train.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughMineral + i);
			cts.rtn.parameter.train.announce = (AnnounceIndex)(AnnounceNotEnoughMineral + i);
			cts.rtn.parameter.train.progress = 0;
            return;
        }
    }
    if (cts.commit) {
        synchronized(this, GameLockObject) {
            if (!_training) {
                _training = new Training();
                memset(_training, 0, sizeof(Training));
            }
            else if (_training->count) {
                int remaining = 0;
                for (int i = 0; i < _training->count; i++) {
                    remaining += Asset::sharedAsset()->rune(_training->indices[i]).progress;
                }
                remaining -= (int)_training->progress;
                if (cts.commit <= remaining) {
                    cts.rtn.parameter.retry.remaining = remaining;
                    cts.rtn.state = CommandStateRetry;
                    return;
                }
            }
            _training->indices[_training->count++] = index;
        }
        _force->reviseResource(0, (fixed)-data.resources[0]);
        _force->reviseResource(1, (fixed)-data.resources[1]);
        _force->startTraining(index);
        
        if (_training->count == 1) {
            _dirtyui = true;
        }
        cts.rtn.state = CommandStateEnabled;
        cts.rtn.parameter.enabled.stack = CommandStackRewind;
        cts.rtn.parameter.enabled.all = false;
    }
    else {
		cts.rtn.state = CommandStateTrain;
		cts.rtn.parameter.train.index = index;
		cts.rtn.parameter.train.msg = MessageIndex::None;
		cts.rtn.parameter.train.announce = AnnounceNone;
		cts.rtn.parameter.train.progress = 0;
    }
}
void Unit::ctBuild(CommandTriggerState& cts) {
    UnitIndex index(cts.cursor);
    CommandBuildTarget target = (CommandBuildTarget)readByte(cts.cursor);
	bool unabled = readBoolean(cts.cursor);

    if (unabled) {
        cts.rtn.state = CommandStateBuildUnabled;
        cts.rtn.parameter.build.index = index;
        cts.rtn.parameter.build.msg = MessageIndex(cts.cursor);
		cts.rtn.parameter.build.announce = (AnnounceIndex)readByte(cts.cursor);
        cts.rtn.parameter.build.target = target;
        return;
    }
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    const Force* force = _force;
    
    if (force) {
        for (int i = 0; i < 2; i++) {
            if (force->resource(i) < data.resources[i]) {
                cts.rtn.state = CommandStateBuildUnabled;
                cts.rtn.parameter.build.index = index;
                cts.rtn.parameter.build.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughMineral + i);
				cts.rtn.parameter.build.announce = (AnnounceIndex)(AnnounceNotEnoughMineral + i);
                cts.rtn.parameter.build.target = target;
                return;
            }
        }
        int populationDiff = target == CommandBuildTargetNone ? data.population - _data->population : data.population;
        if (populationDiff > 0 && force->population() + populationDiff > force->supply()) {
            cts.rtn.state = CommandStateBuildUnabled;
            cts.rtn.parameter.build.index = index;
            cts.rtn.parameter.build.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughSupply);
			cts.rtn.parameter.build.announce = AnnounceNone;
            cts.rtn.parameter.build.target = target;
            return;
        }
    }
    /*
     커밋일 경우
        타겟조건이 맞을 경우
            성공, 리트라이
        아닐 경우
            빌드 언에이블
     아닐 경우
        타겟이 없거나 타겟 조건이 맞을 경우
            빌드
        아닐 경우
            빌드언에이블
     */
    if (cts.commit) {
        bool flag = false;
        switch (target) {
            case CommandBuildTargetNone:
                flag = *cts.target == NULL;
                break;
            case CommandBuildTargetAll:
                flag = (*cts.target && (*cts.target)->compareType(ObjectMaskFrame) && static_cast<Frame*>(*cts.target)->index == index);
                break;
            case CommandBuildTargetVisible:
                flag = force && (*cts.target && (*cts.target)->compareType(ObjectMaskFrame) && static_cast<Frame*>(*cts.target)->index == index) && Map::sharedMap()->isVisible((*cts.target)->bounds(), force->alliance());
                break;
            case CommandBuildTargetSpawn:
                flag = force && (*cts.target && (*cts.target)->compareType(ObjectMaskFrame) && static_cast<Frame*>(*cts.target)->index == index) && Map::sharedMap()->isSpawnable((*cts.target)->point(), force);
                break;
        }
        if (flag) {
			if (cts.commit && cts.commit <= _busy) {
                cts.rtn.parameter.retry.remaining = _busy;
				cts.rtn.state = CommandStateRetry;
			}
			else {
				cts.rtn.state = CommandStateEnabled;
				cts.rtn.parameter.enabled.stack = CommandStackSingle;
				cts.rtn.parameter.enabled.all = !target;
			}
            return;
        }
    }
    else if (!(*cts.target) || (target && (*cts.target)->compareType(ObjectMaskFrame) && static_cast<Frame*>(*cts.target)->index == index)) {
        cts.rtn.state = CommandStateBuild;
        cts.rtn.parameter.build.index = index;
        cts.rtn.parameter.build.target = target;
        return;
    }
    cts.rtn.state = CommandStateBuildUnabled;
    cts.rtn.parameter.build.index = index;
    cts.rtn.parameter.build.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
	cts.rtn.parameter.build.announce = AnnounceBuildUnabled;
    cts.rtn.parameter.build.target = target;
}
bool Unit::ctCooltime(CommandTriggerState& cts) {
    int index = readByte(cts.cursor);
    
    fixed remaining = _cooltimes[index].remaining;
    if (remaining) {
        if (readBoolean(cts.cursor)) {
            cts.cursor += 11;       //fixed+short+boolean
        }
        cts.rtn.state = CommandStateCooltime;
        cts.rtn.parameter.cooltime.initial = _cooltimes[index].initial;
        cts.rtn.parameter.cooltime.remaining = remaining;
        cts.rtn.parameter.cooltime.enabled = true;
        return true;
    }
    if (readBoolean(cts.cursor)) {
        AbilityValue<fixed> cooltime = readAbilityFixed(cts.cursor);
        bool reverse = readBoolean(cts.cursor);
        
        if (cts.commit) {
            fixed cooltimeValue = cooltime.value(this);
            if (reverse) {
                if (cooltimeValue) {
                    _cooltimes[index].remaining = _cooltimes[index].initial = fixed::One / cooltimeValue;
                }
            }
            else {
                _cooltimes[index].remaining = _cooltimes[index].initial = cooltimeValue;
            }
        }
    }
    cts.ip++;
    return false;
}
void Unit::ctEnabled(CommandTriggerState& cts) {
    CommandStack stack = (CommandStack)readByte(cts.cursor);
    int source = readByte(cts.cursor);
    
	if (source == 0 && cts.commit) {
		if (stack != CommandStackEnabled && _command == cts.key) {
			cts.rtn.state = CommandStateEmpty;
			return;
		}
		else {
			int busy = _busy;
			if (_command == cts.key) busy++;
			foreach(const NextAction&, nextAction, _nextActions) {
				if (nextAction.command == cts.key) busy++;
			}
			if (cts.commit <= busy) {
                cts.rtn.parameter.retry.remaining = busy;
				cts.rtn.state = CommandStateRetry;
				return;
			}
		}
	}
    cts.rtn.state = CommandStateEnabled;
    cts.rtn.parameter.enabled.stack = stack;
    cts.rtn.parameter.enabled.all = source == 2;
}
void Unit::ctStop(CommandTriggerState& cts) {
	int ip1 = readShort(cts.cursor);

	cts.ip = ip1;
}

enum CommandTriggerOp {
    CT_Effect,
    CT_EffectRange,
    CT_EffectAngle,
    CT_EffectLine,
    CT_Attack,
    CT_AttackRange,
    CT_AttackAngle,
    CT_AttackLine,
    CT_Compare,
    CT_Revise,
    CT_ReviseRange,
    CT_ReviseAngle,
    CT_ReviseLine,
    CT_IsLevel,
    CT_HasUnit,
    CT_HasUnits,
	CT_HasTier,
    CT_HasRune,
    CT_IsState,
    CT_ChangeState,
    CT_MaintainTarget,
    CT_HasTarget,
    CT_IsTarget,
    CT_IsBuffed,
    CT_Buff,
    CT_BuffRange,
    CT_BuffAngle,
    CT_BuffLine,
    CT_Cloak,
    CT_Detect,
	CT_IsAmbush,
	CT_IsCallable,
    CT_IsTransporting,
    CT_IsResource,
    CT_IsBuilding,
    CT_IsProducing,
    CT_IsTraining,
    CT_Random,
    CT_WatchRandom,
    CT_ChildrenCount,
    CT_Event,
    CT_Sound,
	CT_Announce,
    CT_Vibrate,
    CT_Message,
	CT_AttackEstimation,
	CT_ReviseEstimation,
	CT_IsSelecting,
	CT_IsCommanding,
	CT_Empty,
    CT_Unabled,
    CT_Target,
    CT_Range,
    CT_Angle,
    CT_Line,
    CT_RallyPoint,
    CT_Produce,
    CT_Train,
    CT_Build,
    CT_Cooltime,
    CT_Enabled,
	CT_Stop
};

#ifdef UseCommandTriggerTest
static const char* CommandTriggerOpName[] = {
    "CT_Effect",
    "CT_EffectRange",
    "CT_EffectAngle",
    "CT_EffectLine",
    "CT_Attack",
    "CT_AttackRange",
    "CT_AttackAngle",
    "CT_AttackLine",
    "CT_Compare",
    "CT_Revise",
    "CT_ReviseRange",
    "CT_ReviseAngle",
    "CT_ReviseLine",
    "CT_IsLevel",
    "CT_HasUnit",
    "CT_HasUnits",
    "CT_HasTier",
    "CT_HasRune",
    "CT_IsState",
    "CT_ChangeState",
    "CT_MaintainTarget",
    "CT_HasTarget",
    "CT_IsTarget",
    "CT_IsBuffed",
    "CT_Buff",
    "CT_BuffRange",
    "CT_BuffAngle",
    "CT_BuffLine",
    "CT_Cloak",
    "CT_Detect",
	"CT_IsAmbush",
	"CT_IsCallable",
    "CT_IsTransporting",
    "CT_IsResource",
    "CT_IsBuilding",
    "CT_IsProducing",
    "CT_IsTraining",
    "CT_Random",
    "CT_WatchRandom",
    "CT_ChildrenCount",
    "CT_Event",
    "CT_Sound",
	"CT_Announce",
    "CT_Vibrate",
    "CT_Message",
	"CT_AttackEstimation",
	"CT_ReviseEstimation",
	"CT_IsSelecting",
	"CT_IsCommanding",
    "CT_Empty",
    "CT_Unabled",
    "CT_Target",
    "CT_Range",
    "CT_Angle",
    "CT_Line",
    "CT_RallyPoint",
    "CT_Produce",
    "CT_Train",
    "CT_Build",
    "CT_Cooltime",
    "CT_Enabled",
	"CT_Stop"
};
#endif

CommandReturn Unit::executeCommand(const CommandIndex& key, const CSArray<byte, 2>* commandTrigger, Object** target, fixed& estimation, int selectionFlag, int commit) {
    if (commandTrigger) {
        CommandTriggerState cts;
        cts.key = key;
        cts.rtn.state = CommandStateEmpty;
        cts.target = target;
        cts.commit = commit;
        cts.ip = 0;
        cts.estimation = &estimation;
        cts.selectionFlag = selectionFlag;
        
        do {
            const CSArray<byte, 1>* trigger = commandTrigger->objectAtIndex(cts.ip);
            
            cts.cursor = trigger->pointer();
            
            CommandTriggerOp opCode = (CommandTriggerOp)readByte(cts.cursor);
            
#ifdef UseCommandTriggerTest
            if (commit) UnitLog(this, "command trigger (frame:%d) (ip:%d) (%s)", Map::sharedMap()->frame(), cts.ip, CommandTriggerOpName[opCode]);
#endif
            
            switch(opCode) {
                case CT_Effect:
                    ctEffect(cts);
                    break;
                case CT_EffectRange:
                    ctEffectRange(cts);
                    break;
                case CT_EffectAngle:
                    ctEffectAngle(cts);
                    break;
                case CT_EffectLine:
                    ctEffectLine(cts);
                    break;
                case CT_Attack:
                    ctAttack(cts);
                    break;
                case CT_AttackRange:
                    ctAttackRange(cts);
                    break;
                case CT_AttackAngle:
                    ctAttackAngle(cts);
                    break;
                case CT_AttackLine:
                    ctAttackLine(cts);
                    break;
                case CT_Compare:
                    ctCompare(cts);
                    break;
                case CT_Revise:
                    ctRevise(cts);
                    break;
                case CT_ReviseRange:
                    ctReviseRange(cts);
                    break;
                case CT_ReviseAngle:
                    ctReviseAngle(cts);
                    break;
                case CT_ReviseLine:
                    ctReviseLine(cts);
                    break;
                case CT_IsLevel:
                    ctIsLevel(cts);
                    break;
                case CT_HasUnit:
                    ctHasUnit(cts);
                    break;
                case CT_HasUnits:
                    ctHasUnits(cts);
                    break;
                case CT_HasTier:
                    ctHasTier(cts);
                    break;
                case CT_HasRune:
                    ctHasRune(cts);
                    break;
                case CT_IsState	:
                    ctIsState(cts);
                    break;
                case CT_ChangeState:
                    ctChangeState(cts);
                    break;
                case CT_MaintainTarget:
                    ctMaintainTarget(cts);
                    break;
                case CT_HasTarget:
                    ctHasTarget(cts);
                    break;
                case CT_IsTarget:
                    ctIsTarget(cts);
                    break;
                case CT_IsBuffed	:
                    ctIsBuffed(cts);
                    break;
                case CT_Buff:
                    ctBuff(cts);
                    break;
                case CT_BuffRange:
                    ctBuffRange(cts);
                    break;
                case CT_BuffAngle:
                    ctBuffAngle(cts);
                    break;
                case CT_BuffLine:
                    ctBuffLine(cts);
                    break;
                case CT_Cloak:
                    ctCloak(cts);
                    break;
                case CT_Detect:
                    ctDetect(cts);
                    break;
				case CT_IsAmbush:
					ctIsAmbush(cts);
					break;
				case CT_IsCallable:
                    ctIsCallable(cts);
                    break;
                case CT_IsTransporting:
                    ctIsTransporting(cts);
                    break;
                case CT_IsResource:
                    ctIsResource(cts);
                    break;
                case CT_IsBuilding:
                    ctIsBuilding(cts);
                    break;
                case CT_IsProducing:
                    ctIsProducing(cts);
                    break;
                case CT_IsTraining:
                    ctIsTraining(cts);
                    break;
                case CT_Random:
                    ctRandom(cts);
                    break;
                case CT_WatchRandom:
                    ctWatchRandom(cts);
                    break;
                case CT_ChildrenCount:
                    ctChildrenCount(cts);
                    break;
                case CT_Event:
                    ctEvent(cts);
                    break;
                case CT_Sound:
                    ctSound(cts);
                    break;
				case CT_Announce:
					ctAnnounce(cts);
					break;
                case CT_Vibrate:
                    ctVibrate(cts);
                    break;
                case CT_Message:
                    ctMessage(cts);
                    break;
				case CT_AttackEstimation:
					if (ctAttackEstimation(cts)) {
						return cts.rtn;
					}
					break;
				case CT_ReviseEstimation:
					if (ctReviseEstimation(cts)) {
						return cts.rtn;
					}
					break;
				case CT_IsSelecting:
					ctIsSelecting(cts);
					break;
				case CT_IsCommanding:
					ctIsCommanding(cts);
					break;
                case CT_Empty:
                    ctEmpty(cts);
                    return cts.rtn;
                case CT_Unabled	:
                    ctUnabled(cts);
                    return cts.rtn;
                case CT_Target:
                    if (ctTarget(cts)) {
                        return cts.rtn;
                    }
                    break;
                case CT_Range:
                    if (ctRange(cts)) {
                        return cts.rtn;
                    }
                    break;
                case CT_Angle:
                    if (ctAngle(cts)) {
                        return cts.rtn;
                    }
                    break;
                case CT_Line:
                    if (ctLine(cts)) {
                        return cts.rtn;
                    }
                    break;
                case CT_RallyPoint:
                    ctRallyPoint(cts);
                    return cts.rtn;
                case CT_Produce:
                    ctProduce(cts);
                    return cts.rtn;
                case CT_Train:
                    ctTrain(cts);
                    return cts.rtn;
                case CT_Build:
                    ctBuild(cts);
                    return cts.rtn;
                case CT_Cooltime:
                    if (ctCooltime(cts)) {
                        return cts.rtn;
                    }
                    break;
                case CT_Enabled:
                    ctEnabled(cts);
                    return cts.rtn;
				case CT_Stop:
					ctStop(cts);
					break;
            }
            
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)cts.cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "command trigger error:%d,%d,%d", opCode, pos, len);
#endif
        }
        while(cts.ip < commandTrigger->count());
    }
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    return rtn;
}

