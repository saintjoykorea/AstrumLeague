//
//  Unit.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Unit_h
#define Unit_h

#include "UnitBaseGame.h"
#include "CommandReturn.h"
#include "AbilityValue.h"
#include "Resource.h"
#include "AnimationEffect.h"
#include "Growup.h"

#define MaxWorking          5
#define MaxTransporting     10
#define CooltimeCount       4
#define EffectCount         3

#define CommandSelectionFlagMultiGroup  1
#define CommandSelectionFlagAll         2

enum UnitState {
    UnitStateJoin,
    UnitStateDestroy
};

enum Navigating : byte {
    NavigatingNone,
    NavigatingPrepared,
    NavigatingRegistered
};

class Unit : public Object, public UnitBaseGame {
public:
    const uint key;
private:
    struct Motion {
        Animation* animation;
		AnimationIndex index;
        float rate;
        float remaining;
		bool rescaled;
		bool relocated;
		bool rewind;
		bool stop;
		byte buff;
		byte dir;
    };
    struct Action {
        const CSArray<byte, 2>* trigger;
        ushort ip;
        byte loop;
        Action* prev;
    };
    struct NextAction {
		CommandIndex command;
        const CSArray<byte, 2>* trigger;
        Object* target;
		bool stack;
		bool rewind;

		NextAction(const CommandIndex& command, const CSArray<byte, 2>* trigger, Object* target, bool stack, bool rewind);
		~NextAction();

		inline NextAction(const NextAction&) {
			CSAssert(false, "invalid operation");
		}
		inline NextAction& operator=(const NextAction&) {
			CSAssert(false, "invalid operation");
			return *this;
		}
    };
    struct Buff {
		BuffIndex index;
		UnitBaseGame* origin;
		Object* target;
        ushort ip;
        byte loop;
        fixed waiting;
        fixed remaining;
		fixed stack;
		fixed revision;
		fixed damage;
		uint damageCount;
		CSArray<uint>* damageOrigins;
		fixed cooltimes[CooltimeCount];
        AnimationEffect* effects[EffectCount];
        const Force* originForce;
        bool vision;
        bool cloaking;
        bool detecting;
		bool resurrecting;
		bool motion;
        Buff* next;
    };
    struct Training {
        RuneIndex indices[MaxWorking];
        byte count;
        fixed progress;
    };
    struct Producing {
        UnitIndex indices[MaxWorking];
        byte count;
        byte stuck;
        fixed progress;
        Object* rallyPoint;
    };
    struct Transporting {
        CSArray<Unit>* crews;
        CSArray<Unit>* follows;
    };
    struct Gathering {
        Unit* base;
        Resource* resource;
        bool occupied;
        byte waiting;
        byte cargoKind;
        fixed cargo;
    };
    struct Escaping {
        FPoint point;
        fixed waiting;
        bool commit;
    };
    struct Jumping {
        FPoint dest;
        FPoint move;
        float da;
        float gda;
        fixed waiting;
        bool commit;
    };
    struct RevisionTime {
        uint key;
        fixed time;
    };
    struct Speech {
        const CSString* msg;
        float remaining;
    };

    Force* const _originForce;
    Motion _motion;
	Animation* _minimap;
    Action* _action;
	CommandIndex _command;
	CSArray<NextAction>* _nextActions;
    Object* _originTarget;
    Object* _target;
	Unit* _aggro;
    Path* _path;
    Movement _movement;
    AnimationEffect* _effects[EffectCount];
    Producing* _producing;
    Training* _training;
    Transporting* _transporting;
    Gathering* _gathering;
    Escaping* _escaping;
    Jumping* _jumping;
    Buff* _buffs;
    Speech* _speech;
    Unit* _parent;
    CSArray<Unit>* _children;
    UnitIndex _originIndex;
    fixed _maxHealth;
    fixed _maxShield;
    fixed _health;
    fixed _shield;
    fixed _resurrection;
    fixed _maxMana;
    fixed _mana;
	fixed _revision;
	CSArray<RevisionTime>* _revisionTimes;
	fixed _damage;
    fixed _damaging;
	uint _damageCount;
	CSArray<uint>* _damageOrigins;
	fixed _sight;
    fixed _waiting;
    fixed _waitingReserved;
    fixed _freezing;
    fixed _progress;
	fixed _aggroDuration;
	fixed _scale;
	fixed _scaleBase;
	fixed _scaleUp;
    FPoint _visionPoint;
    FPoint* _originPoint;
    
