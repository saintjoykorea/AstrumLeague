//
//  Map.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Map_h
#define Map_h

#include "Terrain.h"
#include "NavMesh.h"
#include "Vision.h"
#include "Grid.h"
#include "Tile.h"
#include "Area.h"
#include "Event.h"
#include "Speech.h"
#include "Squad.h"
#include "Message.h"
#include "Mission.h"
#include "Action.h"
#include "Camera.h"
#include "Unit.h"
#include "Resource.h"
#include "FogBuilding.h"
#include "Frame.h"
#include "Navigation.h"
#include "Spawn.h"
#include "Growup.h"
#include "GameAnimation.h"
#include "Valuations.h"

#include "GameConfig.h"

#include "Animations.h"

#define MaxNumberings       7

#define TargetRange         1
#define DragCheckRange      2
#define DragRange           1

enum PingLevel : byte {
    PingLevel_0,
    PingLevel_1,
    PingLevel_2,
    PingLevel_3,
    PingLevelDisconnected
};

#define PVPPingDelay_0			0.05f
#define PVPPingDelay_1			0.15f
#define PVPPingDelay_2			0.25f

#define PVPPingLevelUpdateDuration		2.0f
#define PVPLatencyDecrementPerSecond	0.05f
#define PVPMaxLatency					0.5f

//TODO:SNAPSHOT 구현시 256kb 소모예상 (타일은 다른 식으로 필요)

enum CommandSet {
    CommandSetTarget,
    CommandSetMain,
	CommandSetSub,
    CommandSetBuildProduce,
    CommandSetAbility,
    CommandSetTrain
};

enum GameLockOp {
	GameLockMap = 1,
	GameLockObject,
	GameLockEffect,
	GameLockForce
};

enum MapLocation {
    MapLocationEnabled,
    MapLocationUnabled,
    MapLocationCollided
};

enum MapState : byte {
    MapStateNone,
    MapStateAuto,
    MapStateVictory,
    MapStateDefeat,
    MapStateEnd,
	MapStateInterrupt
};

enum MapControl {
    MapControlStatus,
    MapControlMinimap,
    MapControlScroll,
    MapControlHorizontalScroll,
    MapControlVerticalScroll,
    MapControlZoom,
    MapControlSelect,
    MapControlDragSelect,
    MapControlClickSelect,
    MapControlFocusSelection,
    MapControlCancelSelection,
    MapControlSelectAttackers,
    MapControlSelectBarracks,
    MapControlSelectLabors,
    MapControlSelection,
    MapControlNavigation,
    MapControlNumbering,
    MapControlCancelNumbering,
    MapControlCommand,
    MapControlCancelProducingCommand,
    MapControlCancelBuildingCommand,
    MapControlCancelTrainingCommand,
    MapControlUnloadCommand,
    MapControlSpawn,
    MapControlMenu,
    MapControlAuto,
    MapControlSpeed,
    MapControlMessage,
    MapControlMinimapExpand,
	MapControlPlayTime,
    MapControlCount
};

enum MapControlState : byte {
    MapControlStateHidden,
    MapControlStateUnabled,
    MapControlStateEnabled
};

struct MapControlStateCommand {
    MapControlState state;
	const ActionTarget* target;

	MapControlStateCommand(MapControlState state, const ActionTarget* target);
	MapControlStateCommand(const MapControlStateCommand& other);
	~MapControlStateCommand();

	MapControlStateCommand operator=(const MapControlStateCommand& other);
};

enum MapAttainOrder : byte {
    MapAttainOrderNone,
    MapAttainOrderAscent,
    MapAttainOrderDescent
};

struct Warning {
    ImageIndex icon;
    CSVector2 point;
    float remaining;
    bool essential;
};

#define MapGraphInterval	3

#define MapGraphResourceGatheringWeight		300

enum MapGraphType {
	MapGraphTypeResource,
	MapGraphTypeArmy,
	MapGraphTypeCount
};

struct MapGraph {
	float rate[MaxPlayers][MapGraphTypeCount];
};

enum SpawnCommit {
    SpawnCommitNone,
    SpawnCommitRun,
    SpawnCommitRunAuto
};

enum MapLightState : byte {
    MapLightStateOff,
    MapLightStateOffGrid,
    MapLightStateOffSpawn,
    MapLightStateOn
};

#define SpawnWidth      10       //TODO:TEMP
#define SpawnHeight     5      //TODO:TEMP

class Map {
public:
    Animations decorations;
private:
    MapIndex _index;
    const CSLocaleString* _name;
    const CSLocaleString* _description;
    GameType _type;
	ImageIndex _background;
	ImageIndex _thumbnail;
	AnimationIndex _button;
    Terrain* _terrain;
	Vision* _vision;
    CSScratch* _minimap;
    CSArray<IPoint>* _visionPoints;
    CSArray<IPoint>* _minimapPoints;
	float _minimapRemaining;
    Tile* _tiles;
    NavMesh* _navMesh;
    Grid* _grid;
    CSArray<Object>* _objects;
    CSArray<Unit>* _units;
	CSArray<Unit>* _minimapUnits;
    CSArray<Resource>* _resources;
    CSArray<FogBuilding>* _fogBuildings;
    CSArray<Squad, 2>* _squads;
    CSArray<Frame>* _buildingLocations;
    const CSArray<FPoint>* _points;
    const CSArray<Area>* _areas;
    const CSArray<byte, 3>* _triggers;
    Force* _forces[MaxPlayers];
    Force* _force;
    uint _key;
    uint _frame;
    float _playTime;
    mutable uint _mark;
    mutable byte _findPathCount;
	byte _visionFrameDivision;
#ifdef UseDynamicSecretTable
	//ushort _secretResetSeq;
#endif
	enum : byte {
        PlayingObserveAll,
        PlayingObserveVision,
        PlayingParticipate
    } _playing;
    enum : byte {
        AliveFinish,
        AlivePause,
        AliveActive
    } _alive;
    bool _running;
    bool _updated;
	bool _paused;
    MapState _state;
    GameLevelCorrection _levelCorrection;
    ushort _maxCardLevel;
    ushort _maxRuneLevel;
    float _offscreenProgress;
    const CSImage* _offscreenCapture;
    CSArray<const Message>* _messages;
    CSArray<Mission>* _missions;
    uint _missionSeq;
    CSArray<Event>* _events;
    Warning _warning;
    Speech* _speech;
    CSArray<AnimationEffect>* _effects[EffectCount];
    CSDictionary<byte, GameAnimation, false>* _animations;
    struct SoundEntry {
        SoundIndex index;
		bool essential;
        float volume;
        float time;
    };
	CSArray<SoundEntry>* _sounds;
    Animation* _distortionAnimation;
    Animation* _rallyPointAnimation;
    Valuations _valuations;
    
