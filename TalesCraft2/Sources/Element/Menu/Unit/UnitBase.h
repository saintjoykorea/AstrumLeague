//
//  UnitBase.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 26..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef UnitBase_h
#define UnitBase_h

#include "Asset.h"

#include "FPoint.h"

class ForceBase;
class Unit;
class Spell;

enum Building : byte {
    BuildingNone,
    BuildingTargetReady,
    BuildingTargetProgress,
    BuildingTransform,
    BuildingIndependent,
    BuildingDependent,
    BuildingNeglact,
    BuildingReserve
};

class UnitBase {
protected:
    UnitIndex _index;
public:
    ushort level;
    ushort skin;
protected:
    UnitBase(const UnitIndex& index, int level, int skin);
	UnitBase(CSBuffer* buffer);
	UnitBase(const CSJSONObject* json);
public:
    virtual ~UnitBase();
    
    inline const UnitIndex& index() const {
        return _index;
    }
    inline virtual const UnitData& data() const {
        return Asset::sharedAsset()->unit(_index);
    }
    inline virtual const SkinData* skinData() const {
        return data().skin(skin);
    }
    inline bool isMaxLevel() const {
        return level >= Asset::sharedAsset()->gamePreference.maxCardLevel;
    }
    virtual const ForceBase* force() const = 0;

    inline virtual const UnitBase* parent() const {
        return NULL;
    }
    inline virtual ObjectType type() const {
        return data().type;
    }
    inline virtual fixed maxHealth() const {
        return ability(AbilityHealth) + ability(AbilityShield);
    }
    inline virtual fixed health() const {
        return maxHealth();
    }
    inline virtual fixed maxMana() const {
        return ability(AbilityMana);
    }
    inline virtual fixed mana() const {
        return ability(AbilityManaInitial);
    }
    inline virtual int state() const {
        return 0;
    }
    inline virtual Building building() const {
        return BuildingNone;
    }
    inline virtual bool isVisible(int allience) const {
        return true;
    }
    inline virtual bool isAlive() const {
        return health();
    }
    inline virtual bool isDead() const {
        return maxHealth() && !health();
    }
    inline virtual bool isCloaking() const {
        return false;
    }
    inline virtual bool isProducing() const {
        return false;
    }
    inline virtual bool isTraining() const {
        return false;
    }
    inline virtual bool isTransporting() const {
        return false;
    }
	inline virtual bool isGathering() const {
		return false;
	}
	inline virtual bool isGathering(int kind) const {
		return false;
	}
    inline virtual const FPoint& reachablePoint() const {
        return FPoint::Zero;
    }
    inline virtual bool isReachable(const FPoint& point) const {
        return false;
    }
    inline virtual int childrenCount() const {
        return 0;
    }
    inline virtual fixed buffStack(const BuffIndex& buff) const {
        return fixed::Zero;
    }
    inline virtual bool isBuffed(const BuffIndex& buff, fixed stack) const {
        return false;
    }
    inline virtual const CSArray<BuffIndex>* buffs() const {
        return NULL;
    }
    
    bool compare(int op, fixed value) const;

    bool match(const UnitBase* unit, const ForceBase* force, const byte* condition) const;
    
	inline bool match(const UnitBase* unit, const byte* condition) const {
		return match(unit, unit->force(), condition);
	}
	inline bool match(const ForceBase* force, const byte* condition) const {
		return match(NULL, force, condition);
	}

    static int matchLength(const byte* condition);

protected:
    virtual fixed abilityElement(int index, fixed value, const UnitBase* target) const;
public:
    fixed ability(int index, fixed value = fixed::Zero, const UnitBase* target = NULL) const;
    inline fixed ability(Ability abt, fixed value = fixed::Zero, const UnitBase* target = NULL) const {
        return ability(Asset::sharedAsset()->abilityRefs[abt], value, target);
    }
    fixed maxAbility(int index, fixed value = fixed::Zero) const;
    inline fixed maxAbility(Ability abt, fixed value = fixed::Zero) const {
        return maxAbility(Asset::sharedAsset()->abilityRefs[abt], value);
    }
private:
    const CSArray<const AbilityTargetData*>* abilityTargetDatas(int index) const;
public:
    const CSArray<AbilityDescription>* abilityValues() const;
    const CSArray<AbilityDescription>* abilityDescriptions() const;
    const CSArray<AbilityDescription>* abilityLevelupValues(bool differenceOnly) const;
    const CSArray<AbilityDescription>* abilityLevelupDescriptions() const;
    const CSArray<AbilityDescription>* abilitySkinValues(int skin, bool compare, bool differenceOnly) const;
    const CSArray<AbilityDescription>* abilitySkinDescriptions(int skin, bool compare, bool differenceOnly) const;
};

#endif /* UnitBase_h */
