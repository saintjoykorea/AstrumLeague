//
//  Player.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 27..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Player.h"

Player::Player(CSBuffer* buffer, bool dummy) : User(buffer, dummy), deckTheme(ThemeIndex::None) {
    
}

Player::Player(const CSJSONObject* json) : User(json), deckTheme(ThemeIndex::None) {
    
}

void Player::writeTo(CSBuffer* buffer, bool containsDeck) const {
    User::writeTo(buffer);

    if (containsDeck) {
        deckTheme.writeTo(buffer);
        
        const CSArray<Card>* cards = this->cards();
        const CSArray<Rune>* runes = this->runes();

        buffer->writeLength(cards->count());
        foreach (const Card*, card, cards) {
            card->writeTo(buffer);
        }
        buffer->writeLength(runes->count());
        foreach (const Rune*, rune, runes) {
            rune->writeTo(buffer);
        }
    }
}


