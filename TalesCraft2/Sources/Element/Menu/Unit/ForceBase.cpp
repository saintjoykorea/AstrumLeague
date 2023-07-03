//
//  ForceBase.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 9. 11..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "ForceBase.h"

const Card* ForceBase::cardForIndex(const UnitIndex& index) const {
    foreach(const Card*, card, cards()) {
        if (card->index() == index) {
            return card;
        }
    }
    return NULL;
}

const Rune* ForceBase::runeForIndex(const RuneIndex& index) const {
    foreach(const Rune*, rune, runes()) {
        if (rune->index == index) {
            return rune;
        }
    }
    return NULL;
}

int ForceBase::unitCount(const UnitIndex& index, bool alive) const {
    return !alive && cardForIndex(index) ? 1 : 0;
}


