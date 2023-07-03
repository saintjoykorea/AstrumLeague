//
//  Map+environment.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "Sound.h"

#include "PVPGateway.h"

#define EventRange              10
#define WarningRange            20
#define WarningDuration         5.0f
#define SoundMaxDistance        16
#define SoundRefDistance		2.0f
#define SoundRollOffFactor		0.75f
#define SoundDuration           0.2f
#define VibrationDuration       0.8f
#define VibrationBounce         6
#define LightDuration           0.2f

static float distanceSoundVolume(float distance) {		//inverse distance clamped from OpenAL
	return SoundRefDistance / (SoundRefDistance + SoundRollOffFactor * (CSMath::max(distance, SoundRefDistance) - SoundRefDistance));
}

static void objectAnimationSoundCallback(const CSAnimation* animation, const ushort* indices, int indexCount, float volume, bool essential) {
    CSAssert(indexCount == 3, "invalid data");
    SoundIndex index(indices[0], indices[1], indices[2]);
    
    if (!Map::sharedMap()->offscreenProgress()) {
		CSCamera camera;
		Map::sharedMap()->camera()->capture(camera);
        
		CSMatrix capture;
        if (animation->capture(animation->progress(), &camera, capture)) {
            CSVector2 p0 = (CSVector2)capture.translationVector();
            CSVector2 p1 = (CSVector2)camera.target();
                
            float d = p0.distance(p1) / TileSize;
                
            if (d < SoundMaxDistance) {
				volume *= distanceSoundVolume(d);
            }
            else {
                return;
            }
        }
        Map::sharedMap()->sound(index, volume, essential);
    }
}

CSAnimationSoundDelegate* const Map::ObjectAnimationSoundDelegate = new CSAnimationSoundDelegate0(&objectAnimationSoundCallback);

bool Map::event(const Force* force, const AnimationIndex& animation, const CSColor& color, const CSVector2& point) {
    const Force* playerForce = _force;
    
    if (playerForce && force == playerForce) {
        event(animation, color, point);
        return true;
    }
    return false;
}

bool Map::event(const Force* force, bool toAlliance, const AnimationIndex& animation, const CSColor& color, const CSVector2& point) {
    const Force* playerForce = _force;
    
    if (playerForce && force && (force->alliance() == playerForce->alliance()) == toAlliance) {
        event(animation, color, point);
        return true;
    }
    return false;
}

void Map::event(const AnimationIndex& animation, const CSColor& color, const CSVector2& point) {
	if (!_offscreenProgress && controlState(MapControlMinimap) != MapControlStateHidden) {
		synchronized(_events, GameLockMap) {
			foreach(Event&, otherEvent, _events) {
				if (otherEvent.animation->index() == animation && otherEvent.color == color && otherEvent.point.distanceSquared(point) <= EventRange * EventRange) {
					return;
				}
			}
			new (&_events->addObject()) Event(animation, color, point);
		}
	}
}

bool Map::warn(const ImageIndex& icon, const CSVector2& point, bool essential) {
	if (!_offscreenProgress && controlState(MapControlMinimap) != MapControlStateHidden && (!_warning.remaining || essential)) {
		CSVector2 target;
		synchronized(_camera, GameLockMap) {
			const CSCamera& camera = _camera->capture();
			target = (CSVector2)camera.target() / TileSize;
		}
		float d2 = point.distanceSquared(target);

		if (d2 >= WarningRange * WarningRange) {
			if (!_warning.remaining) sound(SoundFxWarning, 1.0f, true);

			_warning.icon = icon;
			_warning.point = point;
			_warning.remaining = WarningDuration;
			_warning.essential = essential;
			return true;
		}
	}
	return false;
}

