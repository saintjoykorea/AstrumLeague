//
//  Map.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl
#define AssetImpl

#include "Map.h"

#include "Prop.h"
#include "FogResource.h"

#include "Sound.h"

#include "PVPGateway.h"

#include "MapThumb.h"

#include "GameRecord.h"

#define SecretUpdateDuration    5

#define LatencyUpdateDuration   3

#define PVPOffscreenLatency     5

#define ObjectDisplayAdditionalRange      3

#define GraphicsQualityAutoLowLatency		0.04f
#define GraphicsQualityAutoHighLatency		0.02f

static const uint LoadingMemory = 64 * 1024768;

static const uint PreloadingReadyMemory = 16 * 1024768;

Map* Map::__map = NULL;

Map::Map(const MapIndex& index, CSBuffer* buffer, const ForceParam* forces, int forceIndex, uint randomSeed, GameLevelCorrection levelCorrection, int maxCardLevel, int maxRuneLevel) :
	_updateLock(true),
	_markLock(true),
	_phaseLock(true),
	_selectionLock(true),
	_controlLock(true)
{
    CSAssert(!__map, "invalid state");
    
    CSLog("map load");

	__map = this;
    
#ifdef UseLoggerPath
	logger = CSLogger::create(CSFile::storagePath(UseLoggerPath));
#endif
	
	CSMemoryUsage memoryUsage = CSDevice::memoryUsage();
    CSLog("map load memory check:total:%d mb free:%d mb threshold:%d mb", (int)(memoryUsage.totalMemory / 1024768), (int)(memoryUsage.freeMemory / 1024768), (int)(memoryUsage.threshold / 1024768));
    if (memoryUsage.freeMemory && memoryUsage.freeMemory - LoadingMemory < memoryUsage.threshold) {
        int64 purge = memoryUsage.threshold + LoadingMemory - memoryUsage.freeMemory;
        CSLog("map load purge:%d mb", (int)(purge / 1024768));
        Asset::sharedAsset()->purge(purge);
    }
    
    _index = index;
    _levelCorrection = levelCorrection;
    _maxCardLevel = maxCardLevel;
    _maxRuneLevel = maxRuneLevel;
    
    _random = new CSRandom(randomSeed);
    
    CSArray<sbyte>* pointIndices = CSArray<sbyte>::arrayWithCapacity(MaxPlayers);
    bool allianceExists[MaxAlliances] = {};
    int playerCount = 0;

	_background = ImageIndex::None;
	_thumbnail = ImageIndex::None;
	_button = AnimationIndex::None;

    int levelCount = buffer->readLength();
    CSAssert(index.indices[3] < levelCount, "invalid data");
    for (int i = 0; i < levelCount; i++) {
        int length = buffer->readInt();
        int nextpos = buffer->position() + length;
        if (i == index.indices[3]) {
            _name = CSObject::retain(buffer->readLocaleString());
            _description = CSObject::retain(buffer->readLocaleString());
            _type = (GameType)buffer->readByte();
			if (buffer->readBoolean()) _background = ImageIndex(buffer);
            if (buffer->readBoolean()) _thumbnail = ImageIndex(buffer);
			if (buffer->readBoolean()) _button = AnimationIndex(buffer);
            
            int forceCount = buffer->readByte();
            for (int i = 0; i < forceCount; i++) {
                int alliance = buffer->readByte();
                int color = buffer->readByte();
                pointIndices->addObject(buffer->readByte());
                if (forces[i].player) {
                    _forces[i] = new Force(forces[i].player, i, alliance, color, forces[i].control);
                    allianceExists[alliance] = true;
                    playerCount++;
                }
            }
            int autoForceCount = buffer->readByte();
            for (int i = 0; i < autoForceCount; i++) {
                int alliance = buffer->readByte();
                int color = buffer->readByte();
                pointIndices->addObject(buffer->readByte());
                GameControl control = (GameControl)buffer->readByte();
                Player* player = Player::playerWithBuffer(buffer, true);
                _forces[forceCount + i] = new Force(player, forceCount + i, alliance, color, control);
                allianceExists[alliance] = true;
                playerCount++;
            }
        }
        buffer->setPosition(nextpos);
    }
    
    _force = forceIndex >= 0 ? _forces[forceIndex] : NULL;
    _playing = _force != NULL ? PlayingParticipate : PlayingObserveAll;
    
    _terrain = new Terrain(buffer);
    _minimap = CSScratch::create(CSImageFormatRawRGBA4444, _terrain->width() - _terrain->thumbnail().left - _terrain->thumbnail().right, _terrain->height() - _terrain->thumbnail().top - _terrain->thumbnail().bottom);
    
    IPoint p;

    static const uint minimapHiddenRgba = (255 - VisionLuminanceHidden) * 15 / 255;
    for (p.y = 0; p.y < _terrain->height(); p.y++) {
        for (p.x = 0; p.x < _terrain->width(); p.x++) {
            _minimap->update(p.x - _terrain->thumbnail().left, p.y - _terrain->thumbnail().top, minimapHiddenRgba);
        }
    }
    _minimapPoints = new CSArray<IPoint>();
    
	_vision = new Vision(_terrain);
    for (p.y = 0; p.y <= _terrain->height(); p.y++) {
        for (p.x = 0; p.x <= _terrain->width(); p.x++) {
            _vision->update(p.x, p.y, VisionLuminanceHidden);
        }
    }
    _visionPoints = new CSArray<IPoint>();
    
    _tiles = (Tile*)fcalloc(_terrain->width() * _terrain->height(), sizeof(Tile));
	{
		bool* fills = (bool*)fmalloc(_terrain->width() * _terrain->height());
		int i = 0;
		for (p.y = 0; p.y < _terrain->height(); p.y++) {
			for (p.x = 0; p.x < _terrain->width(); p.x++, i++) {
				fixed altitude = (_terrain->altitude(p) + _terrain->altitude(IPoint(p.x + 1, p.y)) + _terrain->altitude(IPoint(p.x, p.y + 1)) + _terrain->altitude(IPoint(p.x + 1, p.y + 1))) / 4;

				new (&_tiles[i]) Tile(buffer, altitude);

				fills[i] = _tiles[i].attribute() < TileAttributeMove;
			}
		}
		_navMesh = new NavMesh(_terrain->width(), _terrain->height(), fills);
		free(fills);
	}

    createGrid();
    
    _objects = new CSArray<Object>(2048);
    _units = new CSArray<Unit>(512);
	_minimapUnits = new CSArray<Unit>();
    _resources = new CSArray<Resource>(128);
    _fogBuildings = new CSArray<FogBuilding>(128);
    _messages = new CSArray<const Message>();
    _missions = new CSArray<Mission>();
    _events = new CSArray<Event>();
    _warning.icon = ImageIndex(ImageSetGame, ImageGameWarning);
    for (int i = 0; i < EffectCount; i++) {
        _effects[i] = new CSArray<AnimationEffect>();
    }
    _animations = new CSDictionary<byte, GameAnimation, false>();
    _sounds = new CSArray<SoundEntry>();
    _distortionAnimation = new Animation(AnimationSetGame, AnimationGameDistortion, SoundControlEffect, false);
    _rallyPointAnimation = new Animation(AnimationSetGame, AnimationGameRallyCommand, SoundControlEffect, false);
    
    memset(_control.states, MapControlStateEnabled, sizeof(_control.states));
    _control.selectionStates = new CSDictionary<uint, MapControlState>();
    _control.commandStates = new CSDictionary<CommandIndex, MapControlStateCommand>();
    _control.spawnStates = new CSDictionary<UnitIndex, MapControlStateCommand>();
    
    _color.current = CSColor::White;
	_color.lightDegree = 1;
    _color.lightState = MapLightStateOn;

    _setting = new Setting();

    _selection.groupedSelections = new CSArray<Unit, 2>();
    _selection.expandedSelections = new CSArray<Unit>();
    _selection.selections = _selection.expandedSelections;
    _selection.dragTargets = new CSArray<Unit>();
    _selection.dragEffects = new CSArray<Effect>();
    _selection.navigations = new CSArray<Navigation>();
    for (int i = 0; i < MaxNumberings; i++) {
        _selection.numberings[i] = new CSArray<Unit, 2>();
    }
    _selection.clickEffects = new CSArray<Effect>();
    
    _latency.framePerSecond = GameMaxFramePerSecond;
    _latency.frameDelayFloat = _latency.frameDelayFixed = fixed::One / GameMaxFramePerSecond;
	_latency.updateSeq = -(LatencyUpdateDuration * GameMaxFramePerSecond);

	_visionFrameDivision = 1;

	_preloadImages = new CSArray<ImageGroupIndex>();
	_preloadDelegate = new CSResourceDelegate0(this, &Map::preloadCallback);

    for (int kind = 0; kind < 2; kind++) {
        int resourceGroupCount = buffer->readLength();
        for (int i = 0; i < resourceGroupCount; i++) {
            int resourceCount = buffer->readLength();
            for (int j = 0; j < resourceCount; j++) {
                FPoint point(buffer);
                int amount = buffer->readShort();
                
                Resource* resource = Resource::resource(kind, ResourceIndex(i, j), amount);
                resource->locate(point);
                
				for (int k = 0; k < MaxAlliances; k++) {
                    if (allianceExists[k]) {
                        FogResource::fog(resource, k);
                    }
                }
            }
        }
		foreach(const ResourceLevelData&, subdata, Asset::sharedAsset()->resource(kind).levels) {
			Asset::sharedAsset()->animation(subdata.animation)->preload(_preloadDelegate);
		}
    }
    
    for (int i = 0; i < 2; i++) {
        int propGroupCount = buffer->readLength();
        for (int j = 0; j < propGroupCount ; j++) {
            int propCount = buffer->readLength();
            
            for (int k = 0; k < propCount; k++) {
                AnimationIndex index(buffer);
                FPoint point(buffer);
                float translation = buffer->readFloat() * TileSize;
                float scale = buffer->readFloat();

				float yaw = buffer->readFloat();
				float pitch = buffer->readFloat();
				float roll = buffer->readFloat();
                CSQuaternion rotation = CSQuaternion::rotationYawPitchRoll(yaw, pitch, roll);

				float range = buffer->readFixed();
                Prop::prop(index, translation, scale, rotation, range, i)->locate(point);

				Asset::sharedAsset()->animation(index)->preload(_preloadDelegate);
            }
        }
    }
    
    _squads = CSObject::retain(buffer->readArray<Squad, 2>());

	foreach(const CSArray<Squad>*, squads, _squads) {
		foreach(const Squad&, squad, squads) {
			preload(Asset::sharedAsset()->unit(squad.index).skin(squad.skin));
		}
	}
    
    struct StartPoint {
        FPoint point;
        byte dir;
        
        StartPoint(CSBuffer* buffer) :
            point(buffer),
            dir(buffer->readByte())
        {
        
        }
    };
    CSArray<StartPoint>* points = buffer->readArray<StartPoint>();
    bool allPointFixed = points->count() == playerCount;
    for (int i = 0; i < MaxPlayers; i++) {
        if (_forces[i]) {
            int pointIndex = pointIndices->objectAtIndex(i);
            bool pointFixed;
            if (pointIndex < 0) {
                pointIndex = _random->nextInt(0, points->count() - 1);
                
                //pointIndex = 0;       //TODO:DEBUG
                
                while (pointIndices->containsObjectIdenticalTo(pointIndex)) {
                    pointIndex = (pointIndex + 1) % points->count();
                }
                pointIndices->objectAtIndex(i) = pointIndex;
                pointFixed = allPointFixed;
            }
            else {
                pointFixed = true;
            }
            _forces[i]->point = points->objectAtIndex(pointIndex).point;
            _forces[i]->pointDir = points->objectAtIndex(pointIndex).dir;
            _forces[i]->pointFixed = pointFixed;
        }
    }
    _points = CSObject::retain(buffer->readArray<FPoint>());
    _areas = CSObject::retain(buffer->readArray<Area>(false));

	FPoint cameraPoint;
    bool cameraInv;
	if (_playing == PlayingParticipate) {
		cameraPoint = _force->point;
        cameraInv = _force->pointDir != 0;           //TODO:DIFF WITH ORIGIN PROJECT
	}
	else {
		for (int i = 0; i < MaxPlayers; i++) {
			if (_forces[i]) {
				cameraPoint = _forces[i]->point;
                cameraInv = _forces[i]->pointDir != 0;          //TODO:DIFF WITH ORIGIN PROJECT
				break;
			}
		}
	}
	_camera = new Camera(_terrain, buffer, convertMapToFlatWorldSpace(cameraPoint), cameraInv);

	_triggers = CSObject::retain(buffer->readArray<byte, 3>());
    CSAssert(buffer->position() == buffer->length(), "invalid data");
    
    /*
    foreach (const UnitIndex&, unit, Asset::sharedAsset()->baseTheme.units) {
        preload(Asset::sharedAsset()->unit(unit).baseSkin());
    }
    */
    for (int i = 0; i < MaxPlayers; i++) {
        if (_forces[i]) {
            const ThemeIndex& theme = _forces[i]->player->deckTheme;
            if (theme) {
                for (CSDictionary<UnitIndex, ushort>::ReadonlyIterator i = Asset::sharedAsset()->theme(theme).skins->iterator(); i.isValid(); i.next()) {
                    preload(Asset::sharedAsset()->unit(i.key()).skin(i.object()));
                }
            }
            foreach(const Card*, card, _forces[i]->cards()) {
				preload(card->skinData());
            }
        }
    }
    
#ifdef CS_CONSOLE_DEBUG
	foreach(const ImageGroupIndex&, image, _preloadImages) {
		CSLog("preload image:(%d, %d)", image.indices[0], image.indices[1]);
	}
#endif
	memoryUsage = CSDevice::memoryUsage();
	
	int64 threshold = memoryUsage.freeMemory - PreloadingReadyMemory - memoryUsage.threshold;
	if (threshold > 0) {
		threshold += Asset::sharedAsset()->pool->cost();

		while (_preloadImages->count() && Asset::sharedAsset()->pool->cost() < threshold) {
			const ImageGroupIndex& imageIndex = _preloadImages->lastObject();
			const CSRootImage* image = static_cast<const CSRootImage*>(Asset::sharedAsset()->image(imageIndex));

			if (image) {
				CSLog("preload image ready:(%d, %d) - (%d mb / %d mb)", imageIndex.indices[0], imageIndex.indices[1], (int)(Asset::sharedAsset()->pool->cost() / 1024768), (int)(threshold / 1024768));
				_preloadImages->removeLastObject();
			}
			else {
				CSLog("preload image fail:(%d, %d) - (%d mb / %d mb)", imageIndex.indices[0], imageIndex.indices[1], (int)(Asset::sharedAsset()->pool->cost() / 1024768), (int)(threshold / 1024768));
				break;
			}
		}
	}

	_graph = new CSArray<MapGraph>();

    _running = true;
    
    CSApplication::sharedApplication()->OnReload.add(this, &Map::onReload);
    CSApplication::sharedApplication()->OnPause.add(this, &Map::onPause);
    CSApplication::sharedApplication()->OnResume.add(this, &Map::onResume);
    
    //=======================================================================================
    //resource distance log
#ifdef CS_CONSOLE_DEBUG
    for (int i = 0; i < _areas->count(); i++) {
        const Area& area = _areas->objectAtIndex(i);
        int counts[2] = {};
        float distances[2] = {};
        
        CSLog("resource to area:%d", i);
        foreach(const Resource*, resource, _resources) {
            if (area.contains(resource->point())) {
                float d = area.point.distance(resource->point());
                CSLog("     kind:%d distance:%.2f", resource->kind, d);
                
                counts[resource->kind]++;
                distances[resource->kind] += d;
            }
        }
        CSLog("     total:%d / %d distance:%.2f / %.2f", counts[0], counts[1], distances[0], distances[1]);
    }
#endif
    //=======================================================================================
}