    struct {
        MapControlState states[MapControlCount];
        CSDictionary<uint, MapControlState>* selectionStates;
        CSDictionary<CommandIndex, MapControlStateCommand>* commandStates;
        CSDictionary<UnitIndex, MapControlStateCommand>* spawnStates;
    } _control;
    struct {
        float level;
        float seq;
    } _vibration;
    struct {
        CSColor current;
        CSColor delta;
        float time;
        float lightDegree;
        MapLightState lightState;
    } _color;
    fixed* _timer;
	MapAttainOrder _attainOrders[3];
    struct UnitSetting {
        const CSArray<Force>* forces;
        const byte* condition;
        CSArray<const Growup>* growups;
        const CSString* name;
        CSArray<AbilityRevision>* abilities;
		fixed scaleBase;
		fixed scaleUp;
        
        UnitSetting(const CSArray<Force>* forces, const byte* condition);
        ~UnitSetting();
        
        inline UnitSetting(const UnitSetting&) {
            CSAssert(false, "invalid operation");
        }
        inline UnitSetting& operator=(const UnitSetting&) {
            CSAssert(false, "invalid operation");
            return *this;
        }
    };
    CSArray<UnitSetting>* _unitSettings;
    
    struct Setting {
        struct Thread {
            struct Stack {
                ushort func;
                ushort ip;
                Stack* prev;
                
                Stack(int func);
            };
            int threadId;
            Stack* current;
            fixed waiting;
			bool skip;
            CSArray<Unit>* units;
            
            Thread(int threadId, int func);
            ~Thread();
        };
        int threadId;
        CSArray<Thread*>* threads;
        struct Timer {
            fixed remaining;
            bool set;
        } timers[16];
        fixed values[1000];
		bool skipCommit;
		bool skipReady;
		bool voiceEnabled;
		bool soundEnabled;

        Setting();
        ~Setting();
    };
    Setting* _setting;
    
    Camera* _camera;
    struct {
        CSArray<Unit, 2>* groupedSelections;
        CSArray<Unit>* expandedSelections;
        CSArray<Unit>* selections;
        CSArray<Unit>* dragTargets;
        CSArray<Effect>* dragEffects;
        CSArray<Navigation>* navigations;
        CSArray<Unit, 2>* numberings[MaxNumberings];
        CSArray<Effect>* clickEffects;
        Object* target;
        uint mask;
        bool dragging;
        bool following;
    } _selection;
    
    struct {
        fixed frameDelayFixed;
        float frameDelayFloat;
        byte framePerSecond;
        byte localFramePerSecond;
        int updateSeq;
        float updateLatecy;
        float updateAccumLatency;
        float timeoutLatency;
    } _latency;
    
    struct {
        GameMode code;
        GameModeType type;
        GameMode originCode;
        union {
            struct {
                Action* action;
                byte actionRestriction;
                byte speed;
				bool skipReady;
				bool skipCommit;
            } single;
            struct {
                CSBuffer* localBuffer;
                CSBuffer* globalBuffer;
                uint maxFrame;
                float latency;
                float currentLatency;
                float totalLatency;
                uint totalLatencyCount;
                double commandElapsed;
                byte framePerStep;
				PingLevel pingLevel;
                ushort observerDelayFrame;
                bool chatBlocked;
                bool chatToTeam;
                bool teamPlaying;
                bool partyPlaying;
#ifdef UsePVPTest
                struct {
                    float latency;
                    float jump;
                    float response;
                    uint seq;
                } stat;
#endif
            } online;
            struct {
                byte speed;
                float totalTime;
                uint maxFrame;
                uint nextFrame;
                uint dataPosition;
                CSBuffer* buffer;
            } replay;
        } context;
    } _mode;
    CSRandom* _random;
	CSArray<MapGraph>* _graph;
	float _graphInterval;
    
    CSResourceDelegate* _preloadDelegate;
    CSArray<ImageGroupIndex>* _preloadImages;
    
    CSThread* _updateThread;
    mutable CSLock _updateLock;
	mutable CSLock _markLock;
	mutable CSLock _phaseLock;
    mutable CSLock _selectionLock;
    mutable CSLock _controlLock;
	mutable CSLock _navMeshLock;
#ifdef UseLoggerPath
public:
	CSLogger* logger;
private:
#endif
    static Map* __map;
    
    struct ForceParam {
        const Player* player;
        GameControl control;
    };
    
    Map(const MapIndex& index, CSBuffer* buffer, const ForceParam* forces, int forceIndex, uint randomSeed, GameLevelCorrection levelCorrection, int maxCardLevel, int maxRuneLevel);
    ~Map();
    
    void createGrid();
    
    static void* operator new(size_t size);
public:
    static inline Map* sharedMap() {
        return __map;
    }
    static bool loadForOfflinePlay(GameOfflineMode mode, const MapIndex& index, const Player* player, const Player* otherPlayer, int ai);
    static bool loadForOnlinePlay(GameOnlineMode mode, bool resume);
    static bool loadForReplay(GameReplayMode mode, const char* subpath);
    static void destroy();

	void start(CSLayer* layer);
private:
    void preload(const SkinData* skin);

	const CSResource* preloadCallback(CSResourceType resourceType, const ushort* indices, int indexCount);

    void onPause();
    void onResume();
    void onReload();
    //========================================================================================
    //base
public:
    inline const CSThread* updateThread() const {
        return _updateThread;
    }
    inline bool syncUpdating() const {
        return !_updateThread || _updateThread->isActive();
    }
    inline CSLock& updateLock() {
        return _updateLock;
    }
	inline CSLock& markLock() {
		return _markLock;
	}
	inline CSLock& phaseLock() {
        return _phaseLock;
    }
    inline CSLock& selectionLock() {
        return _selectionLock;
    }

