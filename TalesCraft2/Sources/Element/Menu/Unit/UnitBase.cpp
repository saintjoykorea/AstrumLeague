//
//  UnitBase.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 1. 9..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "UnitBase.h"

#include "ForceBase.h"

#include "Object.h"

#include "VirtualUnit.h"
#include "Unit.h"

UnitBase::UnitBase(const UnitIndex& index, int level, int skin) : _index(index), level(level), skin(skin) {
    
}

UnitBase::UnitBase(CSBuffer* buffer) : _index(buffer), level(buffer->readShort()), skin(buffer->readShort()) {

}

UnitBase::UnitBase(const CSJSONObject* json) : _index(json->numberForKey("cardId")), level(json->numberForKey("level")), skin(json->numberForKey("skin")) {

}

UnitBase::~UnitBase() {

}

bool UnitBase::compare(int op, fixed value) const {
    switch (op) {
        case 1:
            value = maxHealth() * value;
        case 0:
            return health() > value;
        case 3:
            value = maxHealth() * value;
        case 2:
            return health() < value;
        case 5:
            value = maxMana() * value;
        case 4:
            return mana() >= value;
        case 7:
            value = maxMana() * value;
        case 6:
            return mana() < value;
        case 8:
            return force() && force()->resource(0) >= value;
        case 9:
            return force() && force()->resource(1) >= value;
    }
    return false;
}

int UnitBase::matchLength(const byte *condition) {
    int len = 0;
    len++;
    len++;
    int unitCount = condition[len++];
    len += unitCount * (4 + 1);
    len += 4;
    len += 4;
    len += 4;
    if (condition[len++]) {
        len += 1 + 8;
    }
    len++;
    len++;
    len++;
    len += 4;
    int buffCount = condition[len++];
    len += buffCount * (4 + 8 + 1);
    return len;
}

bool UnitBase::match(const UnitBase* unit, const ForceBase* force, const byte* condition) const {
    if (readBoolean(condition) && unit == this) {
        return false;
    }
    switch (readByte(condition)) {
        case 0:
            if (!isDead()) {
                return false;
            }
            break;
        case 1:
            if (!isAlive()) {
                return false;
            }
            break;
    }

    int unitCount = readByte(condition);
    
    if (unitCount) {
        bool flag0 = true;
        bool flag1 = false;
        
        for (int i = 0; i < unitCount; i++) {
            UnitIndex index(condition);
            bool equals = readBoolean(condition);
            
            if (equals) {
                flag0 = false;
                if (_index == index) {
                    flag1 = true;
                }
            }
            else {
                if (_index == index) {
                    return false;
                }
            }
        }
        if (!flag0 && !flag1) {
            return false;
        }
    }
    
    uint typeMask = readInt(condition);
    if (typeMask && !Object::compareType(type(), typeMask)) {
        return false;
    }

    uint attributeIncludeMask = readInt(condition);
	uint attributeExcludeMask = readInt(condition);

	if (attributeIncludeMask || attributeExcludeMask) {
		const UnitData& data = this->data();
		uint attribute = data.attribute;
		if (data.attackType >= 0) attribute |= (1 << data.attackType) << 8;
		if (data.armorType >= 0) attribute |= (1 << data.armorType) << 16;
		if (attributeIncludeMask && (attribute & attributeIncludeMask) == 0) {
			return false;
		}
		if (attributeExcludeMask && (attribute & attributeExcludeMask) != 0) {
			return false;
		}
	}

	if (readBoolean(condition)) {
        int op = readByte(condition);
        fixed value = readFixed(condition);
        if (!compare(op, value)) {
            return false;
        }
    }
    
    int allianceMask = readByte(condition);
    
    if (allianceMask) {
        //적포함/제외
        //아군포함/제외
        //플레이어포함/제외
        //중립포함/제외
        if (!(allianceMask & 1)) {
            if (!this->force() || !force || this->force()->alliance() != force->alliance()) {
                return false;
            }
        }
        if (!(allianceMask & 2)) {
            if (!this->force() || !force || (this->force() != force && this->force()->alliance() == force->alliance())) {
                return false;
            }
        }
        if (!(allianceMask & 4)) {
            if (this->force() == force) {
                return false;
            }
        }
        if (!(allianceMask & 8)) {
            if (!this->force()) {
                return false;
            }
        }
    }
    
    int stateMask = readByte(condition);
    
    if (stateMask &&  (1 << state() & stateMask) == 0) {
        return false;
    }
    
    int buildingMask = readByte(condition);
    
    if (buildingMask && (1 << building() & buildingMask) == 0) {
        return false;
    }
    
    uint etcMask = readInt(condition);
    
    if (etcMask) {
        if ((etcMask & 3) && (!force || isVisible(force->alliance())) != ((etcMask & 1) == 1)) {
            return false;
        }
        etcMask >>= 2;
        
        if ((etcMask & 3) && isCloaking() != ((etcMask & 1) == 1)) {
            return false;
        }
        etcMask >>= 2;
        
        if ((etcMask & 3) && isProducing() != ((etcMask & 1) == 1)) {
            return false;
        }
        etcMask >>= 2;
        
        if ((etcMask & 3) && isTraining() != ((etcMask & 1) == 1)) {
            return false;
        }
        etcMask >>= 2;
        
        if ((etcMask & 3) && isTransporting() != ((etcMask & 1) == 1)) {
            return false;
        }
        etcMask >>= 2;
        
        if ((etcMask & 3) && (unit || force)) {
            const FPoint& point = unit ? unit->reachablePoint() : force->reachablePoint();
            
            if (isReachable(point) != ((etcMask & 1) == 1)) {
                return false;
            }
        }
        etcMask >>= 2;

		if ((etcMask & 4)) {
			switch (etcMask & 3) {
				case 0:
					if (isGathering()) return false;
					break;
				case 1:
					if (!isGathering(0)) return false;
					break;
				case 2:
					if (!isGathering(1)) return false;
					break;
				case 3:
					if (!isGathering()) return false;
					break;
			}
		}
		etcMask >>= 3;
    }
    
    int buffCount = readByte(condition);
    
    if (buffCount) {
        bool flag0 = true;
        bool flag1 = false;
        
        for (int i = 0; i < buffCount; i++) {
            BuffIndex buff(condition);
            fixed stack = readFixed(condition);
            
            if (readBoolean(condition)) {
                flag0 = false;
                if (isBuffed(buff, stack)) {
                    flag1 = true;
                }
            }
            else {
                if (isBuffed(buff, stack)) {
                    return false;
                }
            }
        }
        
        if (!flag0 && !flag1) {
            return false;
        }
    }
    
    return true;
}