void Map::sound(const SoundIndex& index, float volume, bool essential) {
    if (!_offscreenProgress && _setting->soundEnabled) {
        synchronized(_sounds, GameLockMap) {
			for (int i = 0; i < _sounds->count(); i++) {
				SoundEntry& otherEntry = _sounds->objectAtIndex(i);

				if (otherEntry.index == index && otherEntry.essential == essential) {
					if (otherEntry.time == 0 && otherEntry.volume < volume) {
						otherEntry.volume = volume;
						
						int j = i;
						while (j > 0) {
							SoundEntry& prevEntry = _sounds->objectAtIndex(j - 1);

							if (essential != prevEntry.essential || volume <= prevEntry.volume) {
								break;
							}
							j--;
						}
						if (j != i) {
							SoundEntry e = _sounds->objectAtIndex(i);
							_sounds->removeObjectAtIndex(i);
							_sounds->insertObject(j, e);
						}
					}
					return;
				}
			}
			int i = 0;
			while (i < _sounds->count()) {
				const SoundEntry& otherEntry = _sounds->objectAtIndex(i);

				if (essential && !otherEntry.essential) {
					break;
				}
				if (essential == otherEntry.essential && volume > otherEntry.volume) {
					break;
				}
				i++;
			}
            SoundEntry& entry = _sounds->insertObject(i);
            entry.index = index;
            entry.volume = volume;
			entry.essential = essential;
			entry.time = 0;
        }
    }
}

void Map::sound(const SoundIndex& index, const Object* target, float volume, bool essential) {
    if (!_offscreenProgress && _setting->soundEnabled && target->isLocated() && isVisible(target->bounds())) {
        float d;
        synchronized(_camera, GameLockMap) {
            d = ((CSVector2)target->point()).distance((CSVector2)_camera->capture().target() / (float)TileSize);
        }
        if (d < SoundMaxDistance) {
			volume *= distanceSoundVolume(d);

			sound(index, volume, essential);
        }
    }
}

void Map::sound(const SoundIndex &index, const Force* force, float volume, bool essential) {
    AssertOnUpdateThread();
    
    if (!_offscreenProgress && _setting->soundEnabled) {
        float distance2 = SoundMaxDistance * SoundMaxDistance;
        
        CSVector2 position;
        synchronized(_camera, GameLockMap) {
            position = (CSVector2)_camera->capture().target() / (float)TileSize;
        }
        
        const Unit* nearUnit = NULL;
        
        foreach (const Unit*, unit, _units) {
            if (unit->force() == force && unit->isLocated()) {
                const CSVector2& target = (CSVector2)unit->point();
                
                float d2 = position.distanceSquared(target);
                
                if (d2 < distance2) {
                    nearUnit = unit;
                    distance2 = d2;
                }
            }
        }
        if (nearUnit) {
            float d = CSMath::sqrt(distance2);
            
            if (d < SoundMaxDistance) {
                sound(index, volume * (SoundMaxDistance - d) / SoundMaxDistance, essential);
            }
        }
    }
}

void Map::updateSounds(float delta) {
	if (_sounds->count()) {
		CSArray<SoundEntry>* sounds = new CSArray<SoundEntry>();
		synchronized(_sounds, GameLockMap) {
			int i = 0;
			while (i < _sounds->count()) {
				SoundEntry& entry = _sounds->objectAtIndex(i);
				if (entry.time >= SoundDuration) {
					_sounds->removeObjectAtIndex(i);
				}
				else {
					if (entry.time == 0) {
						sounds->addObject(entry);
					}
					entry.time += delta;
					i++;
				}
			}
		}
		foreach(const SoundEntry&, entry, sounds) {
			Sound::play(SoundControlEffect, entry.index, entry.essential ? SoundPlayEffectEssential : SoundPlayEffect, entry.volume);
		}
		sounds->release();
	}
}

void Map::voice(Voice type) {
	if (!_offscreenProgress && _setting->voiceEnabled) {
		switch (type) {
			case VoiceCommand:
			case VoiceSelect:
				break;
			default:
				CSAssert(false, "invalid operation");
				return;
		}
		SoundIndex index = SoundIndex::None;

		synchronized(_selectionLock, GameLockMap) {
			if (_selection.selections == _selection.expandedSelections) {
				foreach(const CSArray<Unit>*, units, _selection.groupedSelections) {
					const Unit* unit = units->objectAtIndex(0);
					const SkinData* skin = unit->skinData();
					if (skin) {
						const VoiceData* voice = skin->voice();
						if (voice && voice->sounds[type]) {
							index = voice->sounds[type]->objectAtIndex(Sound::voiceCounter() % voice->sounds[type]->count());
							break;
						}
					}
				}
			}
			else {
				foreach(const Unit*, unit, _selection.selections) {
					const SkinData* skin = unit->skinData();
					if (skin) {
						const VoiceData* voice = skin->voice();
						if (voice && voice->sounds[type]) {
							index = voice->sounds[type]->objectAtIndex(Sound::voiceCounter() % voice->sounds[type]->count());
							break;
						}
					}
				}
			}
		}
		if (index) {
			Sound::play(SoundControlEffect, index, SoundPlayVoiceSingle);
		}
	}
}