void Map::createGrid() {
    CSColor3* colors = (CSColor3*)fmalloc((_terrain->width() + 1) * (_terrain->height() + 1) * sizeof(CSColor3));
    IPoint p;
    for (p.y = 0; p.y <= _terrain->height(); p.y++) {
        for (p.x = 0; p.x <= _terrain->width(); p.x++) {
            const IPoint nps[] = {
                IPoint(p.x - 1, p.y - 1),
                IPoint(p.x, p.y - 1),
                IPoint(p.x - 1, p.y),
                IPoint(p.x, p.y)
            };
            TileAttribute attr = TileAttributeBuild;
            for (int i = 0; i < 4; i++) {
                const IPoint& np = nps[i];
                
                if (np.x >= 0 && np.x < _terrain->width() && np.y >= 0 && np.y < _terrain->height()) {
                    TileAttribute a = tile(np).attribute();
                    if (a < attr) attr = a;
                }
            }
            CSColor3& color = colors[p.y * (_terrain->width() + 1) + p.x];
            if (attr == TileAttributeBuild) color = CSColor3::Green;
            else if (attr >= TileAttributeMove) color = CSColor3::Orange;
            else color = CSColor3::Red;
        }
    }
    _grid = new Grid(_terrain, colors);
    free(colors);
}