    const UnitData* _data;      //caching for performance

    struct Watching {
        const void* trigger;
        Unit* target;
    };
    CSArray<Watching>* _watchings;
	CSArray<uint>* _watchingExclusions;

    struct {
        fixed initial;
        fixed remaining;
        bool displaying;
    } _cooltimes[CooltimeCount];
    
    CSArray<const Growup>* _growups;
    AbilityValue<float>* _rangeDisplayValue;
    float _rangeDisplay;
    mutable float _distanceDisplay;
    float _healthDisplay;
    float _z;
	uint _resurrecting;
	ushort _originLevel;
    ushort _exp;
    ushort _kill;
    byte _state;
    byte _originState;
    byte _supply;
    byte _pathDelay;
    byte _motions[MotionCount];
    Building _building;
    ushort _reserveBuildingCount;
    Navigating _navigating;
    
    enum WaitingState : byte {
        WaitingStateNone,
        WaitingStateInterrupt,
        WaitingStateStop,
        WaitingStateFreeze
    };
    WaitingState _waitingState;
    bool _waitingReservedOn;
    
    enum Alive : byte {
        AliveNone,
        AliveNo,
        AliveYes,
        AliveResurrect
    } _alive;
	byte _busy;
    bool _cloaking;
    bool _detecting;
    bool _commandRewind;
    bool _forceDisposed;
    mutable bool _dirtyui;
    mutable float _alpha;

    const CSString* _name;
    CSArray<AbilityRevision>* _abilities;
    
    Unit(Force* force, const UnitIndex& index, int level, int skin, Unit* parent);
    ~Unit();
public:
#ifdef GameImpl
    static Unit* unit(Force* force, const UnitIndex& index, int level, int skin, Unit* parent);
#endif
    void deactivate();
    
    void dispose() override;
    
    void destroy();

    //property
	inline Unit* asUnit() override {
		return this;
	}
    inline const UnitData& data() const override {
        return *_data;
    }
    inline const SkinData* skinData() const override {
        return _data->skin(skin);
    }
	inline ObjectType type() const override {
		return _type;
	}
	bool setType(ObjectType type);
	
    void setCollision(bool collision);
    
    inline Movement* movement() override {
        return &_movement;
    }
    
    CSVector3 worldPoint(ObjectLayer layer) const override;
    
    const CSColor& color(bool visible, bool detecting) const override;
    
    inline Unit* parent() {
        return _parent;
    }
    inline const Unit* parent() const override {
        return _parent;
    }
    void setForce(Force* force, int level);
	inline void setForce(Force* force) {
		setForce(force, level);
	}
    inline void resetForce() {
        setForce(_originForce, _originLevel);
    }
    bool setIndex(const UnitIndex& index, int state = UnitStateJoin);
    inline bool resetIndex() {
        return setIndex(_originIndex, _originState);
    }
    inline const UnitIndex& originIndex() const {
        return _originIndex;
    }
    inline fixed health() const override {
        return _health + _shield;
    }
    inline fixed maxHealth() const override {
        return _maxHealth + _maxShield;
    }
    inline fixed mana() const override {
        return _mana;
    }
    inline fixed maxMana() const override {
        return _maxMana;
    }
    inline int state() const override {
        return _state;
    }
    inline Building building() const override {
        return _building;
    }
    inline fixed sight() const {
        return _sight;
    }
    inline bool isAlive() const override {
        return _alive == AliveYes;
    }
    inline bool isDead() const override {
        return _alive == AliveNo;
    }
    bool isCloaking() const override;
    bool isDetecting() const;
    void setDetecting(bool detecting);
    void setDetecting(bool detecting, Buff* buff);
	
private:
    void changeVision(const FPoint& point, fixed sight, bool visible, bool lock = true);
    inline void changeVision(bool visible) {
        changeVision(_visionPoint, _sight, visible, true);
    }
public:
    void locate(const FPoint& point) override;
    void unlocate() override;
    bool isDetected(int alliance) const;
    bool isDetected(const Force* otherForce) const;
    bool isVisible(int alliance) const override;
    bool isVisible(const Force* otherForce) const override;
	bool isHiding() const;