void Map::voice(Voice type, const Unit* unit) {
	if (!_offscreenProgress && _setting->voiceEnabled) {
		SoundPlay play;
		
		switch (type) {
			case VoiceSpawn:
				if (!isPlaying() || unit->force() != _force) return;
				play = SoundPlayVoiceSingle;
				break;
			case VoiceDestroy:
				if (!unit->isVisible(_force)) return;
				play = SoundPlayVoice;
				break;
			default:
				CSAssert(false, "invalid operation");
				return;
		}

		const SkinData* skin = unit->skinData();

		if (skin) {
			const VoiceData* voice = skin->voice();

			if (voice && voice->sounds[type]) {
				float d;
				synchronized(_camera, GameLockMap) {
					d = ((CSVector2)unit->point()).distance((CSVector2)_camera->capture().target() / (float)TileSize);
				}
				if (d < SoundMaxDistance) {
					SoundIndex index = voice->sounds[type]->objectAtIndex(randInt(0, voice->sounds[type]->count() - 1));

					Sound::play(SoundControlEffect, index, play);
				}
			}
		}
	}
}

void Map::announce(AnnounceIndex index) {
	if (!_offscreenProgress && _setting->voiceEnabled) {
		const AnnounceData& data = Asset::sharedAsset()->announce(Account::sharedAccount()->announce);

		SoundIndex voice = data.voice(index);

		if (voice) {
			Sound::play(SoundControlEffect, voice, SoundPlayVoiceSingle);
		}
	}
}

void Map::announce(const Force* force, AnnounceIndex index) {
	if (_playing == PlayingParticipate && _force == force) {
		announce(index);
	}
}

void Map::announce(const Force* force, bool toAlliance, AnnounceIndex index) {
	if (_playing == PlayingParticipate && _force != force && (_force->alliance() == force->alliance()) == toAlliance) {
		announce(index);
	}
}

float Map::vibration() const {
    if (_vibration.level) {
        float progress = _vibration.seq / VibrationDuration;
        
        float v = _vibration.level * (1.0f - progress) * CSMath::cos(progress * VibrationBounce * FloatPi * 2);
        
        return v;
    }
    else {
        return 0.0f;
    }
}
void Map::vibrate(int level, const Object* target) {
    if (!_offscreenProgress) {
        if (target) {
            if (target->isLocated() && isVisible(target->bounds())) {
                float d;
                synchronized(_camera, GameLockMap) {
                    d = target->point().distance((CSVector2)_camera->capture().target() / (float)TileSize);
                }
                if (d < EventRange) {
                    float v = level * (EventRange - d) / EventRange;
                    
                    if (_vibration.level < v) {
                        _vibration.level = v;
                        _vibration.seq = 0.0f;
                    }
                }
            }
        }
        else {
            _vibration.level = level;
            _vibration.seq = 0.0f;
        }
    }
}

void Map::animation(int key, const AnimationIndex &index, const GameLayerIndex &layer0, const CSVector2 &offset0, const GameLayerIndex &layer1, const CSVector2 &offset1) {
    synchronized(_animations, GameLockMap) {
        new (&_animations->setObject(key)) GameAnimation(index, layer0, offset0, layer1, offset1);
    }
}

bool Map::pushMessage(const Force* force, const Message* msg) {
    const Force* playerForce = _force;
    
    if (playerForce && force == playerForce) {
        return pushMessage(msg);
    }
    return false;
}

bool Map::pushMessage(const Force* force, bool toAlliance, const Message* msg) {
    const Force* playerForce = _force;
    
    if (playerForce && force && (force->alliance() == playerForce->alliance()) == toAlliance) {
        return pushMessage(msg);
    }
    return false;
}

