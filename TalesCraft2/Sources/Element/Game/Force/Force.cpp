//
//  Force.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 18..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Force.h"

#include "Map.h"

static const fixed GatheringCalculationDuration(5);

Force::Force(const Player* player, int index, int alliance, int color, GameControl control) :
    player(CSObject::retain(player)),
    index(index),
    _alliance(alliance),
    _color(color),
    _alive(true),
    _originControl(control),
	_maxSupply(Asset::sharedAsset()->gamePreference.maxSupply),
    _trainings(new CSDictionary<RuneIndex, bool>()),
    _spawns(new CSArray<Spawn>(SpawnSlots)),
    _cards(new CSArray<Card>(CardSlots)),
    _runes(new CSArray<Rune>(RuneTiers * RuneSlots))
{
#ifdef UseCheatDeck
	_cards->addObject(Card::card(this, UnitIndex(3, 0), 1, 0));       //군견
	_cards->addObject(Card::card(this, UnitIndex(3, 1), 1, 0));       //검병
	_cards->addObject(Card::card(this, UnitIndex(3, 2), 1, 0));       //궁수
	_cards->addObject(Card::card(this, UnitIndex(3, 3), 1, 0));       //놀
	_cards->addObject(Card::card(this, UnitIndex(3, 4), 1, 0));       //레인저
	//_cards->addObject(Card::card(this, UnitIndex(3, 5), 1, 0));       //해골도적
	_cards->addObject(Card::card(this, UnitIndex(3, 6), 1, 0));       //오크전사
	_cards->addObject(Card::card(this, UnitIndex(3, 7), 1, 0));       //바이킹
	//_cards->addObject(Card::card(this, UnitIndex(3, 8), 1, 0));       //부두술사
	//_cards->addObject(Card::card(this, UnitIndex(3, 9), 1, 0));       //광전사
	//_cards->addObject(Card::card(this, UnitIndex(3, 10), 1, 0));      //치유사
	//_cards->addObject(Card::card(this, UnitIndex(3, 11), 1, 0));      //나이트
	//_cards->addObject(Card::card(this, UnitIndex(3, 12), 1, 0));      //울프라이더
	//_cards->addObject(Card::card(this, UnitIndex(3, 13), 1, 0));      //스나이퍼
	//_cards->addObject(Card::card(this, UnitIndex(3, 14), 1, 0));      //아이언팬츠
	//_cards->addObject(Card::card(this, UnitIndex(3, 15), 1, 0));      //드워프캐논
	//_cards->addObject(Card::card(this, UnitIndex(3, 16), 1, 0));      //다크레인
	//_cards->addObject(Card::card(this, UnitIndex(3, 17), 1, 0));      //골렘
	//_cards->addObject(Card::card(this, UnitIndex(3, 18), 1, 0));      //임프
	//_cards->addObject(Card::card(this, UnitIndex(3, 19), 1, 0));      //가고일
	//_cards->addObject(Card::card(this, UnitIndex(3, 20), 1, 0));      //쉐도우파이터
	//_cards->addObject(Card::card(this, UnitIndex(3, 21), 1, 0));      //고블린벌룬
	//_cards->addObject(Card::card(this, UnitIndex(3, 22), 1, 0));      //소환마법사
	//_cards->addObject(Card::card(this, UnitIndex(3, 23), 1, 0));      //성기사
	//_cards->addObject(Card::card(this, UnitIndex(3, 24), 1, 0));      //메카밤
	//_cards->addObject(Card::card(this, UnitIndex(3, 25), 1, 0));      //보호사제
	//_cards->addObject(Card::card(this, UnitIndex(3, 26), 1, 0));      //냉기마법사
	//_cards->addObject(Card::card(this, UnitIndex(3, 27), 1, 0));      //화염마법사
	//_cards->addObject(Card::card(this, UnitIndex(3, 28), 1, 0));      //드루이드
	//_cards->addObject(Card::card(this, UnitIndex(3, 29), 1, 0));      //고블린과학자
	//_cards->addObject(Card::card(this, UnitIndex(3, 30), 1, 0));      //창병
	//_cards->addObject(Card::card(this, UnitIndex(3, 31), 1, 0));      //닌자고블린
	//_cards->addObject(Card::card(this, UnitIndex(3, 32), 1, 0));      //미니드래곤
	//_cards->addObject(Card::card(this, UnitIndex(3, 33), 1, 0));      //파도마법사
	//_cards->addObject(Card::card(this, UnitIndex(3, 34), 1, 0));      //배트
	//_cards->addObject(Card::card(this, UnitIndex(3, 35), 1, 0));      //두더지
	//_cards->addObject(Card::card(this, UnitIndex(3, 36), 1, 0));      //하피
	//_cards->addObject(Card::card(this, UnitIndex(3, 37), 1, 0));      //발록
	//_cards->addObject(Card::card(this, UnitIndex(3, 38), 1, 0));      //학살자
    //_cards->addObject(Card::card(this, UnitIndex(3, 39), 1, 0));      //드워프헌터
    //_cards->addObject(Card::card(this, UnitIndex(3, 40), 1, 0));      //꼬마마녀
    //_cards->addObject(Card::card(this, UnitIndex(3, 41), 1, 0));        //오크투사
	//_cards->addObject(Card::card(this, UnitIndex(4, 0), 1, 0));       //감시탑
	//_cards->addObject(Card::card(this, UnitIndex(4, 1), 1, 0));       //스프링밤
	//_cards->addObject(Card::card(this, UnitIndex(4, 2), 1, 0));       //아이스타워
	//_cards->addObject(Card::card(this, UnitIndex(4, 3), 1, 0));       //감전탑
	//_cards->addObject(Card::card(this, UnitIndex(5, 0), 1, 0));       //용맹
	//_cards->addObject(Card::card(this, UnitIndex(5, 1), 1, 0));       //신속
	//_cards->addObject(Card::card(this, UnitIndex(5, 2), 1, 0));       //화염구
	//_cards->addObject(Card::card(this, UnitIndex(5, 3), 1, 0));       //독
	//_cards->addObject(Card::card(this, UnitIndex(5, 4), 1, 0));       //감전
	//_cards->addObject(Card::card(this, UnitIndex(5, 5), 1, 0));       //눈보라
	//_cards->addObject(Card::card(this, UnitIndex(5, 6), 1, 0));       //시간단축
	//_cards->addObject(Card::card(this, UnitIndex(5, 7), 1, 0));       //매혹
	//_cards->addObject(Card::card(this, UnitIndex(5, 8), 1, 0));       //축복
	//_cards->addObject(Card::card(this, UnitIndex(5, 9), 1, 0));       //안개
	//_cards->addObject(Card::card(this, UnitIndex(5, 10), 1, 0));       //지옥의문
    //_cards->addObject(Card::card(this, UnitIndex(5, 11), 1, 0));       //번개폭풍
    //_cards->addObject(Card::card(this, UnitIndex(5, 12), 1, 0));       //천리안
    //_cards->addObject(Card::card(this, UnitIndex(5, 13), 1, 0));       //골렘뭉치
    //_cards->addObject(Card::card(this, UnitIndex(5, 14), 1, 0));       //지진
    //_cards->addObject(Card::card(this, UnitIndex(5, 15), 1, 0));       //귀환
    
    //_runes->addObject(Rune::rune(this, RuneIndex(1, 4), 1));
    //_runes->addObject(Rune::rune(this, RuneIndex(1, 34), 1));
    //_runes->addObject(Rune::rune(this, RuneIndex(1, 38), 1));
    //_runes->addObject(Rune::rune(this, RuneIndex(1, 39), 1));

    while (_spawns->count() < SpawnSlots && rollSpawn(_spawns->count()));
#else
    resetCardsImpl();
    resetRunesImpl();
#endif

#ifdef UseCheatResource
    _resources[0] = _resources[1] = (fixed)99999;
#endif
    setControl(control);

    switch (Map::sharedMap()->levelCorrection()) {
        case GameLevelCorrectionMin:
            baseUnitLevel = 1;
            break;
        case GameLevelCorrectionMax:
            baseUnitLevel = Asset::sharedAsset()->gamePreference.maxCardLevel;
            break;
        case GameLevelCorrectionAvg:
            {
                int level = 0;
                foreach(const Card*, card, _cards) level += card->level;
                baseUnitLevel = level ? level / _cards->count() : 1;
            }
            break;
        default:
            {
                baseUnitLevel = player->level;
                int maxLevel = Map::sharedMap()->maxCardLevel();
                if (maxLevel && baseUnitLevel > maxLevel) baseUnitLevel = maxLevel;
            }
            break;
    }
    
    GameLeak_addObject(this);
}

