//
//  Account.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2015. 7. 31..
//  Copyright (c) 2015brgames. All rights reserved.
//
#define AccountImpl

#include "Account.h"

#include "PVPGateway.h"

#include "GameConfig.h"

Account* Account::__account = NULL;

//TODO:for test
Account::Account() :
	_cards(new CSArray<Card>(CardSlots)),
	_runes(new CSArray<Rune>(RuneTiers* RuneSlots)),
	_allCards(new CSArray<Card>()),
	_allRunes(new CSArray<Rune>()),
	_emoticons(new CSArray<EmoticonIndex>(EmoticonSlots)) 
{
	CSAssert(!__account, "invalid state");
	__account = this;

	userId = CSTime::currentTimeSecond();
	//name = CSString::createWithFormat("%" PRId64, userId);
	name = new CSString("AstrumLeague");
	level = 1;
	deckTheme = ThemeIndex::None;

	_allCards->addObject(Card::card(this, UnitIndex(3, 0), 1, 0));       //군견
	_allCards->addObject(Card::card(this, UnitIndex(3, 1), 1, 0));       //검병
	_allCards->addObject(Card::card(this, UnitIndex(3, 2), 1, 0));       //궁수
	_allCards->addObject(Card::card(this, UnitIndex(3, 3), 1, 0));       //놀
	_allCards->addObject(Card::card(this, UnitIndex(3, 4), 1, 0));       //레인저
	//_allCards->addObject(Card::card(this, UnitIndex(3, 5), 1, 0));       //해골도적
	_allCards->addObject(Card::card(this, UnitIndex(3, 6), 1, 0));       //오크전사
	_allCards->addObject(Card::card(this, UnitIndex(3, 7), 1, 0));       //바이킹
	_allCards->addObject(Card::card(this, UnitIndex(3, 8), 1, 0));       //부두술사
	_allCards->addObject(Card::card(this, UnitIndex(3, 9), 1, 0));       //광전사
	_allCards->addObject(Card::card(this, UnitIndex(3, 10), 1, 0));      //치유사
	_allCards->addObject(Card::card(this, UnitIndex(3, 11), 1, 0));      //나이트
	_allCards->addObject(Card::card(this, UnitIndex(3, 12), 1, 0));      //울프라이더
	_allCards->addObject(Card::card(this, UnitIndex(3, 13), 1, 0));      //스나이퍼
	_allCards->addObject(Card::card(this, UnitIndex(3, 14), 1, 0));      //아이언팬츠
	_allCards->addObject(Card::card(this, UnitIndex(3, 15), 1, 0));      //드워프캐논
	_allCards->addObject(Card::card(this, UnitIndex(3, 16), 1, 0));      //다크레인
	_allCards->addObject(Card::card(this, UnitIndex(3, 17), 1, 0));      //골렘
	_allCards->addObject(Card::card(this, UnitIndex(3, 18), 1, 0));      //임프
	_allCards->addObject(Card::card(this, UnitIndex(3, 19), 1, 0));      //가고일
	_allCards->addObject(Card::card(this, UnitIndex(3, 20), 1, 0));      //쉐도우파이터
	_allCards->addObject(Card::card(this, UnitIndex(3, 21), 1, 0));      //고블린벌룬
	_allCards->addObject(Card::card(this, UnitIndex(3, 22), 1, 0));      //소환마법사
	_allCards->addObject(Card::card(this, UnitIndex(3, 23), 1, 0));      //성기사
	_allCards->addObject(Card::card(this, UnitIndex(3, 24), 1, 0));      //메카밤
	_allCards->addObject(Card::card(this, UnitIndex(3, 25), 1, 0));      //보호사제
	_allCards->addObject(Card::card(this, UnitIndex(3, 26), 1, 0));      //냉기마법사
	_allCards->addObject(Card::card(this, UnitIndex(3, 27), 1, 0));      //화염마법사
	_allCards->addObject(Card::card(this, UnitIndex(3, 28), 1, 0));      //드루이드
	_allCards->addObject(Card::card(this, UnitIndex(3, 29), 1, 0));      //고블린과학자
	_allCards->addObject(Card::card(this, UnitIndex(3, 30), 1, 0));      //창병
	_allCards->addObject(Card::card(this, UnitIndex(3, 31), 1, 0));      //닌자고블린
	_allCards->addObject(Card::card(this, UnitIndex(3, 32), 1, 0));      //미니드래곤
	_allCards->addObject(Card::card(this, UnitIndex(3, 33), 1, 0));      //파도마법사
	_allCards->addObject(Card::card(this, UnitIndex(3, 34), 1, 0));      //배트
	_allCards->addObject(Card::card(this, UnitIndex(3, 35), 1, 0));      //두더지
	_allCards->addObject(Card::card(this, UnitIndex(3, 36), 1, 0));      //하피
	_allCards->addObject(Card::card(this, UnitIndex(3, 37), 1, 0));      //발록
	_allCards->addObject(Card::card(this, UnitIndex(3, 38), 1, 0));      //학살자
	_allCards->addObject(Card::card(this, UnitIndex(3, 39), 1, 0));      //드워프헌터
	_allCards->addObject(Card::card(this, UnitIndex(3, 40), 1, 0));      //꼬마마녀
	_allCards->addObject(Card::card(this, UnitIndex(3, 41), 1, 0));        //오크투사
#ifndef UseNoVision
	_allCards->addObject(Card::card(this, UnitIndex(4, 0), 1, 0));       //감시탑
	_allCards->addObject(Card::card(this, UnitIndex(4, 1), 1, 0));       //스프링밤
	_allCards->addObject(Card::card(this, UnitIndex(4, 2), 1, 0));       //아이스타워
	_allCards->addObject(Card::card(this, UnitIndex(4, 3), 1, 0));       //감전탑
#endif
	
	//_allCards->addObject(Card::card(this, UnitIndex(5, 0), 1, 0));       //용맹
	//_allCards->addObject(Card::card(this, UnitIndex(5, 1), 1, 0));       //신속
	_allCards->addObject(Card::card(this, UnitIndex(5, 2), 1, 0));       //화염구
	_allCards->addObject(Card::card(this, UnitIndex(5, 3), 1, 0));       //독
	_allCards->addObject(Card::card(this, UnitIndex(5, 4), 1, 0));       //감전
	_allCards->addObject(Card::card(this, UnitIndex(5, 5), 1, 0));       //눈보라
	_allCards->addObject(Card::card(this, UnitIndex(5, 6), 1, 0));       //시간단축
	_allCards->addObject(Card::card(this, UnitIndex(5, 7), 1, 0));       //매혹
	//_allCards->addObject(Card::card(this, UnitIndex(5, 8), 1, 0));       //축복
	_allCards->addObject(Card::card(this, UnitIndex(5, 9), 1, 0));       //안개
	_allCards->addObject(Card::card(this, UnitIndex(5, 10), 1, 0));       //지옥의문
	_allCards->addObject(Card::card(this, UnitIndex(5, 11), 1, 0));       //번개폭풍
	//_allCards->addObject(Card::card(this, UnitIndex(5, 12), 1, 0));       //천리안
	_allCards->addObject(Card::card(this, UnitIndex(5, 13), 1, 0));       //골렘뭉치
	_allCards->addObject(Card::card(this, UnitIndex(5, 14), 1, 0));       //지진
	//_allCards->addObject(Card::card(this, UnitIndex(5, 15), 1, 0));       //귀환
	//_allCards->addObject(Card::card(this, UnitIndex(5, 16), 1, 0));       //속공

	_cards->addObject(cardForIndex(UnitIndex(3, 0)));
	_cards->addObject(cardForIndex(UnitIndex(3, 1)));
	_cards->addObject(cardForIndex(UnitIndex(3, 2)));
	_cards->addObject(cardForIndex(UnitIndex(3, 3)));
	_cards->addObject(cardForIndex(UnitIndex(3, 4)));
	_cards->addObject(cardForIndex(UnitIndex(3, 6)));
	_cards->addObject(cardForIndex(UnitIndex(3, 7)));
	_cards->addObject(cardForIndex(UnitIndex(3, 8)));
}