    inline const MapIndex& index() const {
        return _index;
    }
    inline const CSLocaleString* name() const {
        return _name;
    }
    inline const CSLocaleString* description() const {
        return _description;
    }
    inline GameType type() const {
        return _type;
    }
	inline const ImageIndex& background() const {
		return _background;
	}
	inline const CSImage* thumbnail() const {
		return _thumbnail ? Asset::sharedAsset()->image(_thumbnail) : _terrain->thumbnail().image;
	}
	inline const AnimationIndex& button() const {
		return _button;
	}
    inline const Terrain* terrain() const {
        return _terrain;
    }
    inline uint frame() const {
        return _frame;
    }
    inline float playTime() const {
        return _playTime;
    }
#ifdef GameImpl
    inline CSRandom* random() {
        return _random;
    }
    inline Force* force() const {
        return _force;
    }
    inline Force* force(int index) const {
        CSAssert(index >= 0 && index < MaxPlayers, "invalid operation");
        return _forces[index];
    }
#else
    inline const Force* force() const {
        return _force;
    }
    inline const Force* force(int index) const {
        CSAssert(index >= 0 && index < MaxPlayers, "invalid operation");
        return _forces[index];
    }
#endif
    inline MapState state() const {
        return _state;
    }
    inline GameLevelCorrection levelCorrection() const {
        return _levelCorrection;
    }
    inline int maxCardLevel() const {
        return _maxCardLevel;
    }
    inline int maxRuneLevel() const {
        return _maxRuneLevel;
    }
    
    GameResult result() const;
    
    inline GameMode mode() const {
        return _mode.code;
    }
    inline GameMode modeOrigin() const {
        return _mode.originCode;
    }
    inline GameModeType modeType() const {
        return _mode.type;
    }

    //========================================================================================
    //tile
private:
    inline Tile& tileImpl(const IPoint& point) {
        CSAssert(point.x >= 0 && point.x < _terrain->width() && point.y >= 0 && point.y < _terrain->height(), "invalid point");
        return _tiles[point.y * _terrain->width() + point.x];
    }
public:
    inline const Tile& tile(const IPoint& point) const {
        CSAssert(point.x >= 0 && point.x < _terrain->width() && point.y >= 0 && point.y < _terrain->height(), "invalid point");
        return _tiles[point.y * _terrain->width() + point.x];
    }
    //========================================================================================
    //location
public:
    static void tiledPosition(fixed collider, FPoint& point);
    
    MapLocation locatable(const Object* obj, const FPoint& point, const Force* vision) const;
    MapLocation locatable(const Object* obj, ObjectType type, fixed collider, const Force* vision) const;
    MapLocation locatable(const Object* obj, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
    MapLocation locatable(ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
    MapLocation locatable(CSArray<const Object*>* collisions, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
private:
    MapLocation locatableImpl(CSArray<const Object*>* collisions, ObjectType type, const FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
public:
    MapLocation locatablePosition(const Object* obj, FPoint& point, const Force* vision) const;
	MapLocation locatablePosition(const Object* obj, FPoint& point, const Force* vision, MapLocation duplicate) const;
	MapLocation locatablePosition(const Object* obj, ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
	MapLocation locatablePosition(ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
    MapLocation locatablePosition(CSArray<const Object*>* collisions, ObjectType type, FPoint& point, fixed collider, const Force* vision, MapLocation duplicate) const;
    
    MapLocation buildable(const Object* obj, ObjectType type, const FPoint& point, fixed collider, const Force* vision, CommandBuildTarget target) const;
private:
    bool buildingPosition(const Force* force, const UnitIndex& index, const Area& area, FPoint& point) const;
public:
#ifdef GameImpl
    void locate(Object* obj);
    void unlocate(Object* obj);
    bool relocate(Object* obj, bool vision, ObjectType type, fixed collider, bool collision);
    
    bool hasPath(const FPoint& p0, const FPoint& p1) const;

    ProgressPathReturn progressPath(const Object* obj, const Object* target, Path* path) const;
    
    struct FindPathReturn {
        Path* path;
        bool retry;
    };
    FindPathReturn findPath(const Object* obj, const Object* target) const;
    bool move(Object* obj, const Object* target, const FPoint& dest, fixed distance, int dir);
    fixed moveableDistance(const Object* obj, const Object* target, const FPoint& dest) const;
    bool flock(Object* obj, fixed distance, MapLocation duplicate);
#endif
    //========================================================================================
    //play
public:
    bool isVisible(const IBounds& bounds) const;
    bool isVisible(const IBounds& bounds, int alliance) const;
    bool isDetected(const IBounds& bounds, int alliance) const;
    bool isSpawnable(const FPoint& point, const Force* force) const;
#ifdef GameImpl
    void resetVision();
    void changeVision(const Object* obj, const FPoint& point, fixed range, bool detecting, int alliance, bool visible, bool lock = true);
    void changeMinimap(const Object* obj);
    Unit* findRefinery(const FPoint& point);
    Resource* findGas(const FPoint& point);
    Resource* findResource(const Unit* unit, int kind, bool inArea);
    Resource* findResource(const Unit* unit, const Resource* originResource);
	bool isResourceRemaining(const Resource* originResource);
    Unit* findBase(const Unit* unit);
    struct GatheringStateReturn {
        ushort current;
        ushort max;
    };
    GatheringStateReturn findGatheringState(const Unit* unit, int kind);
    const Area* findArea(const Unit* unit);
#endif
    int unitCount(const Force* force, const UnitIndex& index) const;
	CSDictionary<UnitIndex, int>* unitCounts(const Force* force, uint typeMask) const;
	const Unit* findUnit(const Force* force, const UnitIndex& index) const;
private:
    bool findUnitIndex(uint key, uint& result) const;
    bool findResourceIndex(uint key, uint& result) const;
public:
#ifdef GameImpl
	Unit* findUnit(uint key);
	Resource* findResource(uint key);
	
	inline uint registerKey() {
        return _key++;
    }
	void registerObject(Object* obj);
    void registerUnit(Unit* unit);
    void unregisterUnit(Unit* unit);
	void registerMinimapUnit(Unit* unit);
	void unregisterMinimapUnit(Unit* unit);
	void registerResource(Resource* resource);
    void unregisterResource(Resource* resource);
    void registerFogBuilding(FogBuilding* fogBuilding);
    void unregisterFogBuilding(FogBuilding* fogBuilding);
    
    inline const CSArray<Unit>* units() const {
        return _units;
    }
    inline const CSArray<Object>* objects() const {
        return _objects;
    }
    
    void effectToAll(UnitBaseGame* src, const byte* condition, const AnimationIndex& index, bool attach);
    fixed attackToAll(UnitBaseGame* src, const byte* condition, const AbilityValue<fixed>& rate, fixed time);
    fixed reviseToAll(UnitBaseGame* src, const byte* condition, int op, const AbilityValue<fixed>& value, fixed time);
    void buffToAll(UnitBaseGame* src, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack);
    void freezeToAll(UnitBaseGame* src, const byte* condition, const AbilityValue<fixed>& time);

    CSArray<Unit>* unitsWithRange(const UnitBaseGame* match, const Object* origin, const FPoint& point, fixed range, const byte* condition);
    void effectWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach);
    fixed attackWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time);
    fixed reviseWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time);
    void buffWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack);
    void escapeWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& time);
    void jumpWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing, bool forward);
    void warpWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, bool vision);
    void freezeWithRange(UnitBaseGame* src, const Object* origin, const Object* target, bool nearby, fixed range, const byte* condition, const AbilityValue<fixed>& time);
    
    CSArray<Unit>* unitsWithAngle(const UnitBaseGame* match, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition);
    void effectWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach);
    fixed attackWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time);
    fixed reviseWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time);
    void buffWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack);
    void escapeWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& time);
    void jumpWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing);
    void freezeWithAngle(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed angle, const byte* condition, const AbilityValue<fixed>& time);
    
    CSArray<Unit>* unitsWithLine(const UnitBaseGame* match, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, FPoint& p0, FPoint& p1);
    void effectWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AnimationIndex& index, bool nearbyTo, bool attach);
    fixed attackWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& rate, const AbilityValue<fixed>& attenuation, fixed time);
    fixed reviseWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, int op, const AbilityValue<fixed>& value, const AbilityValue<fixed>& attenuation, fixed time);
    void buffWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const BuffIndex& index, int timeOp, const AbilityValue<fixed>& time, const AbilityValue<fixed>& stack);
    void escapeWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& time);
    void jumpWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& distance, float altitude, fixed time, bool pushing);
    void freezeWithLine(UnitBaseGame* src, const Object* origin, const Object* target, fixed range, fixed thickness, const byte* condition, const AbilityValue<fixed>& time);

    struct WatchReturn {
        Unit* target;
        bool inside;
    };
    WatchReturn attackWatch(const Unit* unit, const CSArray<uint>* exclusions = NULL);
    WatchReturn watch(const UnitBaseGame* src, const Unit* unit, const byte* condition, const AbilityValue<fixed>& range, const CSArray<uint>* exclusions = NULL);
    uint count(const UnitBase* src, const Object* target, const byte* condition, fixed range) const;
    bool targetToAdvance(const Unit* unit, FPoint& p) const;
    bool targetToRetreat(const Unit* unit, FPoint& p) const;
    void removeSquad(const Unit* unit);