Map::~Map() {
    CSLog("map destroy");
    
    if (_updateThread) {
        synchronized(_updateLock, GameLockMap) {
            _alive = AliveFinish;
            _updateLock.signal();
        }
        _updateThread->stop();
        CSLog("update thread detached");
        _updateThread->release();
        _updateThread = NULL;
    }

    CSThread::mainThread()->autoreleasePool()->drain();
    
    switch (_mode.type) {
        case GameModeTypeOnline:
            _mode.context.online.localBuffer->release();
            _mode.context.online.globalBuffer->release();
            break;
        case GameModeTypeReplay:
            _mode.context.replay.buffer->release();
            break;
        case GameModeTypeOffline:
            if (_mode.context.single.action) {
                delete _mode.context.single.action;
            }
            break;
    }
    
    _name->release();
    _description->release();
    
    CSObject::release(_buildingLocations);
    
    foreach(Object*, obj, _objects) {
        obj->dispose();
    }
    _objects->release();
    _units->release();
	_minimapUnits->release();
    _resources->release();
    _fogBuildings->release();

    _selection.groupedSelections->release();
    _selection.expandedSelections->release();
    _selection.dragTargets->release();
    _selection.dragEffects->release();
    _selection.navigations->release();
    for (int i = 0; i < MaxNumberings; i++) {
        _selection.numberings[i]->release();
    }
    _selection.clickEffects->release();
    CSObject::release(_selection.target);
    
    CSObject::release(_offscreenCapture);
    _areas->release();
    CSObject::release(_speech);
    CSObject::release(_squads);
    CSObject::release(_points);
    CSObject::release(_unitSettings);
    CSObject::release(_triggers);
    delete _setting;
    
    _messages->release();
    _missions->release();
    for (int i = 0; i < EffectCount; i++) {
        _effects[i]->release();
    }
    _animations->release();
    _sounds->release();
    _distortionAnimation->release();
    _rallyPointAnimation->release();
    _events->release();
    _control.selectionStates->release();
    _control.commandStates->release();
    _control.spawnStates->release();
    for (int i = 0; i < MaxPlayers; i++) {
        CSObject::release(_forces[i]);
    }
    int len = _terrain->width() * _terrain->height();
    for (int i = 0; i < len; i++) {
        _tiles[i].~Tile();
    }
    free(_tiles);

    _visionPoints->release();
    _minimapPoints->release();
    _camera->release();
	delete _vision;
	_minimap->release();
	delete _grid;
    delete _navMesh;
    delete _terrain;
    _random->release();
    _preloadImages->release();
    _preloadDelegate->release();
	_graph->release();

    CSApplication::sharedApplication()->OnReload.remove(this);
    CSApplication::sharedApplication()->OnPause.remove(this);
    CSApplication::sharedApplication()->OnResume.remove(this);
    
    GameLeak_showObjects();

    //CSSecret::sharedSecret()->clear();
    //CSSecret::sharedSecret()->reset(true);
#ifdef UseLoggerPath
	logger->release();
#endif
    __map = NULL;

	Sound::stop(SoundControlGameBgm);
	Sound::stop(SoundControlEffect);
}

void* Map::operator new(size_t size) {
    void* ptr = ::operator new(size);
    memset(ptr, 0, size);
    return ptr;
}

static CSBuffer* createBufferOfMap(const MapIndex& index) {
    return Asset::readFromFileWithDecryption(CSString::cstringWithFormat("maps/map[%03d][%03d][%03d].xmf", index.indices[0], index.indices[1], index.indices[2]), false, true);
}

bool Map::loadForOfflinePlay(GameOfflineMode mode, const MapIndex& index, const Player* player, const Player* otherPlayer, int ai) {
    CSAssert(ai >= 0 && ai <= AILevelMax, "invalid operation");
    ForceParam forces[MaxPlayers] = {};
    forces[0].player = player;
	forces[0].control = GameControlUser;
    
    CSBuffer* buffer = createBufferOfMap(index);
    
    if (!buffer) {
        return false;
    }
    if (ai) {
        MapThumb* thumb = new MapThumb(index, buffer);
        if (!thumb) {
            return false;
        }
#ifdef UseFullAIPlayersTest
        for (int i = 1; i < thumb->slots->count(); i++) {
            forces[i].player = otherPlayer ? otherPlayer : Player::playerForTraining();
            forces[i].control = (GameControl)(GameControlAI_0 + ai - 1);
        }
#else
        int allianceMask = 1 << thumb->slots->objectAtIndex(0).alliance;

        for (int i = 1; i < thumb->slots->count(); i++) {
            const MapThumbSlot& slot = thumb->slots->objectAtIndex(i);
            int currentAllianceMask = 1 << slot.alliance;
            
            if (!(allianceMask & currentAllianceMask)) {
                forces[i].player = otherPlayer ? otherPlayer : Player::playerForTraining();
				forces[i].control = (GameControl)(GameControlAI_0 + ai - 1);
                allianceMask |= currentAllianceMask;
            }
        }
#endif
        thumb->release();
        
        buffer->setPosition(0);
    }

    new Map(index, buffer, forces, 0, randInt(), GameLevelCorrectionNone, 0, 0);
    
    buffer->release();
    
    __map->_mode.originCode = __map->_mode.code = (GameMode)mode;
    __map->_mode.type = GameModeTypeOffline;
    
    return true;
}

