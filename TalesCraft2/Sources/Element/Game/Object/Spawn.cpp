//
//  Card+spell.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 5. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Spawn.h"

#include "Map.h"

Spawn::Spawn(Force* force, const UnitIndex& index, int level, int skin) : UnitBaseGame(force, index, level, skin) {
    if (!level) this->level = force->unitLevel(index);
    if (!skin) this->skin = force->unitSkin(index);

    const UnitData& data = this->data();
    if (!data.build) _remaining = data.progress;            //TODO:temp

    GameLeak_addObject(this);
}

Spawn::~Spawn() {
    GameLeak_removeObject(this);

	release(_name);
	release(_abilities);
}

void Spawn::activate() {
    _remaining = fixed::Zero;
}

fixed Spawn::abilityElement(int index, fixed value, const UnitBase *target) const {
	value = UnitBase::abilityElement(index, value, target);

	if (_abilities) {
		synchronized_to_update_thread(this, GameLockObject) {
			if (_abilities) {
				foreach(const AbilityRevision&, abtr, _abilities) {
					if (abtr.index == index && (!abtr.condition || (target && target->match(this, abtr.condition)))) {
						if (abtr.value >= fixed::Zero) value = abtr.value;
						value += abtr.diff;
						break;
					}
				}
			}
		}
	}

	return value;
}

const CSString* Spawn::name() const {
	if (_name) {
		synchronized(this, GameLockObject) {
			if (_name) {
				_name->retain();
				_name->autorelease();
				return _name;
			}
		}
	}
	return data().name->value();
}

void Spawn::rename(const CSString* name) {
	synchronized(this, GameLockObject) {
		retain(_name, name);
	}
}

fixed Spawn::attack(Unit* target, fixed rate, fixed time) {
    return target->revise(1, ability(AbilityAttack, fixed::Zero, target) * rate, time, this);
}