#endif
private:
    void updateObjects(bool running);
    void updateForces();
	void updatePlay();
    //========================================================================================
    //environment
private:
	bool updateColor(float delta);
public:
    inline void setLightState(MapLightState state) {
        _color.lightState = state;
    }
    inline MapLightState lightState() const {
        return _color.lightState;
    }
    inline float lightDegree() const {
        return _color.lightDegree;
    }

private:
	void updateEvents(float delta);
public:
    bool event(const Force* force, const AnimationIndex& animation, const CSColor& color, const CSVector2& point);
    bool event(const Force* force, bool toAlliance, const AnimationIndex& animation, const CSColor& color, const CSVector2& point);
    void event(const AnimationIndex& animation, const CSColor& color, const CSVector2& point);
    bool warn(const ImageIndex& icon, const CSVector2& point, bool essential);
    
    inline const Warning& warning() const {
        return _warning;
    }
    inline void clearWarning() {
        _warning.remaining = 0;
    }
    
private:
    void updateSounds(float delta);
public:
    void sound(const SoundIndex& index, float volume = 1.0f, bool essential = false);
	void sound(const SoundIndex& index, const Object* target, float volume = 1.0f, bool essential = false);
	void sound(const SoundIndex& index, const Force* force, float volume = 1.0f, bool essential = false);

	void voice(Voice type);
	void voice(Voice type, const Unit* unit);
	void announce(AnnounceIndex index);
	void announce(const Force* force, AnnounceIndex index);
	void announce(const Force* force, bool toAlliance, AnnounceIndex index);

    static CSAnimationSoundDelegate* const ObjectAnimationSoundDelegate;

private:
	bool updateVibration(float delta);
public:
    float vibration() const;
    void vibrate(int level, const Object* target);

private:
	void updateAnimations(CSLayer* layer);
public:
    void animation(int key, const AnimationIndex& index, const GameLayerIndex& layer0, const CSVector2& offset0, const GameLayerIndex& layer1, const CSVector2& offset1);
                   
    bool pushMessage(const Force* force, const Message* msg);
    bool pushMessage(const Force* force, bool toAlliance, const Message* msg);
    bool pushMessage(const Message* msg);
    const Message* popMessage();

private:
	void updateMissions();
public:
    const Mission* popMission();
    const Mission* getMission(uint key) const;
    
    inline fixed settingValue(int index) const {
        CSAssert(index >= 0 && index < countof(_setting->values), "invalid operation");
        return _setting->values[index];
    }
    const CSString* settingValueString(const CSString* format, const byte* valueIndices, uint valueCount);

    const Speech* speech() const;
    void nextSpeech();
	void skipWaiting();
    uint timer() const;
    
	inline MapAttainOrder attainOrder(int index) const {
		CSAssert(index >= 0 && index < 3, "invalid operation");
		return _attainOrders[index];
	}