bool Map::pushMessage(const Message* msg) {
    if (_offscreenProgress || controlState(MapControlMessage) == MapControlStateHidden) {
        return false;
    }
    synchronized(_messages, GameLockMap) {
        if (msg->essential || !_messages->containsObject(msg)) {
            _messages->addObject(msg);
        }
    }
    return true;
}

const Message* Map::popMessage() {
	if (_messages->count()) {
		synchronized(_messages, GameLockMap) {
			if (_messages->count()) {
				const Message* msg = _messages->objectAtIndex(0);
				msg->retain();
				msg->autorelease();
				_messages->removeObjectAtIndex(0);
				return msg;
			}
		}
	}
    return NULL;
}

const Mission* Map::popMission() {
	if (_missions->count()) {
		synchronized(_missions, GameLockMap) {
			foreach(Mission*, mission, _missions) {
				if (!mission->confirm) {
					mission->confirm = true;
					mission->retain();
					mission->autorelease();
					return mission;
				}
			}
		}
	}
    return NULL;
}

const Mission* Map::getMission(uint key) const {
	if (_missions->count()) {
		synchronized(_missions, GameLockMap) {
			foreach(Mission*, mission, _missions) {
				if (key == mission->key) {
					mission->confirm = true;
					mission->retain();
					mission->autorelease();
					return mission;
				}
			}
		}
	}
    return NULL;
}

void Map::updateMissions() {
	if (_missions->count()) {
		synchronized(_missions, GameLockMap) {
			int i = 0;
			while (i < _missions->count()) {
				Mission* mission = _missions->objectAtIndex(i);

				if (mission->update()) {
					i++;
				}
				else {
					_missions->removeObjectAtIndex(i);
				}
			}
		}
	}
}

const CSString* Map::settingValueString(const CSString* format, const byte* valueIndices, uint valueCount) {
    CSString* str = CSStringDisplay::customString(format, '$', [&] (CSString* dest, const char* func, const char** params, uint paramCount) {
        if (strcmp(func, "emp") == 0) {
            if (params[0]) {
                int index = CSString::intValue(params[0]);
                switch (index) {
                    case 0:
                        dest->append("#color(0032FFFF)");
                        break;
                    case 1:
                        dest->append("#color(00FFFFFF)");
                        break;
					case 2:
						dest->append("#color(2F75B5FF)");
						break;
                }
            }
            else {
                dest->append("#color()");
            }
            return true;
        }
        else if (strcmp(func, "value") == 0) {
            if (params[0]) {
                uint index = CSString::intValue(params[0]);
                if (index < valueCount) {
                    uint valueIndex = bytesToShort(valueIndices + index * 2);

					if (valueIndex < countof(_setting->values)) {
						fixed v = _setting->values[valueIndex];
						if (!params[1]) {
							dest->appendFormat("%d", (int)v);
						}
						else if (strcmp(params[1], "point") == 0) {
							dest->appendFormat("%.2f", (float)v);
						}
						else if (strcmp(params[1], "rate") == 0) {
							dest->appendFormat("%d%%", (int)(v * 100));
						}
						else if (strcmp(params[1], "time") == 0) {
							dest->appendFormat("%02d:%02d", (int)v / 60, (int)v % 60);
						}
					}
                }
            }
            return true;
        }
        else if (strcmp(func, "force") == 0) {
            if (params[0] && params[1]) {
                const Force* force = NULL;
                if (strncmp(params[0], "slot", 4) == 0) {
                    uint forceIndex = CSString::intValue(params[0] + 4);
                    if (forceIndex < MaxPlayers) {
                        force = _forces[forceIndex];
                    }
                }
                else if (strncmp(params[0], "rank", 4) == 0) {
                    const CSArray<Force>* forces = sortedForcesByAttain();
                    uint rank = CSString::intValue(params[0] + 4);
                    if (rank < forces->count()) {
                        force = forces->objectAtIndex(rank);
                    }
                }
                if (force) {
                    if (strcmp(params[1], "name") == 0) {
                        dest->appendFormat("#color(%08X)%s#color()", (uint)force->originColor(), (const char*)*force->player->name);
                    }
                    else if (strncmp(params[1], "attain", 6) == 0) {
                        uint attainIndex = CSString::intValue(params[1] + 6);
                        if (attainIndex < 3) {
							fixed v = force->attainValue(attainIndex);

							if (!params[2]) {
								dest->appendFormat("%d", (int)v);
							}
							else if (strcmp(params[2], "point") == 0) {
								dest->appendFormat("%.2f", (float)v);
							}
							else if (strcmp(params[2], "rate") == 0) {
								dest->appendFormat("%d%%", (int)(v * 100));
							}
							else if (strcmp(params[2], "time") == 0) {
								dest->appendFormat("%02d:%02d", (int)v / 60, (int)v % 60);
							}
                        }
                    }
                }
            }
            return true;
        }
        return false;
    });
    return str;
}

