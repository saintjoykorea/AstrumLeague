//
//  AbilityData.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 26..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "AbilityData.h"

#include "VirtualForce.h"
#include "VirtualUnit.h"

#include "Rune.h"

static fixed abilityValue(const CSArray<byte, 2>* trigger, const UnitBase *unit, const Rune* rune, fixed value, const UnitBase *target);

AbilityDisplayData::AbilityDisplayData(CSBuffer* buffer) :
    icon(buffer),
    name(CSObject::retain(buffer->readLocaleString())),
    display((AbilityDisplay)buffer->readByte()),
	view(buffer->readByte())
{
    
}

AbilityDisplayData::~AbilityDisplayData() {
    name->release();
}

AbilityTargetData::AbilityTargetData(CSBuffer* buffer) :
    name(CSObject::retain(buffer->readLocaleString()))
{
    VirtualForce* force = buffer->readBoolean() ? new VirtualForce(buffer) : NULL;
    VirtualUnit* target = new VirtualUnit(buffer);
    target->setForce(force);
    
    this->force = force;
    this->target = target;
}

AbilityTargetData::~AbilityTargetData() {
    CSObject::release(name);
    if (force) {
        static_cast<const VirtualForce*>(force)->release();
    }
    static_cast<const VirtualUnit*>(target)->release();
}

AbilityData::AbilityData(CSBuffer* buffer) :
    display(buffer->readBoolean() ? new AbilityDisplayData(buffer) : NULL),
    trigger(CSObject::retain(buffer->readArray<byte, 2>()))
{

}

AbilityData::~AbilityData() {
    if (display) delete display;
    CSObject::release(trigger);
}

fixed AbilityData::value(const UnitBase *unit, const Rune* rune, fixed value, const UnitBase *target) const {
    return trigger ? abilityValue(trigger, unit, rune, value, target) : value;
}

//=================================================================================
static void readAbilityTargetRef(CSBuffer* buffer, const AbilityTargetData*& v) {
    v = &Asset::sharedAsset()->abilityTargets->objectAtIndex(buffer->readShort());
}

AbilityUnitData::Entry::Entry(CSBuffer* buffer) :
    trigger(CSObject::retain(buffer->readArray<byte, 2>())),
    targets(CSObject::retain(buffer->readArrayFunc<const AbilityTargetData*>(&readAbilityTargetRef))),
    min(buffer->readFixed()),
    max(buffer->readFixed())
{
    
}

AbilityUnitData::Entry::~Entry() {
    CSObject::release(trigger);
    CSObject::release(targets);
}

AbilityUnitData::AbilityUnitData(CSBuffer* buffer) {
    int triggerCount = buffer->readLength();
    
    if (triggerCount) {
        CSDictionary<ushort, Entry>* entries = new CSDictionary<ushort, Entry>(triggerCount);
        for (int i = 0; i < triggerCount; i++) {
            int index = buffer->readShort();
            
            new (&entries->setObject(index)) Entry(buffer);
        }
        this->entries = entries;
    }
    else {
        this->entries = NULL;
    }
    descriptions = CSObject::retain(buffer->readArray<AbilityDescriptionData>());
}

AbilityUnitData::~AbilityUnitData() {    
    CSObject::release(entries);
    CSObject::release(descriptions);
}

fixed AbilityUnitData::value(int index, const UnitBase *unit, const Rune* rune, fixed value, const UnitBase *target) const {
    const Entry* entry;
    if (entries && (entry = entries->tryGetObjectForKey(index)) && entry->trigger) {
        fixed newValue = abilityValue(entry->trigger, unit, rune, value, target);
        
        if (entry->min && newValue < CSMath::min(entry->min, value)) newValue = entry->min;
        if (entry->max && newValue > CSMath::max(entry->max, value)) newValue = entry->max;
        value = newValue;
    }
    return value;
}
const CSArray<const AbilityTargetData*>* AbilityUnitData::targets(int index) const {
    const Entry* entry;
    if (entries && (entry = entries->tryGetObjectForKey(index))) {
        return entry->targets;
    }
    else {
        return NULL;
    }
}
//=================================================================================
AbilityDescriptionData::AbilityDescriptionData(CSBuffer* buffer) :
    name(CSObject::retain(buffer->readLocaleString())),
    str(CSObject::retain(buffer->readLocaleString())),
    trigger(CSObject::retain(buffer->readArray<byte, 2>()))
{
    
}
AbilityDescriptionData::~AbilityDescriptionData() {
    name->release();
    str->release();
    CSObject::release(trigger);
}

