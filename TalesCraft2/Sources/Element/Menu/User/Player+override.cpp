//
//  Player+override.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 27..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Player.h"

#include "Account.h"

class _Player : public Player {
private:
    CSArray<Card>* _cards;
    CSArray<Rune>* _runes;
public:
    _Player();
    _Player(CSBuffer* buffer, bool locale);
    _Player(const CSJSONObject* json);
private:
    ~_Player();
public:
    inline const CSArray<Card>* cards() const override {
        return _cards;
    }
    inline const CSArray<Rune>* runes() const override {
        return _runes;
    }
};

_Player::_Player() :
    _cards(new CSArray<Card>(CardSlots)),
    _runes(new CSArray<Rune>(RuneTiers * RuneSlots))
{
    const Account* account = Account::sharedAccount();

	level = account->level;

	deckTheme = ThemeIndex::None;

	int minCardLevel;
    int maxCardLevel;
	int minRuneLevel;
	int maxRuneLevel;

    const CSArray<UnitIndex>* cardIndices = Asset::sharedAsset()->openCards;
	const CSArray<RuneIndex>* runeIndices = Asset::sharedAsset()->openRunes;
    
    if (account->allCards()->count()) {
        minCardLevel = Asset::sharedAsset()->gamePreference.maxCardLevel;
        maxCardLevel = 1;
        
        foreach(const Card*, card, account->allCards()) {
			int level = CSMath::max((int)card->level, 1);

            if (minCardLevel > level) {
                minCardLevel = level;
            }
            if (maxCardLevel < level) {
                maxCardLevel = level;
            }
        }
    }
    else {
        minCardLevel = 1;
        maxCardLevel = 1;
    }

	if (account->allRunes()->count()) {
		minRuneLevel = Asset::sharedAsset()->gamePreference.maxRuneLevel;
		maxRuneLevel = 1;

		foreach(const Rune*, rune, account->allRunes()) {
            int level = CSMath::max((int)rune->level, 1);

			if (minRuneLevel > level) {
				minRuneLevel = level;
			}
			if (maxRuneLevel < level) {
				maxRuneLevel = level;
			}
		}
	}
	else {
		minRuneLevel = 1;
		maxRuneLevel = 1;
	}

    for (int i = 0; i < CardSlots; i++) {
        int j = randInt(0, cardIndices->count() - 1);
        
        bool success = false;
        for (int k = 0; k < cardIndices->count(); k++) {
            const UnitIndex& index = cardIndices->objectAtIndex(j);
            
            success = true;
            foreach(const Card*, card, _cards) {
                if (card->index() == index) {
                    success = false;
                    break;
                }
            }
            if (success) {
                break;
            }
            else {
                j = (j + 1) % cardIndices->count();
            }
        }
        if (success) {
            const UnitIndex& index = cardIndices->objectAtIndex(j);
            int level = randInt(minCardLevel, maxCardLevel);
            
            _cards->addObject(Card::card(this, index, level, 0));
        }
		else {
			break;
		}
    }

    //TODO:no runes yet
    /*
	for (int t = 1; t <= RuneTiers; t++) {
		int slotCount = Asset::sharedAsset()->userLevel(level).runeSlotCount[t - 1];

		for (int i = 0; i < slotCount; i++) {
			int j = randInt(0, runeIndices->count() - 1);

			bool success = false;
			for (int k = 0; k < runeIndices->count(); k++) {
				const RuneIndex& index = runeIndices->objectAtIndex(j);
				
				if (Asset::sharedAsset()->rune(index).tier == t) {
					success = true;
					foreach(const Rune*, rune, _runes) {
						if (rune->index == index) {
							success = false;
							break;
						}
					}
				}
				if (success) {
					break;
				}
				else {
					j = (j + 1) % runeIndices->count();
				}
			}
			if (success) {
				const RuneIndex& index = runeIndices->objectAtIndex(j);
				int level = randInt(minRuneLevel, maxRuneLevel);

				_runes->addObject(Rune::rune(this, index, level));
			}
			else {
				break;
			}
		}
	}
    */
}

_Player::_Player(CSBuffer* buffer, bool dummy) : Player(buffer, dummy) {
    deckTheme = ThemeIndex(buffer);
    _cards = retain(buffer->readArrayWithParam<Card>(static_cast<const ForceBase*>(this), false));
    _runes = retain(buffer->readArrayWithParam<Rune>(static_cast<const ForceBase*>(this), false));
}

_Player::_Player(const CSJSONObject* json) :
    Player(json),
    _cards(new CSArray<Card>(CardSlots)),
    _runes(new CSArray<Rune>(RuneTiers * RuneSlots))
{
    deckTheme = json->numberForKey("deckTheme");
    foreach(const CSJSONObject*, jcard, json->jsonArrayForKey("cards")) {
        _cards->addObject(Card::cardWithJSON(jcard, this));
    }
    foreach(const CSJSONObject*, jrune, json->jsonArrayForKey("runes")) {
        _runes->addObject(Rune::runeWithJSON(jrune, this));
    }
}

_Player::~_Player() {
    _cards->release();
    _runes->release();
}
//================================================================
Player* Player::createForTraining() {
    return new _Player();
}
Player* Player::createWithBuffer(CSBuffer* buffer, bool locale) {
    return new _Player(buffer, locale);
}
Player* Player::createWithJSON(const CSJSONObject* json) {
    return new _Player(json);
}
Player* Player::playerForTraining() {
    return autorelease(new _Player());
}
Player* Player::playerWithBuffer(CSBuffer* buffer, bool locale) {
    return autorelease(new _Player(buffer, locale));
}
Player* Player::playerWithJSON(const CSJSONObject* json) {
    return autorelease(new _Player(json));
}