    ObjectDisplay display() const override;
public:
    inline Object* target() const {
        return _target;
    }
    
    int producingCount(const UnitIndex& index) const;
	bool isProducing() const override;
    inline bool isTraining() const override {
        return _training != NULL;
    }
	bool isTransporting() const override;
	bool isGathering() const override;
	bool isGathering(int kind) const override;
    inline const FPoint& reachablePoint() const override {
        return _point;
    }
    bool isReachable(const FPoint& point) const override;
    int childrenCount() const override;

private:
    fixed abilityElement(int index, fixed value, const UnitBase* target) const override;
    
    const CSArray<BuffIndex>* buffs() const override;
    // degree / affect
private:
    fixed recover(fixed value);
public:
    fixed revise(int op, fixed value, fixed time, UnitBaseGame* src);
    void resurrect(fixed value);
    fixed attack(Unit* target, fixed rate, fixed time) override;
    void freeze(fixed time);
    void escape(const FPoint& point, fixed time);
    void jump(const FPoint& point, fixed distance, float altitude, fixed time, bool pushing);
    bool warp(const FPoint& point);
private:
    bool commitFreezing();
    bool commitEscaping();
    bool commitJumping();
    bool updateEscaping();
    bool updateJumping();
    
    // map setting
public:
    const CSString* name() const;
    void rename(const CSString* name);
    
    inline CSArray<AbilityRevision>*& abilities() {
        assertOnLocked();
        return _abilities;
    }
    void obtainExp(int exp);
    inline int exp() const {
        return _exp;
    }
    void addGrowup(const Growup* growup);
    void clearGrowups();
    
    // motion
private:
    void spriteElement(CSGraphics* graphics, const byte* data);
public:
    inline int motionDirection() const {
        return _motion.dir;
    }
	Animation* captureMotion() const;
    void setMotion(const AnimationIndex& index, float rate, bool rewind, bool buff);
    bool turnMotion(int dir);
    void setBaseMotion(int index, bool buff);
	void cancelBaseMotion(int from, int to);
	void stopMotion(bool buff);
	void removeMotion(bool buff);
private:
    void locateMotion();
    void updateMotion();

	//scale
public:
	void updateScale();
	inline void resetScale() {
		_scaleBase = fixed::One;
		_scaleUp = fixed::Zero;
		updateScale();
	}
	inline void setScaleBase(fixed scaleBase) {
		_scaleBase = scaleBase;
		updateScale();
	}
	inline void setScaleUp(fixed scaleUp) {
		_scaleUp = scaleUp;
		updateScale();
	}
	inline void setScale(fixed scaleBase, fixed scaleUp) {
		_scaleBase = scaleBase;
		_scaleUp = scaleUp;
		updateScale();
	}

    // buff
private:
    void removeBuff(Buff*& buff, bool lock);
public:
    fixed buffStack(const BuffIndex& index) const override;
    bool isBuffed(const BuffIndex& index, fixed stack) const override;
    void clearBuffs(bool lock);
    void buff(const BuffIndex& index, int timeOp, fixed time, fixed stack, UnitBaseGame* origin);
private:
    void updateBuffs();

private:
    // call
    enum SendCallReturn {
        SendCallReturnNone,
        SendCallReturnFar,
        SendCallReturnNear
    };
    SendCallReturn sendCall(Unit* src, bool commit);
    
    // command
public:
    CommandReturn command(const CommandIndex& key, Object* target, fixed& estimation, int selectionFlag, int commit, bool stack);
    void cancelBuildingCommand();
    void cancelProducingCommand(int slot);
    bool cancelProducingCommand(const UnitIndex& index);
    void cancelTrainingCommand(int slot);
    void unloadCommand(int slot);
    // action
private:
    void clearEffects();
    void commandAction(CommandStack cs, const CommandIndex& command, const CSArray<byte, 2>* actionTrigger, Object* target, bool stack);
    void nextAction(const CSArray<byte, 2>* trigger, Object* target, bool stack, bool rewind);
    void cancelAction();
    void changeAction();
    bool interruptAction();
public:
    inline Navigating navigating() const {
        return _navigating;
    }
    