bool Map::loadForOnlinePlay(GameOnlineMode mode, bool resume) {
    const Match* match = Account::sharedAccount()->match();
    if (!match) return false;

    int accountIndex = match->accountIndex();

    CSAssert(mode == match->mode, "invalid operation");
    mode = match->mode;
    
    ForceParam forces[MaxPlayers] = {};
    for (int i = 0; i < match->slots()->count(); i++) {
        ForceParam& force = forces[i];
        
        const MatchSlot& slot = match->slots()->objectAtIndex(i);
        
        force.player = slot.player();
        force.control = slot.control;
    }
    CSBuffer* buffer = createBufferOfMap(match->mapIndex);
    
    if (!buffer) {
        return false;
    }
    
    new Map(match->mapIndex, buffer, forces, accountIndex, match->randomSeed, match->levelCorrection, match->maxCardLevel, match->maxRuneLevel);
    
    buffer->release();
    
    const Party* party = Account::sharedAccount()->party();
    
    __map->_mode.code = (GameMode)mode;
    __map->_mode.originCode = (GameMode)match->mode;
    __map->_mode.type = GameModeTypeOnline;
    __map->_mode.context.online.localBuffer = new CSBuffer(PVPPacketCapacity);
    __map->_mode.context.online.observerDelayFrame = accountIndex < 0 ? match->observerDelay * GameMaxFramePerSecond : 0;
    __map->_mode.context.online.teamPlaying = __map->_mode.context.online.partyPlaying = (party && party->teamPlayerCount >= 2);
    if (!__map->_mode.context.online.teamPlaying && accountIndex >= 0) {
        int alliance = match->slots()->objectAtIndex(accountIndex).alliance;
        int allianceCount = 0;
        foreach(const MatchSlot&, slot, match->slots()) {
            if (slot.player() && slot.alliance == alliance) {
                allianceCount++;
            }
        }
        if (allianceCount >= 2) __map->_mode.context.online.teamPlaying = true;
    }
    __map->_mode.context.online.chatToTeam = __map->_mode.context.online.teamPlaying;
    
    CSBuffer* playBuffer = __map->_mode.context.online.globalBuffer = new CSBuffer(256 * 1024);
    
    playBuffer->writeFloat(0);      //time
    playBuffer->writeInt(0);        //frame
    match->mapIndex.writeTo(playBuffer);
    playBuffer->writeShort(GameVersion);
    playBuffer->writeByte(match->mode);
    
    for (int i = 0; i < MaxPlayers; i++) {
        if (forces[i].player) {
            playBuffer->writeBoolean(true);
            forces[i].player->writeTo(playBuffer, true);
        }
        else {
            playBuffer->writeBoolean(false);
        }
        playBuffer->writeByte(forces[i].control);
    }
    playBuffer->writeInt(match->randomSeed);
    playBuffer->writeByte(match->levelCorrection);
    playBuffer->writeShort(match->maxCardLevel);
    playBuffer->writeShort(match->maxRuneLevel);
    playBuffer->setPosition(playBuffer->length());
    
	__map->_paused = match->state == MatchStatePause;
    
    PVPGateway::sharedGateway()->requestMatchSync();

    if (__map->isPlaying()) {
        int i = match->accountIndex();
        if (i >= 0) {
            const Player* player = match->slots()->objectAtIndex(i).player();
            if (match->maxCardLevel) {
                foreach(const Card*, card, player->cards()) {
                    if (card->level > match->maxCardLevel) {
                        const CSString* str = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameCardLevelLimited), match->maxCardLevel);

                        __map->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), str, true));

                        break;
                    }
                }
            }
            if (match->maxRuneLevel) {
                foreach(const Rune*, rune, player->runes()) {
                    if (rune->level > match->maxRuneLevel) {
                        const CSString* str = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameRuneLevelLimited), match->maxRuneLevel);

                        __map->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), str, true));

                        break;
                    }
                }
            }
        }
    }
    return true;
}

bool Map::loadForReplay(GameReplayMode mode, const char* subpath) {
    CSBuffer* replayBuffer = Asset::readFromFileWithDecryption(subpath, true, true);
    if (!replayBuffer) return false;
    
    float totalTime = replayBuffer->readFloat();
    uint maxFrame = replayBuffer->readInt();
    MapIndex index(replayBuffer);
    int gameVersion = replayBuffer->readShort();
    if (gameVersion != GameVersion) return false;
    
    GameMode originMode = (GameMode)replayBuffer->readByte();
    
    ForceParam forces[MaxPlayers];

    for (int i = 0; i < MaxPlayers; i++) {
        forces[i].player = replayBuffer->readBoolean() ? Player::playerWithBuffer(replayBuffer, false) : NULL;
        forces[i].control = (GameControl)replayBuffer->readByte();
    }

    CSBuffer* buffer = createBufferOfMap(index);
    
    if (!buffer) {
        return false;
    }
    uint randomSeed = replayBuffer->readInt();
    GameLevelCorrection levelCorrection = (GameLevelCorrection)replayBuffer->readByte();
    int maxCardLevel = replayBuffer->readShort();
    int maxRuneLevel = replayBuffer->readShort();

    new Map(index, buffer, forces, -1, randomSeed, levelCorrection, maxCardLevel, maxRuneLevel);
    
    buffer->release();
    
    __map->_mode.code = (GameMode)mode;
    __map->_mode.type = GameModeTypeReplay;
    __map->_mode.originCode = originMode;
    __map->_mode.context.replay.totalTime = totalTime;
    __map->_mode.context.replay.maxFrame = maxFrame;
    __map->_mode.context.replay.dataPosition = replayBuffer->position();
    __map->_mode.context.replay.buffer = replayBuffer;
    
    replayBuffer->retain();
    
    return true;
}

bool Map::saveReplay(const char *subpath) {
    if (_mode.type == GameModeTypeOnline) {
        bool success;
        synchronized(_mode.context.online.globalBuffer, GameLockMap) {
            uint length = _mode.context.online.globalBuffer->length();
            _mode.context.online.globalBuffer->setLength(0);
            _mode.context.online.globalBuffer->writeFloat(_playTime);
            _mode.context.online.globalBuffer->writeInt(_mode.context.online.maxFrame);
            _mode.context.online.globalBuffer->setLength(length);
            
            success = Asset::writeToFileWithEncryption(_mode.context.online.globalBuffer, subpath, true);
        }
        return success;
    }
    else {
        return false;
    }
}

void Map::destroy() {
    if (__map) {
        delete __map;
    }
}

void Map::preload(const SkinData* skin) {
	if (skin) {
		for (int i = 0; i < MotionCount; i++) {
			if (skin->motions[i]) {
				Asset::sharedAsset()->animation(skin->motions[i]->animation)->preload(_preloadDelegate);
			}
		}
		foreach(const AnimationIndex&, animation, skin->preloadAnimations) {
			Asset::sharedAsset()->animation(animation)->preload(_preloadDelegate);
		}
	}
}

void Map::start(CSLayer* layer) {
    if (!_updateThread) {
		updateSetting();
		updateObjects(true);
		updateAnimations(layer);

		CSThread::mainThread()->resetTime();

        CSLock::resetWaitingTime();

        _alive = AliveActive;
        
        _updateThread = new CSThread();
		_updateThread->addTask(CSTask::task(CSDelegate0<void>::delegate(this, &Map::update)));
        _updateThread->start(false);
    }
}

void Map::onPause() {
    CSLog("onPause");
    if (_alive == AliveActive) {
        _alive = AlivePause;
        
        synchronized(_updateLock, GameLockMap) {}        //wait update finish
    }
}

