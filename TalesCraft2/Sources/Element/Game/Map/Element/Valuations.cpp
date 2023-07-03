//
//  Valuations.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 4. 16..
//  Copyright © 2019년 brgames. All rights reserved.
//
#include "Valuations.h"

#include "VirtualUnit.h"

Valuations::ValuationKey::ValuationKey(const UnitIndex& index0, int level0, const UnitIndex& index1, int level1) : index0(index0), level0(level0), index1(index1), level1(level1) {
    
}

Valuations::ValuationKey::operator uint() const {
    uint v0 = (level0 << 12) | (index0.indices[0] << 8) | index0.indices[1];
    uint v1 = (level1 << 12) | (index1.indices[0] << 8) | index1.indices[1];
    
    return (v0 << 16) | v1;
}

bool Valuations::ValuationKey::operator ==(const ValuationKey& other) const {
    //return index0 == other.index0 && index1 == other.index1 && level0 == other.level0 && level1 == other.level1;
    
    return memcmp(this, &other, sizeof(ValuationKey)) == 0;
}

Valuations::Valuations() {
    static const uint capacity = (16 * 6) * (16 * 3);       //(기본유닛(8) + 덱유닛(8)) * 플레이어수(6) * (기본유닛(8) + 덱유닛(8)) * 상대플레이어수(3)
    
    _valuations = new CSDictionary<ValuationKey, fixed>(capacity);
}
    
Valuations::~Valuations() {
    _valuations->release();
}

fixed Valuations::valuation(const UnitIndex& index0, int level0, const UnitIndex& index1, int level1) {
    ValuationKey key(index0, level0, index1, level1);
    
    const fixed* pv = _valuations->tryGetObjectForKey(key);
    
    if (pv) return *pv;
    
    VirtualUnit* target = VirtualUnit::unit(NULL, index1, level1, 0);

    fixed v = fixed::Zero;

    fixed maxHealth = target->maxHealth();
    
    if (maxHealth) {
        VirtualUnit* src = VirtualUnit::unit(NULL, index0, level0, 0);       //force, buff, abilities, skin의 valuation을 계산하지 않는다.

        fixed damage = src->ability(AbilityAttack, fixed::Zero, target);
        if (damage) {
            fixed speed = src->ability(AbilityAttackSpeed, fixed::Zero, target);
            if (speed) {
                damage *= speed;
                
                if (target->ability(AbilityAttack, fixed::Zero, src)) {
                    fixed range = src->ability(AbilityAttackRange, fixed::Zero, target) - target->ability(AbilityAttackRange, fixed::Zero, src);
                    
                    //사거리차에 의해 서로 데미지를 입히기 시작하는 시점보다 우선데미지를 미리 때려놓고 시작한다고 가정
                    
                    if (range > fixed::Zero) {
                        fixed moveSpeed = target->ability(AbilityMoveSpeed);
                        if (!moveSpeed) {
                            v = fixed::One;
                            goto rtn;
                        }
                        maxHealth -= damage * range / moveSpeed;
                        if (maxHealth <= fixed::Zero) {
                            v = fixed::One;
                            goto rtn;
                        }
                    }
                }
                v = CSMath::min(damage / maxHealth, fixed::One);
            }
        }
    }
rtn:
    CSLog("valuation:(%d,%d,%d) -> (%d,%d,%d) = %.3f", index0.indices[0], index0.indices[1], level0, index1.indices[0], index1.indices[1], level1, (float)v);
    _valuations->setObject(key, v);
    return v;
}