fixed UnitBase::abilityElement(int index, fixed value, const UnitBase* target) const {
    const UnitData& data = this->data();
    
    value = data.ability.value(index, this, NULL, value, target);
    
	const SkinData* skin = skinData();
    if (skin) {
        value = skin->ability.value(index, this, NULL, value, target);
    }
    
    const ForceBase* force = this->force();
    if (force) {
        foreach(const Rune*, rune, force->runes()) {
            if (force->isTrained(rune->index) && rune->match(this)) {
                value = rune->ability(index, this, value, target);
            }
        }        
    }
    return value;
}

fixed UnitBase::ability(int index, fixed value, const UnitBase* target) const {
    value = abilityElement(index, value, target);
    
    const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(index);
    
    value = data.value(this, NULL, value, target);
    
    return value;
}

fixed UnitBase::maxAbility(int index, fixed value) const {
    fixed max = ability(index, value);
    
    foreach(const AbilityTargetData*, subdata, abilityTargetDatas(index)) {
        fixed current = ability(index, value, subdata->target);
        if (current > max) {
            max = current;
        }
    }
    return max;
}

const CSArray<const AbilityTargetData*>* UnitBase::abilityTargetDatas(int index) const {
    CSArray<const AbilityTargetData*>* targets = new CSArray<const AbilityTargetData*>();

    const CSArray<const AbilityTargetData*>* currentTargets;
    
    currentTargets = data().ability.targets(index);
    if (currentTargets) {
        targets->addObjectsFromArray(currentTargets);
    }
	const SkinData* skin = skinData();
	if (skin) {
		currentTargets = skin->ability.targets(index);
		foreach(const AbilityTargetData*, target, currentTargets) {
			if (!targets->containsObjectIdenticalTo(target)) {
				targets->addObject(target);
			}
		}
	}
    foreach(const BuffIndex&, buff, buffs()) {
        currentTargets = Asset::sharedAsset()->buff(buff).ability.targets(index);
        foreach(const AbilityTargetData*, target, currentTargets) {
            if (!targets->containsObjectIdenticalTo(target)) {
                targets->addObject(target);
            }
        }
    }
    const ForceBase* force = this->force();
    
    if (force) {
        foreach(const Rune*, rune, force->runes()) {
            if (force->isTrained(rune->index) && rune->match(this)) {
                currentTargets = rune->data().ability.targets(index);
                
                foreach(const AbilityTargetData*, target, currentTargets) {
                    if (!targets->containsObjectIdenticalTo(target)) {
                        targets->addObject(target);
                    }
                }
            }
        }
    }
    if (targets->count()) {
        targets->autorelease();
        return targets;
    }
    else {
        targets->release();
        return NULL;
    }
}