void Map::onResume() {
    CSLog("onResume");
    if (_alive == AlivePause) {
        synchronized(_updateLock, GameLockMap) {
            _alive = AliveActive;
            _updateLock.signal();
        }
    }
}

void Map::onReload() {
    synchronized(_updateLock, GameLockMap) {
        delete _terrain;
		delete _vision;
        _minimap->release();
		delete _grid;

        CSBuffer* buffer = createBufferOfMap(_index);
        
        CSAssert(buffer, "invalid state");

		int levelCount = buffer->readLength();
		for (int i = 0; i < levelCount; i++) {
			int length = buffer->readInt();
			int nextpos = buffer->position() + length;
			buffer->setPosition(nextpos);
		}
        _terrain = new Terrain(buffer);
        buffer->release();
        
        _minimap = CSScratch::create(CSImageFormatRawRGBA4444, _terrain->width() - _terrain->thumbnail().left - _terrain->thumbnail().right, _terrain->height() - _terrain->thumbnail().top - _terrain->thumbnail().bottom);
		_vision = new Vision(_terrain);
        createGrid();
    }
    resetVision();
}

const CSResource* Map::preloadCallback(CSResourceType resourceType, const ushort* indices, int indexCount) {
    if (resourceType == CSResourceTypeImage) {
        CSAssert(indexCount >= 2, "invalid operation");
        ImageGroupIndex index(indices[0], indices[1]);
        if (!_preloadImages->containsObject(index)) {
            _preloadImages->addObject(index);
        }
    }
    return NULL;
}

PingLevel Map::pingLevel() const {
    return _mode.type == GameModeTypeOnline ? _mode.context.online.pingLevel : PingLevelDisconnected;
}

float Map::averageLatency() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.totalLatencyCount ? _mode.context.online.totalLatency / _mode.context.online.totalLatencyCount : 0;
}

bool Map::timeout(CSLayer* layer) {
#ifndef UseAutoPerformanceTest
	switch (_state) {
        case MapStateNone:
        case MapStateAuto:
            if (_latency.updateSeq >= LatencyUpdateDuration * _latency.framePerSecond && _latency.updateAccumLatency) {
				_latency.timeoutLatency = CSThread::mainThread()->timeAverage();
				
				CSThread::mainThread()->resetTime();

				_latency.localFramePerSecond = CSMath::clamp((int)(_latency.updateSeq / _latency.updateAccumLatency), GameMinFramePerSecond, GameMaxFramePerSecond);

                _latency.updateLatecy = _latency.updateAccumLatency / _latency.updateSeq;
				_latency.updateAccumLatency = 0.0f;
				_latency.updateSeq = 0;

#if !defined(UseBenchMarkTest) && !defined(UseUsePathTest)
                if (_mode.type == GameModeTypeOnline) {
                    const Match* match = Account::sharedAccount()->match();

                    if (!match) _mode.context.online.pingLevel = PingLevelDisconnected;
                    else if (match->state == MatchStatePlay) {
                        int bottleneck = 0;
                        if (_mode.context.online.currentLatency < PVPPingDelay_0) {
                            _mode.context.online.pingLevel = PingLevel_0;
                            bottleneck = 1;
                        }
                        else if (_mode.context.online.currentLatency < PVPPingDelay_1) _mode.context.online.pingLevel = PingLevel_1;
                        else if (_mode.context.online.currentLatency < PVPPingDelay_2) _mode.context.online.pingLevel = PingLevel_2;
                        else {
                            _mode.context.online.pingLevel = PingLevel_3;
                            bottleneck = -1;
                        }

                        if (!_offscreenProgress) {
                            CSLog("pvp latency:fps:%d ping:%.3f step:%d bottleneck:%d latency:%.3f/%.3f", 
                                _latency.localFramePerSecond, 
                                _mode.context.online.latency,
                                _mode.context.online.framePerStep,
                                bottleneck,
                                _mode.context.online.currentLatency,
                                _mode.context.online.totalLatencyCount ? _mode.context.online.totalLatency / _mode.context.online.totalLatencyCount : 0);

                            PVPGateway::sharedGateway()->requestMatchLatency(_latency.localFramePerSecond, bottleneck, _mode.context.online.totalLatency, _mode.context.online.totalLatencyCount);
                        }

                        _mode.context.online.currentLatency = 0;

                        GameRecord::recordMatch();
                    }
                }
                else if (_mode.type == GameModeTypeOffline) {
                    if (_latency.framePerSecond != _latency.localFramePerSecond) {
                        _latency.framePerSecond = _latency.localFramePerSecond;
                        _latency.frameDelayFloat = _latency.frameDelayFixed = fixed::One / _latency.localFramePerSecond;
                    }
                }
#endif

#ifdef CS_CONSOLE_DEBUG
				CSMemoryUsage memoryUsage = CSDevice::memoryUsage();

				CSLog("performance state:%d timeout(%.4f) update(%.4f) memory(%d / %d / %d)",
					_latency.localFramePerSecond,
					_latency.timeoutLatency,
					_latency.updateLatecy,
					(int)(memoryUsage.freeMemory / 1024768),
					(int)(memoryUsage.totalMemory / 1024768),
					(int)(memoryUsage.threshold / 1024768));
# ifdef CS_DIAGNOSTICS
				const CSDictionary<CSString, CSDiagnosticsRecord>* records = CSThread::mainThread()->diagnostics()->allRecords();
				if (records) {
					for (CSDictionary<CSString, CSDiagnosticsRecord>::ReadonlyIterator i = records->iterator(); i.isValid(); i.next()) {
						CSLog("%s:%s", (const char*)*i.key(), i.object().toString());
					}
				}
# endif
#endif
            }
            break;
    }
#endif
	if (_mode.type == GameModeTypeOnline) {
        if (_mode.context.online.localBuffer->remaining()) {
            const Match* match = Account::sharedAccount()->match();
            
            if (match && match->state == MatchStatePlay) {
                PVPGateway::sharedGateway()->requestMatchCommand(_mode.context.online.localBuffer->data()->bytes(), _mode.context.online.localBuffer->length());
            }
            _mode.context.online.localBuffer->clear();
        }
    }
    
    float delta = layer->timeoutInterval();
    
    bool refresh = false;
    
    _terrain->update(delta);
    if (_camera->update(delta)) refresh = true;
	synchronized(_camera, GameLockMap) {
		const CSCamera* pcamera = &_camera->capture();
		if (!_distortionAnimation->update(delta, pcamera)) _distortionAnimation->rewind();
		if (!_rallyPointAnimation->update(delta, pcamera)) _rallyPointAnimation->rewind();
	}
	decorations.update(delta);
	updateAnimations(layer);
	if (updateColor(delta)) refresh = true;
	if (updateVibration(delta)) refresh = true;
	updateEvents(delta);
	updateSounds(delta);
	updateEmoticons(delta);
	_minimapRemaining += delta;

    if (_preloadImages->count()) {
        Asset::sharedAsset()->image(_preloadImages->lastObject());
        _preloadImages->removeLastObject();
    }

    if (_updated) {
        _updated = false;
        layer->refresh();
        return true;
    }
    else if (refresh && !_offscreenProgress) {
        layer->refresh();
    }
    return false;
}

void Map::updatePlay() {
    updateSelection();
    updateMissions();
    updateSetting();
    updateForces();
    updateObjects(_running);
    _frame++;
}