    inline bool isDamaging() const {
        return _damaging != fixed::Zero;
    }

public:
    // building
    enum StartBuilding {
        StartBuildingDependent = BuildingDependent,
        StartBuildingIndependent = BuildingIndependent,
        StartBuildingTransform = BuildingTransform
    };
    bool startBuilding(StartBuilding param);
    
    enum StopBuilding {
        StopBuildingTarget,
        StopBuildingCancel,
        StopBuildingDestroy
    };
    
    bool stopBuilding(StopBuilding param);
private:
    bool reserveBuilding();
    bool resumeBuilding();
    
    void updateBuilding();
    
    // etc
private:
    void updateSpeech();
    void updateRevisions();
    void updateProducing();
    void updateTraining();
    void updateTransporting();
    void updateDegrees();
    void updateSight();
    bool updateWaiting();
    bool endAction();
public:
    bool update(bool running) override;
private:
    float damagingDisplay() const;
    CSColor motionColor() const;
    CSColor selectionColor() const;
    void drawBottomCursor(CSGraphics* graphics);
    void drawTopCursor(CSGraphics* graphics);
public:
    void drawRallyPoint(CSGraphics* graphics, CSArray<const Object>* targets);
    void draw(CSGraphics* graphics, ObjectLayer layer) override;
	void drawMinimap(CSGraphics* graphics, float scale, float delta);
public:
    inline void setDistanceDisplay(float distance) const {
        _distanceDisplay = distance;
    }
    void addSpeech(const CSString* msg);
    void removeSpeech();
    float speechRemaining() const;
public:
    float progress() const;
    int workingCount() const;

    //buff trigger
private:
	AnimationIndex btAnimationIndex(Buff* buff, const byte*& cursor);

    void btMotion(Buff* buff, const byte*& cursor);
    void btBaseMotion(Buff* buff, const byte*& cursor);
    void btChangeBaseMotion(Buff* buff, const byte*& cursor);
    void btEffect(Buff* buff, const byte*& cursor);
    void btRemoveEffect(Buff* buff, const byte*& cursor);
    void btEffectRange(Buff* buff, const byte*& cursor);
	void btEffectAngle(Buff* buff, const byte*& cursor);
	void btEffectLine(Buff* buff, const byte*& cursor);
    void btAttack(Buff* buff, const byte*& cursor);
    void btAttackRange(Buff* buff, const byte*& cursor);
	void btAttackAngle(Buff* buff, const byte*& cursor);
	void btAttackLine(Buff* buff, const byte*& cursor);
	void btWatch(Buff* buff, const byte*& cursor);
    void btCooltime(Buff* buff, const byte*& cursor);
    bool btIsCooltime(Buff* buff, const byte*& cursor);
    void btCompare(Buff* buff, const byte*& cursor);
    void btRevise(Buff* buff, const byte*& cursor);
    void btReviseRange(Buff* buff, const byte*& cursor);
	void btReviseAngle(Buff* buff, const byte*& cursor);
    void btReviseLine(Buff* buff, const byte*& cursor);
	void btReviseReset(Buff* buff, const byte*& cursor);
	void btIsLevel(Buff* buff, const byte*& cursor);
    void btHasUnit(Buff* buff, const byte*& cursor);
    void btHasUnits(Buff* buff, const byte*& cursor);
    void btHasRune(Buff* buff, const byte*& cursor);
    void btIsDamaged(Buff* buff, const byte*& cursor);
    void btIsState(Buff* buff, const byte*& cursor);
	void btChangeState(Buff* buff, const byte*& cursor);
	void btIsType(Buff* buff, const byte*& cursor);
	void btChangeTarget(Buff* buff, const byte*& cursor);
	void btHasTarget(Buff* buff, const byte*& cursor);
	void btIsTarget(Buff* buff, const byte*& cursor);
    void btIsBuffed(Buff* buff, const byte*& cursor);
    void btBuff(Buff* buff, const byte*& cursor);
    void btBuffRange(Buff* buff, const byte*& cursor);
	void btBuffAngle(Buff* buff, const byte*& cursor);
	void btBuffLine(Buff* buff, const byte*& cursor);
    void btCloak(Buff* buff, const byte*& cursor);
    void btDetect(Buff* buff, const byte*& cursor);
	void btIsAmbush(Buff* buff, const byte*& cursor);
	void btEscape(Buff* buff, const byte*& cursor);
    void btEscapeRange(Buff* buff, const byte*& cursor);
	void btEscapeAngle(Buff* buff, const byte*& cursor);
	void btEscapeLine(Buff* buff, const byte*& cursor);
    void btJump(Buff* buff, const byte*& cursor);
    void btJumpRange(Buff* buff, const byte*& cursor);
	void btJumpAngle(Buff* buff, const byte*& cursor);
	void btJumpLine(Buff* buff, const byte*& cursor);
    void btFreeze(Buff* buff, const byte*& cursor);
    void btFreezeRange(Buff* buff, const byte*& cursor);
	void btFreezeAngle(Buff* buff, const byte*& cursor);
	void btFreezeLine(Buff* buff, const byte*& cursor);
    void btCreate(Buff* buff, const byte*& cursor);
    void btTransform(Buff* buff, const byte*& cursor);
    void btRetransform(Buff* buff, const byte*& cursor);
    void btAttach(Buff* buff, const byte*& cursor);
    void btDetach(Buff* buff, const byte*& cursor);
    void btIsBuilding(Buff* buff, const byte*& cursor);
    void btRandom(Buff* buff, const byte*& cursor);
    void btWatchRandom(Buff* buff, const byte*& cursor);
    void btChildrenCount(Buff* buff, const byte*& cursor);
    void btVision(Buff* buff, const byte*& cursor);
	void btResurrect(Buff* buff, const byte*& cursor);
    void btEvent(Buff* buff, const byte*& cursor);
    void btSound(Buff* buff, const byte*& cursor);
	void btAnnounce(Buff* buff, const byte*& cursor);
    void btVibrate(Buff* buff, const byte*& cursor);
    void btMessage(Buff* buff, const byte*& cursor);
    void btStack(Buff* buff, const byte*& cursor);
    void btIsRemaining(Buff* buff, const byte*& cursor);
    void btLoop(Buff* buff, const byte*& cursor);
    void btWait(Buff* buff, const byte*& cursor);
    bool btStop(Buff* buff, const byte*& cursor);
    
