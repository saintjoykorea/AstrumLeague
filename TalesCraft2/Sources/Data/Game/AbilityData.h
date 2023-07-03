//
//  AbilityData.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 26..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef AbilityData_h
#define AbilityData_h

#include "AssetIndex.h"

enum Ability {
    AbilityHealth,
    AbilityShield,
    AbilitySight,
    AbilityAttack,
    AbilityAttackBlind,
    AbilityAttackRange,
    AbilityAttackSpeed,
    AbilityMoveSpeed,
    AbilityTransportingCapacity,
	AbilityTransportingRange,
    AbilityWorkingSpeed,
    AbilityManaInitial,
    AbilityMana,
    AbilityManaRecovery,
	AbilityScale,
    AbilityCount
};

class ForceBase;
class UnitBase;
class Rune;

struct AbilityTargetData {
    const CSLocaleString* name;
    const ForceBase* force;
    const UnitBase* target;
    
    AbilityTargetData(CSBuffer* buffer);
    ~AbilityTargetData();
    
    inline AbilityTargetData(const AbilityTargetData&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityTargetData& operator=(const AbilityTargetData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

enum AbilityDisplay {
    AbilityDisplayValue,
    AbilityDisplayPoint,
    AbilityDisplayRate,
    AbilityDisplayRateChange
};

enum AbilityDisplayView {
	AbilityDisplayViewUnit = 1,
	AbilityDisplayViewRune
};

struct AbilityDisplayData {
    ImageIndex icon;
    const CSLocaleString* name;
    AbilityDisplay display;
	int view;
    
    AbilityDisplayData(CSBuffer* buffer);
    ~AbilityDisplayData();
    
    inline AbilityDisplayData(const AbilityDisplayData&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityDisplayData& operator=(const AbilityDisplayData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

struct AbilityData {
    const AbilityDisplayData* display;
    const CSArray<byte, 2>* trigger;
    
    AbilityData(CSBuffer* buffer);    
    ~AbilityData();
    
    inline AbilityData(const AbilityData&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityData& operator=(const AbilityData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    
    fixed value(const UnitBase* unit, const Rune* rune, fixed value, const UnitBase* target) const;
};

struct AbilityDescriptionData;

struct AbilityUnitData {
private:
    struct Entry {
        const CSArray<byte, 2>* trigger;
        const CSArray<const AbilityTargetData*>* targets;
        fixed min;
        fixed max;
        
        Entry(CSBuffer* buffer);
        ~Entry();
        
        inline Entry(const Entry&) {
            CSAssert(false, "invalid operation");
        }
        inline Entry& operator=(const Entry&) {
            CSAssert(false, "invalid operation");
            return *this;
        }
    };
    const CSDictionary<ushort, Entry>* entries;
public:
    const CSArray<AbilityDescriptionData>* descriptions;

    AbilityUnitData(CSBuffer* buffer);
    ~AbilityUnitData();
    
    inline AbilityUnitData(const AbilityUnitData&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityUnitData& operator=(const AbilityUnitData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    fixed value(int index, const UnitBase* unit, const Rune* rune, fixed value, const UnitBase* target) const;
    const CSArray<const AbilityTargetData*>* targets(int index) const;
};

enum AbilityDescriptionType {
    AbilityDescriptionTypeUnit,
    AbilityDescriptionTypeUnitLevelup,
    AbilityDescriptionTypeSkin,
    AbilityDescriptionTypeRune,
    AbilityDescriptionTypeRuneLevelup
};
struct AbilityDescriptionData {
    const CSLocaleString* name;
    const CSLocaleString* str;
    const CSArray<byte, 2>* trigger;
    
    AbilityDescriptionData(CSBuffer* buffer);
    ~AbilityDescriptionData();
    
    inline AbilityDescriptionData(const AbilityDescriptionData&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityDescriptionData& operator=(const AbilityDescriptionData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
private:
    const CSString* string(const UnitBase* unit, const Rune* rune, const SkinIndex& skinIndex, AbilityDescriptionType type) const;
public:
	const CSString* stringForUnit(const UnitBase* unit) const;
	const CSString* stringForUnitLevelup(const UnitBase* unit) const;
	const CSString* stringForSkin(const SkinIndex& skinIndex, const UnitBase* unit = NULL) const;
	const CSString* stringForRune(const Rune* rune, const UnitBase* unit = NULL) const;
	const CSString* stringForRuneLevelup(const Rune* rune, const UnitBase* unit = NULL) const;
};

struct AbilityDescription {
    const ImageIndex icon;
    const CSString* name;
    const CSString* value;
    
    AbilityDescription(const ImageIndex& icon, const CSString* name, const CSString* value);
    ~AbilityDescription();
    
    inline AbilityDescription(const AbilityDescription&) {
        CSAssert(false, "invalid operation");
    }
    inline AbilityDescription& operator=(const AbilityDescription&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

fixed abilityDisplayInitialValue(AbilityDisplay display);
const CSString* abilityDisplayString(AbilityDisplay display, fixed v);
const CSString* abilityDisplayString(AbilityDisplay display, fixed v0, fixed v1);

#endif /* AbilityData_h */