static const CSString* adValue(const byte*& cursor) {
    fixed v = readFixed(cursor);
    AbilityDisplay display = (AbilityDisplay)readByte(cursor);
    return abilityDisplayString(display, v);
}

static const CSString* adAbility(const byte*& cursor, bool usingValue, const UnitBase* unit, const Rune* rune, const SkinIndex& skinIndex, AbilityDescriptionType type) {
    int abt = readShort(cursor);
    const UnitBase* target = readBoolean(cursor) ? Asset::sharedAsset()->abilityTargets->objectAtIndex(readShort(cursor)).target : NULL;
    AbilityDisplay display = (AbilityDisplay)readByte(cursor);
    fixed v = usingValue ? readFixed(cursor) : abilityDisplayInitialValue(display);
    
    switch (type) {
        case AbilityDescriptionTypeUnit:
            v = unit->ability(abt, v, target);
            return abilityDisplayString(display, v);
        case AbilityDescriptionTypeUnitLevelup:
            {
                fixed v0 = unit->ability(abt, v, target);
				const_cast<UnitBase*>(unit)->level++;
				fixed v1 = unit->ability(abt, v, target);
				const_cast<UnitBase*>(unit)->level--;
				return abilityDisplayString(display, v0, v1);
            }
		case AbilityDescriptionTypeSkin:
			if (unit && unit->index() == skinIndex.group()) {
				int prevSkin = unit->skin;
				fixed v0 = unit->ability(abt, v, target);
				const_cast<UnitBase*>(unit)->skin = skinIndex.indices[2];
				fixed v1 = unit->ability(abt, v, target);
				const_cast<UnitBase*>(unit)->skin = prevSkin;
				return abilityDisplayString(display, v0, v1);
			}
			else {
				v = Asset::sharedAsset()->skin(skinIndex)->ability.value(abt, NULL, NULL, v, target);
				return abilityDisplayString(display, v);
			}
        case AbilityDescriptionTypeRune:
            v = rune->data().ability.value(abt, unit, rune, v, target);
            return abilityDisplayString(display, v);
        case AbilityDescriptionTypeRuneLevelup:
            {
                fixed v0 = rune->data().ability.value(abt, unit, rune, v, target);
				const_cast<Rune*>(rune)->level++;
                fixed v1 = rune->data().ability.value(abt, unit, rune, v, target);
				const_cast<Rune*>(rune)->level--;
				return abilityDisplayString(display, v0, v1);
            }
    }
    return NULL;
}

const CSString* AbilityDescriptionData::string(const UnitBase* unit, const Rune* rune, const SkinIndex& skinIndex, AbilityDescriptionType type) const {
    const CSString* str = this->str->value();
    if (!trigger) {
        return str;
    }
    if (!str) {
        return NULL;
    }
    CSString* rtn = CSString::string(*str);
    
    for (int i = 0; i < trigger->count(); i++) {
        const CSArray<byte>* line = trigger->objectAtIndex(i);
        
        const byte* cursor = line->pointer();
        
        int opCode = readByte(cursor);
        
        const CSString* e = NULL;
        
        switch (opCode) {
            case 0:
                e = adValue(cursor);
                break;
            case 1:
                e = adAbility(cursor, false, unit, rune, skinIndex, type);
                break;
            case 2:
                e = adAbility(cursor, true, unit, rune, skinIndex, type);
                break;
        }
        
        CSAssert(cursor == line->pointer() + line->count(), "invalid data");
        
        if (e) {
            rtn->replace(CSString::cstringWithFormat("$%d", i), CSString::cstringWithFormat("#color(ffc000ff)%s#color()", (const char*)*e));
        }
    }
    return rtn;
}

const CSString* AbilityDescriptionData::stringForUnit(const UnitBase* unit) const {
	return string(unit, NULL, SkinIndex::None, AbilityDescriptionTypeUnit);
}
const CSString* AbilityDescriptionData::stringForUnitLevelup(const UnitBase* unit) const {
	return string(unit, NULL, SkinIndex::None, AbilityDescriptionTypeUnitLevelup);
}
const CSString* AbilityDescriptionData::stringForSkin(const SkinIndex& skinIndex, const UnitBase* unit) const {
	return string(unit, NULL, skinIndex, AbilityDescriptionTypeSkin);
}
const CSString* AbilityDescriptionData::stringForRune(const Rune* rune, const UnitBase* unit) const {
	return string(unit, rune, SkinIndex::None, AbilityDescriptionTypeRune);
}
const CSString* AbilityDescriptionData::stringForRuneLevelup(const Rune* rune, const UnitBase* unit) const {
	return string(unit, rune, SkinIndex::None, AbilityDescriptionTypeRuneLevelup);
}