    bool updateBuff(Buff* buff);
    
    //command trigger
private:
    struct CommandTriggerState {
        CommandIndex key;
        Object** target;
        byte commit;
        byte selectionFlag;
        ushort ip;
        fixed* estimation;
        const byte* cursor;
        CommandReturn rtn;
    };
    
    void ctEffect(CommandTriggerState& cts);
    void ctEffectRange(CommandTriggerState& cts);
    void ctEffectAngle(CommandTriggerState& cts);
    void ctEffectLine(CommandTriggerState& cts);
    void ctAttack(CommandTriggerState& cts);
    void ctAttackRange(CommandTriggerState& cts);
    void ctAttackAngle(CommandTriggerState& cts);
    void ctAttackLine(CommandTriggerState& cts);
    void ctCompare(CommandTriggerState& cts);
    void ctRevise(CommandTriggerState& cts);
    void ctReviseRange(CommandTriggerState& cts);
    void ctReviseAngle(CommandTriggerState& cts);
    void ctReviseLine(CommandTriggerState& cts);
    void ctIsLevel(CommandTriggerState& cts);
    void ctHasUnit(CommandTriggerState& cts);
    void ctHasUnits(CommandTriggerState& cts);
    void ctHasTier(CommandTriggerState& cts);
    void ctHasRune(CommandTriggerState& cts);
    void ctIsState(CommandTriggerState& cts);
    void ctChangeState(CommandTriggerState& cts);
    void ctMaintainTarget(CommandTriggerState& cts);
    void ctHasTarget(CommandTriggerState& cts);
    void ctIsTarget(CommandTriggerState& cts);
    void ctIsBuffed(CommandTriggerState& cts);
    void ctBuff(CommandTriggerState& cts);
    void ctBuffRange(CommandTriggerState& cts);
    void ctBuffAngle(CommandTriggerState& cts);
    void ctBuffLine(CommandTriggerState& cts);
    void ctCloak(CommandTriggerState& cts);
    void ctDetect(CommandTriggerState& cts);
	void ctIsAmbush(CommandTriggerState& cts);
	void ctIsCallable(CommandTriggerState& cts);
    void ctIsTransporting(CommandTriggerState& cts);
    void ctIsResource(CommandTriggerState &cts);
	void ctIsBuilding(CommandTriggerState& cts);
    void ctIsProducing(CommandTriggerState& cts);
    void ctIsTraining(CommandTriggerState& cts);
    void ctRandom(CommandTriggerState& cts);
    void ctWatchRandom(CommandTriggerState& cts);
    void ctChildrenCount(CommandTriggerState& cts);
    void ctEvent(CommandTriggerState& cts);
    void ctSound(CommandTriggerState& cts);
	void ctAnnounce(CommandTriggerState& cts);
    void ctVibrate(CommandTriggerState& cts);
    void ctMessage(CommandTriggerState& cts);
	bool ctAttackEstimation(CommandTriggerState& cts);
	bool ctReviseEstimation(CommandTriggerState& cts);
	void ctIsSelecting(CommandTriggerState& cts);
	void ctIsCommanding(CommandTriggerState& cts);
	void ctEmpty(CommandTriggerState& cts);
    void ctUnabled(CommandTriggerState& cts);
    bool ctTarget(CommandTriggerState& cts);
    bool ctRange(CommandTriggerState& cts);
    bool ctAngle(CommandTriggerState& cts);
    bool ctLine(CommandTriggerState& cts);
    void ctRallyPoint(CommandTriggerState& cts);
    void ctProduce(CommandTriggerState& cts);
    void ctTrain(CommandTriggerState& cts);
    void ctBuild(CommandTriggerState& cts);
    bool ctCooltime(CommandTriggerState& cts);
    void ctEnabled(CommandTriggerState& cts);
	void ctStop(CommandTriggerState& cts);