void Spawn::stEffect(TriggerState& ts) {
    AnimationIndex index(ts.cursor);
    bool attach = readBoolean(ts.cursor);
    
    if (ts.commit && ts.target) {
        AnimationEffect::effect(index, ts.target, NULL, false, attach, false);
    }
    
    ts.ip++;
}
void Spawn::stEffectRange(TriggerState& ts) {
    bool nearbyTo = readBoolean(ts.cursor);
    AbilityValue<fixed> range = readAbilityFixed(ts.cursor);
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    AnimationIndex index(ts.cursor);
    bool attach = readBoolean(ts.cursor);
    
    if (ts.commit && ts.target) {
        Map::sharedMap()->effectWithRange(this, ts.target, NULL, false, range.value(this), condition, index, nearbyTo, attach);
    }
    ts.ip++;
}
void Spawn::stEffectAll(TriggerState& ts) {
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    AnimationIndex index(ts.cursor);
    bool attach = readBoolean(ts.cursor);
    
    if (ts.commit) {
        Map::sharedMap()->effectToAll(this, condition, index, attach);
    }
    
    ts.ip++;
}
void Spawn::stAttack(TriggerState& ts) {
    AbilityValue<fixed> rate = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit && ts.target && ts.target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(ts.target);
        
        attack(target, rate.value(this, target), time);
    }
    ts.ip++;
}
void Spawn::stAttackRange(TriggerState& ts) {
    AbilityValue<fixed> range = readAbilityFixed(ts.cursor);
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(ts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit && ts.target) {
        Map::sharedMap()->attackWithRange(this, ts.target, NULL, false, range.value(this), condition, rate, attenuation, time);
    }
    ts.ip++;
}
void Spawn::stAttackAll(TriggerState& ts) {
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    AbilityValue<fixed> rate = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit) {
        Map::sharedMap()->attackToAll(this, condition, rate, time);
    }
    ts.ip++;
}
void Spawn::stCompare(TriggerState& ts) {
    int op = readByte(ts.cursor);
    fixed value = readAbilityFixed(ts.cursor).value(this);
    int ip1 = readShort(ts.cursor);
    
    ts.ip = compare(op, value) ? ts.ip + 1 : ip1;
}
void Spawn::stRevise(TriggerState& ts) {
    int op = readByte(ts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit && ts.target && ts.target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(ts.target);
        
        target->revise(op, value.value(this, target), time, this);
    }
    ts.ip++;
}
void Spawn::stReviseRange(TriggerState& ts) {
    AbilityValue<fixed> range = readAbilityFixed(ts.cursor);
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    int op = readByte(ts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(ts.cursor);
	AbilityValue<fixed> attenuation = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit && ts.target) {
        Map::sharedMap()->reviseWithRange(this, ts.target, NULL, false, range.value(this), condition, op, value, attenuation, time);
    }
    
    ts.ip++;
}
void Spawn::stReviseAll(TriggerState& ts) {
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    int op = readByte(ts.cursor);
    AbilityValue<fixed> value = readAbilityFixed(ts.cursor);
    fixed time = readFixed(ts.cursor);
    
    if (ts.commit) {
        Map::sharedMap()->reviseToAll(this, condition, op, value, time);
    }
    ts.ip++;
}
void Spawn::stIsLevel(TriggerState& ts) {
    int level = readShort(ts.cursor);
    int ip1 = readShort(ts.cursor);
    
    ts.ip = this->level >= level ? ts.ip + 1 : ip1;
}
void Spawn::stHasUnit(TriggerState& ts) {
    UnitIndex index(ts.cursor);
    bool alive = readBoolean(ts.cursor);
    int unitCount = readByte(ts.cursor);
    int ip1 = readShort(ts.cursor);
    
    ts.ip = _force->unitCount(index, alive) >= unitCount ? ts.ip + 1 : ip1;
}
void Spawn::stHasUnits(TriggerState& ts) {
    int count = readLength(ts.cursor);
    UnitIndex* indices = (UnitIndex*)alloca(count * sizeof(UnitIndex));
    for (int i = 0; i < count; i++) {
        indices[i] = UnitIndex(ts.cursor);
    }
    bool alive = readBoolean(ts.cursor);
    int unitCount = readByte(ts.cursor);
    int ip1 = readShort(ts.cursor);
    
    for (int i = 0; i < count; i++) {
        unitCount -= _force->unitCount(indices[i], alive);
    }
    if (unitCount <= 0) {
        ts.ip++;
        return;
    }
    ts.ip = ip1;
}
void Spawn::stHasRune(TriggerState& ts) {
    RuneIndex index(ts.cursor);
    int ip1 = readShort(ts.cursor);
    
    ts.ip = _force->runeForIndex(index) && _force->isTrained(index) ? ts.ip + 1 : ip1;
}
void Spawn::stIsTarget(TriggerState& ts) {
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    int ip1 = readShort(ts.cursor);
    
    ts.ip = ts.target && ts.target->compareType(ObjectMaskUnit) && ts.target->isLocated() && static_cast<const Unit*>(ts.target)->match(this, condition) ? ts.ip + 1 : ip1;
}
void Spawn::stBuff(TriggerState& ts) {
    BuffIndex index(ts.cursor);
	int timeOp = readByte(ts.cursor);
	AbilityValue<fixed> time;
    switch (timeOp) {
        case 1:
		case 2:
		case 3:
            time = readAbilityFixed(ts.cursor);
            break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
    }
    AbilityValue<fixed> stack = readAbilityFixed(ts.cursor);
    if (ts.commit && ts.target && ts.target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(ts.target);
        
        target->buff(index, timeOp, time.value(this, target), stack.value(this, target), this);
    }
    ts.ip++;
}
void Spawn::stBuffRange(TriggerState& ts) {
    AbilityValue<fixed> range = readAbilityFixed(ts.cursor);
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    BuffIndex index(ts.cursor);
	int timeOp = readByte(ts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(ts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(ts.cursor);
    
    if (ts.commit && ts.target) {
        Map::sharedMap()->buffWithRange(this, ts.target, NULL, false, range.value(this), condition, index, timeOp, time, stack);
    }
    ts.ip++;
}
void Spawn::stBuffAll(TriggerState& ts) {
    const byte* condition = ts.cursor;
    ts.cursor += matchLength(condition);
    BuffIndex index(ts.cursor);
	int timeOp = readByte(ts.cursor);
	AbilityValue<fixed> time;
	switch (timeOp) {
		case 1:
		case 2:
		case 3:
			time = readAbilityFixed(ts.cursor);
			break;
		default:
			time = AbilityValue<fixed>(fixed::Zero, AbilityNone);
			break;
	}
    AbilityValue<fixed> stack = readAbilityFixed(ts.cursor);
    
    if (ts.commit) {
        Map::sharedMap()->buffToAll(this, condition, index, timeOp, time, stack);
    }
    ts.ip++;
}
void Spawn::stEvent(TriggerState& ts) {
    int target = readByte(ts.cursor);
    if (ts.commit && ts.target) {
        const CSColor& color = _force->allianceColor();
        
        static const AnimationIndex animation(AnimationSetGame, AnimationGameMinimapEvent);
        
        switch (target) {
            case 0:
                Map::sharedMap()->event(_force, animation, color, ts.target->point());
                break;
            case 1:
                Map::sharedMap()->event(_force, true, animation, color, ts.target->point());
                break;
            case 2:
                Map::sharedMap()->event(_force, false, animation, color, ts.target->point());
                break;
            default:
                Map::sharedMap()->event(animation, color, ts.target->point());
                break;
        }
    }
    ts.ip++;
}
void Spawn::stSound(TriggerState& ts) {
	int soundCount = readLength(ts.cursor);
	const byte* soundCursor = ts.cursor;
	ts.cursor += soundCount * sizeof(SoundIndex);
    float volume = readFloat(ts.cursor);
    if (ts.commit) {
		soundCursor += (randInt(0, soundCount - 1) * sizeof(SoundIndex));
		SoundIndex soundIndex(soundCursor);

        if (ts.target) {
            Map::sharedMap()->sound(soundIndex, ts.target, volume, true);
        }
        else {
            Map::sharedMap()->sound(soundIndex, _force, volume, true);
        }
    }
    ts.ip++;
}
void Spawn::stAnnounce(TriggerState& ts) {
	int target = readByte(ts.cursor);
	AnnounceIndex index = (AnnounceIndex)readByte(ts.cursor);

	if (ts.commit) {
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
	ts.ip++;
}
void Spawn::stVibrate(TriggerState& ts) {
    int level = readByte(ts.cursor);
    
    if (ts.commit) {
        Map::sharedMap()->vibrate(level, ts.target);
    }
    ts.ip++;
}
void Spawn::stMessage(TriggerState &ts) {
    int target = readByte(ts.cursor);
    
    MessageIndex index(ts.cursor);
    
    if (ts.commit) {
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
    ts.ip++;
}
void Spawn::stStop(TriggerState& ts) {
    int ip1 = readShort(ts.cursor);
    
    ts.ip = ip1;
}
void Spawn::stEmpty(TriggerState &ts) {
    ts.rtn.state = CommandStateEmpty;
}
void Spawn::stUnabled(TriggerState& ts) {
    ts.rtn.state = CommandStateUnabled;
    ts.rtn.parameter.unabled.msg = MessageIndex(ts.cursor);
	ts.rtn.parameter.unabled.announce = (AnnounceIndex)readByte(ts.cursor);
}
bool Spawn::stTarget(TriggerState& ts) {
    const byte* condition = NULL;
    if (readBoolean(ts.cursor)) {
        condition = ts.cursor;
        ts.cursor += UnitBase::matchLength(condition);
    }
    AnimationIndex animation(ts.cursor);
    int effect = readByte(ts.cursor);
    bool all = readBoolean(ts.cursor);
    
    if (ts.target &&
            (!condition || (ts.target->compareType(ObjectMaskUnit) && ts.target->isLocated() && static_cast<const Unit*>(ts.target)->match(this, condition))) &&
            (all || Map::sharedMap()->isVisible(ts.target->bounds(), _force->alliance()))) {
            
        ts.ip++;
        return false;
    }
    else if (ts.commit) {
        ts.rtn.state = CommandStateUnabled;
        ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		ts.rtn.parameter.unabled.announce = AnnounceNone;
        return true;
    }
    else {
        ts.rtn.state = CommandStateTarget;
        ts.rtn.parameter.target.distance = 0.0f;
        ts.rtn.parameter.target.condition = condition;
        ts.rtn.parameter.target.destAnimation = animation;
		ts.rtn.parameter.target.sourceAnimation = AnimationIndex::None;
        ts.rtn.parameter.target.effect = effect;
        ts.rtn.parameter.target.all = all;
        return true;
    }
}
bool Spawn::stRange(TriggerState& ts) {
    AbilityValue<float> range = readAbilityFloat(ts.cursor);
    const byte* condition = NULL;
    if (readBoolean(ts.cursor)) {
        condition = ts.cursor;
        ts.cursor += matchLength(condition);
    }
    AnimationIndex animation(ts.cursor);
    int effect = readByte(ts.cursor);
    bool all = readBoolean(ts.cursor);
    
    if (ts.target && (all || Map::sharedMap()->isVisible(ts.target->bounds(), _force->alliance()))) {
        ts.ip++;
        return false;
    }
    else if (ts.commit) {
        ts.rtn.state = CommandStateUnabled;
        ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		ts.rtn.parameter.unabled.announce = AnnounceNone;
        return true;
    }
    else {
        ts.rtn.state = CommandStateRange;
        ts.rtn.parameter.range.distance = 0.0f;
        ts.rtn.parameter.range.condition = condition;
        ts.rtn.parameter.range.range = range.value(this);
		ts.rtn.parameter.range.destAnimation = animation;
		ts.rtn.parameter.range.sourceAnimation = AnimationIndex::None;
        ts.rtn.parameter.range.effect = effect;
        ts.rtn.parameter.range.all = all;
        return true;
    }
}
void Spawn::stBuild(TriggerState& ts) {
    CommandBuildTarget target = (CommandBuildTarget)readByte(ts.cursor);
    bool flag = false;
    if (ts.target) {
        switch (target) {
            case CommandBuildTargetAll:
                flag = true;
                break;
            case CommandBuildTargetVisible:
                flag = Map::sharedMap()->isVisible(ts.target->bounds(), _force->alliance());
                break;
            case CommandBuildTargetSpawn:
                flag = Map::sharedMap()->isSpawnable(ts.target->point(), _force);
                break;
        }
    }
    if (flag) {
        ts.rtn.state = CommandStateEnabled;
        ts.rtn.parameter.enabled.stack = CommandStackSingle;
        ts.rtn.parameter.enabled.all = false;
    }
    else if (ts.commit) {
        ts.rtn.state = CommandStateUnabled;
        ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
		ts.rtn.parameter.unabled.announce = AnnounceNone;
    }
    else {
        ts.rtn.state = CommandStateBuild;
        ts.rtn.parameter.build.index = _index;
        ts.rtn.parameter.build.target = target;
    }
}
void Spawn::stEnabled(TriggerState& ts) {
    bool creation = readBoolean(ts.cursor);
    ts.rtn.state = CommandStateEnabled;
    if (!creation) {
        ts.target = NULL;
    }
    ts.rtn.parameter.enabled.stack = CommandStackSingle;
    ts.rtn.parameter.enabled.all = false;
}

enum SpawnTriggerOp {
    ST_Effect,
    ST_EffectRange,
    ST_EffectAll,
    ST_Attack,
    ST_AttackRange,
    ST_AttackAll,
    ST_Compare,
    ST_Revise,
    ST_ReviseRange,
    ST_ReviseAll,
    ST_IsLevel,
    ST_HasUnit,
    ST_HasUnits,
    ST_HasRune,
    ST_IsTarget,
    ST_Buff,
    ST_BuffRange,
    ST_BuffAll,
    ST_Event,
    ST_Sound,
	ST_Announce,
    ST_Vibrate,
    ST_Message,
    ST_Stop,
    ST_Empty,
    ST_Unabled,
    ST_Target,
    ST_Range,
    ST_Build,
    ST_Enabled
};

#ifdef UseSpawnTriggerTest
static const char* SpawnTriggerOpNames[] = {
    "ST_Effect",
    "ST_EffectRange",
    "ST_EffectAll",
    "ST_Attack",
    "ST_AttackRange",
    "ST_AttackAll",
    "ST_Compare",
    "ST_Revise",
    "ST_ReviseRange",
    "ST_ReviseAll",
    "ST_IsLevel",
    "ST_HasUnit",
    "ST_HasUnits",
    "ST_HasRune",
    "ST_IsTarget",
    "ST_Buff",
    "ST_BuffRange",
    "ST_BuffAll",
    "ST_Event",
    "ST_Sound",
	"ST_Announce",
    "ST_Vibrate",
    "ST_Message",
    "ST_Stop",
    "ST_Empty",
    "ST_Unabled",
    "ST_Target",
    "ST_Range",
    "ST_Build",
    "ST_Enabled"
};
#endif

CommandReturn Spawn::run(Object* target, bool commit) {
    const UnitData& data = this->data();
    
    if (data.spellTrigger) {
        TriggerState ts;
        ts.rtn.state = CommandStateEmpty;
        ts.target = target;
        ts.commit = commit;
        ts.ip = 0;

        bool enabled = true;
        for (int i = 0; i < 2; i++) {
            if (_force->resource(i) < data.resources[i]) {
                enabled = false;
                break;
            }
        }
        if (enabled && data.population && _force->population() + data.population > _force->supply()) {
            enabled = false;
        }
        if (!enabled) ts.commit = false;

        do {
            const CSArray<byte, 1>* trigger = data.spellTrigger->objectAtIndex(ts.ip);
            
            ts.cursor = trigger->pointer();
            
            SpawnTriggerOp opCode = (SpawnTriggerOp)readByte(ts.cursor);
            
#ifdef UseSpawnTriggerTest
            if (ts.commit) CSLog("spell trigger (frame:%d) (ip:%d) (%s)", Map::sharedMap()->frame(), ts.ip, SpawnTriggerOpNames[opCode]);
#endif
            switch(opCode) {
                case ST_Effect:
                    stEffect(ts);
                    break;
                case ST_EffectRange:
                    stEffectRange(ts);
                    break;
                case ST_EffectAll:
                    stEffectAll(ts);
                    break;
                case ST_Attack:
                    stAttack(ts);
                    break;
                case ST_AttackRange:
                    stAttackRange(ts);
                    break;
                case ST_AttackAll:
                    stAttackAll(ts);
                    break;
                case ST_Compare:
                    stCompare(ts);
                    break;
                case ST_Revise:
                    stRevise(ts);
                    break;
                case ST_ReviseRange:
                    stReviseRange(ts);
                    break;
                case ST_ReviseAll:
                    stReviseAll(ts);
                    break;
                case ST_IsLevel:
                    stIsLevel(ts);
                    break;
                case ST_HasUnit:
                    stHasUnit(ts);
                    break;
                case ST_HasUnits:
                    stHasUnits(ts);
                    break;
                case ST_HasRune:
                    stHasRune(ts);
                    break;
                case ST_IsTarget:
                    stIsTarget(ts);
                    break;
                case ST_Buff:
                    stBuff(ts);
                    break;
                case ST_BuffRange:
                    stBuffRange(ts);
                    break;
                case ST_BuffAll:
                    stBuffAll(ts);
                    break;
                case ST_Event:
                    stEvent(ts);
                    break;
                case ST_Sound:
                    stSound(ts);
                    break;
				case ST_Announce:
					stAnnounce(ts);
					break;
                case ST_Vibrate:
                    stVibrate(ts);
                    break;
                case ST_Message:
                    stMessage(ts);
                    break;
                case ST_Stop:
                    stStop(ts);
                    break;
                case ST_Empty:
                    stEmpty(ts);
                    goto exit;
                case ST_Unabled:
                    stUnabled(ts);
                    goto exit;
                case ST_Target:
                    if (stTarget(ts)) {
                        goto exit;
                    }
                    break;
                case ST_Range:
                    if (stRange(ts)) {
                        goto exit;
                    }
                    break;
                case ST_Build:
                    stBuild(ts);
                    goto exit;
                case ST_Enabled:
                    stEnabled(ts);
                    goto exit;
            }
            
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)ts.cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "spell trigger error:%d,%d,%d", opCode, pos, len);
#endif
        }
        while(ts.ip < data.spellTrigger->count());
exit:
        if (ts.rtn.state > CommandStateUnabled) {
            if (_remaining) {
                ts.rtn.state = CommandStateCooltime;
                ts.rtn.parameter.cooltime.initial = data.progress;
                ts.rtn.parameter.cooltime.remaining = _remaining;
                ts.rtn.parameter.cooltime.enabled = enabled;
                return ts.rtn;
            }
            for (int i = 0; i < 2; i++) {
                if (_force->resource(i) < data.resources[i]) {
                    ts.rtn.state = CommandStateUnabled;
                    ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughMineral + i);
					ts.rtn.parameter.unabled.announce = (AnnounceIndex)(AnnounceNotEnoughMineral + i);
                    return ts.rtn;
                }
            }
            if (data.population && _force->population() + data.population > _force->supply()) {
                ts.rtn.state = CommandStateUnabled;
                ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandNotEnoughSupply);
				ts.rtn.parameter.unabled.announce = AnnounceNone;
                return ts.rtn;
            }
        }
        if (ts.rtn.state == CommandStateEnabled && commit) {
            if (ts.target) {
                for (int i = 0; i <= data.addition; i++) {
                    FPoint p = ts.target->point();
                    if (Map::sharedMap()->locatablePosition(data.type, p, data.collider, NULL, MapLocationCollided) == MapLocationUnabled) {
                        ts.rtn.state = CommandStateUnabled;
                        ts.rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandSummonUnabled);
                        ts.rtn.parameter.unabled.announce = AnnounceBuildUnabled;
                        return ts.rtn;
                    }
                    else {
                        Unit* unit = Unit::unit(_force, _index, level, skin, NULL);
                        unit->locate(p);

                        if (data.build) unit->startBuilding(Unit::StartBuildingIndependent);     //TODO:temp

                        Map::sharedMap()->voice(VoiceSpawn, unit);
                    }
                }
            }
            if (!data.build) _remaining = data.progress;
            _force->reviseResource(0, (fixed)-data.resources[0]);
            _force->reviseResource(1, (fixed)-data.resources[1]);
        }
        return ts.rtn;
    }
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    return rtn;
}

void Spawn::update() {
    if (_remaining) {
#ifdef UseCheatProgress
		_remaining -= Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One) * UseCheatProgress;
#else
		_remaining -= Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One);
#endif
        if (_remaining < fixed::Zero) _remaining = fixed::Zero;
    }
}