fixed abilityDisplayInitialValue(AbilityDisplay display) {
    switch (display) {
        case AbilityDisplayRateChange:
            return fixed::One;
    }
    return fixed::Zero;
}
const CSString* abilityDisplayString(AbilityDisplay display, fixed v) {
    switch (display) {
        case AbilityDisplayPoint:
            {
                float fv = v;
                int iv = CSMath::round(fv * 100);
                if (iv % 10) return CSString::stringWithFormat("%.2f", fv);
                if (iv % 100) return CSString::stringWithFormat("%.1f", fv);
                return CSString::stringWithFormat("%d", iv / 100);
            }
            break;
        case AbilityDisplayRate:
            {
                float fv = v * 100;
                int iv = CSMath::round(fv * 10);
                if (iv % 10) return CSString::stringWithFormat("%.1f%%", fv);
                return CSString::stringWithFormat("%d%%", iv / 10);
            }
        case AbilityDisplayRateChange:
            {
                float fv = (v - fixed::One) * 100;
                int iv = CSMath::round(fv * 10);
                if (iv % 10) return CSString::stringWithFormat("%.1f%%", fv);
                return CSString::stringWithFormat("%d%%", iv / 10);
            }
    }
    return CSString::stringWithFormat("%d", (int)CSMath::ceil(v));
}
const CSString* abilityDisplayString(AbilityDisplay display, fixed v0, fixed v1) {
    CSString* str = const_cast<CSString*>(abilityDisplayString(display, v0));
    
    if (v0 != v1) {
        if (display == AbilityDisplayRateChange) display = AbilityDisplayRate;
        if (v0 < v1) {
            str->append("#color(00ff00ff) + ");
            str->append(*abilityDisplayString(display, v1 - v0));
        }
        else {
            str->append("#color(ff0000ff) - ");
            str->append(*abilityDisplayString(display, v0 - v1));
        }
    }
    
    return str;
}

AbilityDescription::AbilityDescription(const ImageIndex& icon, const CSString* name, const CSString* value) : icon(icon), name(CSObject::retain(name)), value(CSObject::retain(value)) {
    
}
AbilityDescription::~AbilityDescription() {
    CSObject::release(name);
    CSObject::release(value);
}

//================================================================================================================================================
struct AbilityTriggerState {
    fixed values[4];
    const UnitBase* unit;
	const Rune* rune;
    const UnitBase* target;
    ushort ip;
    const byte* cursor;

	inline int level() const {
		if (rune) return rune->level;
		else if (unit) return unit->level;
		return 0;
	}
	inline const ForceBase* force() const {
		if (rune) return rune->force;
		else if (unit) return unit->force();
		return NULL;
	}
};

static void atValueOp(AbilityTriggerState& ats, fixed& value) {
    int valueOp = readByte(ats.cursor);
    switch (valueOp) {
        case 1:
            {
                fixed v = readFixed(ats.cursor);
                int op = readByte(ats.cursor);
				int level = ats.level();

                if (level > 1) {
                    int d = level - 1;
                    
                    switch (op) {
                        case 0:
                            value *= CSMath::pow(fixed::One + v, (fixed)d);
                            break;
                        case 1:
                            value *= fixed::One + v * d;
                            break;
                        case 2:
                            value += v * d;
                            break;
                    }
                }
            }
            break;
        case 2:
            {
                int index = readShort(ats.cursor);
				if (ats.unit) value = ats.unit->ability(index, value, ats.target);
				else if (ats.rune) ats.rune->ability(index, ats.unit, value, ats.target);
            }
            break;
        case 3:
            {
                int index = readShort(ats.cursor);
                if (ats.target) value = ats.target->ability(index, value, ats.unit);
            }
            break;
        case 4:
            {
                int index = readShort(ats.cursor);
                if (ats.unit) {
                    const UnitBase* parent = ats.unit->parent();
                    if (parent) value = parent->ability(index, value, ats.target);
                }
            }
            break;
        case 5:
            value = ats.unit ? value * ats.unit->childrenCount() : fixed::Zero;
            break;
        case 6:
            {
                BuffIndex index(ats.cursor);
                
                value = ats.unit ? value * ats.unit->buffStack(index) : fixed::Zero;
            }
            break;
        case 7:
            {
                BuffIndex index(ats.cursor);
                
                value = ats.target ? value * ats.target->buffStack(index) : fixed::Zero;
            }
            break;
    }
}