private:
	int compareForceByAttain(const Force* a, const Force* b) const;
public:
    const CSArray<Force>* sortedForcesByAttain() const;
    
    inline bool isPlaying() const {
        return _playing == PlayingParticipate;
    }
    inline bool isFocusing() const {
#ifdef UseObserverPlayTest
        return false;
#else
        return _playing != PlayingObserveAll;
#endif
    }
    inline const Force* vision() const {
        return isFocusing() ? _force : NULL;
    }
    inline float offscreenProgress() const {
        return _offscreenProgress;
    }
    inline MapControlState controlState(MapControl control) const {
        return _control.states[control];
    }
    MapControlState controlStateSelection(const Unit* unit) const;
    MapControlState controlStateCommand(const CommandIndex& command, const ActionTarget** target) const;
    MapControlState controlStateSpawn(const UnitIndex& spawn, const ActionTarget** target) const;
    bool isActionAllowed(ActionRestriction restriction);
private:
    void observe(Force* force);
public:
    bool isSingle() const;
    void setFocusing(bool focusing);
    float replayTotalTime() const;
    float replayProgress() const;
    float replayNextProgress() const;
    void setReplayProgress(float progress);
    void stopReplayProgress();
    bool saveReplay(const char* subpath);
    bool isOnlinePartyPlaying() const;
    uint onlineFrame() const;
    uint onlineObserverDelayRemaining() const;

    int speed() const;
    void setSpeed(int speed);
	inline bool isPaused() const {
		return _paused;
	}
	inline void pause(bool paused) {
		_paused = paused;
	}
    void setAuto(bool on);
    bool isAuto() const;
    //========================================================================================
    //transform
public:
    inline Camera* camera() {
        return _camera;
    }
    inline const Camera* camera() const {
        return _camera;
    }
    inline CSVector2 convertMapToFlatWorldSpace(const FPoint& point) const {
        return CSVector2(point.x * TileSize, point.y * TileSize);
    }
    inline CSVector3 convertMapToWorldSpace(const FPoint& point) const {
        return CSVector3(point.x * TileSize, point.y * TileSize, (float)_terrain->altitude(point) * TileSize);
    }
    bool convertViewToMapSpace(const CSVector2& vp, FPoint& result, bool altitude) const;
private:
    Object* targetImpl(const CSVector2& vp, float range, const byte* condition = NULL);
    CSArray<Object>* targetsImpl(const CSVector2& vp, float range, const byte* condition = NULL);
    
    inline Object* targetImpl(const CSVector2& vp, const byte* condition = NULL) {
        return targetImpl(vp, TargetRange, condition);
    }
    inline CSArray<Object>* targetsImpl(const CSVector2& vp, const byte* condition = NULL) {
        return targetsImpl(vp, TargetRange, condition);
    }
    CSArray<Object>* targetsWithRangeImpl(const CSVector2& vp, fixed range, const byte* condition = NULL);
    CSArray<Object>* targetsWithAngleImpl(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed angle, const byte* condition = NULL);
    CSArray<Object>* targetsWithLineImpl(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed thickness, const byte* condition = NULL);
public:
    inline const Object* target(const CSVector2& vp, float range, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetImpl(vp, range, condition);
    }
    inline const Object* target(const CSVector2& vp, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetImpl(vp, TargetRange, condition);
    }
    inline const CSArray<Object>* targets(const CSVector2& vp, float range, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetsImpl(vp, range, condition);
    }
    inline const CSArray<Object>* targets(const CSVector2& vp, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetsImpl(vp, TargetRange, condition);
    }
    inline const CSArray<Object>* targetsWithRange(const CSVector2& vp, fixed range, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetsWithRangeImpl(vp, range, condition);
    }
    inline const CSArray<Object>* targetsWithAngle(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed angle, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetsWithAngleImpl(units, vp, range, angle, condition);
    }
    inline const CSArray<Object>* targetsWithLine(const CSArray<Unit>* units, const CSVector2& vp, fixed range, fixed thickness, const byte* condition = NULL) const {
        return const_cast<Map*>(this)->targetsWithLineImpl(units, vp, range, thickness, condition);
    }
    //========================================================================================
    //ui
public:
    inline const CSArray<Navigation>* navigations() const {
        _selectionLock.assertOnActive();
        return _selection.navigations;
    }
    inline const CSArray<Unit, 2>* numberings(int index) const {
        CSAssert(index >= 0 && index < MaxNumberings, "invalid operation");
        _selectionLock.assertOnActive();
        return _selection.numberings[index];
    }
    inline const CSArray<Unit, 2>* groupedSelections() const {
        _selectionLock.assertOnActive();
        return _selection.groupedSelections;
    }
    inline const CSArray<Unit>* expandedSelections() const {
        _selectionLock.assertOnActive();
        return _selection.expandedSelections;
    }
    inline const CSArray<Unit>* selections() const {
        _selectionLock.assertOnActive();
        return _selection.selections;
    }
    inline const Unit* selectedUnit() const {
        _selectionLock.assertOnActive();
        return _selection.expandedSelections->count() == 1 ? _selection.expandedSelections->objectAtIndex(0) : NULL;
    }
    inline const Object* selectedTarget() const {
        _selectionLock.assertOnActive();
        return _selection.target;
    }
    inline void followScroll(bool on) {
        _selection.following = on;
    }
private:
    bool select(uint mask, bool insight, bool excludeNumbering, bool commit);
public:
    inline bool selectAttackers(bool insight, bool excludeNumbering, bool commit) {
        return controlState(MapControlSelectAttackers) == MapControlStateEnabled && select(ObjectMaskAttacker, insight, excludeNumbering, commit);
    }
    inline bool selectBarracks(bool commit) {
        return controlState(MapControlSelectBarracks) == MapControlStateEnabled && select(ObjectMaskBarrack, false, false, commit);
    }
    inline bool selectLabors(bool commit) {
        return controlState(MapControlSelectLabors) == MapControlStateEnabled && select(ObjectMask(ObjectTypeGroundLabor), false, false, commit);
    }

    bool dragCheck(const CSVector2& vp);
    void drag(const CSVector2& vp);
    bool dragEnd(bool merge);
    void dragCancel();
    
    void focusSelection(int index, bool swap);
