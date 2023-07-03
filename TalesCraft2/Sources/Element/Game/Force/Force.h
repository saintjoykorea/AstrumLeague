//
//  Force.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 18..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Force_h
#define Force_h

#include "Player.h"

#include "AI.h"

enum Score {
    ScoreResource_0,
    ScoreResource_1,
    ScoreSummon,
    ScoreKill,
    ScoreDeath,
    ScoreAction,
    ScoreCount
};

enum TrainingState {
	TrainingStateNone,
	TrainingStateReady,
	TrainingStateProgress,
	TrainingStateComplete
};

#define AILevelMax          5
#define AILevelUser         AILevelMax

class Unit;
class Spawn;

class Force : public CSObject, public ForceBase {
public:
    const Player* player;
    FPoint point;
    byte pointDir;
    bool pointFixed;
    const byte index;
    GameResult result;
    ushort baseUnitLevel;
private:
    const byte _alliance;
    const byte _color;
    bool _alive;
    GameControl _originControl;
    AI* _ai;
    int _supply;
	int _maxSupply;
    int _population;
    fixed _resources[2];
    fixed _resourceChargeTime;
    fixed _gatheringSeq;
    struct {
        fixed current;
        fixed calculation;
    } _gatherings[2];
    CSDictionary<RuneIndex, bool>* _trainings;
    CSArray<Spawn>* _spawns;
    Card* _nextCard;
    CSArray<Card>* _cards;
    CSArray<Rune>* _runes;
    const fixed* _attainValues[3];
    Animation* _emoticon;
public:
    Force(const Player* player, int index, int alliance, int color, GameControl control);
private:
    ~Force();
public:
    static inline Force* force(const Player* player, int index, int alliance, int color, GameControl control) {
        return autorelease(new Force(player, index, alliance, color, control));
    }

    //================================================
    //override ForceBase
    inline int alliance() const override {
        return _alliance;
    }
    inline fixed resource(int kind) const override {
        return _resources[kind];
    }
    inline const CSArray<Card>* cards() const override {
        return _cards;
    }
    inline const CSArray<Rune>* runes() const override {
        return _runes;
    }
    int unitCount(const UnitIndex& index, bool alive) const override;
    bool isTrained(const RuneIndex& index) const override;
    
    inline const FPoint& reachablePoint() const override {
        return point;
    }

    //================================================
    inline const CSColor& originColor() const {
        return Asset::forceColors[_color];
    }
    
    const CSColor& allianceColor() const;
    
    inline AI* ai() {
        return _ai;
    }
    inline const AI* ai() const {
        return _ai;
    }
    inline bool isAlive() const {
        return _alive;
    }
    
    inline int population() const {
        return _population;
    }
    void revisePopulation(int population);
    
    int supply() const;
    void reviseSupply(int supply);
	inline void setMaxSupply(int maxSupply) {
		_maxSupply = maxSupply;
	}
    void reviseResource(int kind, fixed amount);
private:
    void setAI(int level, bool fully, fixed lifetime);
	bool hasTeam() const;
public:
    void setControl(GameControl control);
    
    inline fixed gathering(int kind) const {
        return _gatherings[kind].calculation;
    }
    inline void gatherResource(int kind, fixed amount) {
        _gatherings[kind].current += amount;
    }

    inline fixed resourceChargeTime() const {
        return _resourceChargeTime;
    }
private:
    void updateResource();
    void updateGatherings();
public:
	TrainingState trainingState(const RuneIndex& index) const;
    void startTraining(const RuneIndex& index);
    void completeTraining(const RuneIndex& index);
    void cancelTraining(const RuneIndex& index);
    
    void update();

    inline const Card* nextCard() const {
        return _nextCard;
    }
    Spawn* spawn(int slot);
    const Spawn* spawn(int slot) const;
    Spawn* spawn(const UnitIndex& index);
    const Spawn* spawn(const UnitIndex& index) const;
    int spawnCount() const;
    bool rollCard();
    bool rollSpawn(int slot);
private:
    void updateSpawns();
private:
    void addCardImpl(Card* card);
    void removeCardImpl(UnitIndex index);
    void removeAllCardsImpl();
    void resetCardsImpl();
    void addRuneImpl(Rune* rune);
    void removeRuneImpl(RuneIndex index);
    void removeAllRunesImpl();
    void resetRunesImpl();

    int cardLevel(const Card* card) const;
    int runeLevel(const Rune* rune) const;
public:
    void addCard(Card* card);
    void removeCard(const UnitIndex& index);
    void removeAllCards();
    void resetCards();
    void addRune(Rune* rune);
    void removeRune(const RuneIndex& index);
    void removeAllRunes();
    void resetRunes();
    
    int unitSkin(const UnitIndex& index) const;
    int unitLevel(const UnitIndex& index) const;
    
    inline void setAttainValue(int i, const fixed* value) {
        _attainValues[i] = value;
    }
    inline fixed attainValue(int i) const {
        return _attainValues[i] ? *_attainValues[i] : fixed::Zero;
    }
public:
    inline bool hasEmoticon() const {
        return _emoticon != NULL;
    }
    bool showEmoticon(const EmoticonIndex& emoticon);
    void updateEmoticon(float delta);
    void drawEmoticon(CSGraphics* graphics, const CSRect& frame) const;
};

#endif /* Force_h */
