//
//  Spawn.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 5. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Spawn_h
#define Spawn_h

#include "UnitBaseGame.h"
#include "Object.h"
#include "CommandReturn.h"
#include "AbilityValue.h"
#include "Growup.h"

class Spawn : public CSObject, public UnitBaseGame {
private:
    fixed _remaining;

	const CSString* _name;
	CSArray<AbilityRevision>* _abilities;
public:
    Spawn(Force* force, const UnitIndex& index, int level, int skin);
private:
    ~Spawn();
public:
    static inline Spawn* spawn(Force* force, const UnitIndex& index, int level, int skin) {
        return autorelease(new Spawn(force, index, level, skin));
    }
    void activate();
    
	inline Spawn* asSpawn() override {
		return this;
	}

	const CSString* name() const;
	void rename(const CSString* name);
	inline CSArray<AbilityRevision>*& abilities() {
		assertOnLocked();
		return _abilities;
	}
private:
	fixed abilityElement(int index, fixed value, const UnitBase* target) const override;
public:
	fixed attack(Unit* target, fixed rate, fixed time) override;
private:
    struct TriggerState {
        Object* target;
        bool commit;
        ushort ip;
        const byte* cursor;
        CommandReturn rtn;
    };
    void stEffect(TriggerState& ts);
    void stEffectRange(TriggerState& ts);
    void stEffectAll(TriggerState& ts);
    void stAttack(TriggerState& ts);
    void stAttackRange(TriggerState& ts);
    void stAttackAll(TriggerState& ts);
    void stCompare(TriggerState& ts);
    void stRevise(TriggerState& ts);
    void stReviseRange(TriggerState& ts);
    void stReviseAll(TriggerState& ts);
    void stBuff(TriggerState& ts);
    void stBuffRange(TriggerState& ts);
    void stBuffAll(TriggerState& ts);
    void stIsLevel(TriggerState& ts);
    void stHasUnit(TriggerState& ts);
    void stHasUnits(TriggerState& ts);
    void stHasRune(TriggerState& ts);
    void stIsTarget(TriggerState& ts);
    void stEvent(TriggerState& ts);
    void stSound(TriggerState& ts);
	void stAnnounce(TriggerState& ts);
    void stVibrate(TriggerState& ts);
    void stMessage(TriggerState& ts);
    void stStop(TriggerState& ts);
    void stEmpty(TriggerState &ts);
    void stUnabled(TriggerState& ts);
    bool stTarget(TriggerState& ts);
    bool stRange(TriggerState& ts);
    void stBuild(TriggerState& ts);
    void stEnabled(TriggerState& ts);
public:
    CommandReturn run(Object* target, bool commit);

    void update();
private:
    AbilityValue<byte> readAbilityByte(const byte*& cursor);
    AbilityValue<ushort> readAbilityShort(const byte*& cursor);
    AbilityValue<float> readAbilityFloat(const byte*& cursor);
    AbilityValue<fixed> readAbilityFixed(const byte*& cursor);
};

#endif /* Spawn_h */