private:
    void cancelSelectionImpl();
public:
    void cancelSelection();
    void cancelSelection(int index);
    bool selectNavigation(int index);
    void resetNavigation(Unit* unit, Navigating state);
    bool selectNumbering(int index);
    void setNumbering(int index, bool merge);
    void cancelNumbering(int index);
    void removeNumbering(const Unit* unit);
#ifdef GameImpl
    void resetNumbering(Unit* unit);
#endif
private:
    void addSelectionImpl(Unit* unit);
public:
#ifdef GameImpl
    void addSelection(Unit* unit);
    void autoSelection(Unit* unit);
    bool removeSelection(const Object* obj);
#endif
    void cancelTarget();
private:
    void updateSelection();
public:
    void healthRate(float& playerRate, float& enemyRate) const;

    const CSArray<Unit>* commandUnits(const CommandIndex& index, const Object* target) const;
private:
    CommandReturn commandImpl(CSArray<Unit>* units, const CommandIndex& index, Object* target, const FPoint* point, int selectionFlag, CommandCommit commitType);
public:
    CommandReturn command(const CommandIndex& index, const Object* target, CommandCommit commit);
private:
    const Unit* spawnTarget(const Spawn* src, const byte* condition);
public:
    CommandReturn spawn(int slot, const Object* target, SpawnCommit commit);
    uint unitCount(const UnitIndex& index) const;
    uint producingCount(const UnitIndex& index) const;
    void cancelBuildingCommand(Unit* unit);
    void cancelProducingCommand(const UnitIndex& index);
    void cancelProducingCommand(Unit* unit, int slot);
    void cancelTrainingCommand(Unit* unit, int slot);
    void unloadCommand(Unit* unit, int slot);
	enum ClickCommand {
		ClickCommandNone,
		ClickCommandAttack,
		ClickCommandMove
	};
    bool click(const CSVector2& vp, ClickCommand command, bool all, bool merge);
    void showBuildingLocations(const UnitIndex& index);
    void hideBuildingLocations();
    bool isTeamPlaying() const;
    void setChatBlocked(bool block);
    bool isChatBlocked() const;
    void setChatToTeam(bool chatToTeam);
    bool isChatToTeam() const;
    
    void chat(Force* force, const User* user, const CSString* msg, bool toAlliance);
private:
	void updateEmoticons(float delta);
    void emoticon(Force* force, const EmoticonIndex& index);
    void ping(Force* force, int index, const CSVector2& point);
public:
    void emoticon(const EmoticonIndex& index);
    void ping(int index, const CSVector2& point);
    void end();
	void skip();
	bool isSkipAllowed() const;
    
    struct MonitorStatus {
        float updateLatency;
        float timeoutLatency;
        uint objectCount;
        uint unitCount;
        //uint secretCount;
    };
    MonitorStatus monitorStatus() const;

	inline const CSArray<MapGraph>* graph() const {
		return _graph;
	}
    //========================================================================================
    //update
public:
    inline int framePerSecond() const {
        return _latency.framePerSecond;
    }
    inline int localFramePerSecond() const {
        return _latency.localFramePerSecond;
    }
    inline fixed frameDelayFixed() const {
        return _latency.frameDelayFixed;
    }
    inline float frameDelayFloat() const {
        return _latency.frameDelayFloat;
    }
	inline int visionFrameDivision() const {
		return _visionFrameDivision;
	}
    PingLevel pingLevel() const;
    float averageLatency() const;

    bool timeout(CSLayer* layer);
private:
    void update();
    //========================================================================================
    //draw
private:
    void drawSpawnArea(CSGraphics* graphics);
public:
    void drawScreen(CSGraphics* graphics);
    void drawMinimap(CSGraphics* graphics, const CSRect& rect);
    void drawMinimapState(CSGraphics* graphics, const CSRect& rect, float eventScale);
    void drawAnimations(CSGraphics* graphics);
    
    //========================================================================================
    //streaming
private:
    void writeUnits(CSBuffer* buffer, const CSArray<Unit>* units);
    void writeTarget(CSBuffer* buffer, const Object* target);
    void writeControl(CSBuffer* buffer, GameControl control);
    void writeLatency(CSBuffer* buffer, int framePerSecond);
    void writeCommand(CSBuffer* buffer, const CSArray<Unit>* units, const CommandIndex& index, const Object* target, const FPoint* point);
    void writeCancelBuildingCommand(CSBuffer* buffer, const Unit* unit);
    void writeCancelProducingCommand(CSBuffer* buffer, const CSArray<Unit>* units, const UnitIndex& index);
    void writeCancelProducingCommand(CSBuffer* buffer, const Unit* unit, int slot);
    void writeCancelTrainingCommand(CSBuffer* buffer, const Unit* unit, int slot);
    void writeUnloadCommand(CSBuffer* buffer, const Unit* unit, int slot);
    void writeSpawn(CSBuffer* buffer, int slot, const Object* target);
    void writeStep(CSBuffer* buffer, int op);
    void writeEmoticon(CSBuffer* buffer, const EmoticonIndex& index);
    void writePing(CSBuffer* buffer, int index, const CSVector2& point);
public:
    void write(int framePerStep, uint frame, float serverDelay, const void* data, uint length);
private:
    CSArray<Unit>* readUnits(CSBuffer* buffer, const Force* force, uint endpos);
    bool readTarget(CSBuffer* buffer, const Force* force, uint endpos, Object*& target);
    bool readControl(CSBuffer* buffer, Force* force, uint endpos);
    bool readLatency(CSBuffer* buffer, uint endpos);
    bool readCommand(CSBuffer* buffer, Force* force, uint endpos);
    bool readCancelBuildingCommand(CSBuffer* buffer, Force* force, uint endpos);
    bool readCancelProducingCommand(CSBuffer* buffer, Force* force, uint endpos);
    bool readCancelTrainingCommand(CSBuffer* buffer, Force* force, uint endpos);
    bool readUnloadCommand(CSBuffer* buffer, Force* force, uint endpos);
    bool readSpawn(CSBuffer* buffer, Force* force, uint endpos);
	bool readStep(CSBuffer* buffer, Force* force, uint endpos);
    bool readChat(CSBuffer* buffer, Force* force, uint endpos);
    bool readEmoticon(CSBuffer* buffer, Force* force, uint endpos);
    bool readPing(CSBuffer* buffer, Force* force, uint endpos);
    void read(CSBuffer* buffer);
    
    //========================================================================================
    //settings