const Speech* Map::speech() const {
	if (_speech) {
		synchronized(_controlLock, GameLockMap) {
			if (_speech) {
				_speech->retain();
				_speech->autorelease();
				return _speech;
			}
		}
	}
    return NULL;
}

void Map::nextSpeech() {
    AssertOnMainThread();
    
    switch (_mode.type) {
        case GameModeTypeOnline:
			writeStep(_mode.context.online.localBuffer, 0);
            break;
        case GameModeTypeOffline:
			if (_speech) {
				synchronized(_controlLock, GameLockMap) {
					if (_speech) {
						_speech->confirm = true;
					}
				}
			}
            break;
    }
}

void Map::skipWaiting() {
	AssertOnMainThread();

	if (_setting->skipReady) {
		switch (_mode.type) {
			case GameModeTypeOnline:
				writeStep(_mode.context.online.localBuffer, 1);
				break;
			case GameModeTypeOffline:
				_setting->skipCommit = true;
				break;
		}
	}
}

uint Map::timer() const {
    fixed* timer = _timer;
    
    return timer ? (uint)*timer : 0;
}

int Map::compareForceByAttain(const Force* a, const Force* b) const {
	for (int i = 0; i < 3; i++) {
		MapAttainOrder order = _attainOrders[i];
		switch (order) {
			case MapAttainOrderAscent:
				{
					fixed av = a->attainValue(i);
					fixed bv = b->attainValue(i);
					if (av > bv) return 1;
					else if (av < bv) return -1;
				}
				break;
			case MapAttainOrderDescent:
				{
					fixed av = a->attainValue(i);
					fixed bv = b->attainValue(i);
					if (av < bv) return 1;
					else if (av > bv) return -1;
				}
				break;
		}
	}
	return a->result == GameResultDefeat && b->result != GameResultDefeat;
}

const CSArray<Force>* Map::sortedForcesByAttain() const {
    CSArray<Force>* forces = CSArray<Force>::arrayWithCapacity(MaxPlayers);
    for (int i = 0; i < MaxPlayers; i++) {
        if (_forces[i]) {
            forces->addObject(_forces[i]);
        }
    }

	forces->sort(this, &Map::compareForceByAttain);

    return forces;
}

MapControlStateCommand::MapControlStateCommand(MapControlState state, const ActionTarget* target) : state(state), target(CSObject::retain(target)) {

}
MapControlStateCommand::MapControlStateCommand(const MapControlStateCommand& other) : state(other.state), target(CSObject::retain(other.target)) {

}
MapControlStateCommand::~MapControlStateCommand() {
	CSObject::release(target);
}

MapControlStateCommand MapControlStateCommand::operator=(const MapControlStateCommand& other) {
	state = other.state;
	CSObject::retain(target, other.target);
	return *this;
}

MapControlState Map::controlStateSelection(const Unit* unit) const {
    synchronized(_controlLock, GameLockMap) {
		const MapControlState* state = _control.selectionStates->tryGetObjectForKey(unit->key);
        
        if (state) {
            return *state;
        }
    }
    return controlState(MapControlSelect);
}

MapControlState Map::controlStateCommand(const CommandIndex& command, const ActionTarget** target) const {
    synchronized(_controlLock, GameLockMap) {
        const MapControlStateCommand* state = _control.commandStates->tryGetObjectForKey(command);
        
        if (target) {
            if (state && state->state == MapControlStateEnabled) {
                *target = state->target;
            }
            else {
				*target = NULL;
            }
        }
        if (state) {
            return state->state;
        }
    }
    return controlState(MapControlCommand);
}