void Map::update() {
    static const float OffscreenFrameDelay = 1.0f / GameMinFramePerSecond;
    
    for (; ; ) {
        double currentTime = CSTime::currentTime();
        
        float frameDelay = _latency.frameDelayFloat;
        
        synchronized(_updateLock, GameLockMap) {
            switch (_mode.type) {
                case GameModeTypeOnline:
                    {
                        uint maxFrame = _mode.context.online.maxFrame;
                        
                        if (maxFrame >= _mode.context.online.observerDelayFrame) {
                            maxFrame -= _mode.context.online.observerDelayFrame;
                        
                            if (_frame < maxFrame) {
                                uint frame = maxFrame - _frame;
                                
                                if (frame > PVPOffscreenLatency * _latency.framePerSecond) {
                                    frame -= _mode.context.online.framePerStep;
                                    
                                    CSLog("pvp offscreen(%d / %d):%d", _frame, maxFrame, frame);
#ifdef UsePVPTest
                                    _mode.context.online.stat.jump += frame * frameDelay;
#endif
                                    for (uint i = 0; i < frame; i++) {
                                        synchronized(_mode.context.online.globalBuffer, GameLockMap) {
                                            read(_mode.context.online.globalBuffer);
                                        }
                                        updatePlay();
                                        
                                        if (CSTime::currentTime() - currentTime >= OffscreenFrameDelay) {
                                            break;
                                        }
                                    }
                                    
                                    _mode.context.online.latency = 0.0f;
                                    
                                    _offscreenProgress = (double)_frame / maxFrame;
                                }
                                else {
                                    uint latencyFrame = _mode.context.online.latency / frameDelay;
                                    
                                    if (frame > _mode.context.online.framePerStep + latencyFrame + 1) {
                                        frame -= _mode.context.online.framePerStep + latencyFrame;
                                        
                                        CSLog("pvp jump(%d / %d):latency:%d / %.3f jump:%d ", _frame, maxFrame, latencyFrame, _mode.context.online.latency, frame);
#ifdef UsePVPTest
                                        _mode.context.online.stat.jump += frame * frameDelay;
#endif
                                        for (uint i = 0; i < frame; i++) {
                                            synchronized(_mode.context.online.globalBuffer, GameLockMap) {
                                                read(_mode.context.online.globalBuffer);
                                            }
                                            updatePlay();
                                        }
                                    }
                                    else {
                                        synchronized(_mode.context.online.globalBuffer, GameLockMap) {
                                            read(_mode.context.online.globalBuffer);
                                        }
                                        updatePlay();
                                    }
                                    _offscreenProgress = 0.0f;
                                }
                            }
							else if (_paused) {
								updateObjects(false);

								_offscreenProgress = 0.0f;
							}
                            else {
                                CSLog("pvp stuck(%d)", _frame);
#ifdef UsePVPTest
                                _mode.context.online.stat.latency += frameDelay;
#endif
                                _offscreenProgress = 0.0f;
                            }
#ifdef UsePVPTest
                            if (++_mode.context.online.stat.seq == 1000) {
                                CSLog("pvp stat:latency:%.3f jump:%.3f response:%.3f", _mode.context.online.stat.latency, _mode.context.online.stat.jump, _mode.context.online.stat.response);
                                
                                _mode.context.online.stat.latency = 0.0f;
                                _mode.context.online.stat.jump = 0.0f;
                                _mode.context.online.stat.response = 0.0f;
                                _mode.context.online.stat.seq = 0;
                            }
#endif
                        }
                    }
                    break;
                case GameModeTypeReplay:
                    if (_frame < _mode.context.replay.maxFrame) {
                        uint nextFrame = _mode.context.replay.nextFrame;
                        
                        if (_frame < nextFrame) {
                            uint frame = nextFrame - _frame;
                            
                            for (uint i = 0; i < frame; i++) {
                                read(_mode.context.replay.buffer);
                                updatePlay();
                                
                                if (CSTime::currentTime() - currentTime >= OffscreenFrameDelay) {
                                    break;
                                }
                            }
                            if (_frame < nextFrame) {
                                _offscreenProgress = (double)_frame / nextFrame;
                            }
                            else {
                                _offscreenProgress = 0.0f;
                            }
                        }
                        else {
                            _offscreenProgress = 0.0f;
                            if (!_paused) {
                                for (int i = 0; i <= _mode.context.replay.speed; i++) {
                                    read(_mode.context.replay.buffer);
                                    updatePlay();
                                }
                            }
                        }
                    }
                    else {
                        _offscreenProgress = 0.0f;
                        _state = MapStateEnd;
                    }
                    break;
                case GameModeTypeOffline:
                    _offscreenProgress = 0.0f;
                    if (!_paused) {
                        for (int i = 0; i <= _mode.context.single.speed; i++) {
                            updatePlay();
                        }
                    }
                    else {
                        updateObjects(false);
                    }
                    break;
            }
			/*
#ifdef UseDynamicSecretTable
# ifdef CS_SECRET_THREAD_SAFE
            if (++_secretResetSeq >= SecretUpdateDuration * _latency.framePerSecond) {
                CSSecret::sharedSecret()->reset(false);
                _secretResetSeq = 0;
            }
# else
#  error "CS_SECRET_THREAD_SAFE should be activated"
# endif
#else
# ifdef CS_SECRET_THREAD_SAFE
#  error "CS_SECRET_THREAD_SAFE not necessary"
# endif
#endif
			*/
        }

        _updated = true;
        
        _updateThread->keepAlive();
        _updateThread->autoreleasePool()->drain();
        
        switch (_alive) {
            case AliveActive:
                {
                    float currentDelay = CSTime::currentTime() - currentTime;
					if (++_latency.updateSeq > 0) {
						_latency.updateAccumLatency += currentDelay;
					}
                    if (currentDelay < frameDelay) {
                        CSThread::sleep(frameDelay - currentDelay);
                    }
                    else {
                        CSThread::yield();
                    }
                }
                break;
            case AlivePause:
                CSLog("update pause");
                synchronized(_updateLock, GameLockMap) {
                    if (_alive == AlivePause) {
                        _updateLock.wait();
                    }
                }
                if (_alive != AliveFinish) break;
            case AliveFinish:
                CSLog("update finishing");
                return;
        }
    }
}

GameResult Map::result() const {
    if (isPlaying()) {
        switch (_state) {
            case MapStateVictory:
                return GameResultVictory;
            case MapStateDefeat:
                return GameResultDefeat;
            case MapStateEnd:
                return GameResultDraw;
        }
    }
    return GameResultNone;
}

void Map::drawSpawnArea(CSGraphics* graphics) {
    const Force* force = _force;
    if (force) {
        synchronized(_phaseLock, GameLockMap) {
            foreach(const Unit*, unit, _units) {
                if (unit->force() == force && unit->compareType(ObjectMaskBase)) {
                    CSVector3 p = convertMapToWorldSpace(unit->point());
                    CSZRect rect(p.x - SpawnWidth * TileSize, p.y - SpawnHeight * TileSize, p.z, SpawnWidth * 2 * TileSize, SpawnHeight * 2 * TileSize);     //TODO
                    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSpawnArea), rect);
                }
            }
        }
    }
}

