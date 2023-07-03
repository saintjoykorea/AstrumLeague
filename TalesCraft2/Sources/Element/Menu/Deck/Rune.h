//
//  Rune.h
//  TalesCraft2
//
//  Created by ChangSun on 2017. 1. 18..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef Rune_h
#define Rune_h

#include "UnitBase.h"

class Rune : public CSObject {
public:
	const ForceBase* force;
	RuneIndex index;
    ushort level;

    Rune(const ForceBase* force, const RuneIndex& index, int level);
    Rune(CSBuffer* buffer, const ForceBase* force);
    Rune(const CSJSONObject* json, const ForceBase* force);
protected:
    virtual ~Rune() = default;
public:
    static inline Rune* rune(const ForceBase* force, const RuneIndex& index, int level) {
        return autorelease(new Rune(force, index, level));
    }
    static inline Rune* runeWithBuffer(CSBuffer* buffer, const ForceBase* force) {
        return autorelease(new Rune(buffer, force));
    }
    static inline Rune* runeWithJSON(const CSJSONObject* json, const ForceBase* force) {
        return autorelease(new Rune(json, force));
    }
    
    inline const RuneData& data() const {
        return Asset::sharedAsset()->rune(index);
    }

    void writeTo(CSBuffer* buffer) const;

	inline bool isMaxLevel() const {
		return level >= Asset::sharedAsset()->gamePreference.maxRuneLevel;
	}

	inline bool isEnabled() const {
		return level > 0;
	}
    
	inline bool match(const UnitBase* unit) const {
		return data().match(unit);
	}

    inline fixed ability(int index, const UnitBase* unit = NULL, fixed value = fixed::Zero, const UnitBase* target = NULL) const {
        return data().ability.value(index, unit, this, value, target);
    }
    inline fixed ability(Ability abt, const UnitBase* unit = NULL, fixed value = fixed::Zero, const UnitBase* target = NULL) const {
        return ability(Asset::sharedAsset()->abilityRefs[abt], unit, value, target);
    }

	const CSArray<AbilityDescription>* abilityValues(const UnitBase* unit = NULL) const;
	const CSArray<AbilityDescription>* abilityLevelupValues(bool differenceOnly, const UnitBase* unit = NULL) const;
    const CSArray<AbilityDescription>* abilityDescriptions(const UnitBase* unit = NULL) const;
    const CSArray<AbilityDescription>* abilityLevelupDescriptions(const UnitBase* unit = NULL) const;
};

#endif /* Rune_h */