MapControlState Map::controlStateSpawn(const UnitIndex& spawn, const ActionTarget** target) const {
    synchronized(_controlLock, GameLockMap) {
        const MapControlStateCommand* state = _control.spawnStates->tryGetObjectForKey(spawn);
        
        if (target) {
            if (state && state->state == MapControlStateEnabled) {
                *target = state->target;
            }
            else {
				*target = NULL;
            }
        }
        if (state) {
            return state->state;
        }
    }
    return controlState(MapControlSpawn);
}

bool Map::isActionAllowed(ActionRestriction restriction) {
	return !isSingle() || (_mode.context.single.actionRestriction & restriction) == 0;
}

bool Map::updateColor(float delta) {
	bool refresh = false;
    if (_color.time) {
        float p;
        if (_color.time > delta) {
            p = delta;
            _color.time -= delta;
        }
        else {
            p = _color.time;
            _color.time = 0.0f;
        }
        _color.current += _color.delta * p;
		//refresh = true;
    }
    if (_color.lightState == MapLightStateOn) {
        if (_color.lightDegree < 1.0f) {
            _color.lightDegree = CSMath::min(_color.lightDegree + delta / LightDuration, 1.0f);
			refresh = true;
        }
    }
    else if (_color.lightDegree) {
		_color.lightDegree = CSMath::max(_color.lightDegree - delta / LightDuration, 0.0f);
		refresh = true;
    }
	return refresh;
}

void Map::updateEvents(float delta) {
	if (_events->count()) {
		synchronized(_events, GameLockMap) {
			int i = 0;
			while (i < _events->count()) {
				Event& event = _events->objectAtIndex(i);

				if (event.animation->update(delta)) {
					i++;
				}
				else {
					_events->removeObjectAtIndex(i);
				}
			}
		}
	}
    if (_warning.remaining) {
        _warning.remaining = CSMath::max(_warning.remaining - delta, 0.0f);
    }
}

bool Map::updateVibration(float delta) {
    if (_vibration.level) {
        _vibration.seq += delta;
        
        if (_vibration.seq >= VibrationDuration) {
            _vibration.seq = 0.0f;
            _vibration.level = 0.0f;
        }
		return true;
    }
	return false;
}

void Map::updateAnimations(CSLayer* layer) {
	if (_animations->count()) {
		float delta = layer->timeoutInterval();

		synchronized(_animations, GameLockMap) {
			if (_animations->count()) {
				for (CSDictionary<byte, GameAnimation, false>::Iterator i = _animations->iterator(); i.isValid(); i.next()) {
					GameAnimation& animation = i.object();

					bool prevVisible = animation.visible;

					animation.visible = true;
					for (int i = 0; i < 2; i++) {
						if (animation.positions[i].layerIndex.indices[0] != GameLayerKeyMap) {
							CSLayer* sublayer = layer->findLayer(animation.positions[i].layerIndex, false);

							if (sublayer) {
								if (animation.positions[i].width) {
									animation.animation->setClientScale(animation.scale * sublayer->width() / animation.positions[i].width);
								}
								CSVector2 lp = sublayer->centerMiddle() + animation.positions[i].offset;
								sublayer->convertToViewSpace(&lp);
								animation.animation->setClientPosition(lp, (bool)i);
							}
							else {
								animation.visible = false;
								break;
							}
						}
						else {
							CSVector3 wp = convertMapToWorldSpace(animation.positions[i].offset);
							CSVector3 vp;
							synchronized(_camera, GameLockMap) {
								vp = CSVector3::transformCoordinate(wp, _camera->capture().viewProjection());
							}
							CSVector2 lp((0.5f + vp.x * 0.5f) * ProjectionWidth, (0.5f - vp.y * 0.5f) * ProjectionHeight);
							animation.animation->setClientPosition(lp, (bool)i);
						}
					}
					if (prevVisible && animation.visible && !animation.animation->update(delta)) {
						i.remove();
					}
				}
			}
		}
	}
}