Force::~Force() {
    player->release();
    if (_ai) delete _ai;
    _trainings->release();
    _spawns->release();
    release(_nextCard);
    _cards->release();
    _runes->release();
    release(_emoticon);
    
    GameLeak_removeObject(this);
}

void Force::revisePopulation(int population) {
    CSAssert(_population + population >= 0, "invalid state");
    _population += population;
}

int Force::supply() const {
    return CSMath::min((int)_supply, (int)_maxSupply);
}

void Force::reviseSupply(int supply) {
    CSAssert(_supply + supply >= 0, "invalid state");
    _supply += supply;
}

void Force::reviseResource(int kind, fixed amount) {
    CSAssert(_resources[kind] + amount >= fixed::Zero, "invalid state");
    _resources[kind] += amount;
}

void Force::setAI(int level, bool fully, fixed lifetime) {
    if (!_ai) _ai = new AI(level, fully, lifetime);
    else {
        _ai->level = level;
        _ai->fully = fully;
        _ai->lifetime = lifetime;
    }
}

bool Force::hasTeam() const {
	for (int i = 0; i < MaxPlayers; i++) {
		const Force* other = Map::sharedMap()->force(i);
		if (other && other != this && other->isAlive() && other->alliance() == _alliance) {
			return true;
		}
	}
	return false;
}