    CommandReturn executeCommand(const CommandIndex& key, const CSArray<byte, 2>* commandTrigger, Object** target, fixed& estimation, int selectionFlag, int commit);
    
    //action trigger
private:
    void setWatching(const void* trigger, Unit* target);
    bool getWatching(const void* trigger, Unit*& target);
    void releaseWatchings();
    void updateWatchings();
    bool isWatchable(const Unit* target) const;
    
    void atMotion(const byte*& cursor);
    void atBaseMotion(const byte*& cursor);
	void atCancelBaseMotion(const byte*& cursor);
    void atChangeBaseMotion(const byte *&cursor);
    void atProgressMotion(const byte*& cursor);
    bool atTurn(const byte*& cursor);
    void atEffect(const byte*& cursor);
    void atRemoveEffect(const byte*& cursor);
    void atEffectRange(const byte*& cursor);
    void atEffectAngle(const byte*& cursor);
    void atEffectLine(const byte*& cursor);
    void atAttackWatch(const byte*& cursor);
    void atAttack(const byte*& cursor);
    void atAttackRange(const byte*& cursor);
    void atAttackAngle(const byte*& cursor);
    void atAttackLine(const byte*& cursor);
    void atWatch(const byte*& cursor);
	void atWatchExclude(const byte*& cursor);
    void atCooltime(const byte*& cursor);
    bool atIsCooltime(const byte*& cursor);
    void atShowCooltime(const byte*& cursor);
    void atCompare(const byte*& cursor);
    void atRevise(const byte*& cursor);
    void atReviseRange(const byte*& cursor);
    void atReviseAngle(const byte*& cursor);
    void atReviseLine(const byte*& cursor);
    void atReviseReset(const byte*& cursor);
    void atResurrect(const byte*& cursor);
    void atIsLevel(const byte*& cursor);
    void atHasUnit(const byte*& cursor);
    void atHasUnits(const byte*& cursor);
    void atHasRune(const byte*& cursor);
    void atIsDamaged(const byte*& cursor);
    void atIsState(const byte*& cursor);
    void atChangeState(const byte*& cursor);
    void atChangeType(const byte*& cursor);
	void atChangeBusy(const byte*& cursor);
    void atChangeTarget(const byte*& cursor);
    void atHasTarget(const byte*& cursor);
    void atIsTarget(const byte*& cursor);
    void atIsBuffed(const byte*& cursor);
    void atBuff(const byte*& cursor);
    void atBuffRange(const byte*& cursor);
    void atBuffAngle(const byte*& cursor);
    void atBuffLine(const byte*& cursor);
    void atCloak(const byte*& cursor);
    void atDetect(const byte*& cursor);
	void atIsAmbush(const byte*& cursor);
	bool atMove(const byte*& cursor);
    bool atFlock(const byte*& cursor);
    void atCollision(const byte*& cursor);
    void atPoint(const byte*& cursor);
    bool atEscape(const byte*& cursor);
    bool atEscapeRange(const byte*& cursor);
    bool atEscapeAngle(const byte*& cursor);
    bool atEscapeLine(const byte*& cursor);
    bool atJump(const byte*& cursor);
    bool atJumpRange(const byte*& cursor);
    bool atJumpAngle(const byte*& cursor);
    bool atJumpLine(const byte*& cursor);
    void atWarpTo(const byte*& cursor);
    void atWarpFrom(const byte*& cursor);
    void atWarpRangeTo(const byte*& cursor);
    void atWarpRangeFrom(const byte*& cursor);
    bool atFreeze(const byte*& cursor);
    bool atFreezeRange(const byte*& cursor);
    bool atFreezeAngle(const byte*& cursor);
    bool atFreezeLine(const byte*& cursor);
    bool atBuild(const byte*& cursor);
    void atCreate(const byte*& cursor);
    void atTransform(const byte*& cursor);
    void atCancel(const byte*& cursor);
    void atCopy(const byte*& cursor);
	void atAttachTarget(const byte*& cursor);
	void atDetachTarget(const byte*& cursor);
    void atAttach(const byte*& cursor);
    void atDetach(const byte*& cursor);
    void atIsTransporting(const byte *&cursor);
    void atFindCall(const byte*& cursor);
    bool atSendCall(const byte*& cursor);
    void atReceiveCall(const byte*& cursor);
    void atUnload(const byte*& cursor);
    void atIsResource(const byte*& cursor);
    void atFindResource(const byte*& cursor);
    bool atOccupyResource(const byte*& cursor);
    void atGatherResource(const byte*& cursor);
    bool atFindBase();
    void atFindBase(const byte*& cursor);
    void atReturnCargo(const byte*& cursor);
    void atHasCargo(const byte*& cursor);
    void atReturnGathering(const byte*& cursor);
    void atRefineryEnabled(const byte*& cursor);
    void atRefineryOccupied(const byte*& cursor);
    void atIsBuilding(const byte*& cursor);
    void atIsProducing(const byte*& cursor);
    void atIsTraining(const byte*& cursor);
    void atIsAlliance(const byte*& cursor);
    void atNavigate(const byte*& cursor);
    void atSupply(const byte*& cursor);
    void atAdvance(const byte*& cursor);
    void atRetreat(const byte*& cursor);
    void atRandom(const byte*& cursor);
    void atWatchRandom(const byte*& cursor);
    void atChildrenCount(const byte*& cursor);
    void atChildrenTarget(const byte*& cursor);
	void atChildrenCommand(const byte*& cursor);
    void atParentInRange(const byte*& cursor);
    void atDisplayRange(const byte*& cursor);
    void atTile(const byte*& cursor);
    void atEvent(const byte*& cursor);
    void atSound(const byte*& cursor);
	void atAnnounce(const byte*& cursor);
    void atVibrate(const byte*& cursor);
    void atMessage(const byte*& cursor);
    void atLoop(const byte*& cursor);
    void atWaitReserved(const byte*& cursor);
    bool atWait(const byte*& cursor);
    bool atStop(const byte*& cursor);

    bool updateAction();
    
    AbilityValue<byte> readAbilityByte(const byte*& cursor);
    AbilityValue<ushort> readAbilityShort(const byte*& cursor);
    AbilityValue<float> readAbilityFloat(const byte*& cursor);
    AbilityValue<fixed> readAbilityFixed(const byte*& cursor);
    AbilityValue<byte> readAbilityByte(const Buff* buff, const byte*& cursor);
    AbilityValue<ushort> readAbilityShort(const Buff* buff, const byte*& cursor);
    AbilityValue<float> readAbilityFloat(const Buff* buff, const byte*& cursor);
    AbilityValue<fixed> readAbilityFixed(const Buff* buff, const byte*& cursor);
	UnitIndex readRandomUnitIndex(const byte*& cursor);
public:
	static void drawGage(CSGraphics* graphics, const CSRect& rect, float progress, const CSColor& color, float ext = 1.0f);
};

#endif /* Unit_h */