const CSArray<AbilityDescription>* UnitBase::abilityValues() const {
    CSArray<AbilityDescription>* values = CSArray<AbilityDescription>::array();
    for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
        const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);
        
        if (data.display && (data.display->view & AbilityDisplayViewUnit)) {
            fixed initial = abilityDisplayInitialValue(data.display->display);
            
            fixed value = ability(i, initial, NULL);
            
            if (value != initial) {
                new (&values->addObject()) AbilityDescription(data.display->icon, data.display->name->value(), abilityDisplayString(data.display->display, value));
            }
            foreach(const AbilityTargetData*, subdata, abilityTargetDatas(i)) {
                fixed targetValue = ability(i, initial, subdata->target);

				if (targetValue != value) {
					const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*subdata->name, (const char*)*data.display->name);

					new (&values->addObject()) AbilityDescription(data.display->icon, name, abilityDisplayString(data.display->display, targetValue));
				}
            }
        }
    }
    return values;
}

const CSArray<AbilityDescription>* UnitBase::abilityLevelupValues(bool differenceOnly) const {
    CSArray<AbilityDescription>* values = CSArray<AbilityDescription>::array();
    for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
        const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);
        
        if (data.display && (data.display->view & AbilityDisplayViewUnit)) {
            fixed initial = abilityDisplayInitialValue(data.display->display);
            
			fixed value0 = ability(i, initial, NULL);
			const_cast<UnitBase*>(this)->level++;
			fixed value1 = ability(i, initial, NULL);
			const_cast<UnitBase*>(this)->level--;
            
            if ((value0 != initial || value1 != initial) && (!differenceOnly || value0 != value1)) {
                new (&values->addObject()) AbilityDescription(data.display->icon, data.display->name->value(), abilityDisplayString(data.display->display, value0, value1));
            }
            foreach(const AbilityTargetData*, subdata, abilityTargetDatas(i)) {
				fixed targetValue0 = ability(i, initial, subdata->target);
				const_cast<UnitBase*>(this)->level++;
				fixed targetValue1 = ability(i, initial, subdata->target);
				const_cast<UnitBase*>(this)->level--;
                
				if ((targetValue0 != value0 || targetValue1 != value1) && (!differenceOnly || targetValue0 != targetValue1)) {
                    const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*subdata->name, (const char*)*data.display->name);
                
                    new (&values->addObject()) AbilityDescription(data.display->icon, name, abilityDisplayString(data.display->display, targetValue0, targetValue1));
                }
            }
        }
    }
    return values;
}

const CSArray<AbilityDescription>* UnitBase::abilityDescriptions() const {
    CSArray<AbilityDescription>* descriptions = CSArray<AbilityDescription>::array();
    
    foreach(const AbilityDescriptionData&, subdata, data().ability.descriptions) {
        new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForUnit(this));
    }
    foreach(const BuffIndex&, buff, buffs()) {
        foreach(const AbilityDescriptionData&, subdata, Asset::sharedAsset()->buff(buff).ability.descriptions) {
            new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForUnit(this));
        }
    }
    const ForceBase* force = this->force();
    
    if (force) {
        foreach(const Rune*, rune, force->runes()) {
            if (force->isTrained(rune->index) && rune->match(this)) {
                foreach(const AbilityDescriptionData&, subdata, rune->data().ability.descriptions) {
                    new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForRune(rune, this));
                }
            }
        }
    }
    return descriptions;
}