void Force::setControl(GameControl control) {
    synchronized(Map::sharedMap()->updateLock(), GameLockMap) {
		switch (control) {
			case GameControlNone:
				if (_alive) {
					_alive = false;

					if (!Map::sharedMap()->isPlaying() || Map::sharedMap()->force() != this) {
						const CSString* msg = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameGiveup), (uint)allianceColor(), (const char*)*player->name);

						Map::sharedMap()->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), msg, true));
					}
					if (!_ai && hasTeam()) {
						_ai = new AI(AILevelUser, false, fixed::Zero);
					}
				}
				break;
			case GameControlUser:
				if (!_alive) {
					_alive = true;

					if (!Map::sharedMap()->isPlaying() || Map::sharedMap()->force() != this) {
						const CSString* msg = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameReturn), (uint)allianceColor(), (const char*)*player->name);

						Map::sharedMap()->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), msg, true));
					}
				}
                if (_ai) {
                    delete _ai;
                    _ai = NULL;
                }
                break;
            case GameControlAI_0:
			case GameControlAI_1:
			case GameControlAI_2:
			case GameControlAI_3:
			case GameControlAI_4:
				setAI(control - GameControlAI_0 + 1, true, fixed::Zero);
                break;
            case GameControlAuto:
                setAI(AILevelUser, false, fixed::Zero);
                break;
            case GameControlAITemporary:
				setAI(AILevelUser, true, hasTeam() ? fixed::Zero : (fixed)GameReconnectWaitingTime);
                break;
        }
    }
}

const CSColor& Force::allianceColor() const {
    if (Map::sharedMap()->isFocusing()) {
        const Force* force = Map::sharedMap()->force();
        if (force) {
            if (force == this) {
                return Asset::playerColor0;
            }
            else if (force->alliance() == _alliance) {
                return Asset::allianceColor;
            }
            else {
                return Asset::enemyColor;
            }
        }
    }
    return originColor();
}

void Force::updateResource() {
    _resourceChargeTime += Map::sharedMap()->frameDelayFixed();
    if (_resourceChargeTime > Asset::sharedAsset()->gamePreference.resourceChargeTime) {
        _resourceChargeTime = fixed::Zero;
        _resources[0] = CSMath::min(_resources[0] + fixed::One, (fixed)Asset::sharedAsset()->gamePreference.resourceChargeMax);
    }
}