private:
    UnitSetting& addUnitSetting(const CSArray<Force>* forces, const byte* condition);
    static bool matchUnitSetting(const CSArray<Force>* forces, const byte* condition, const Unit* target);
public:
    void applyUnitSetting(Unit* unit);
    ActionCode action() const;
    void passAction(ActionCode code, const CSArray<Unit>* units, const CommandIndex* command, const Object* target);
private:
    int mtReadByte(const byte*& cursor);
    int mtReadShort(const byte*& cursor);
    int mtReadInt(const byte*& cursor);
    fixed mtReadFixed(const byte*& cursor, Setting::Thread* thread);
    AssetIndex2 mtReadAssetIndex2(const byte*& cursor);
    const CSLocaleString* mtReadLocaleString(const byte*& cursor);
    FPoint mtReadPoint(const byte*& cursor, Setting::Thread* thread);
	CSArray<FPoint>* mtReadPoints(const byte*& cursor, Setting::Thread* thread);
    CSArray<Squad*>* mtReadSquads(const byte*& cursor);
    CSArray<Unit>* mtReadUnits(const byte*& cursor, Setting::Thread* thread);
	CSArray<Resource>* mtReadResources(const byte*& cursor, Setting::Thread* thread);
    CSArray<Force>* mtReadForces(const byte*& cursor);
    bool mtReadPlaying(const byte*& cursor);
    
    void mtMeleeStart(const byte*& cursor, Setting::Thread* thread);
    void mtMeleePlay(const byte*& cursor, Setting::Thread* thread);
	void mtHasForce(const byte*& cursor, Setting::Thread* thread);
    void mtCall(const byte*& cursor, Setting::Thread* thread);
    void mtValue(const byte*& cursor, Setting::Thread* thread);
    bool mtCompareValue(const byte*& cursor, Setting::Thread* thread);
    void mtResource(const byte*& cursor, Setting::Thread* thread);
    void mtSupply(const byte*& cursor, Setting::Thread* thread);
	void mtMaxSupply(const byte*& cursor, Setting::Thread* thread);
    bool mtHasResource(const byte*& cursor, Setting::Thread* thread);
    bool mtHasPopulation(const byte*& cursor, Setting::Thread* thread);
	bool mtHasRune(const byte*& cursor, Setting::Thread* thread);
    void mtPoint(const byte*& cursor, Setting::Thread* thread);
    void mtAppear(const Squad& squad, const FPoint* point);
    void mtAppear(const byte*& cursor, Setting::Thread* thread);
    void mtLeave(const byte*& cursor, Setting::Thread* thread);
	bool mtIsAlive(const byte*& cursor, Setting::Thread* thread);
    bool mtIsDestroyed(const byte*& cursor, Setting::Thread* thread);
	bool mtIsTarget(const byte*& cursor, Setting::Thread* thread);
	bool mtIsEncountered(const byte*& cursor, Setting::Thread* thread);
    void mtCommand(const byte*& cursor, Setting::Thread* thread);
    void mtTransform(const byte*& cursor, Setting::Thread* thread);
    void mtBuff(const byte*& cursor, Setting::Thread* thread);
    void mtSpawn(const byte*& cursor, Setting::Thread* thread);
    void mtPlayer(const byte*& cursor, Setting::Thread* thread);
    void mtUnit(const byte*& cursor, Setting::Thread* thread);
    void mtRevise(const byte*& cursor, Setting::Thread* thread);
    void mtWarp(const byte*& cursor, Setting::Thread* thread);
    void mtVision(const byte*& cursor, Setting::Thread* thread);
    void mtTimer(const byte*& cursor, Setting::Thread* thread);
    bool mtIsTimeout(const byte*& cursor, Setting::Thread* thread);
    void mtShowTimer(const byte*& cursor, Setting::Thread* thread);
    void mtState(const byte*& cursor, Setting::Thread* thread);
    void mtActivate(const byte*& cursor, Setting::Thread* thread);
    void mtEffect(const byte*& cursor, Setting::Thread* thread);
    void mtRemoveEffect(const byte*& cursor, Setting::Thread* thread);
    void mtSpeech(const byte*& cursor, Setting::Thread* thread);
    void mtSpeechBubble(const byte*& cursor, Setting::Thread* thread);
    bool mtScroll(const byte*& cursor, Setting::Thread* thread);
    bool mtZoom(const byte*& cursor, Setting::Thread* thread);
    void mtEvent(const byte*& cursor, Setting::Thread* thread);
    bool mtColor(const byte*& cursor, Setting::Thread* thread);
    void mtSound(const byte*& cursor, Setting::Thread* thread);
	void mtAnnounce(const byte*& cursor, Setting::Thread* thread);
    void mtVibrate(const byte*& cursor, Setting::Thread* thread);
    void mtMessage(const byte*& cursor, Setting::Thread* thread);
    void mtMission(const byte*& cursor, Setting::Thread* thread);
    void mtControl(const byte*& cursor, Setting::Thread* thread);
    void mtAttach(const byte*& cursor, Setting::Thread* thread);
    void mtGrowup(const byte*& cursor, Setting::Thread* thread);
    void mtAttain(const byte*& cursor, Setting::Thread* thread);
    void mtSelect(const byte*& cursor, Setting::Thread* thread);
	bool mtIsSelected(const byte*& cursor, Setting::Thread* thread);
    bool mtAction(const byte*& cursor, Setting::Thread* thread);
    void mtActionFinish(const byte*& cursor, Setting::Thread* thread);
    void mtAnimation(const byte*& cursor, Setting::Thread* thread);
    void mtLevel(const byte*& cursor, Setting::Thread* thread);
    void mtWait(const byte*& cursor, Setting::Thread* thread);
    bool mtStop(const byte*& cursor, Setting::Thread* thread);
	void mtSkipReady(const byte*& cursor, Setting::Thread* thread);
	void mtIsSkipped(const byte*& cursor, Setting::Thread* thread);

    enum UpdateSettingReturn {
        UpdateSettingReturnNone,
		UpdateSettingReturnSkip,
        UpdateSettingReturnEnd,
        UpdateSettingReturnInterrupt
    };
    UpdateSettingReturn updateSetting(Setting::Thread* thread, bool skip);
    void updateSetting();
    
    //========================================================================================
    //ai