bool Map::isSingle() const {
    switch (_mode.code) {
        case GameModeTestOffline:
            return true;
    }
    return false;
}

void Map::setFocusing(bool focusing) {
    AssertOnMainThread();
    
    switch (_playing) {
        case PlayingObserveAll:
            if (focusing) {
				if (!_force) {
					for (int i = 0; i < MaxPlayers; i++) {
						if (_forces[i] && _forces[i]->player->userId) {
							_force = _forces[i];
							break;
						}
					}
				}
				if (_force) {
					resetVision();
				}
                _playing = PlayingObserveVision;
            }
            break;
        case PlayingObserveVision:
            if (!focusing) {
                if (_force) {
                    resetVision();
                }
                _playing = PlayingObserveAll;
            }
            break;
    }
}

void Map::observe(Force* force) {
    AssertOnMainThread();
#ifdef UseObserverPlayTest
    if (force)
#else
    if (_playing != PlayingParticipate)
#endif
    {
        if (_force != force) {
			_force = force;
            
            synchronized(_selectionLock, GameLockMap) {
                _selection.mask = 0;
                _selection.groupedSelections->removeAllObjects();
                _selection.expandedSelections->removeAllObjects();
                _selection.selections = _selection.expandedSelections;
                _selection.navigations->removeAllObjects();
                CSObject::release(_selection.target);
                _selection.following = false;
            }
            if (_force) {
                synchronized(_phaseLock, GameLockMap) {
                    foreach(Unit*, unit, _units) {
                        if (unit->force() == _force && unit->navigating()) {
                            resetNavigation(unit, unit->navigating());
                        }
                    }
                }
            }
        }
        if (_playing == PlayingObserveVision) {
            resetVision();
        }
    }
}

float Map::replayTotalTime() const {
    return _mode.type == GameModeTypeReplay ? _mode.context.replay.totalTime : 0.0f;
}
float Map::replayProgress() const {
    return _mode.type == GameModeTypeReplay ? (float)((double)_frame / _mode.context.replay.maxFrame) : 0.0f;
}
float Map::replayNextProgress() const {
    return _mode.type == GameModeTypeReplay && _frame < _mode.context.replay.nextFrame ? (float)((double)_mode.context.replay.nextFrame / _mode.context.replay.maxFrame) : 0.0f;
}
void Map::setReplayProgress(float progress) {
    if (_mode.type == GameModeTypeReplay) {
        uint nextFrame = _mode.context.replay.maxFrame * progress;
        if (_mode.context.replay.nextFrame < nextFrame) {
            _mode.context.replay.nextFrame = nextFrame;
        }
    }
}
void Map::stopReplayProgress() {
    if (_mode.type == GameModeTypeReplay) {
        _mode.context.replay.nextFrame = 0;
    }
}

bool Map::isOnlinePartyPlaying() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.partyPlaying;
}
uint Map::onlineFrame() const {
    return _mode.type == GameModeTypeOnline ? _mode.context.online.maxFrame : 0;
}
uint Map::onlineObserverDelayRemaining() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.maxFrame < _mode.context.online.observerDelayFrame ? CSMath::max((_mode.context.online.observerDelayFrame - _mode.context.online.maxFrame) / GameMaxFramePerSecond, (uint)1) : 0;
}

int Map::speed() const {
    switch (_mode.type) {
        case GameModeTypeOffline:
            return _mode.context.single.speed;
        case GameModeTypeReplay:
            return _mode.context.replay.speed;
    }
    return 0;
}
void Map::setSpeed(int speed) {
    switch (_mode.type) {
        case GameModeTypeOffline:
            _mode.context.single.speed = speed;
            break;
        case GameModeTypeReplay:
            _mode.context.replay.speed = speed;
            break;
    }
}
void Map::setAuto(bool on) {
    AssertOnMainThread();
    
    if (_playing == PlayingParticipate) {
        if (_mode.type == GameModeTypeOnline) {
            PVPGateway::sharedGateway()->requestMatchAuto(on);
        }
        else {
            _force->setControl(on ? GameControlAuto : GameControlUser);
        }
    }
}

bool Map::isAuto() const {
    return _playing == PlayingParticipate && _force->ai() != NULL;
}

