//
//  VirtualForce.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 5..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#include "VirtualForce.h"

#include "Player.h"

#include "VirtualUnit.h"

VirtualForce::VirtualForce() :
    _alliance(0),
    _resources{fixed::Zero, fixed::Zero},
    _cards(new CSArray<Card>(CardSlots)),
    _runes(new CSArray<Rune>(RuneSlots * RuneTiers)),
    _units(NULL)
{
    
}

VirtualForce::VirtualForce(const ForceBase* other) :
    _alliance(other->alliance()),
    _resources{other->resource(0), other->resource(1)},
    _cards(CSObject::copy(other->cards())),
    _runes(CSObject::copy(other->runes())),
    _units(NULL)
{
    {
        UnitIndex index;
        for (index.indices[0] = 0; index.indices[0] < Asset::sharedAsset()->units->count(); index.indices[0]++) {
            for (index.indices[1] = 0; index.indices[1] < Asset::sharedAsset()->units->objectAtIndex(index.indices[0])->count(); index.indices[1]++) {
                int count = other->unitCount(index, true);
                if (count) {
                    if (!_units) _units = new CSDictionary<UnitIndex, ushort>();
                    _units->setObject(index, count);
                }
            }
        }
    }
}

VirtualForce::VirtualForce(CSBuffer* buffer) :
    _alliance(buffer->readByte()),
	_resources{ buffer->readFixed(), buffer->readFixed() },
    _cards(CSObject::retain(buffer->readArrayWithParam<Card>(static_cast<const ForceBase*>(this), false))),
    _runes(CSObject::retain(buffer->readArrayWithParam<Rune>(static_cast<const ForceBase*>(this), false))),
    _units(NULL)
{
    int unitCount = buffer->readLength();
    if (unitCount) {
        _units = new CSDictionary<UnitIndex, ushort>(unitCount);
        for (int i = 0; i < unitCount; i++) {
            UnitIndex index(buffer);
            int count = buffer->readShort();
            
            _units->setObject(index, count);
        }
    }
}

VirtualForce::~VirtualForce() {
    _cards->release();
    _runes->release();
	release(_units);
}

void VirtualForce::removeAllCards() {
    _cards->removeAllObjects();
}

void VirtualForce::addCard(const Card* card) {
    _cards->addObject(Card::card(this, card->index(), card->level, card->skin));
}

void VirtualForce::removeCard(const UnitIndex& index) {
    for (int i = 0; i < _cards->count(); i++) {
        const Card* card = _cards->objectAtIndex(i);
        if (card->index() == index) {
            _cards->removeObjectAtIndex(i);
            break;
        }
    }
}

void VirtualForce::removeAllRunes() {
    _runes->removeAllObjects();
}

void VirtualForce::addRune(const Rune* rune) {
    _runes->addObject(const_cast<Rune*>(rune));
}

void VirtualForce::removeRune(const RuneIndex& index) {
    for (int i = 0; i < _runes->count(); i++) {
        const Rune* rune = _runes->objectAtIndex(i);
        if (rune->index == index) {
            _runes->removeObjectAtIndex(i);
            break;
        }
    }
}

int VirtualForce::unitCount(const UnitIndex& index, bool alive) const {
    if (alive) {
        ushort* count;
        return _units && (count = _units->tryGetObjectForKey(index)) ? *count : 0;
    }
    foreach (const Card*, card, _cards) {
        if (card->index() == index) {
            return 1;
        }
    }
    return 0;
}