void Map::drawScreen(CSGraphics *graphics) {
    if (_offscreenProgress && _offscreenCapture) {
        graphics->pushTransform();
        graphics->translate(CSVector3(0, ProjectionHeight, 0));
        graphics->scale(CSVector3(1, -1, 1));
        graphics->drawImage(_offscreenCapture, CSRect(0, 0, ProjectionWidth, ProjectionHeight));            //TODO:grab image 는 뒤집힘
        graphics->popTransform();
        return;
    }
    graphics->push();
    synchronized(_camera, GameLockMap) {
        graphics->setCamera(_camera->capture());
    }
    graphics->translate(CSVector3(ProjectionWidth * 0.5f + vibration(), ProjectionHeight * 0.5f, 0));
    
    CSColor ambient = _terrain->ambient();
    CSLight light = _terrain->light();
    ambient *= _color.current;
    light.color *= (CSColor3)_color.current;
    if (_color.lightDegree < 1.0f) {
        float luminance = CSMath::lerp(0.6f, 1.0f, _color.lightDegree);
        ambient.r *= luminance;
        ambient.g *= luminance;
        ambient.b *= luminance;
        light.color *= luminance;
    }
    graphics->setColor(ambient);
    graphics->setLight(light);
    graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
    
    struct ObjectDrawing {
        Object* obj;
		float order;
		float ground;
        CSColor color;
		byte layer;
        bool distortion;
        
        ~ObjectDrawing() {
            obj->release();
        }
    };
    CSArray<ObjectDrawing>* objects = new CSArray<ObjectDrawing>(256);
    
    CSBoundingFrustum frustum = graphics->camera().boundingFrustum();
    const CSMatrix& view = graphics->camera().view();
    float zfar = graphics->camera().zfar();
    
	synchronized(_phaseLock, GameLockMap) {
#ifndef UseNoVision
		foreach(const IPoint&, p, _visionPoints) {
			byte luminances[9];
			IPoint vp;
			int vi = 0;
			for (vp.y = p.y - 1; vp.y <= p.y + 1; vp.y++) {
				for (vp.x = p.x - 1; vp.x <= p.x + 1; vp.x++) {
					luminances[vi++] = vp.x >= 0 && vp.x < _terrain->width() && vp.y >= 0 && vp.y < _terrain->height() ? tile(vp).luminance() : VisionLuminanceHidden;
				}
			}
			_vision->update(p.x, p.y, (luminances[0] + luminances[1] + luminances[3] + luminances[4]) / 4);
			_vision->update(p.x + 1, p.y, (luminances[1] + luminances[2] + luminances[4] + luminances[5]) / 4);
			_vision->update(p.x, p.y + 1, (luminances[3] + luminances[4] + luminances[6] + luminances[7]) / 4);
			_vision->update(p.x + 1, p.y + 1, (luminances[4] + luminances[5] + luminances[7] + luminances[8]) / 4);

			tileImpl(p).rollbackVision();
		}
		_visionPoints->removeAllObjects();
#endif
	
        foreach(Object*, obj, _objects) {
            ObjectDisplay display = obj->display();
            
            if (display.visible) {
                CSVector3 wp = obj->worldPoint(ObjectLayerBottom);
                
                if (frustum.contains(CSBoundingSphere(wp, (obj->displayRange() + ObjectDisplayAdditionalRange) * TileSize))) {
                    float z = ((wp.x * view.m13) + (wp.y * view.m23) + (wp.z * view.m33) + view.m43) * (1.0f / ((wp.x * view.m14) + (wp.y * view.m24) + (wp.z * view.m34) + view.m44));
                    float order = zfar * display.layer - z;
                    int i = 0;
                    while (i < objects->count() && order >= objects->objectAtIndex(i).order) {
                        i++;
                    }
                    obj->retain();
                    
                    ObjectDrawing& drawing = objects->insertObject(i);
                    drawing.obj = obj;
					drawing.order = order;
					drawing.ground = wp.z;
					drawing.layer = display.layer;
                    drawing.distortion = display.visible == ObjectVisibleDistortion;
                    
                    if (display.visible == ObjectVisibleNormal) {
                        drawing.color = ambient;
                        IPoint ip = obj->point();
						float luminance;
						if (ip.x >= 0 && ip.x < _terrain->width() && ip.y >= 0 && ip.y < _terrain->height()) {
							luminance = tile(ip).luminance() / 255.0f;
						}
						else {
							luminance = VisionLuminanceHidden / 255.0f;
						}
                        if (luminance < 1.0f) {
                            drawing.color.r *= luminance;
                            drawing.color.g *= luminance;
                            drawing.color.b *= luminance;
                        }
                    }
                    else {
                        drawing.color = CSColor::White;
                    }
                }
            }
        }
    }
	foreach(const ObjectDrawing&, obj, objects) {
		if (!obj.distortion) {
			obj.obj->draw(graphics, ObjectLayerReady);
		}
	}
    CSGraphics* shadowGraphics = _terrain->shadowGraphics();
    shadowGraphics->reset();
    shadowGraphics->clear(CSColor::White);
    shadowGraphics->setUsingRenderOrder(false);
    shadowGraphics->setLight(light);
    shadowGraphics->setColor(CSColor::Black);

    const CSVector3& cameraTarget = graphics->camera().target();

    CSMatrix shadowWorld = CSMatrix::Identity;
    shadowWorld.m11 = shadowWorld.m22 = shadowWorld.m33 = ShadowScale;
    shadowWorld.m41 = -cameraTarget.x * ShadowScale;
    shadowWorld.m42 = -cameraTarget.y * ShadowScale;
        
    foreach (const ObjectDrawing&, obj, objects) {
		if (!obj.distortion) {
			shadowWorld.m43 = -obj.ground * ShadowScale;
			shadowGraphics->setWorld(shadowWorld);
			obj.obj->draw(shadowGraphics, ObjectLayerShadow);
		}
    }
    shadowGraphics->commit();

	_terrain->drawSurface(graphics);

    graphics->setColor(CSColor::White);

    if (_color.lightDegree < 1.0f) {
        switch (_color.lightState) {
            case MapLightStateOffGrid:
                _grid->draw(graphics, CSMath::lerp(0.5f, 0.0f, _color.lightDegree));
                break;
            case MapLightStateOffSpawn:
                drawSpawnArea(graphics);
                break;
        }
    }

#ifndef UseNoVision
	_vision->draw(graphics);
#endif

#ifdef UsePathTest
    synchronized(_navMeshLock, GameLockMap) {
        _navMesh->draw(graphics, _terrain);
    }
#endif

	graphics->resetOffscreen();
	foreach (const ObjectDrawing&, obj, objects) {
        if (!obj.distortion) {
            graphics->setColor(obj.color);
            obj.obj->draw(graphics, ObjectLayerBottomCursor);
        }
    }
	graphics->commit();
	
	graphics->setLayer(1);

    foreach (const ObjectDrawing&, obj, objects) {
        if (!obj.distortion) {
            graphics->setColor(obj.color);
            obj.obj->draw(graphics, ObjectLayerBottom);
        }
    }
	graphics->commit();

	graphics->setUsingRenderOrder(false);
	foreach(const ObjectDrawing&, obj, objects) {
		if (!obj.distortion) {
			graphics->setColor(obj.color);
			obj.obj->drawEffect(graphics, ObjectLayerBottom, true);
		}
	}
	graphics->setUsingRenderOrder(true);
	graphics->commit();

    graphics->setColor(ambient);

	_terrain->drawWater(graphics, _vision->image());

	graphics->resetOffscreen();
    CSArray<const Object>* rallyTargets = new CSArray<const Object>();
    synchronized(_selectionLock, GameLockMap) {
        foreach(Unit*, unit, _selection.selections) {
            unit->drawRallyPoint(graphics, rallyTargets);
        }
    }
    {
        int i = 0;
        while (i < rallyTargets->count()) {
            const Object* target = rallyTargets->objectAtIndex(i);
            
            static const uint ObjectMaskUnitAndResource = ObjectMaskUnit | ObjectMaskResource;
            if (target->compareType(ObjectMaskUnitAndResource)) i++;
            else {
                _rallyPointAnimation->draw(graphics, target->worldPoint(ObjectLayerBottom));
                rallyTargets->removeObjectAtIndex(i);
            }
        }
    }
	graphics->commit();
    
	if (objects->count()) {
		int layer = objects->objectAtIndex(0).layer;
		int s = 0;
		do {
			int e = s;
			do {
				const ObjectDrawing& obj = objects->objectAtIndex(e);
				if (obj.layer != layer) {
					layer = obj.layer;
					break;
				}
				if (obj.distortion) {
					_distortionAnimation->drawScaled(graphics, obj.obj->worldPoint(ObjectLayerMiddle), obj.obj->collider());
				}
				else {
					graphics->setColor(obj.color);

					graphics->push();
					if (rallyTargets->containsObjectIdenticalTo(obj.obj)) {
						graphics->setStrokeColor(obj.obj->color(true, true));
						graphics->setStrokeWidth(3);
					}
					obj.obj->drawEffect(graphics, ObjectLayerMiddle, false);
					obj.obj->draw(graphics, ObjectLayerMiddle);
					graphics->pop();

					obj.obj->drawEffect(graphics, ObjectLayerMiddle, true);
				}
			} while (++e < objects->count());

			graphics->commit();
			for (int i = s; i < e; i++) {
				const ObjectDrawing& obj = objects->objectAtIndex(i);
				if (!obj.distortion) {
					graphics->setColor(obj.color);
					obj.obj->draw(graphics, ObjectLayerTop);
				}
			}
			graphics->commit();
			graphics->setUsingRenderOrder(false);
			for (int i = s; i < e; i++) {
				const ObjectDrawing& obj = objects->objectAtIndex(i);
				if (!obj.distortion) {
					graphics->setColor(obj.color);
					obj.obj->drawEffect(graphics, ObjectLayerTop, true);
				}
			}
			graphics->setUsingRenderOrder(true);
			graphics->commit();

			s = e;
		} while (s < objects->count());
	}

	graphics->setLayer(0);

	foreach (const ObjectDrawing&, obj, objects) {
        if (!obj.distortion) {
            graphics->setColor(obj.color);
            obj.obj->draw(graphics, ObjectLayerTopCursor);
        }
    }
	graphics->commit();

    rallyTargets->release();
    objects->release();

    graphics->pop();
    
    if (_offscreenProgress) {
        CSObject::retain(_offscreenCapture, graphics->grab());
    }
    else {
        CSObject::release(_offscreenCapture);
    }
}

