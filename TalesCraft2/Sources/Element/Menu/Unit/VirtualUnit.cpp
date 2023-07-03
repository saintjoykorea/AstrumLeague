//
//  VirtualUnit.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 5..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "VirtualUnit.h"

#include "ForceBase.h"

#include "Card.h"

VirtualUnit::VirtualUnit(const ForceBase* force, const UnitIndex& index, int level, int skin) :
    UnitBase(index, level, skin),
    _force(force)
{
    fixed maxMana = this->maxMana();
    _manaRate = maxMana ? ability(AbilityManaInitial) / maxMana : fixed::One;
}

VirtualUnit::VirtualUnit(const UnitBase* other) :
    UnitBase(other->index(), other->level, other->skin),
    _force(other->force()),
    _healthRate(other->maxHealth() ? other->health() / other->maxHealth() : fixed::One),
    _manaRate(other->maxMana() ? other->mana() / other->maxMana() : fixed::One),
    _state(other->state()),
    _building(other->building()),
    _cloaking(other->isCloaking()),
    _producing(other->isProducing()),
    _training(other->isTraining()),
    _transporting(other->isTransporting()),
    _childrenCount(other->childrenCount())
{
    const CSArray<BuffIndex>* buffs = other->buffs();
    if (buffs) {
        _buffs = new CSArray<Buff>();
        foreach(const BuffIndex&, buffIndex, other->buffs()) {
            Buff& buff = _buffs->addObject();
            buff.index = buffIndex;
            buff.stack = other->buffStack(buffIndex);
        }
    }
    else {
        _buffs = NULL;
    }
}

VirtualUnit::VirtualUnit(CSBuffer* buffer) :
    UnitBase(buffer),
    _force(NULL),
    _healthRate(buffer->readFixed()),
    _manaRate(buffer->readFixed()),
    _state(buffer->readByte()),
    _building((Building)buffer->readByte()),
    _visible(buffer->readBoolean()),
    _cloaking(buffer->readBoolean()),
    _producing(buffer->readBoolean()),
    _training(buffer->readBoolean()),
    _transporting(buffer->readBoolean()),
    _reachable(buffer->readBoolean()),
    _childrenCount(buffer->readByte())
{
    int buffCount = buffer->readLength();
    if (buffCount) {
        _buffs = new CSArray<Buff>(buffCount);
        for (int i = 0; i < buffCount; i++) {
            Buff& buff = _buffs->addObject();
            buff.index = BuffIndex(buffer);
            buff.stack = buffer->readInt();
        }
    }
    else {
        _buffs = NULL;
    }
}

VirtualUnit::~VirtualUnit() {
    CSObject::release(_buffs);
}

void VirtualUnit::setBuff(const BuffIndex& index, fixed stack) {
    if (!_buffs) {
        _buffs = new CSArray<Buff>();
        Buff& buff = _buffs->addObject();
        buff.index = index;
        buff.stack = stack;
    }
    else {
        int i = 0;
        while (i < _buffs->count()) {
            Buff& buff = _buffs->objectAtIndex(i);
            if (buff.index == index) {
                buff.stack = stack;
                return;
            }
            else if ((int)buff.index < (int)index) {
                break;
            }
            i++;
        }
        Buff& newbuff = _buffs->insertObject(i);
        newbuff.index = index;
        newbuff.stack = stack;
    }
}
void VirtualUnit::removeBuff(const BuffIndex &index) {
    if (_buffs) {
        for (int i = 0; i < _buffs->count(); i++) {
            if (_buffs->objectAtIndex(i).index == index) {
                _buffs->removeObjectAtIndex(i);
                break;
            }
        }
    }
}
fixed VirtualUnit::buffStack(const BuffIndex& index) const {
    foreach(Buff&, buff, _buffs) {
        if (buff.index == index) {
            return buff.stack;
        }
    }
    return fixed::Zero;
}
bool VirtualUnit::isBuffed(const BuffIndex& index, fixed stack) const {
    foreach(Buff&, buff, _buffs) {
        if (buff.index == index) {
            return buff.stack >= stack;
        }
    }
    return false;
}
fixed VirtualUnit::abilityElement(int index, fixed value, const UnitBase* target) const {
    value = UnitBase::abilityElement(index, value, target);
    
    foreach(const Buff&, buff, _buffs) {
        value = Asset::sharedAsset()->buff(buff.index).ability.value(index, this, NULL, value, target);
    }
    return value;
}
const CSArray<BuffIndex>* VirtualUnit::buffs() const {
    if (!_buffs) {
        return NULL;
    }
    CSArray<BuffIndex>* buffs = CSArray<BuffIndex>::arrayWithCapacity(_buffs->count());
    foreach(const Buff&, buff, _buffs) {
        buffs->addObject(buff.index);
    }
    return buffs;
}