Account::Account(const CSJSONObject* json) :
	Player(json),
	_cards(new CSArray<Card>(CardSlots)),
	_runes(new CSArray<Rune>(RuneTiers * RuneSlots)),
	_allCards(new CSArray<Card>()),
	_allRunes(new CSArray<Rune>()),
	_emoticons(new CSArray<EmoticonIndex>(EmoticonSlots))
{
	CSAssert(!__account, "invalid state");
	__account = this;

	foreach(const CSJSONObject*, jcard, json->jsonArrayForKey("allCards")) {
		Card* card = Card::cardWithJSON(jcard, this);
		_allCards->addObject(card);
	}
	foreach(const CSJSONObject*, jrune, json->jsonArrayForKey("allRunes")) {
		_allRunes->addObject(Rune::runeWithJSON(jrune, this));
	}
	foreach(const CSValue<int64>*, jcard, json->jsonArrayForKey("cards")) {
		Card* card = cardForIndex(UnitIndex(*jcard));
		if (card) _cards->addObject(card);
	}
	foreach(const CSValue<int64>*, jrune, json->jsonArrayForKey("runes")) {
		Rune* rune = runeForIndex(RuneIndex(*jrune));
		if (rune) _runes->addObject(rune);
	}

	deckTheme = json->numberForKey("deckTheme");
	
	foreach(const CSValue<int64>*, jemoticon, json->jsonArrayForKey("emoticons")) {
		_emoticons->addObject(EmoticonIndex(*jemoticon));
	}

	announce = json->numberForKey("announce");
}

Account::~Account() {
    if (PVPGateway::sharedGateway()) {
        PVPGateway::sharedGateway()->requestShutdown(false);
    }
	_cards->release();
	_runes->release();
    _allCards->release();
    _allRunes->release();
	_emoticons->release();
    release(_party);
    release(_match);
    release(lastPartyId);
}

//TODO:for test
void Account::load() {
	release(__account);
	new Account();
}

void Account::load(const CSJSONObject* json) {
    release(__account);
    new Account(json);
}

void Account::destroy() {
    release(__account);
}

Card* Account::cardForIndex(const UnitIndex& index) {
	foreach(Card*, card, _allCards) {
		if (card->index() == index) return card;
	}
	return NULL;
}

Rune* Account::runeForIndex(const RuneIndex& index) {
	foreach(Rune*, rune, _allRunes) {
		if (rune->index == index) return rune;
	}
	return NULL;
}
