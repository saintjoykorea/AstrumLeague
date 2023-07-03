//
//  Account.h
//  TalesCraft2
//
//  Created by ChangSun on 2015. 7. 31..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef Account_h
#define Account_h

#include "Player.h"
#include "Party.h"
#include "Match.h"

class Account : public Player {
private:
    CSArray<Card>* _cards;
    CSArray<Rune>* _runes;
    CSArray<Card>* _allCards;
    CSArray<Rune>* _allRunes;
    CSArray<EmoticonIndex>* _emoticons;
	Party* _party;
    Match* _match;
public:
    ushort announce;
    const CSString* lastPartyId;
    uint lastMatchId;
private:
    static Account* __account;
    
    Account();         //TODO:for test
    Account(const CSJSONObject* json);
    ~Account();
public:
    static void load();         //TODO:for test
    static void load(const CSJSONObject* json);
    static void destroy();
#ifdef AccountImpl
    static inline Account* sharedAccount() {
        return __account;
    }
#else
    static inline const Account* sharedAccount() {
        return __account;
    }
#endif
    inline const CSArray<Card>* cards() const override {
        return _cards;
    }
    inline const CSArray<Rune>* runes() const override {
        return _runes;
    }
    inline CSArray<Card>* cards() {
        return _cards;
    }
    inline CSArray<Rune>* runes() {
        return _runes;
    }
    inline const CSArray<Card>* allCards() const {
        return _allCards;
    }
    inline const CSArray<Rune>* allRunes() const {
        return _allRunes;
    }
    inline CSArray<Card>* allCards() {
        return _allCards;
    }
    inline CSArray<Rune>* allRunes() {
        return _allRunes;
    }
    Card* cardForIndex(const UnitIndex& index);
    inline const Card* cardForIndex(const UnitIndex& index) const {
        return const_cast<Account*>(this)->cardForIndex(index);
    }
    Rune* runeForIndex(const RuneIndex& index);
    inline const Rune* runeForIndex(const RuneIndex& index) const {
        return const_cast<Account*>(this)->runeForIndex(index);
    }

    inline const CSArray<EmoticonIndex>* emoticons() const {
        return _emoticons;
    }
    inline CSArray<EmoticonIndex>* emoticons() {
        return _emoticons;
    }
    inline Party*& party() {
        return _party;
    }
    inline const Party* party() const {
        return _party;
    }
    inline Match*& match() {
        return _match;
    }
    inline const Match* match() const {
        return _match;
    }
};

#endif /* Account_h */
