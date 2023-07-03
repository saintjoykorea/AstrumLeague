//
//  VirtualUnit.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 5..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef VirtualUnit_h
#define VirtualUnit_h

#include "UnitBase.h"

class VirtualUnit : public CSObject, public UnitBase {
private:
    const ForceBase* _force;
    const UnitBase* _parent;
    fixed _healthRate;
    fixed _manaRate;
    byte _state;
    Building _building;
    bool _visible;
    bool _cloaking;
    bool _producing;
    bool _training;
    bool _transporting;
    bool _reachable;
    byte _childrenCount;
    struct Buff {
        BuffIndex index;
        fixed stack;
    };
    CSArray<Buff>* _buffs;
public:
    VirtualUnit(const ForceBase* force, const UnitIndex& index, int level, int skin);
    VirtualUnit(const UnitBase* other);
    VirtualUnit(CSBuffer* buffer);
private:
    ~VirtualUnit();
public:
    static inline VirtualUnit* unit(const ForceBase* force, const UnitIndex& index, int level, int skin) {
        return autorelease(new VirtualUnit(force, index, level, skin));
    }
    static inline VirtualUnit* unitWithUnit(const UnitBase* other) {
        return autorelease(new VirtualUnit(other));
    }
    static inline VirtualUnit* unitWithBuffer(CSBuffer* buffer) {
        return autorelease(new VirtualUnit(buffer));
    }
    
    inline void setIndex(const UnitIndex& index) {
        _index = index;
    }
    inline void setForce(const ForceBase* force) {
        _force = force;
    }
    inline void setParent(const UnitBase* parent) {
        _parent = parent;
    }
    inline void setHealthRate(fixed healthRate) {
        _healthRate = healthRate;
    }
    inline void setManaRate(fixed manaRate) {
        _manaRate = manaRate;
    }
    inline void setState(int state) {
        _state = state;
    }
    inline void setBuilding(Building building) {
        _building = building;
    }
    inline void setVisible(bool visible) {
        _visible = visible;
    }
    inline void setCloaking(bool cloaking) {
        _cloaking = cloaking;
    }
    inline void setProducing(bool producing) {
        _producing = producing;
    }
    inline void setTraining(bool training) {
        _training = training;
    }
    inline void setTransporting(bool transporting) {
        _transporting = transporting;
    }
    inline void setChildrenCount(bool childrenCount) {
        _childrenCount = childrenCount;
    }
    
    void setBuff(const BuffIndex& index, fixed stack);
    void removeBuff(const BuffIndex& index);
    
    inline const ForceBase* force() const override {
        return _force;
    }
    inline const UnitBase* parent() const override {
        return _parent;
    }
    inline fixed health() const override {
        return maxHealth() * _healthRate;
    }
    inline fixed mana() const override {
        return maxMana() * _manaRate;
    }
    inline int state() const override {
        return _state;
    }
    inline Building building() const override {
        return _building;
    }
    inline bool isVisible(int alliance) const override {
        return _visible;
    }
    inline bool isCloaking() const override {
        return _cloaking;
    }
    inline bool isProducing() const override {
        return _producing;
    }
    inline bool isTraining() const override {
        return _training;
    }
    inline bool isTransporting() const override {
        return _transporting;
    }
    inline bool isReachable(const FPoint& point) const override {
        return _reachable;
    }
    inline int childrenCount() const override {
        return _childrenCount;
    }
    fixed buffStack(const BuffIndex& index) const override;
    bool isBuffed(const BuffIndex& index, fixed stack) const override;
protected:
    const CSArray<BuffIndex>* buffs() const override;

    fixed abilityElement(int index, fixed value, const UnitBase* target) const override;
};

#endif /* VirtualUnit_h */