void Map::drawMinimap(CSGraphics* graphics, const CSRect& rect) {
    synchronized(_phaseLock, GameLockMap) {
        foreach(const IPoint&, p, _minimapPoints) {
            Tile& tile = this->tileImpl(p);
            
            CSColor color = tile.color(p);
            
            uint rgba =
            ((((uint)CSMath::round(color.r * 15)) & 15) << 12) |
            ((((uint)CSMath::round(color.g * 15)) & 15) << 8) |
            ((((uint)CSMath::round(color.b * 15)) & 15) << 4) |
            (((uint)CSMath::round(color.a * 15)) & 15);
            
            _minimap->update(p.x - _terrain->thumbnail().left, p.y - _terrain->thumbnail().top, rgba);
            
            tile.rollbackMinimap();
        }
        _minimapPoints->removeAllObjects();
    }
    
    const Terrain::Thumbnail& thumbnail = _terrain->thumbnail();
    
    float scale = CSMath::min(rect.size.width / thumbnail.image->width(), rect.size.height / thumbnail.image->height());
    
    CSRect frame = CSRect(rect.center() - thumbnail.image->width() * 0.5f * scale,
                          rect.middle() - thumbnail.image->height() * 0.5f * scale,
                          thumbnail.image->width() * scale,
                          thumbnail.image->height() * scale);
                          
    graphics->drawImage(thumbnail.image, frame);
    
    graphics->drawImage(_minimap->image(), frame);
}
void Map::drawMinimapState(CSGraphics* graphics, const CSRect& rect, float eventScale) {
    graphics->push();
    
    const Terrain::Thumbnail& thumbnail = _terrain->thumbnail();
    
    int w = _terrain->width() - thumbnail.left - thumbnail.right;
    int h = _terrain->height() - thumbnail.top - thumbnail.bottom;
    float scale = CSMath::min(rect.size.width / w, rect.size.height / h);
    
    graphics->translate(CSVector3(rect.center() - (w * 0.5f + thumbnail.left) * scale, rect.middle() - (h * 0.5f + thumbnail.top) * scale, 0));
    
	synchronized(_minimapUnits, GameLockMap) {
		foreach(Unit*, unit, _minimapUnits) {
			unit->drawMinimap(graphics, scale, _minimapRemaining);
		}
	}
	_minimapRemaining = 0;

	if (_events->count()) {
		synchronized(_events, GameLockMap) {
			foreach(Event&, event, _events) {
				graphics->setColor(event.color);
				event.animation->setClientPosition(event.point * scale);
				event.animation->setClientScale(eventScale);
				event.animation->draw(graphics);
			}
		}
		graphics->setColor(CSColor::White);
	}
    
    FPoint quad[4];
    if (convertViewToMapSpace(CSVector2::Zero, quad[0], false) &&
        convertViewToMapSpace(CSVector2(ProjectionWidth, 0), quad[1], false) &&
        convertViewToMapSpace(CSVector2(ProjectionWidth, ProjectionHeight), quad[2], false) &&
        convertViewToMapSpace(CSVector2(0, ProjectionHeight), quad[3], false))
    {
        graphics->begin();
        graphics->addVertex(CSVertex(CSVector3((float)quad[0].x * scale, (float)quad[0].y * scale, 0)));
        graphics->addVertex(CSVertex(CSVector3((float)quad[1].x * scale, (float)quad[1].y * scale, 0)));
        graphics->addVertex(CSVertex(CSVector3((float)quad[2].x * scale, (float)quad[2].y * scale, 0)));
        graphics->addVertex(CSVertex(CSVector3((float)quad[3].x * scale, (float)quad[3].y * scale, 0)));
        graphics->addIndex(0);
        graphics->addIndex(1);
        graphics->addIndex(1);
        graphics->addIndex(2);
        graphics->addIndex(2);
        graphics->addIndex(3);
        graphics->addIndex(3);
        graphics->addIndex(0);
        graphics->end(NULL, GL_LINES);
    }
    graphics->pop();
}

void Map::drawAnimations(CSGraphics* graphics) {
	if (_animations->count()) {
		synchronized(_animations, GameLockMap) {
			if (_animations->count()) {
				for (CSDictionary<byte, GameAnimation, false>::Iterator i = _animations->iterator(); i.isValid(); i.next()) {
					GameAnimation& animation = i.object();
					if (animation.visible) {
						animation.animation->draw(graphics);
					}
				}
			}
		}
	}
}

//==============================================================================
#ifdef UseLeakTest
static CSArray<CSObject*>* __gameLeakObjects = new CSArray<CSObject*>();

void GameLeak_addObject(CSObject* obj) {
    synchronized(__gameLeakObjects) {
        __gameLeakObjects->addObject(obj);
    }
}
void GameLeak_removeObject(CSObject* obj) {
    synchronized(__gameLeakObjects) {
        __gameLeakObjects->removeObjectIdenticalTo(obj);
    }
}
void GameLeak_showObjects() {
    synchronized(__gameLeakObjects) {
        int count = __gameLeakObjects->count();
        for (int i = 0; i < count; i++) {
            CSObject* obj = __gameLeakObjects->objectAtIndex(i);
            CSLog("game leak object : %s - %08X", typeid(obj).name(), obj);
        }
        __gameLeakObjects->removeAllObjects();
    }
}
#endif