void Force::updateGatherings() {
    _gatheringSeq += Map::sharedMap()->frameDelayFixed();
    if (_gatheringSeq >= GatheringCalculationDuration) {
        for (int i = 0; i < 2; i++) {
            _gatherings[i].calculation = _gatherings[i].current / _gatheringSeq;
            _gatherings[i].current = fixed::Zero;
        }
        _gatheringSeq = fixed::Zero;
    }
}

TrainingState Force::trainingState(const RuneIndex& index) const {
	const Rune* rune = runeForIndex(index);
	
	if (rune) {
		synchronized_to_update_thread(this, GameLockForce) {
			const bool* state = _trainings->tryGetObjectForKey(index);

			if (!state) return TrainingStateReady;
			else if (*state) return TrainingStateComplete;
			else return TrainingStateProgress;
		}
	}
	return TrainingStateNone;
}

void Force::startTraining(const RuneIndex& index) {
    AssertOnUpdateThread();
    
    CSAssert(runeForIndex(index) && !_trainings->containsKey(index), "invalid state");
    
    _trainings->setObject(index) = false;
}
void Force::completeTraining(const RuneIndex& index) {
    AssertOnUpdateThread();
    
	_trainings->setObject(index) = true;
}
void Force::cancelTraining(const RuneIndex& index) {
    AssertOnUpdateThread();
    
    _trainings->removeObject(index);
}

int Force::unitCount(const UnitIndex& index, bool alive) const {
    if (alive) {
        return Map::sharedMap()->unitCount(this, index);
    }
    foreach (const Card*, card, _cards) {
        if (card->index() == index) {
            return 1;
        }
    }
    return 0;
}

bool Force::isTrained(const RuneIndex& index) const {
	synchronized_to_update_thread(this, GameLockForce) {
		const bool* state = _trainings->tryGetObjectForKey(index);
		return state && *state;
	}
	return false;
}

void Force::update() {
    updateResource();
    updateGatherings();
    updateSpawns();
}

Spawn* Force::spawn(int slot) {
    return _spawns->objectAtIndex(slot);
}

const Spawn* Force::spawn(int slot) const {
    return _spawns->objectAtIndex(slot);
}

Spawn* Force::spawn(const UnitIndex& index) {
    foreach(Spawn*, spawn, _spawns) {
        if (spawn->index() == index) {
            return spawn;
        }
    }
    return NULL;
}

const Spawn* Force::spawn(const UnitIndex& index) const {
    return const_cast<Force*>(this)->spawn(index);
}

int Force::spawnCount() const {
    return _spawns->count();
}

bool Force::rollCard() {
    int index = Map::sharedMap()->random()->nextInt(0, _cards->count() - 1);
    
    for (int i = 0; i < _cards->count(); i++) {
        Card* card = _cards->objectAtIndex(index);
        bool flag = true;
        foreach(const Spawn*, spawn, _spawns) {
            if (spawn->index() == card->index()) {
                flag = false;
                break;
            }
        }
        if (flag) {
            retain(_nextCard, card);
            return true;
        }
        else index = (index + 1) % _cards->count();
    }
    release(_nextCard);
    return false;
}

bool Force::rollSpawn(int slot) {
    if (!_nextCard && !rollCard()) return false;

    if (slot < _spawns->count()) _spawns->removeObjectAtIndex(slot);

    _spawns->insertObject(slot, Spawn::spawn(this, _nextCard->index(), cardLevel(_nextCard), _nextCard->skin));

    rollCard();

    return true;
}

void Force::updateSpawns() {
    foreach(Spawn*, spawn, _spawns) spawn->update();
}

void Force::addCardImpl(Card *card) {
    for (int i = 0; i < _cards->count(); i++) {
        Card* otherCard = _cards->objectAtIndex(i);
        
        if (otherCard == card) {
            return;
        }
        if (otherCard->index() == card->index()) {
            _cards->removeObjectAtIndex(i);
            break;
        }
    }
    _cards->addObject(card);

    if (_spawns->count() < SpawnSlots) rollSpawn(_spawns->count());
}

