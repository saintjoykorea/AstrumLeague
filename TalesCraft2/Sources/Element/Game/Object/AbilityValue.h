//
//  AbilityValue.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 5..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef AbilityValue_h
#define AbilityValue_h

#include "UnitBase.h"

#define AbilityNone     -1

template <typename T>
struct AbilityValue {
private:
    T _value;
    short _ability;
public:
    inline AbilityValue() {
        
    }
    inline AbilityValue(T value, int ability) : _value(value), _ability(ability) {
        
    }
    inline T value(const UnitBase* unit, const UnitBase* target = NULL) const {
        return _ability == AbilityNone ? _value : (T)unit->ability(_ability, (fixed)_value, target);
    }
    inline T maxValue(const UnitBase* unit) const {
        return _ability == AbilityNone ? _value : (T)unit->maxAbility(_ability, (fixed)_value);
    }
    inline bool operator==(const AbilityValue<T>& other) const {
        return _value == other._value && _ability == other._ability;
    }
    inline bool operator!=(const AbilityValue<T>& other) const {
        return _value != other._value || _ability != other._ability;
    }
};

#endif /* AbilityValue_h */