private:
    /*
    void atCall(Force* force, AI::Thread* thread, const byte*& cursor);
    
    const CSArray<Force>* atValueForces(Force* force, const byte*& cursor);
    fixed atValueUnitLevel(Force* force, const byte*& cursor);
    fixed atValueRuneLevel(Force* force, const byte*& cursor);
    int atValueUnitCount(Force* force, const byte*& cursor);
    int atValueAreaUnitCount(Force* force, const Area* area, const byte*& cursor);
    fixed atValueResource(Force* force, const byte*& cursor);
    fixed atValueAreaResource(Force* force, const Area* area, const byte*& cursor);
    fixed atValueUnitResource(Force* force, const byte*& cursor);
    fixed atValueUnitChannelResourceRate(Force* force, const byte*& cursor);
    fixed atValueTrainingResource(Force* force, const byte*& cursor);
    int atValueUnitChannelCount(Force* force, const byte*& cursor);
    fixed atValueUnitChannelRate(Force* force, const byte*& cursor);
    fixed atValueValuationRate(Force* force, const byte*& cursor);
    fixed atValueAreaValuationRate(Force* force, const Area* area, const byte*& cursor);
    fixed atValueUnitValuationRate(Force* force, const byte*& cursor);
    fixed atValueAreaEmptyRate(Force* force, const Area* area, const byte*& cursor);
    fixed atValueAreaCostRate(Force* force, const Area* area, const byte*& cursor);
    fixed atValueBattleRate(Force* force, const byte*& cursor);
    fixed atValueAreaBattleRate(Force* force, const Area* area, const byte*& cursor);
    
    void atValue(Force* force, AI::Thread* thread, const byte*& cursor);
    void atRateValue(Force* force, AI::Thread* thread, const byte*& cursor);
    void atCompareValue(Force* force, AI::Thread* thread, const byte*& cursor);
    void atValueFunc(Force* force, AI::Thread* thread, const byte*& cursor);
    void atIsUnitEnabled(Force* force, AI::Thread* thread, const byte*& cursor);
    void atIsTrainingEnabled(Force* force, AI::Thread* thread, const byte*& cursor);
    void atRandom(Force* force, AI::Thread* thread, const byte*& cursor);
    void atRandomWeights(Force* force, AI::Thread* thread, const byte*& cursor);
    void atValueFuncRandomWeights(Force* force, AI::Thread* thread, const byte*& cursor);
    
    void atCombatAttack(Force* force, fixed attackRate);
    void atCombatRetreat(Force* force, fixed retreatRate);
    void atCombat(Force* force, AI::Thread* thread, const byte*& cursor);
    
    bool atWaitResource(Force* force, AI::Thread* thread, int resource0, int resource1, int priority, fixed remaining);
    void atWaitResourceRelease(Force* force, AI::Thread* thread);
    Object* atCommandTarget(Force* force, AI::Thread* thread, const CSArray<Unit>* units, const byte* condition);
    bool atCommand(Force* force, AI::Thread* thread, const byte*& cursor);
    bool atSpawn(Force* force, AI::Thread* thread, const byte*& cursor);
    void atChangeArea(Force* force, AI::Thread* thread, const byte*& cursor);
    void atNewArea(Force* force, AI::Thread* thread, const byte*& cursor);
    void atGiveupArea(Force* force, AI::Thread* thread, const byte*& cursor);
    void atGiveup(Force* force, AI::Thread* thread, const byte*& cursor);
	void atEmoticon(Force* force, AI::Thread* thread, const byte*& cursor);
	bool atWait(Force* force, AI::Thread* thread, const byte*& cursor);
    bool atStop(Force* force, AI::Thread* thread, const byte*& cursor);
    
    bool updateAI(Force* force, AI::Thread* thread);
    */
    void updateAI(Force* force);

	void updateGraph();
    //========================================================================================
};

#define AssertOnMainThread()        CSAssert(CSThread::mainThread()->isActive(), "invalid operation")
#define AssertOnUpdateThread()      CSAssert(!Map::sharedMap()->updateThread() || Map::sharedMap()->updateLock().assertOnActive(), "invalid operation")

#define synchronized_to_update_thread(lock, op)     synchronized(lock, op, !Map::sharedMap()->syncUpdating())

#ifdef CS_CONSOLE_DEBUG
static inline void UnitLog(const Object* obj, const char* format, ...) {
	bool flag;
	synchronized(Map::sharedMap()->selectionLock(), GameLockMap) {
		flag = Map::sharedMap()->selectedUnit() == obj || Map::sharedMap()->selectedTarget() == obj;
	}
	if (flag) {
        va_list args;
        va_start(args, format);
        CSLogv(format, args);
        va_end(args);
    }
}
#else
#define UnitLog(obj, ...);
#endif

#ifdef UseMoveTest
#define MoveLog(obj, ...)  UnitLog(obj, __VA_ARGS__)
#else
#define MoveLog(obj, ...);
#endif

#ifdef UseLoggerPath
#define LoggerLog(...)	Map::sharedMap()->logger->print(__VA_ARGS__)
#define LoggerFlush()	Map::sharedMap()->logger->flush()
#else
#define LoggerLog(...);
#define LoggerFlush();
#endif

#ifdef UseSyncTest
#define SyncLog(...)    CSLog(__VA_ARGS__)
#else
#define SyncLog(...);
#endif

#ifdef UseLeakTest
void GameLeak_addObject(CSObject* obj);
void GameLeak_removeObject(CSObject* obj);
void GameLeak_showObjects();
#else
#define GameLeak_addObject(obj);
#define GameLeak_removeObject(obj);
#define GameLeak_showObjects();
#endif

#endif /* Map_h */