void Force::addCard(Card* card) {
    CSAssert(card->force() == this, "invalid operation");
    
    if (CSThread::mainThread()->isActive()) {
        addCardImpl(card);
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate1<void, Card*>* inv = CSDelegate1<void, Card*>::delegate(this, &Force::addCardImpl);
        inv->setParam0(card);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::removeCardImpl(UnitIndex index) {
    for (int i = 0; i < _cards->count(); i++) {
        const Card* card = _cards->objectAtIndex(i);
        if (card->index() == index) {
            _cards->removeObjectAtIndex(i);

            for (int j = 0; j < _spawns->count(); j++) {
                const Spawn* spawn = _spawns->objectAtIndex(j);
                if (spawn->index() == index) {
                    rollSpawn(j);
                    break;
                }
            }
            if (_nextCard && _nextCard->index() == index) {
                _nextCard->release();
                _nextCard = NULL;
            }
            break;
        }
    }
}

void Force::removeCard(const UnitIndex& index) {
    if (CSThread::mainThread()->isActive()) {
        removeCardImpl(index);
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate1<void, UnitIndex>* inv = CSDelegate1<void, UnitIndex>::delegate(this, &Force::removeCardImpl);
        inv->setParam0(index);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::removeAllCardsImpl() {
    _cards->removeAllObjects();
    _spawns->removeAllObjects();
    release(_nextCard);
    _spawns->removeAllObjects();
}

void Force::removeAllCards() {
    if (CSThread::mainThread()->isActive()) {
        removeAllCardsImpl();
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate0<void>* inv = CSDelegate0<void>::delegate(this, &Force::removeAllCardsImpl);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::resetCardsImpl() {
    _cards->removeAllObjects();
    _spawns->removeAllObjects();
    release(_nextCard);
    _spawns->removeAllObjects();

    foreach (const Card*, card, player->cards()) {
        int level = cardLevel(card);
        _cards->addObject(Card::card(this, card->index(), level, card->skin));
    }

    while (_spawns->count() < SpawnSlots && rollSpawn(_spawns->count()));
}

void Force::resetCards() {
    if (CSThread::mainThread()->isActive()) {
        resetCardsImpl();
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate0<void>* inv = CSDelegate0<void>::delegate(this, &Force::resetCardsImpl);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::addRuneImpl(Rune* rune) {
    for (int i = 0; i < _runes->count(); i++) {
        Rune* otherRune = _runes->objectAtIndex(i);
        
        if (otherRune == rune) {
            return;
        }
        if (otherRune->index == rune->index) {
            _runes->removeObjectAtIndex(i);
            break;
        }
    }
    _runes->addObject(rune);
}

void Force::addRune(Rune* rune) {
	CSAssert(rune->force == this, "invalid operation");

    if (CSThread::mainThread()->isActive()) {
        addRuneImpl(rune);
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate1<void, Rune*>* inv = CSDelegate1<void, Rune*>::delegate(this, &Force::addRuneImpl);
        inv->setParam0(rune);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::removeRuneImpl(RuneIndex index) {
    for (int i = 0; i < _runes->count(); i++) {
        const Rune* rune = _runes->objectAtIndex(i);
        if (rune->index == index) {
            _runes->removeObjectAtIndex(i);
        }
    }
}

void Force::removeRune(const RuneIndex& index) {
    if (CSThread::mainThread()->isActive()) {
        removeRuneImpl(index);
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate1<void, RuneIndex>* inv = CSDelegate1<void, RuneIndex>::delegate(this, &Force::removeRuneImpl);
        inv->setParam0(index);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::removeAllRunesImpl() {
    _runes->removeAllObjects();
}

void Force::removeAllRunes() {
    if (CSThread::mainThread()->isActive()) {
        removeAllRunesImpl();
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate0<void>* inv = CSDelegate0<void>::delegate(this, &Force::removeAllRunesImpl);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

void Force::resetRunesImpl() {
    _runes->removeAllObjects();
    
    foreach (const Rune*, rune, player->runes()) {
        _runes->addObject(Rune::rune(this, rune->index, runeLevel(rune)));
    }
}

void Force::resetRunes() {
    if (CSThread::mainThread()->isActive()) {
        resetRunesImpl();
    }
    else {
        Map::sharedMap()->updateLock().unlock();
        CSDelegate0<void>* inv = CSDelegate0<void>::delegate(this, &Force::resetRunesImpl);
        CSThread::mainThread()->addTask(CSTask::task(inv, 0, CSTaskActivityWait));
        Map::sharedMap()->updateLock().lock(GameLockMap);
    }
}

int Force::unitSkin(const UnitIndex& index) const {
    foreach(const Card*, card, _cards) {
        if (card->index() == index) {
            return card->skin;
        }
    }
    if (player->deckTheme) {
        const ThemeData& data = Asset::sharedAsset()->theme(player->deckTheme);
        const ushort* skin = data.skins->tryGetObjectForKey(index);
        if (skin) return *skin;
    }
    return 0;
}

int Force::unitLevel(const UnitIndex& index) const {
	switch (Map::sharedMap()->levelCorrection()) {
		case GameLevelCorrectionMin:
			return 1;
		case GameLevelCorrectionMax:
			return Asset::sharedAsset()->gamePreference.maxCardLevel;
        default:
            foreach(const Card*, card, _cards) {
                if (card->index() == index) return card->level;
            }
            return baseUnitLevel;
	}
}

int Force::cardLevel(const Card* card) const {
    switch (Map::sharedMap()->levelCorrection()) {
        case GameLevelCorrectionMin:
            return 1;
        case GameLevelCorrectionMax:
            return Asset::sharedAsset()->gamePreference.maxCardLevel;
        default:
            {
                int level = card->level;
                int maxLevel = Map::sharedMap()->maxCardLevel();
                if (maxLevel && level > maxLevel) level = maxLevel;
                return level;
            }
    }
}

int Force::runeLevel(const Rune* rune) const {
    switch (Map::sharedMap()->levelCorrection()) {
        case GameLevelCorrectionMin:
            return 1;
        case GameLevelCorrectionMax:
            return Asset::sharedAsset()->gamePreference.maxRuneLevel;
        default:
            {
                int level = rune->level;
                int maxLevel = Map::sharedMap()->maxRuneLevel();
                if (maxLevel && level > maxLevel) level = maxLevel;
                return level;
            }
    }
}

bool Force::showEmoticon(const EmoticonIndex& emoticon) {
    const EmoticonData& data = Asset::sharedAsset()->emoticon(emoticon);
    
    if (data.message) {
        CSString* msg = copy(data.message->value());
        
        if (msg) {
            msg->replace("$user", CSString::cstringWithFormat("#color(%08X)%s#color()", (uint)allianceColor(), (const char*)*player->name));
            
            Map::sharedMap()->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconChat), msg));
            
            msg->release();
        }
    }
	synchronized(this, GameLockForce) {
		if (_emoticon) {
			if (_emoticon->index() == data.animation) {
				_emoticon->rewind();
				return true;
			}
			_emoticon->release();
		}
		_emoticon = new Animation(data.animation, SoundControlEffect, false);
	}
    return true;
}

void Force::updateEmoticon(float delta) {
	if (_emoticon) {
		synchronized(this, GameLockForce) {
			if (_emoticon && !_emoticon->update(delta)) {
				_emoticon->release();
				_emoticon = NULL;
			}
		}
	}
}

void Force::drawEmoticon(CSGraphics *graphics, const CSRect& frame) const {
	if (_emoticon) {
		Animation* emoticon = NULL;
		synchronized(this, GameLockForce) {
			emoticon = retain(_emoticon);
		}
        if (emoticon) {
            graphics->push();
            graphics->translate(CSVector2(frame.center(), 60));
			emoticon->draw(graphics);
            
            graphics->setStrokeWidth(2);
            graphics->setFont(Asset::sharedAsset()->extraSmallFont);
            graphics->setColor(allianceColor());
            graphics->drawStringScaled(player->name, CSVector2(0, frame.bottom() - 60), CSAlignCenterBottom, frame.size.width);
            
            graphics->pop();

			emoticon->release();
        }
    }
}