const CSArray<AbilityDescription>* UnitBase::abilityLevelupDescriptions() const {
    CSArray<AbilityDescription>* descriptions = CSArray<AbilityDescription>::array();
    foreach(const AbilityDescriptionData&, subdata, data().ability.descriptions) {
        new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForUnitLevelup(this));
    }
    foreach(const BuffIndex&, buff, buffs()) {
        foreach(const AbilityDescriptionData&, subdata, Asset::sharedAsset()->buff(buff).ability.descriptions) {
            new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForUnitLevelup(this));
        }
    }
    const ForceBase* force = this->force();
    
    if (force) {
        foreach(const Rune*, rune, force->runes()) {
            if (force->isTrained(rune->index) && rune->match(this)) {
                foreach(const AbilityDescriptionData&, subdata, rune->data().ability.descriptions) {
                    new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForRune(rune, this));
                }
            }
        }
    }
    return descriptions;
}

const CSArray<AbilityDescription>* UnitBase::abilitySkinValues(int skin, bool compare, bool differenceOnly) const {
	CSArray<AbilityDescription>* values = CSArray<AbilityDescription>::array();

	const SkinData* skinData = this->data().skin(skin);

	if (skinData) {
		if (compare) {
			for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
				const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);

				if (data.display && (data.display->view & AbilityDisplayViewUnit)) {
					fixed initial = abilityDisplayInitialValue(data.display->display);

					int prevSkin = this->skin;
					fixed value0 = ability(i, initial, NULL);
					const_cast<UnitBase*>(this)->skin = skin;
					fixed value1 = ability(i, initial, NULL);
					const_cast<UnitBase*>(this)->skin = prevSkin;

					if ((value0 != initial || value1 != initial) && (!differenceOnly || value0 != value1)) {
						new (&values->addObject()) AbilityDescription(data.display->icon, data.display->name->value(), abilityDisplayString(data.display->display, value0, value1));
					}
					foreach(const AbilityTargetData*, subdata, abilityTargetDatas(i)) {
						fixed targetValue0 = ability(i, initial, subdata->target);
						const_cast<UnitBase*>(this)->skin = skin;
						fixed targetValue1 = ability(i, initial, subdata->target);
						const_cast<UnitBase*>(this)->skin = prevSkin;

						if ((targetValue0 != value0 || targetValue1 != value1) && (!differenceOnly || targetValue0 != targetValue1)) {
							const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*subdata->name, (const char*)*data.display->name);

							new (&values->addObject()) AbilityDescription(data.display->icon, name, abilityDisplayString(data.display->display, targetValue0, targetValue1));
						}
					}
				}
			}
		}
		else {
			for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
				const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);

				if (data.display && (data.display->view & AbilityDisplayViewUnit)) {
					fixed initial = abilityDisplayInitialValue(data.display->display);

					fixed value = skinData->ability.value(i, this, NULL, initial, NULL);

					if (value != initial) {
						new (&values->addObject()) AbilityDescription(data.display->icon, data.display->name->value(), abilityDisplayString(data.display->display, value));
					}
					foreach(const AbilityTargetData*, subdata, skinData->ability.targets(i)) {
						fixed targetValue = skinData->ability.value(i, this, NULL, initial, subdata->target);

						if (targetValue != value) {
							const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*subdata->name, (const char*)*data.display->name);

							new (&values->addObject()) AbilityDescription(data.display->icon, name, abilityDisplayString(data.display->display, targetValue));
						}
					}
				}
			}
		}
	}

	return values;
}

const CSArray<AbilityDescription>* UnitBase::abilitySkinDescriptions(int skin, bool compare, bool differenceOnly) const {
    CSArray<AbilityDescription>* descriptions = CSArray<AbilityDescription>::array();
	
	if (!differenceOnly) {
		foreach(const AbilityDescription&, other, abilityDescriptions()) {
			new (&descriptions->addObject()) AbilityDescription(other.icon, other.name, other.value);
		}
	}
	const SkinData* data = this->data().skin(skin);
	if (data) {
		foreach(const AbilityDescriptionData&, subdata, data->ability.descriptions) {
			new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForSkin(SkinIndex(_index, skin), compare ? this : NULL));
		}
	}
    return descriptions;
}