static void atSetValue(AbilityTriggerState& ats) {
    fixed& target = ats.values[readByte(ats.cursor)];
    
    fixed value;
    
    int sourceOp = readByte(ats.cursor);
    
    switch (sourceOp) {
        case 0:
        case 1:
        case 2:
        case 3:
            value = ats.values[sourceOp];
            break;
        case 4:
            value = readFixed(ats.cursor);
            break;
        case 5:
            {
                int count = readLength(ats.cursor);
                fixed* values = (fixed*)alloca(count * sizeof(fixed));
                for (int i = 0; i < count; i++) {
                    values[i] = readFixed(ats.cursor);
                }
                value = values[CSMath::clamp(ats.level() - 1, 0, count - 1)];
            }
            break;
        case 6:
            {
                int index = readShort(ats.cursor);
                
				if (ats.unit) value = ats.unit->ability(index, fixed::Zero, ats.target);
				else if (ats.rune) value = ats.rune->ability(index, ats.unit, fixed::Zero, ats.target);
				else value = fixed::Zero;
            }
            break;
        case 7:
            {
                int index = readShort(ats.cursor);
                
                value = ats.target ? ats.target->ability(index, fixed::Zero, ats.unit) : fixed::Zero;
            }
            break;
        case 8:
            {
                int index = readShort(ats.cursor);
                
                const UnitBase* parent;
                
                value = ats.unit && (parent = ats.unit->parent()) ? parent->ability(index, fixed::Zero, ats.target) : fixed::Zero;
            }
            break;
    }
    
    atValueOp(ats, value);
    
    int combineOp = readByte(ats.cursor);
    
    switch (combineOp) {
        case 0:
            target = value;
            break;
        case 1:
            target += value;
            break;
        case 2:
            target -= value;
            break;
        case 3:
            target *= value;
            break;
        case 4:
            if (value) {
                target /= value;
            }
            break;
        case 5:
            if (target > value) {
                target = value;
            }
            break;
        case 6:
            if (target < value) {
                target = value;
            }
            break;
    }
    ats.ip++;
}
static void atCompareValue(AbilityTriggerState& ats) {
    fixed target = ats.values[readByte(ats.cursor)];
    
    fixed value = readFixed(ats.cursor);
    
    atValueOp(ats, value);
    
    int compareOp = readByte(ats.cursor);
    
    bool flag = false;
    
    switch (compareOp) {
        case 0:
            flag = target== value;
            break;
        case 1:
            flag = target > value;
            break;
        case 2:
            flag = target >= value;
            break;
        case 3:
            flag = target < value;
            break;
        case 4:
            flag = target <= value;
            break;
        default:
            flag = false;
            break;
    }
    
    int ip1 = readShort(ats.cursor);
    
    ats.ip = flag ? ats.ip + 1 : ip1;
}
static void atCompare(AbilityTriggerState& ats) {
    const UnitBase* unit = readBoolean(ats.cursor) ? ats.target : ats.unit;
    
    fixed value;
    int sourceOp = readByte(ats.cursor);
    switch (sourceOp) {
        case 0:
        case 1:
        case 2:
        case 3:
            value = ats.values[sourceOp];
            break;
        case 4:
            value = readFixed(ats.cursor);
            break;
    }
    int compareOp = readByte(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    ats.ip = unit && unit->compare(compareOp, value) ? ats.ip + 1 : ip1;
}
static void atIsLevel(AbilityTriggerState& ats) {
    int level = readShort(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    ats.ip = ats.level() >= level ? ats.ip + 1 : ip1;
}
static void atHasUnit(AbilityTriggerState& ats) {
    UnitIndex index(ats.cursor);
    bool alive = readBoolean(ats.cursor);
    int unitCount = readByte(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    const ForceBase* force = ats.force();
    
    ats.ip = force && force->unitCount(index, alive) >= unitCount ? ats.ip + 1 : ip1;
}
static void atHasUnits(AbilityTriggerState& ats) {
    int count = readLength(ats.cursor);
    UnitIndex* indices = (UnitIndex*)alloca(count * sizeof(UnitIndex));
    for (int i = 0; i < count; i++) {
        indices[i] = UnitIndex(ats.cursor);
    }
    bool alive = readBoolean(ats.cursor);
    int unitCount = readByte(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    bool flag = false;

	const ForceBase* force = ats.force();

    if (force) {
        for (int i = 0; i < count; i++) {
            unitCount -= force->unitCount(indices[i], alive);
        }
        if (unitCount <= 0) {
            flag = true;
        }
    }
    ats.ip = flag ? ats.ip + 1 : ip1;
}
static void atHasRune(AbilityTriggerState& ats) {
    RuneIndex index(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    const ForceBase* force = ats.force();
    
    ats.ip = force && force->runeForIndex(index) && force->isTrained(index) ? ats.ip + 1 : ip1;
}
static void atIsTarget(AbilityTriggerState& ats) {
    const byte* condition = ats.cursor;
    ats.cursor += UnitBase::matchLength(condition);
    int ip1 = readShort(ats.cursor);
    ats.ip = ats.target && ats.target->match(ats.unit, ats.force(), condition) ? ats.ip + 1 : ip1;
}
static void atIsState(AbilityTriggerState& ats) {
    const UnitBase* unit = readBoolean(ats.cursor) ? ats.target : ats.unit;
    int state = readByte(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    ats.ip = unit && unit->state() == state ? ats.ip + 1 : ip1;
}
static void atIsBuffed(AbilityTriggerState& ats) {
    const UnitBase* unit = readBoolean(ats.cursor) ? ats.target : ats.unit;
    BuffIndex index(ats.cursor);
    fixed stack = readFixed(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    ats.ip = unit && unit->isBuffed(index, stack) ? ats.ip + 1 : ip1;
}
static void atIsAmbush(AbilityTriggerState& ats) {
    int ip1 = readShort(ats.cursor);

	bool ambush = ats.target && ats.target->force() && ats.unit && !ats.unit->isVisible(ats.target->force()->alliance());
    
    ats.ip = ambush ? ats.ip + 1 : ip1;
}
static void atChildrenCount(AbilityTriggerState& ats) {
    int count = readByte(ats.cursor);
    int ip1 = readShort(ats.cursor);
    
    ats.ip = ats.unit && ats.unit->childrenCount() >= count ? ats.ip + 1 : ip1;
}
static void atDamage(AbilityTriggerState& ats) {
    int targetOp = readByte(ats.cursor);
    
    int attackType = ats.unit ? ats.unit->data().attackType : -1;
    if (attackType >= 0) {
        int armorType = ats.target ? ats.target->data().armorType : -1;
        if (armorType >= 0) {
            ats.values[targetOp] *= Asset::sharedAsset()->damage(attackType, armorType);
        }
    }
    ats.ip++;
}
static void atStop(AbilityTriggerState& ats) {
    int ip1 = readShort(ats.cursor);
    
    ats.ip = ip1;
}

enum AbilityTriggerOp {
    AT_SetValue,
    AT_CompareValue,
    AT_Compare,
    AT_IsLevel,
    AT_HasUnit,
    AT_HasUnits,
    AT_HasRune,
    AT_IsTarget,
    AT_IsState,
    AT_IsBuffed,
	AT_IsAmbush,
	AT_ChildrenCount,
    AT_Damage,
    AT_Stop
};

static fixed abilityValue(const CSArray<byte, 2>* trigger, const UnitBase *unit, const Rune* rune, fixed value, const UnitBase *target) {
    AbilityTriggerState ats;
    ats.values[0] = value;
    ats.values[1] = ats.values[2] = ats.values[3] = fixed::Zero;
    ats.unit = unit;
	ats.rune = rune;
	ats.target = target;
    ats.ip = 0;
    
    do {
        const CSArray<byte, 1>* line = trigger->objectAtIndex(ats.ip);
        
        ats.cursor = line->pointer();
        
        AbilityTriggerOp opCode = (AbilityTriggerOp)readByte(ats.cursor);
        
        switch(opCode) {
            case AT_SetValue:
                atSetValue(ats);
                break;
            case AT_CompareValue:
                atCompareValue(ats);
                break;
            case AT_Compare:
                atCompare(ats);
                break;
            case AT_IsLevel:
                atIsLevel(ats);
                break;
            case AT_HasUnit:
                atHasUnit(ats);
                break;
            case AT_HasUnits:
                atHasUnits(ats);
                break;
            case AT_HasRune:
                atHasRune(ats);
                break;
            case AT_IsTarget:
                atIsTarget(ats);
                break;
            case AT_IsState:
                atIsState(ats);
                break;
            case AT_IsBuffed:
                atIsBuffed(ats);
                break;
            case AT_IsAmbush:
                atIsAmbush(ats);
                break;
            case AT_ChildrenCount:
                atChildrenCount(ats);
                break;
            case AT_Damage:
                atDamage(ats);
                break;
            case AT_Stop:
                atStop(ats);
                break;
        }
#ifdef CS_ASSERT_DEBUG
        int pos = (uint64)ats.cursor - (uint64)line->pointer();
        int len = line->count();
        CSAssert(pos == len, "ability trigger error:%d,%d,%d", opCode, pos, len);
#endif
    } while(ats.ip < trigger->count());
    
    value = CSMath::max(ats.values[0], fixed::Zero);
    
    return value;
}
