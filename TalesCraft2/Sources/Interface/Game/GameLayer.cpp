//
//
//  GameLayer.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 12..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#include "GameLayer.h"

#include "GameLayerIndex.h"

#include "Sound.h"
#include "GameRecord.h"

#include "PVPGateway.h"

#include "LightEffect.h"
#include "StrokeEffect.h"
#include "LineEffect.h"

#include "WaitingLayer.h"
#include "MessageLayer.h"

#include "UnitAbilityLayer.h"
#include "RuneAbilityLayer.h"
#include "LobbyLayer.h"
#include "GameResultLayer.h"

#define DoubleClickDuration             0.5f
#define DoubleClickScreenDistance       70.0f

//오토타게팅제거 2022-07-11
//#define DoubleClickAutoTargetDuration   1.0f

#define MessageStateDuration            5.0f

#define LongClickDuration               0.5f

#define ProduceCounterDuration          0.1f

#define TargetStickRange                4
#define TargetPointVerticalOffset       40
#define TargetScrollDegree              20
#define TargetScrollBoundaryLeft        100
#define TargetScrollBoundaryRight       100
#define TargetScrollBoundaryTop         100
#define TargetScrollBoundaryBottom      150

#define PVPSyncWaiting                  30
#define PVPSyncDisplayWaiting           5

#define TransitionDuration              0.1f

#define ZoomDistanceRate				0.002f
#define ZoomWheelRate					0.05f
#define ZoomWheelWaiting				0.3f

#define NotificationInterval			300

static constexpr uint SpawnAnimationKey =
    AnimationKeyLayerBottom |
    AnimationKeyLayerMiddle |
    AnimationKeyLayerTop |
    AnimationKeyPosition |
    AnimationKeyScaleRotation |
    AnimationKeyPositionBody |
    AnimationKeyPositionHead |
    (AnimationKeyDirection << 11);

GameLayer* GameLayer::__layer = NULL;

static const Spawn* getSpawn(int slot);

GameLayer::GameLayer() {
    CSAssert(!__layer, "invalid state");
    __layer = this;
    
    setKey(GameLayerIndex(GameLayerKeyScreen, 0));
    setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
    
    setTouchMulti(true);
    setTouchInherit(true);

    _spawnLayers = new CSArray<CSLayer>(4);

    _controlLayer = new CSLayer();
    _controlLayer->setKey(GameLayerIndex(GameLayerKeyControl, 0));
    _controlLayer->setFrame(CSRect(0, ProjectionHeight - 170, ProjectionWidth, 170));
    _controlLayer->setTransition(CSLayerTransitionUp | CSLayerTransitionFade);
    //_controlLayer->setTransitionDuration(TransitionDuration);
    _controlLayer->OnDraw.add(this, &GameLayer::onDrawControl);

    _statusLayer = new CSLayer();
    _statusLayer->setKey(GameLayerIndex(GameLayerKeyStatus, 0));
    _statusLayer->setFrame(CSRect(0, 0, ProjectionWidth, 140));
    _statusLayer->setTransition(CSLayerTransitionDown | CSLayerTransitionFade);
    //_statusLayer->setTransitionDuration(TransitionDuration);
    _statusLayer->OnDraw.add(this, &GameLayer::onDrawStatus);

    _offscreenLayer = new CSLayer();
    _offscreenLayer->setFrame(CSRect((ProjectionWidth - 420) / 2, (ProjectionHeight - 100) / 2, 420, 100));
    _offscreenLayer->OnDraw.add(this, &GameLayer::onDrawOffscreen);
    CSButton* offscreenCancelButton = CSButton::button();
    offscreenCancelButton->setFrame(CSRect(350, 0, 70, 40));
    offscreenCancelButton->OnDraw.add(this, &GameLayer::onDrawOffscreenCancel);
    offscreenCancelButton->OnTouchesEnded.add(this, &GameLayer::onTouchesEndedOffscreenCancel);
    _offscreenLayer->addLayer(offscreenCancelButton);
    
    _ticker = new CSTicker();
    _ticker->setFont(Asset::sharedAsset()->boldSmallFont);
    _ticker->setFrame(CSRect(300, 140, ProjectionWidth - 600, 40));
    _ticker->setTransition(CSLayerTransitionFade);
    _ticker->setTransitionDuration(TransitionDuration);
    _ticker->OnDraw.add(this, &GameLayer::onDrawTicker);
    
    _touchCoverLayer = new CSLayer();
    _touchCoverLayer->setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
    _touchCoverLayer->OnTouchesBegan.add([](CSLayer* layer) {
        Map::sharedMap()->passAction(ActionTouch, NULL, NULL, NULL);
        layer->removeFromParent(false);
    });
	_touchCoverLayer->OnBackKey.add([this](CSLayer* layer) {
		onBackKey();
	});
}

GameLayer::~GameLayer() {
    dispose();
    
    for (int i = 0; i < 5; i++) release(_spawnAnimations[i]);
    
    _controlLayer->release();
    _spawnLayers->release();

    _offscreenLayer->release();
    _ticker->release();
    _touchCoverLayer->release();
}

void GameLayer::dispose() {
    if (_state.code != GameStateDisposed) {
        releaseState();
        
        Map::destroy();
        
        clearLayers(false);
        
        _state.code = GameStateDisposed;
    }
    if (__layer == this) {
        __layer = NULL;
    }
}

GameLayer* GameLayer::layerForOfflinePlay(GameOfflineMode mode, const MapIndex &index, const Player *player, const Player *otherPlayer, int ai) {
    retain(player);        //parameters would be deleted (autofree / autorelease) while game disposing
    retain(otherPlayer);
    if (__layer) {
        __layer->dispose();
        __layer = NULL;
    }
    bool load = Map::loadForOfflinePlay(mode, index, player, otherPlayer, ai);
    release(player);
    release(otherPlayer);
    if (!load) return NULL;
    return autorelease(new GameLayer());
}

GameLayer* GameLayer::layerForOnlinePlay(GameOnlineMode mode, bool resume) {
    if (__layer) {
        __layer->dispose();
        __layer = NULL;
    }
    if (!resume) GameRecord::recordMatch();
    if (!Map::loadForOnlinePlay(mode, resume)) return NULL;
    return autorelease(new GameLayer());
}

GameLayer* GameLayer::layerForReplay(GameReplayMode mode, const char* subpath) {
    char* copysubpath = strdup(subpath);        //parameters would be deleted (autofree / autorelease) while game disposing
    if (__layer) {
        __layer->dispose();
        __layer = NULL;
    }
    bool load = Map::loadForReplay(mode, copysubpath);
    free(copysubpath);
    if (!load) return NULL;
    return autorelease(new GameLayer());
}

void GameLayer::onLink() {
    PVPGateway::sharedGateway()->addDelegate(this);

    /*
	if (Map::sharedMap()->onlineObserverDelayRemaining()) {
		view()->addLayer(GameObserverDelayLayer::layer());
	}
	else if (Account::sharedAccount()->party()) {
        if (Map::sharedMap()->modeType() != GameModeTypeOnline) {
            PVPGateway::sharedGateway()->requestPartyExit();
        }
		switch (Map::sharedMap()->mode()) {
			case GameModeTVReplay:
			case GameModeMatchRecord:
				view()->addLayer(MatchWaitLayer::layer(NULL));
				break;
			default:
				if (Map::sharedMap()->modeType() != GameModeTypeOnline) {
					PVPGateway::sharedGateway()->requestPartyExit();
				}
				break;
		}
	}
    */
    //===========================================================================
    if (Account::sharedAccount()->party()) {
        if (Map::sharedMap()->modeType() != GameModeTypeOnline) {
            PVPGateway::sharedGateway()->requestPartyExit();
        }
    }
    //===========================================================================
     
    //CSSecret::sharedSecret()->OnError.add(this, &GameLayer::onMemorySecretError);

	Map::sharedMap()->start(this);
}

void GameLayer::onUnlink() {
    PVPGateway::sharedGateway()->removeDelegate(this);

    //CSSecret::sharedSecret()->OnError.remove(this);
}

void GameLayer::onStateChanged() {
    if (state() == CSLayerStateFocus) _ticker->resume();
    else _ticker->pause();
}

void GameLayer::onBackKey() {
    showExit();
}

void GameLayer::releaseTargetCommandState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.targetCommand.destinations->release();
    foreach(const Unit*, unit, _state.context.targetCommand.units) {
        unit->setDistanceDisplay(0.0f);
    }
    foreach(Effect*, effect, _state.context.targetCommand.effects) {
        effect->dispose();
    }
    _state.context.targetCommand.effects->release();
    release(_state.context.targetCommand.units);
    release(_state.context.targetCommand.target);
}
void GameLayer::releaseRangeCommandState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.rangeCommand.destinations->release();
    foreach(const Unit*, unit, _state.context.rangeCommand.units) {
        unit->setDistanceDisplay(0.0f);
    }
    foreach(Effect*, effect, _state.context.rangeCommand.effects) {
        effect->dispose();
    }
    _state.context.rangeCommand.effects->release();
    release(_state.context.rangeCommand.units);
}
void GameLayer::releaseAngleCommandState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.angleCommand.destination->release();
    foreach(Effect*, effect, _state.context.angleCommand.effects) {
        effect->dispose();
    }
    _state.context.angleCommand.effects->release();
    release(_state.context.angleCommand.units);
}
void GameLayer::releaseLineCommandState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.lineCommand.destination->release();
    foreach(Effect*, effect, _state.context.lineCommand.effects) {
        effect->dispose();
    }
    _state.context.lineCommand.effects->release();
    release(_state.context.lineCommand.units);
}
void GameLayer::releaseTrainCommandState() {
	if (_state.context.trainCommand.confirmLayer) {
		_state.context.trainCommand.confirmLayer->removeFromParent(true);
		_state.context.trainCommand.confirmLayer->release();
	}
}
void GameLayer::releaseBuildTargetCommandState() {
    Map::sharedMap()->hideBuildingLocations();
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.buildTargetCommand.frame->release();
    _state.context.buildTargetCommand.effect->dispose();
    _state.context.buildTargetCommand.effect->release();
}
void GameLayer::releaseTargetSpawnState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.targetSpawn.destination->release();
    foreach(Effect*, effect, _state.context.targetSpawn.effects) {
        effect->dispose();
    }
    _state.context.targetSpawn.effects->release();
    release(_state.context.targetSpawn.target);
}
void GameLayer::releaseRangeSpawnState() {
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.rangeSpawn.destination->release();
    foreach(Effect*, effect, _state.context.rangeSpawn.effects) {
        effect->dispose();
    }
    _state.context.rangeSpawn.effects->release();
}
void GameLayer::releaseBuildSpawnState() {
    Map::sharedMap()->hideBuildingLocations();
    Map::sharedMap()->camera()->stopScroll(true);
    _mapScroll = CSVector2::Zero;
    Map::sharedMap()->setLightState(MapLightStateOn);
    _state.context.buildSpawn.frame->release();
    _state.context.buildSpawn.effect->dispose();
    _state.context.buildSpawn.effect->release();
}
void GameLayer::releaseMenuState() {
	if (Map::sharedMap()->modeType() != GameModeTypeOnline) {
		Map::sharedMap()->pause(false);
	}
}
void GameLayer::releaseSpeechState() {
    _state.context.speech.speech->release();
}
void GameLayer::releaseState() {
    switch (_state.code) {
        case GameStateScroll:
            Map::sharedMap()->camera()->stopScroll(false);
            break;
        case GameStateZoom:
            Map::sharedMap()->camera()->stopZoom();
            break;
        case GameStateTargetCommand:
            releaseTargetCommandState();
            break;
        case GameStateRangeCommand:
            releaseRangeCommandState();
            break;
        case GameStateAngleCommand:
            releaseAngleCommandState();
            break;
        case GameStateLineCommand:
            releaseLineCommandState();
            break;
		case GameStateTrainCommand:
			releaseTrainCommandState();
			break;
        case GameStateBuildTargetCommand:
            releaseBuildTargetCommandState();
            break;
        case GameStateTargetSpawn:
            releaseTargetSpawnState();
            break;
        case GameStateRangeSpawn:
            releaseRangeSpawnState();
            break;
        case GameStateBuildSpawn:
            releaseBuildSpawnState();
            break;
        case GameStateMenu:
            releaseMenuState();
            break;
        case GameStateSpeech:
            releaseSpeechState();
            break;
    }
}

static void validateEffects(CSArray<Effect>* effects, 
	const CSArray<Unit>* oldSources,
	const CSArray<Unit>* newSources,
	const CSArray<Object>* targets,
	const Object* destination,
	float distance,
	float range,
	int mask, 
	bool validDestination,
	const AnimationIndex& destAnimation, 
	const AnimationIndex& sourceAnimation,
	int tag)
{
	AnimationEffect* destEffect = NULL;

	int i = 0;
	while (i < effects->count()) {
		Effect* effect = effects->objectAtIndex(i);

		if (effect->tag() == tag) {
			switch (effect->type()) {
				case EffectTypeLight:
				case EffectTypeStroke:
					{
						bool isValid = false;
						foreach(const Unit*, target, targets) {
							if (target == effect->destination()) {
								isValid = true;
								break;
							}
						}
						if (!isValid) {
							effect->dispose();
							effects->removeObjectAtIndex(i);
							continue;
						}
					}
					break;
				default:
					{
						const Object* source = effect->source();

						if (source) {
							if (!newSources || !source->compareType(ObjectMaskUnit) || !newSources->containsObjectIdenticalTo(static_cast<const Unit*>(source))) {
								effect->dispose();
								effects->removeObjectAtIndex(i);
								continue;
							}
							if (range) {
								Object* ld = const_cast<Object*>(effect->destination());
								if (destination->isLocated()) {
									FPoint p = source->point() + FPoint::normalize(destination->point() - source->point()) * (source->collider() + range);

									ld->locate(p);
								}
								else {
									ld->unlocate();
								}
							}
							else {
								effect->setDestination(destination);
							}
						}
						else {
							if (effect->type() == EffectTypeAnimation) {
								destEffect = static_cast<AnimationEffect*>(effect);
							}
							effect->setDestination(destination);
						}
					}
					break;
			}
		}
		i++;
	}
	if (destEffect) {
		destEffect->setColor(validDestination ? CSColor::White : CSColor::Red);
	}
	else if (destAnimation) {
		effects->addObject(AnimationEffect::effect(destAnimation, destination, NULL, false, true, true, validDestination ? CSColor::White : CSColor::Red, tag));
	}
	if (mask & (CommandEffectBlink | CommandEffectStroke)) {
		foreach(const Object*, target, targets) {
			bool isNew = true;
			foreach(const Effect*, effect, effects) {
				switch (effect->type()) {
					case EffectTypeLight:
					case EffectTypeStroke:
						break;
					default:
						continue;
				}
				if (target == effect->destination()) {
					isNew = false;
					break;
				}
			}
			if (isNew) {
				if (mask & CommandEffectBlink) {
					effects->addObject(LightEffect::effect(target, EffectDisplayBlink, tag));
				}
				if (mask & CommandEffectStroke) {
					effects->addObject(StrokeEffect::effect(target, EffectDisplayInfinite, tag));
				}
			}
		}
	}
	if (distance) {
		foreach(const Unit*, unit, oldSources) {
			if (!newSources || !newSources->containsObjectIdenticalTo(unit)) {
				unit->setDistanceDisplay(0.0f);
			}
		}
	}
	foreach(const Unit*, unit, newSources) {
		if (!oldSources || !oldSources->containsObject(unit)) {
			const Object* dest;
			if (range) {
				Object* ld = Destination::destination(Map::sharedMap()->force());
				if (destination->isLocated()) {
					FPoint p = unit->point() + FPoint::normalize(destination->point() - unit->point()) * (unit->collider() + range);
					ld->locate(p);
				}
				dest = ld;
			}
			else {
				dest = destination;
			}

			if (mask & (CommandEffectLine_0 | CommandEffectLine_1 | CommandEffectLine_2 | CommandEffectLine_3)) {
				CSColor color;
				if (mask & CommandEffectLine_0) {
					color = Asset::playerColor0;
				}
				else if (mask & CommandEffectLine_1) {
					color = Asset::allianceColor;
				}
				else if (mask & CommandEffectLine_2) {
					color = Asset::enemyColor;
				}
				else {
					color = Asset::neutralColor;
				}
				effects->addObject(LineEffect::effect(unit, dest, ImageIndex(ImageSetGame, ImageGameLine), color, EffectDisplayInfinite, tag));
			}
			if (sourceAnimation) {
				effects->addObject(AnimationEffect::effect(sourceAnimation, unit, dest, false, true, true, CSColor::White, tag));
			}
			if (distance) {
				unit->setDistanceDisplay(distance);
			}
		}
	}
}

static void validateCommandEffects(CSArray<Effect>* effects,
	const CSArray<Unit>*& units,
	const CommandIndex& index,
	const CSArray<Object>* targets,
	const Object* destination,
	float distance,
	float line,
	int mask,
	const AnimationIndex& destAnimation,
	const AnimationIndex& sourceAnimation,
	int tag)
{
	const CSArray<Unit>* newUnits = Map::sharedMap()->commandUnits(index, destination->isLocated() ? destination : NULL);

	validateEffects(effects, units, newUnits, targets, destination, distance, line, mask, true, destAnimation, sourceAnimation, tag);

	CSObject::retain(units, newUnits);
}

static void validateSpawnEffects(CSArray<Effect>* effects,
	const CSArray<Object>* targets,
	const Object* destination,
	int mask,
	bool validDestination,
	const AnimationIndex& destAnimation)
{
	validateEffects(effects, NULL, NULL, targets, destination, 0, 0, mask, validDestination, destAnimation, AnimationIndex::None, 0);
}

bool GameLayer::updatePlaySpeech() {
    const Speech* speech = Map::sharedMap()->speech();
    
    if (speech) {
        if (_state.code == GameStateSpeech) {
            if (retain(_state.context.speech.speech, speech)) {
                _state.context.speech.seq = 0;
            }
            else if (_state.context.speech.seq < speech->msg->length()) {
                _state.context.speech.seq = CSGraphics::stringIndex(speech->msg, _state.context.speech.seq + 1);
            }
        }
        else {
            releaseState();
            //releaseControl();
            
            _state.context.speech.speech = retain(speech);
            _state.context.speech.seq = 0;
            _state.code = GameStateSpeech;
        }
        return true;
    }
    return false;
}

void GameLayer::updatePlay() {
    if (_state.code < GameStateEnd) {
        if (Map::sharedMap()->offscreenProgress()) {
            releaseState();
            //releaseControl();
			
            _state.code = GameStateOffscreen;
        }
        else {
            switch (Map::sharedMap()->state()) {
                case MapStateNone:
                    if (!updatePlaySpeech()) {
                        switch (_state.code) {
                            case GameStateSpeech:
                            case GameStateAuto:
                            case GameStateOffscreen:
                                releaseState();
                                //releaseControl();
                                _state.code = GameStateNone;
                                break;
                        }
                    }
                    break;
                case MapStateAuto:
                    if (!updatePlaySpeech()) {
                        if (_state.code != GameStateAuto) {
                            releaseState();
                            //releaseControl();
                            _state.code = GameStateAuto;
                        }
                    }
                    break;
                case MapStateDefeat:
                    if (Map::sharedMap()->isPlaying() && Map::sharedMap()->modeType() == GameModeTypeOnline) {        //리플레이 승패표시를 위해서 종료되는 시점까지 체크해야 함
                        const Match* match = Account::sharedAccount()->match();
                        if (match && (match->state == MatchStatePlay || match->state == MatchStatePause)) {
                            const Force* force = Map::sharedMap()->force();
                            if (force->result == GameResultNone && force->isAlive()) {
                                return;
                            }
                        }
                    }
                case MapStateVictory:
                case MapStateEnd:
				case MapStateInterrupt:
                    releaseState();
                    //releaseControl();
					//if (_observerPredictLayer) _observerPredictLayer->removeFromParent(true);
                    _state.code = GameStateEnd;
                    
                    if (Map::sharedMap()->modeType() == GameModeTypeOnline) {
                        GameRecord::expireMatch();
                    
                        const Match* match = Account::sharedAccount()->match();
                        /*
                        int64 matchRecordId = Map::sharedMap()->onlineMatchRecordId();
                        if (matchRecordId) {
                            const char* subpath = Asset::replaySubPath(matchRecordId);
                            
                            Map::sharedMap()->saveReplay(subpath);
                        }
                        */
                        if (match) {
                            GameResult result = Map::sharedMap()->result();
                            
                            if (result != GameResultNone) {
                                PVPGateway::sharedGateway()->requestMatchResult(result);
                            }
							PVPGateway::sharedGateway()->requestMatchExit();
                        }
                    }

					//WaitingLayer::show(this);
					//LobbyGateway::sharedGateway()->requestGameResult();
                    view()->addLayer(GameResultLayer::layer(Map::sharedMap()->result()));
                    break;
            }
        }
    }
}

bool GameLayer::checkTargetScroll(const CSVector2 &point) {
    _mapScroll = CSVector2::Zero;

    if (Map::sharedMap()->controlState(MapControlScroll) == MapControlStateEnabled) {
        bool horizontal = Map::sharedMap()->controlState(MapControlHorizontalScroll) == MapControlStateEnabled;
        bool vertical = Map::sharedMap()->controlState(MapControlVerticalScroll) == MapControlStateEnabled;
        
        if (horizontal) {
            if (point.x < TargetScrollBoundaryLeft) {
                _mapScroll.x = -TargetScrollDegree;
                
                if (vertical) {
                    float r = CSMath::clamp((point.y - TargetScrollBoundaryTop) / (ProjectionHeight - TargetScrollBoundaryTop - TargetScrollBoundaryBottom), 0.0f, 1.0f);
                    _mapScroll.y = CSMath::lerp(-TargetScrollDegree, TargetScrollDegree, r);
                }
                return true;
            }
            if (point.x > ProjectionWidth - TargetScrollBoundaryRight) {
                _mapScroll.x = TargetScrollDegree;
                
                if (vertical) {
                    float r = CSMath::clamp((point.y - TargetScrollBoundaryTop) / (ProjectionHeight - TargetScrollBoundaryTop - TargetScrollBoundaryBottom), 0.0f, 1.0f);
                    _mapScroll.y = CSMath::lerp(-TargetScrollDegree, TargetScrollDegree, r);
                }
                return true;
            }
        }
        if (vertical) {
            if (point.y < TargetScrollBoundaryTop) {
                _mapScroll.y = -TargetScrollDegree;
                
                if (horizontal) {
                    float r = CSMath::clamp((point.x - TargetScrollBoundaryLeft) / (ProjectionWidth - TargetScrollBoundaryLeft - TargetScrollBoundaryRight), 0.0f, 1.0f);
                    _mapScroll.x = CSMath::lerp(-TargetScrollDegree, TargetScrollDegree, r);
                }
                return true;
            }
            if (point.y > ProjectionHeight - TargetScrollBoundaryBottom) {
                _mapScroll.y = TargetScrollDegree;
                
                if (horizontal) {
                    float r = CSMath::clamp((point.x - TargetScrollBoundaryLeft) / (ProjectionWidth - TargetScrollBoundaryLeft - TargetScrollBoundaryRight), 0.0f, 1.0f);
                    _mapScroll.x = CSMath::lerp(-TargetScrollDegree, TargetScrollDegree, r);
                }
                return true;
            }
        }
    }
    return false;
}

void GameLayer::updateState() {
    if (_mapScroll != CSVector2::Zero) {
        Map::sharedMap()->camera()->moveScroll(_mapScroll);
    }
	if (_wheelWaiting && --_wheelWaiting == 0) {
		Map::sharedMap()->camera()->stopZoom();
	}
    
    //updateControl();

    //if (_autoLayer && _autoLayer->parent() && Map::sharedMap()->isAuto()) _autoAnimation->update(Map::sharedMap()->frameDelayFloat());

    switch (_state.code) {
        case GameStateCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.command.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateEnabled || rtn.parameter.enabled.all != _state.context.command.all) {
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateTargetCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.targetCommand.index, NULL, CommandCommitNone);
            
                Destination* dest = _state.context.targetCommand.destinations->lastObject();
                
                if (rtn.state != CommandStateTarget || _state.context.targetCommand.rtn != rtn.parameter.target) {
                    dest->unlocate();

                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.targetCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseTargetCommandState();
                    _state.code = GameStateNone;
                }
                else {
                    const Object* target = NULL;

                    int effectTag = _state.context.targetCommand.destinations->count();

                    if (dest->isLocated()) {
                        target = Map::sharedMap()->target(_state.context.targetCommand.point, _state.context.targetCommand.rtn.condition ? TargetStickRange : TargetRange, _state.context.targetCommand.rtn.condition);
                    }

                    CSArray<Object>* targets = NULL;
                    if (target) {
                        targets = CSArray<Object>::arrayWithCapacity(1);
                        targets->addObject(const_cast<Object*>(target));
                    }
                    validateCommandEffects(
                        _state.context.targetCommand.effects,
                        _state.context.targetCommand.units,
                        _state.context.targetCommand.index,
                        targets,
                        target ? target : dest,
                        _state.context.targetCommand.rtn.distance,
                        fixed::Zero,
                        _state.context.targetCommand.rtn.effect,
                        _state.context.targetCommand.rtn.destAnimation,
                        _state.context.targetCommand.rtn.sourceAnimation,
                        effectTag);

                    retain(_state.context.targetCommand.target, target);
                }
            }
            break;
        case GameStateRangeCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.rangeCommand.index, NULL, CommandCommitNone);
                
                Destination* dest = _state.context.rangeCommand.destinations->lastObject();
                
                if (rtn.state != CommandStateRange || _state.context.rangeCommand.rtn != rtn.parameter.range) {
                    dest->unlocate();
                    
                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.rangeCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseRangeCommandState();
                    _state.code = GameStateNone;
                }
                else {
                    int effectTag = _state.context.rangeCommand.destinations->count();

                    const CSArray<Object>* targets = dest->isLocated() ?
                        Map::sharedMap()->targetsWithRange(_state.context.rangeCommand.point, (fixed)_state.context.rangeCommand.rtn.range, _state.context.rangeCommand.rtn.condition) :
                        NULL;

                    validateCommandEffects(
                        _state.context.rangeCommand.effects,
                        _state.context.rangeCommand.units,
                        _state.context.rangeCommand.index,
                        targets,
                        dest,
                        _state.context.rangeCommand.rtn.distance,
                        fixed::Zero,
                        _state.context.rangeCommand.rtn.effect,
                        _state.context.rangeCommand.rtn.destAnimation,
                        _state.context.rangeCommand.rtn.sourceAnimation,
                        effectTag);
                }
            }
            break;
        case GameStateAngleCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.angleCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateAngle || _state.context.angleCommand.rtn != rtn.parameter.angle) {
                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.angleCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseAngleCommandState();
                    _state.code = GameStateNone;
                }
                else {
					const CSArray<Object>* targets = _state.context.angleCommand.destination->isLocated() ?
						Map::sharedMap()->targetsWithAngle(_state.context.angleCommand.units, _state.context.angleCommand.point, (fixed)_state.context.angleCommand.rtn.range, (fixed)_state.context.angleCommand.rtn.angle, _state.context.angleCommand.rtn.condition) :
						NULL;

					validateCommandEffects(
						_state.context.angleCommand.effects,
						_state.context.angleCommand.units,
						_state.context.angleCommand.index,
						targets,
						_state.context.angleCommand.destination,
						0,
						_state.context.angleCommand.rtn.range,
						_state.context.angleCommand.rtn.effect,
						_state.context.angleCommand.rtn.destAnimation,
						_state.context.angleCommand.rtn.sourceAnimation,
						0);
                }
            }
            break;
        case GameStateLineCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.lineCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateLine || _state.context.lineCommand.rtn != rtn.parameter.line) {
                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.lineCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseLineCommandState();
                    _state.code = GameStateNone;
                }
                else {
					const CSArray<Object>* targets = _state.context.lineCommand.destination->isLocated() ?
						Map::sharedMap()->targetsWithLine(_state.context.lineCommand.units, _state.context.lineCommand.point, (fixed)_state.context.lineCommand.rtn.range, (fixed)_state.context.lineCommand.rtn.thickness, _state.context.lineCommand.rtn.condition) :
						NULL;

					validateCommandEffects(
						_state.context.lineCommand.effects,
						_state.context.lineCommand.units,
						_state.context.lineCommand.index,
						targets,
						_state.context.lineCommand.destination,
						0,
						_state.context.lineCommand.rtn.range,
						_state.context.lineCommand.rtn.effect,
						_state.context.lineCommand.rtn.destAnimation,
						_state.context.lineCommand.rtn.sourceAnimation,
						0);
                }
            }
            break;
        case GameStateProduceCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.produceCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateProduce || rtn.parameter.produce.index != _state.context.produceCommand.unit) {
                    if (rtn.state == CommandStateProduceUnabled) {
                        showCommandUnabled(_state.context.produceCommand.index, rtn.parameter.produce.msg, rtn.parameter.produce.announce);
                    }
                    _state.code = GameStateNone;
                }
                else if (_state.context.produceCommand.seq) {
                    _state.context.produceCommand.seq--;
                }
                else if (doCommand(_state.context.produceCommand.index)) {
                    _state.context.produceCommand.seq = ProduceCounterDuration * Map::sharedMap()->framePerSecond();
                }
                else {
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateCancelProducingCommand:
            if (_state.context.cancelProducingCommand.seq) {
                _state.context.cancelProducingCommand.seq--;
            }
            else {
                Map::sharedMap()->cancelProducingCommand(_state.context.cancelProducingCommand.unit);
                _state.context.cancelProducingCommand.seq = ProduceCounterDuration * Map::sharedMap()->framePerSecond();
            }
            break;
        case GameStateTrainCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.trainCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateTrain || rtn.parameter.train.index != _state.context.trainCommand.rune) {
                    if (rtn.state == CommandStateTrainUnabled) {
                        showCommandUnabled(_state.context.trainCommand.index, rtn.parameter.train.msg, rtn.parameter.train.announce);
                    }
					releaseTrainCommandState();
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateBuildCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.buildCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateBuild || rtn.parameter.build.index != _state.context.buildCommand.unit || rtn.parameter.build.target) {
                    if (rtn.state == CommandStateBuildUnabled) {
                        showCommandUnabled(_state.context.buildCommand.index, rtn.parameter.build.msg, rtn.parameter.build.announce);
                    }
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateBuildTargetCommand:
            {
                CommandReturn rtn = Map::sharedMap()->command(_state.context.buildTargetCommand.index, NULL, CommandCommitNone);
                
                if (rtn.state != CommandStateBuild || rtn.parameter.build.index != _state.context.buildTargetCommand.unit || rtn.parameter.build.target != _state.context.buildTargetCommand.target) {
                    if (rtn.state == CommandStateBuildUnabled) {
                        showCommandUnabled(_state.context.buildTargetCommand.index, rtn.parameter.build.msg, rtn.parameter.build.announce);
                    }
                    releaseBuildTargetCommandState();
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateTargetSpawn:
            {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.targetSpawn.slot, NULL, SpawnCommitNone);
                
                if (rtn.state != CommandStateTarget || _state.context.targetSpawn.rtn != rtn.parameter.target) {
                    if (rtn.state == CommandStateUnabled) {
                        showSpawnUnabled(getSpawn(_state.context.targetSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseTargetSpawnState();
                    _state.code = GameStateNone;
                }
                else {
                    const Object* target = NULL;
                    
                    if (_state.context.targetSpawn.destination->isLocated()) {
                        target = Map::sharedMap()->target(_state.context.targetSpawn.point, _state.context.targetSpawn.rtn.condition ? TargetStickRange : TargetRange, _state.context.targetSpawn.rtn.condition);
                    }

					CSArray<Object>* targets = NULL;
					if (target) {
						targets = CSArray<Object>::arrayWithCapacity(1);
						targets->addObject(const_cast<Object*>(target));
					}
					validateSpawnEffects(
						_state.context.targetSpawn.effects,
						targets,
						target ? target : _state.context.targetSpawn.destination,
						_state.context.targetSpawn.rtn.effect,
						true,
						_state.context.targetSpawn.rtn.destAnimation);

					retain(_state.context.targetSpawn.target, target);
                }
            }
            break;
        case GameStateRangeSpawn:
            {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.rangeSpawn.slot, NULL, SpawnCommitNone);
                
                if (rtn.state != CommandStateRange || _state.context.rangeSpawn.rtn != rtn.parameter.range) {
                    if (rtn.state == CommandStateUnabled) {
                        showSpawnUnabled(getSpawn(_state.context.rangeSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseRangeSpawnState();
                    _state.code = GameStateNone;
                }
                else {
					const CSArray<Object>* targets = _state.context.rangeSpawn.destination->isLocated() ?
						Map::sharedMap()->targetsWithRange(_state.context.rangeSpawn.point, (fixed)_state.context.rangeSpawn.rtn.range, _state.context.rangeSpawn.rtn.condition) :
						NULL;
						
					validateSpawnEffects(
						_state.context.rangeSpawn.effects,
						targets,
						_state.context.rangeSpawn.destination,
						_state.context.rangeSpawn.rtn.effect,
						_state.context.rangeSpawn.rtn.all || Map::sharedMap()->tile(IPoint(_state.context.rangeSpawn.destination->point())).isVisible(),
						_state.context.rangeSpawn.rtn.destAnimation);
                }
            }
            break;
        case GameStateBuildSpawn:
            {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.buildSpawn.slot, NULL, SpawnCommitNone);
                
                if (rtn.state != CommandStateBuild || rtn.parameter.build.index != _state.context.buildSpawn.frame->index || rtn.parameter.build.target != _state.context.buildSpawn.frame->target) {
                    if (rtn.state == CommandStateUnabled) {
                        showSpawnUnabled(getSpawn(_state.context.buildSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                    releaseBuildSpawnState();
                    _state.code = GameStateNone;
                }
            }
            break;
            
    }
}
/*
CSLayer* GameLayer::createCommandLayer(const CSRect& frame, const CommandIndex& index) {
    CSLayer* layer = CSLayer::layer();
	layer->setFrame(frame);
	layer->setOrder(1);
    layer->setTagAsInt(index);
	layer->setTransition(CSLayerTransitionFade);
	layer->setTransitionDuration(TransitionDuration);
	layer->setTouchCarry(false);
    layer->OnDraw.add(this, &GameLayer::onDrawCommand);
	layer->OnTouchesBegan.add(this, &GameLayer::onTouchesBeganCommand);
	layer->OnTouchesMoved.add(this, &GameLayer::onTouchesMovedCommand);
    layer->OnTouchesEnded.add(this, &GameLayer::onTouchesEndedCommand);
    layer->OnTouchesCancelled.add(this, &GameLayer::onTouchesCancelledCommand);
    layer->setTouchOut(true);

    layer->setTouchArea(CSLayerTouchAreaRange);
	return layer;
}
*/

CSLayer* GameLayer::getCommandLayer(const CommandIndex& index) {
    /*
    CSArray<CSLayer>* layers;
    
    switch (index.indices[0]) {
        case CommandSetMain:
			return _mainCommandLayer;
		case CommandSetSub:
            layers = _subCommandLayers;
            break;
        case CommandSetBuildProduce:
        case CommandSetAbility:
        case CommandSetTrain:
            layers = _groupCommandLayers;
            break;
        default:
            return NULL;
    }
    
    foreach(CSLayer*, layer, layers) {
        CommandIndex tag(layer->tagAsInt());
        
        if (tag == index) return layer;
    }
    */
    return NULL;
}

/*
void GameLayer::updateDisplayCommand(CSLayer* layer, const CommandReturn& rtn) {
    if (_playerUI) {
        uint subtag;
        
        switch (rtn.state) {
            case CommandStateProduceUnabled:
            case CommandStateProduce:
                if (Map::sharedMap()->producingCount(rtn.parameter.produce.index) && Map::sharedMap()->controlState(MapControlCancelProducingCommand) != MapControlStateHidden) {
                    const UnitIndex& index = rtn.parameter.produce.index;
                    subtag = ((uint)index << 2) | 0;        //TYPE:0~3
                }
                else {
                    layer->clearLayers(true);
                    return;
                }
                break;
            default:
                layer->clearLayers(true);
                return;
        }
        if (layer->layers()->count()) {
            CSLayer* sublayer = layer->layers()->lastObject();
            if (sublayer->tagAsInt() != subtag) {
                sublayer->setTagAsInt(subtag);
            }
            layer->addLayer(sublayer);
        }
        else {
			float width = 40 * layer->width() / 100;

            CSLayer* sublayer = CSLayer::layer();
            sublayer->setTagAsInt(subtag);
			sublayer->setFrame(CSRect(layer->width() - width, 0, width, width));
            sublayer->setTransition(CSLayerTransitionFade);
            sublayer->setTransitionDuration(TransitionDuration);
            sublayer->OnDraw.add(this, &GameLayer::onDrawCancelCommand);
            sublayer->OnTouchesBegan.add(this, &GameLayer::onTouchesBeganCancelCommand);
            sublayer->OnTouchesEnded.add(this, &GameLayer::onTouchesEndedCancelCommand);
            sublayer->OnTouchesCancelled.add(this, &GameLayer::onTouchesCancelledCancelCommand);
            
            layer->addLayer(sublayer);
        }
    }
}
*/
CSLayer* GameLayer::createSpawnLayer(const CSRect& frame, int slot) {
    CSLayer* layer = CSLayer::layer();
	layer->setOrder(1);
    layer->setTagAsInt(slot);
    layer->setFrame(frame);
    layer->OnDraw.add(this, &GameLayer::onDrawSpawn);
	layer->OnTouchesBegan.add(this, &GameLayer::onTouchesBeganSpawn);
	layer->OnTouchesMoved.add(this, &GameLayer::onTouchesMovedSpawn);
    layer->OnTouchesEnded.add(this, &GameLayer::onTouchesEndedSpawn);
    layer->OnTouchesCancelled.add(this, &GameLayer::onTouchesCancelledSpawn);
    layer->setTouchOut(true);
    return layer;
}

CSLayer* GameLayer::getSpawnLayer(int slot) {
    foreach(CSLayer*, layer, _spawnLayers) {
        if (layer->tagAsInt() == slot) {
            return layer;
        }
    }
    return NULL;
}
/*
static float messageLayerHeight(const Message* msg, float width) {
    if (msg->icon) {
        width -= 60;
    }
    float height = CSGraphics::stringSize(msg->content, Asset::sharedAsset()->boldSmallFont, width).height;
    if (msg->icon && height < 54) {
        height = 54;
    }
    return height;
}

void GameLayer::updateDisplayMessages(const CSRect& frame) {
    if (!_observerStatTabOpened && Map::sharedMap()->controlState(MapControlMessage) == MapControlStateEnabled) {
        {
            int i = 0;
            while (i < _emoticonLayers->count()) {
                CSLayer* emoticonLayer = _emoticonLayers->objectAtIndex(i);
                const Force* force = Map::sharedMap()->force(emoticonLayer->tagAsInt());
                if (force->hasEmoticon()) {
                    i++;
                }
                else {
                    emoticonLayer->removeFromParent(false);
                    
                    _emoticonLayers->removeObjectAtIndex(i);
                }
            }
        }
        for (int i = 0; i < MaxPlayers; i++) {
            const Force* force = Map::sharedMap()->force(i);
            if (force && force->hasEmoticon()) {
                int index = -1;
                for (int j = 0; j < _emoticonLayers->count(); j++) {
                    if (_emoticonLayers->objectAtIndex(j)->tagAsInt() == force->index) {
                        index = j;
                        break;
                    }
                }
                CSLayer* emoticonLayer;
                if (index == -1) {
					if (_emoticonLayers->count() * 120 + 120 > frame.size.width) continue;

					emoticonLayer = CSLayer::layer();
					emoticonLayer->setEnabled(false);
					emoticonLayer->setTagAsInt(force->index);
					emoticonLayer->setFrame(CSRect(frame.left() + _emoticonLayers->count() * 120, frame.bottom() - 140, 120, 140));
					emoticonLayer->OnDraw.add(this, &GameLayer::onDrawEmoticonAnimation);
					_emoticonLayers->addObject(emoticonLayer);
                }
                else {
                    emoticonLayer = _emoticonLayers->objectAtIndex(index);
                    emoticonLayer->setFrame(CSRect(frame.left() + index * 120, frame.bottom() - 140, 120, 140), 0.1f);
                }
                addLayer(emoticonLayer);
            }
        }

        float bottom = frame.bottom();
        
        if (_emoticonLayers->count()) {
            bottom -= 140;
        }
        
        CSLayer* topLayer = _messageLayers->lastObject();

        if (!topLayer || !topLayer->tag<const Message>()->essential || topLayer->nextFrame().top() > frame.top()) {
			const Message* msg;
            while ((msg = Map::sharedMap()->popMessage())) {
                bool isNew = true;
                if (!msg->essential) {
                    foreach(const CSLayer*, otherLayer, _messageLayers) {
                        const Message* other = otherLayer->tag<const Message>();
                        if (msg->isEqualToMessage(other)) {
                            isNew = false;
                            break;
                        }
                    }
                }
                if (isNew) {
                    CSLayer* messageLayer = new CSLayer();
                    
                    messageLayer->setKey(GameLayerIndex(GameLayerKeyMessage, 0));
					messageLayer->setEnabled(false);
                    messageLayer->setTag(msg);
                    float height = messageLayerHeight(msg, frame.size.width);
                    CSRect messageFrame(frame.origin.x, bottom - height, frame.size.width, height);
                    messageLayer->setFrame(messageFrame);
                    messageLayer->setTransition(CSLayerTransitionFade);
                    messageLayer->setTransitionDuration(TransitionDuration);
                    messageLayer->setTimeoutInterval(MessageStateDuration);
                    messageLayer->OnDraw.add(this, &GameLayer::onDrawMessage);
                    messageLayer->OnTimeout.add(this, &GameLayer::onTimeoutMessage);
                    addLayer(messageLayer);
                    _messageLayers->insertObject(0, messageLayer);
                    messageLayer->release();
                    break;
                }
            }
        }
        
		float y = bottom;

        for (int i = 0; i < _messageLayers->count(); i++) {
            CSLayer* messageLayer = _messageLayers->objectAtIndex(i);
			const Message* msg = messageLayer->tag<const Message>();
            float height = messageLayerHeight(msg, frame.size.width);
            
            if (!msg->essential && y - height < frame.top()) {
                messageLayer->removeFromParent(true);
                _messageLayers->removeObjectAtIndex(i);
                i--;
            }
            else {
                CSRect messageFrame(frame.origin.x, y - height, frame.size.width, height);
                if (messageFrame != messageLayer->nextFrame()) {
                    messageLayer->setFrame(messageFrame, 0.1f);
                }
                addLayer(messageLayer);
                y -= height;
            }
        }
    }
    else {
        foreach (CSLayer*, emoticonLayer, _emoticonLayers) {
            emoticonLayer->removeFromParent(false);
        }
        _emoticonLayers->removeAllObjects();

        foreach (CSLayer*, messageLayer, _messageLayers) {
            messageLayer->removeFromParent(true);
        }
        _messageLayers->removeAllObjects();
    }
}

static CSRect missionLayerFrame(const Mission* mission) {
    const CSString* msg = mission->message();
    
    static const float screenWidth = ProjectionWidth - ProjectionGameEdgeSide - ProjectionGameEdgeSide;
    
    CSRect frame;
    frame.origin = mission->position;
    frame.origin.x += ProjectionGameEdgeSide;

	CSSize size = CSGraphics::stringSize(msg, mission->font, mission->size.width - 20);
	frame.size.width = mission->widthFixed ? mission->size.width : size.width + 20;
	frame.size.height = mission->heightFixed ? mission->size.height : CSMath::min(size.height + 20, mission->size.height);
    
    if (mission->align & CSAlignRight) {
        frame.origin.x += screenWidth - frame.size.width;
    }
    else if (mission->align & CSAlignCenter) {
        frame.origin.x += (screenWidth - frame.size.width) * 0.5f;
    }
    if (mission->align & CSAlignBottom) {
        frame.origin.y += ProjectionHeight - frame.size.height;
    }
    else if (mission->align & CSAlignMiddle) {
        frame.origin.y += (ProjectionHeight - frame.size.height) * 0.5f;
    }
    if (frame.origin.x < 10 + ProjectionGameEdgeSide) frame.origin.x = 10 + ProjectionGameEdgeSide;
    else if (frame.right() > ProjectionWidth - ProjectionGameEdgeSide - 10) frame.origin.x = ProjectionWidth - ProjectionGameEdgeSide - 10 - frame.size.width;
    if (frame.origin.y < 10) frame.origin.y = 10;
    else if (frame.bottom() > ProjectionHeight - 10) frame.origin.y = ProjectionHeight - 10 - frame.size.height;
    
    return frame;
}

void GameLayer::updateDisplayMissions() {
    const Mission* mission;
    int i = 0;
    while (i < _missionLayers->count()) {
        CSLayer* missionLayer = _missionLayers->objectAtIndex(i);
        
		mission = missionLayer->tag<const Mission>();
        
        mission = Map::sharedMap()->getMission(mission->key);
        
        if (mission) {
			missionLayer->setTag(mission);

            CSRect frame = missionLayerFrame(mission);
            
            if (frame != missionLayer->nextFrame()) {
                if (mission->replaced) missionLayer->setFrame(frame, TransitionDuration);
                else missionLayer->setFrame(frame);
            }
            addLayer(missionLayer);
            i++;
        }
        else {
            missionLayer->removeFromParent(true);
            _missionLayers->removeObjectAtIndex(i);
        }
    }
    while ((mission = Map::sharedMap()->popMission())) {
        CSLayer* missionLayer = new CSLayer();
        missionLayer->setKey(GameLayerIndex(GameLayerKeyMission, mission->key));
        missionLayer->setTag(mission);
		missionLayer->setEnabled(false);
        missionLayer->setTransition(CSLayerTransitionPop | CSLayerTransitionFade);
        missionLayer->setFrame(missionLayerFrame(mission));
        missionLayer->OnDraw.add(this, &GameLayer::onDrawMission);
        addLayer(missionLayer);
        _missionLayers->addObject(missionLayer);
        missionLayer->release();
    }
}

static CSRect gameLayoutFrame(int index, float size) {
	const GameLayoutComponent& c = Option::sharedOption()->gameLayout().components[index];
	size *= c.scale;
	return CSRect(c.position.x - size * 0.5f, c.position.y - size * 0.5f, size, size);
}

static CSColor gameLayoutColor(int index) {
	const GameLayoutComponent& c = Option::sharedOption()->gameLayout().components[index];
	
	return CSColor(1.0f, 1.0f, 1.0f, c.alpha);
}

static CSRect groupCommandFrame(bool playerUI, int index) {
	return playerUI ? gameLayoutFrame(GameLayoutGroupCommand_0 + index, 100) : CSRect(5 + 57 * index, 142, 57, 57);
}
*/
void GameLayer::updateDisplay() {
    if (Map::sharedMap()->action() == ActionTouch) {
        view()->addLayer(_touchCoverLayer);
    }
    else {
        _touchCoverLayer->removeFromParent(false);
    }
    
    bool clear = false;

    switch (_state.code) {
        case GameStateMenu:
        case GameStateAuto:
        case GameStateSpeech:
        case GameStateOffscreen:
        case GameStateEnd:
        case GameStateCheat:
        case GameStateDisposed:
            clear = true;
            break;
    }
    if (clear) {
        //_commandLayers->removeAllObjects();
        _spawnLayers->removeAllObjects();

        switch (_state.code) {
            case GameStateMenu:
                /*
                foreach(CSLayer*, layer, this->layers()) {
                    if (layer != _menuLayer &&
                        !_messageLayers->containsObjectIdenticalTo(layer) &&
                        !_emoticonLayers->containsObjectIdenticalTo(layer))
                    {
                        layer->removeFromParent(true);
                    }
                }

                updateDisplayMessages(CSRect(420, _menuLayer->frame().top() - 400, ProjectionWidth - 840, 320));
                
                addLayer(_menuLayer);
                */
                break;
            case GameStateSpeech:
                /*
                foreach(CSLayer*, layer, this->layers()) {
                    if (layer != _speechLayer) {
                        layer->removeFromParent(true);
                    }
                }
                addLayer(_speechLayer);
                */
                break;
            case GameStateOffscreen:
                foreach(CSLayer*, layer, this->layers()) {
                    if (layer != _offscreenLayer) {
                        layer->removeFromParent(true);
                    }
                }
                addLayer(_offscreenLayer);
                break;
            default:
    			clearLayers(true);
                break;
        }
        return;
    }
    
    //_speechLayer->removeFromParent(true);
    _offscreenLayer->removeFromParent(true);
    //_menuLayer->removeFromParent(true);
    
	const Force* force = Map::sharedMap()->force();

    addLayer(_controlLayer);
    addLayer(_statusLayer);

    /*
	if (!_minimapExpand && Map::sharedMap()->controlState(MapControlStatus) != MapControlStateHidden) {
		insertLayer(0, _statusLayer);
	}
	else {
		_statusLayer->removeFromParent(true);
	}
	if (force) {
		for (int i = 0; i < 2; i++) {
			int resource = (int)force->resource(i);
			int addition = (resource - _displayResources[i]) / 2;
			if (addition) {
				_displayResources[i] += addition;
			}
			else {
				_displayResources[i] = resource;
			}
		}
	}
    */
    /*
    {
        CSArray<CSLayer>* subCommandLayers = CSArray<CSLayer>::arrayWithArray(_subCommandLayers, false);
            
        int layerCount = 0;
        int layerFrameIndex = 0;
        for (int i = 0; i < Asset::sharedAsset()->commands->objectAtIndex(CommandSetSub)->count(); i++) {
            CommandIndex index(CommandSetSub, i);
            CommandReturn rtn = Map::sharedMap()->command(index, NULL, CommandCommitNone);
                
            if (rtn.state != CommandStateEmpty) {
                bool flag = true;
                    
                CSLayer* subCommandLayer = NULL;
                    
                for (int j = 0; j < subCommandLayers->count(); j++) {
                    subCommandLayer = subCommandLayers->objectAtIndex(j);
                        
                    CommandIndex otherIndex(subCommandLayer->tagAsInt());
                        
                    if (otherIndex == index) {
                        flag = false;
                        subCommandLayers->removeObjectAtIndex(j);
                        layerFrameIndex = layerCount + 1;
                        break;
                    }
                }
                if (flag) {
                    subCommandLayer = createCommandLayer(gameLayoutFrame(GameLayoutSubCommand_0 + layerFrameIndex, 100), index);
                    addLayer(subCommandLayer);
                        
                    _subCommandLayers->addObject(subCommandLayer);
                }
                subCommandLayer->setKey(GameLayerIndex(GameLayerKeyMainCommand, layerCount));
				subCommandLayer->setColor(gameLayoutColor(GameLayoutSubCommand_0 + layerCount));
                subCommandLayer->setFrame(gameLayoutFrame(GameLayoutSubCommand_0 + layerCount, 100), TransitionDuration);
                updateDisplayCommand(subCommandLayer, rtn);

                if (++layerCount == 4) break;
            }
        }
        foreach (CSLayer*, subCommandLayer, subCommandLayers) {
			subCommandLayer->removeFromParent(true);
                
            _subCommandLayers->removeObjectIdenticalTo(subCommandLayer);
        }
    }
    */
    
    if (force) {
        CSArray<CSLayer>* spawnLayers = CSArray<CSLayer>::arrayWithArray(_spawnLayers, false);
    
        for (int i = 0; i < force->spawnCount(); i++) {
            CommandReturn rtn = Map::sharedMap()->spawn(i, NULL, SpawnCommitNone);

            if (rtn.state != CommandStateEmpty) {
                bool flag = true;
                
                CSLayer* spawnLayer = NULL;
                
                for (int j = 0; j < spawnLayers->count(); j++) {
                    spawnLayer = spawnLayers->objectAtIndex(j);
                    
                    int prevTag = spawnLayer->tagAsInt();
                    
                    if (prevTag == i) {
                        flag = false;
                        spawnLayers->removeObjectAtIndex(j);
                        break;
                    }
                }

                //================================================================================
                const AnimationIndex& spawnAnimation = getSpawn(i)->skinData()->spawnAnimation;

                if (_spawnAnimations[i] && _spawnAnimations[i]->index() != spawnAnimation) {
                    _spawnAnimations[i]->release();
                    _spawnAnimations[i] = NULL;
                }
                if (_spawnAnimations[i]) {
                    _spawnAnimations[i]->update(Map::sharedMap()->frameDelayFloat());
                }
                else if (spawnAnimation != AnimationIndex::None) {
                    _spawnAnimations[i] = new Animation(spawnAnimation);
                }
                //================================================================================

                CSRect spawnFrame(ProjectionBaseX + 253 + i * 115, 11, 110, 94);

                if (flag) {
                    //spawnLayer = createSpawnLayer(gameLayoutFrame(GameLayoutSpawn_0 + layerFrameIndex, 100), i);
                    spawnLayer = createSpawnLayer(spawnFrame, i);

                    spawnLayer->setTransition(CSLayerTransitionFade);
                    spawnLayer->setTransitionDuration(TransitionDuration);
                    spawnLayer->setTouchCarry(false);
                    _controlLayer->addLayer(spawnLayer);
                    
                    _spawnLayers->addObject(spawnLayer);
                }
                spawnLayer->setKey(GameLayerIndex(GameLayerKeySpawn, i));

				//spawnLayer->setColor(gameLayoutColor(GameLayoutSpawn_0 + layerCount));
                //spawnLayer->setFrame(gameLayoutFrame(GameLayoutSpawn_0 + layerCount, 100), TransitionDuration);
                spawnLayer->setFrame(spawnFrame, TransitionDuration);
            }
        }
        foreach (CSLayer*, spawnLayer, spawnLayers) {
            spawnLayer->removeFromParent(true);
            
            _spawnLayers->removeObjectIdenticalTo(spawnLayer);
        }

        if (force->nextCard()) {
            const AnimationIndex& spawnAnimation = force->nextCard()->skinData()->spawnAnimation;
            if (_spawnAnimations[4] && _spawnAnimations[4]->index() != spawnAnimation) {
                _spawnAnimations[4]->release();
                _spawnAnimations[4] = NULL;
            }
            if (_spawnAnimations[4]) {
                _spawnAnimations[4]->update(Map::sharedMap()->frameDelayFloat());
            }
            else if (spawnAnimation != AnimationIndex::None) {
                _spawnAnimations[4] = new Animation(spawnAnimation);
            }
        }
        else {
            release(_spawnAnimations[4]);
        }
    }

    if (_ticker->isFinished()) {
        _ticker->clearText();
        
        _ticker->removeFromParent(true);
    }
    else {
        addLayer(_ticker);
    }
	
    //updateDisplayMissions();
    
    //updateDisplayMessages(CSRect(ProjectionGameEdgeSide + 350, ProjectionHeight - 102 - 10 - 260, ProjectionWidth - (ProjectionGameEdgeSide + 350) - (ProjectionGameEdgeSide + 420), 260));
}

void GameLayer::onTimeout() {
#ifdef UseTeamSpeak
    if (_teamSpeakImpl) {
        _teamSpeakImpl->update(timeoutInterval());
    }
#endif
    if (Map::sharedMap()->timeout(this)) {
        updatePlay();
        updateState();
        updateDisplay();
    }
}

void GameLayer::onDraw(CSGraphics* graphics) {
    Map::sharedMap()->drawScreen(graphics);
    
	if (Map::sharedMap()->modeType() == GameModeTypeOnline) {
		graphics->setFont(Asset::sharedAsset()->boldMediumFont);

        const Match* match = Account::sharedAccount()->match();
        if (match && match->state == MatchStateSync) {
            int waiting = CSTime::currentTimeSecond() - match->timestamp;
            if (waiting >= PVPSyncDisplayWaiting) {
                waiting = CSMath::max(PVPSyncWaiting - waiting, 0);

				graphics->setStrokeWidth(2);
                graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameOnlineSync), CSVector2(center(), middle() - 30), CSAlignCenterMiddle);
                graphics->drawString(CSString::cstringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameOnlineSyncWaiting), waiting), CSVector2(center(), middle() + 30), CSAlignCenterMiddle);
				graphics->setStrokeWidth(0);
            }
        }
		else if (!Map::sharedMap()->offscreenProgress() && Map::sharedMap()->isPaused()) {
			graphics->setStrokeWidth(2);
			graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameOnlinePause_2), centerMiddle(), CSAlignCenterMiddle);
			graphics->setStrokeWidth(0);
		}
    }
#if defined(UseMonitorStatusAlways) || defined(UseMonitorStatusMenu)
#ifdef UseMonitorStatusMenu
    if (_state.code == GameStateMenu)
#endif
    {
        Map::MonitorStatus status = Map::sharedMap()->monitorStatus();
        
        CSMemoryUsage memoryUsage = CSDevice::memoryUsage();
        
        CSRect rect((ProjectionWidth - 400) / 2, (ProjectionHeight - 250) / 2, 400, 250);
        
        graphics->setColor(CSColor::TranslucentBlack);
        graphics->drawRect(rect, true);
        
        const char* msg = CSString::cstringWithFormat("fps : %d / %d\nupdate latency : %.4f\ntimeout latency : %.4f\nmemory : %" PRId64 " / %" PRId64 " / %" PRId64,
                                                        Map::sharedMap()->localFramePerSecond(),
                                                        Map::sharedMap()->framePerSecond(),
                                                        status.updateLatency,
                                                        status.timeoutLatency,
                                                        memoryUsage.freeMemory / 1024,
                                                        memoryUsage.totalMemory / 1024,
                                                        memoryUsage.threshold / 1024);
        
        graphics->setColor(CSColor::White);
		graphics->setFont(Asset::sharedAsset()->extraSmallFont);
		graphics->drawString(msg, rect.adjustRect(-20));
    }
#endif
}

void GameLayer::onDrawCover(CSGraphics *graphics) {
	Map::sharedMap()->drawAnimations(graphics);
}

void GameLayer::onTouchesBegan() {
	/*
    if (_chatTextField && _chatTextField->isFocused()) {
        _chatTextField->setFocus(false);
        cancelTouches(false);
        return;
    }
    */

    Map::sharedMap()->skipWaiting();

    switch (_state.code) {
        case GameStateNone:
        case GameStateScroll:
            if (Map::sharedMap()->controlState(MapControlZoom) == MapControlStateEnabled && touches()->count() >= 2) {
                releaseState();
                //releaseDoubleClickControl();
                
                _state.code = GameStateZoom;
            }
            break;
        case GameStateTargetCommand:
            {
                CSLayer* commandLayer = getCommandLayer(_state.context.targetCommand.index);
                
                if (commandLayer) {
                    commandLayer->beginTouch(touch(), false);
                    onTouchesMovedCommand(commandLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateRangeCommand:
            {
                CSLayer* commandLayer = getCommandLayer(_state.context.rangeCommand.index);
                
                if (commandLayer) {
                    commandLayer->beginTouch(touch(), false);
                    onTouchesMovedCommand(commandLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateAngleCommand:
            {
                CSLayer* commandLayer = getCommandLayer(_state.context.angleCommand.index);
                
                if (commandLayer) {
                    commandLayer->beginTouch(touch(), false);
                    onTouchesMovedCommand(commandLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateLineCommand:
            {
                CSLayer* commandLayer = getCommandLayer(_state.context.lineCommand.index);
                
                if (commandLayer) {
                    commandLayer->beginTouch(touch(), false);
                    onTouchesMovedCommand(commandLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateBuildTargetCommand:
            {
                CSLayer* commandLayer = getCommandLayer(_state.context.buildTargetCommand.index);
                
                if (commandLayer) {
                    commandLayer->beginTouch(touch(), false);
                    onTouchesMovedCommand(commandLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateTargetSpawn:
            {
                CSLayer* spawnLayer = getSpawnLayer(_state.context.targetSpawn.slot);
                
                if (spawnLayer) {
                    spawnLayer->beginTouch(touch(), false);
                    onTouchesMovedSpawn(spawnLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateRangeSpawn:
            {
                CSLayer* spawnLayer = getSpawnLayer(_state.context.rangeSpawn.slot);
                
                if (spawnLayer) {
                    spawnLayer->beginTouch(touch(), false);
                    onTouchesMovedSpawn(spawnLayer);
                    cancelTouches(false);
                }
            }
            break;
        case GameStateBuildSpawn:
            {
                CSLayer* spawnLayer = getSpawnLayer(_state.context.buildSpawn.slot);
                
                if (spawnLayer) {
                    spawnLayer->beginTouch(touch(), false);
                    onTouchesMovedSpawn(spawnLayer);
                    cancelTouches(false);
                }
            }
            break;
    }
}
void GameLayer::onTouchesMoved() {
    Map::sharedMap()->followScroll(false);
    
    //releaseDoubleClickControl();
    
    switch (_state.code) {
        case GameStateNone:
            {
                CSVector2 p = touch()->point(this);
                if (Map::sharedMap()->controlState(MapControlScroll) == MapControlStateEnabled) {
                    _state.code = GameStateScroll;
                    
                    Map::sharedMap()->passAction(ActionScroll, NULL, NULL, NULL);
                }
                else {
                    break;
                }
            }
        case GameStateScroll:
            {
                const CSTouch* touch = this->touch();
                CSVector2 p = touch->prevPoint(this) - touch->point(this);
                if (Map::sharedMap()->controlState(MapControlHorizontalScroll) != MapControlStateEnabled) {
                    p.x = 0.0f;
                }
                if (Map::sharedMap()->controlState(MapControlVerticalScroll) != MapControlStateEnabled) {
                    p.y = 0.0f;
                }
                Map::sharedMap()->camera()->moveScroll(p);
            }
            break;
        case GameStateZoom:
            {
                const CSArray<const CSTouch>* touches = this->touches();
                
                if (touches->count() >= 2) {
                    const CSTouch* touch0 = touches->objectAtIndex(0);
                    const CSTouch* touch1 = touches->objectAtIndex(1);
                    float len0 = (touch0->prevPoint(this) - touch1->prevPoint(this)).length();
                    float len1 = (touch0->point(this) - touch1->point(this)).length();
                    
                    if (len0 != len1) {
                        Map::sharedMap()->camera()->moveZoom((len1 - len0) * ZoomDistanceRate);
                    }
                }
            }
            break;
    }
}
void GameLayer::onTouchesEnded() {
    switch (_state.code) {
        case GameStateNone:
            /*
            {
                const CSTouch* touch = this->touch();
                
				if (!touch->isMoved()) {
                    CSVector2 point = touch->point(this);

					bool doubleClick = commitDoubleClickControl(this, DoubleClickDuration) && _doubleClickScreenPoint.distance(point) <= DoubleClickScreenDistance;

					Map::ClickCommand command = _selectControl || _buildControl ? Map::ClickCommandNone :
						(Option::sharedOption()->oneTouchToUnitMove() ^ doubleClick ? Map::ClickCommandMove : Map::ClickCommandAttack);
                    
                    if (Map::sharedMap()->click(point, command, doubleClick || _selectControl, false)) {
                        _selection = GameSelectionNormal;
                    }
                    _doubleClickScreenPoint = point;
                }
            }
            */
            break;
        case GameStateScroll:
            Map::sharedMap()->camera()->stopScroll(false);
            _state.code = GameStateNone;
            break;
        case GameStateZoom:
            {
                Map::sharedMap()->camera()->stopZoom();
                
                bool remaining = false;
                foreach(const CSTouch*, touch, touches()) {
                    if (touch->state() != CSTouchStateEnded) {
                        remaining = true;
                        break;
                    }
                }
                if (!remaining) {
                    _state.code = GameStateNone;
                }
            }
            break;
        case GameStateMenu:
            releaseMenuState();
            _state.code = GameStateNone;
            break;
        case GameStateSpeech:
            if (_state.context.speech.speech->msg && _state.context.speech.seq < _state.context.speech.speech->msg->length()) {
                _state.context.speech.seq = _state.context.speech.speech->msg->length();
            }
            else {
                Map::sharedMap()->nextSpeech();
            }
            break;
    }
}
void GameLayer::onTouchesCancelled() {
    switch (_state.code) {
        case GameStateScroll:
            Map::sharedMap()->camera()->stopScroll(false);
            _state.code = GameStateNone;
            break;
        case GameStateZoom:
            Map::sharedMap()->camera()->stopZoom();
            _state.code = GameStateNone;
            break;
    }
}

void GameLayer::onWheel(float offset) {
	if (Map::sharedMap()->controlState(MapControlZoom) == MapControlStateEnabled) {
		Map::sharedMap()->camera()->moveZoom(offset * ZoomWheelRate);

		_wheelWaiting = ZoomWheelWaiting * Map::sharedMap()->framePerSecond();
	}
}

void GameLayer::onKeyDown(int keyCode) {
    /*
	switch (keyCode) {
		case CSKeyCodeA:
		case CSKeyCodeLeftShift:
			if (!_selectControl && _selectAttackersLayer && _selectAttackersLayer->state() == CSLayerStateFocus) {
				onTouchesBeganSelectAttackers(_selectAttackersLayer);
			}
			break;
		case CSKeyCodeS:
			if (_selectBarracksLayer && _selectBarracksLayer->state() == CSLayerStateFocus) {
				releaseState();
				//releaseControl();

				_state.code = GameStateNone;

				if (Map::sharedMap()->selectBarracks(true)) {
					_selection = GameSelectionBarracks;
				}
			}
			break;
		case CSKeyCodeD:
			if (_selectLaborsLayer && _selectLaborsLayer->state() == CSLayerStateFocus) {
				releaseState();
				//releaseControl();

				_state.code = GameStateNone;

				if (Map::sharedMap()->selectLabors(true)) {
					_selection = GameSelectionLabors;
				}
			}
			break;
		case CSKeyCodeQ:
			onKeyMainCommand();
			break;
		case CSKeyCodeW:
			onKeySubCommand(0);
			break;
		case CSKeyCodeE:
			onKeySubCommand(1);
			break;
		case CSKeyCodeR:
			onKeySubCommand(2);
			break;
		case CSKeyCodeT:
			onKeySubCommand(3);
			break;
		case CSKeyCodeF1:
		case CSKeyCodeF2:
		case CSKeyCodeF3:
		case CSKeyCodeF4:
		case CSKeyCodeF5:
		case CSKeyCodeF6:
			if (!_playerUI) {
				_observerStatTab = (ObserverStatTab)(keyCode - CSKeyCodeF1);
			}
			break;
		case CSKeyCode1:
			onKeyGroupCommand(0);
			break;
		case CSKeyCode2:
			onKeyGroupCommand(1);
			break;
		case CSKeyCode3:
			onKeyGroupCommand(2);
			break;
		case CSKeyCode4:
			onKeyGroupCommand(3);
			break;
		case CSKeyCode5:
			onKeyGroupCommand(4);
			break;
		case CSKeyCode6:
			onKeyGroupCommand(5);
			break;
		case CSKeyCode7:
			onKeyGroupCommand(6);
			break;
		case CSKeyCode8:
			onKeyGroupCommand(7);
			break;
		case CSKeyCode9:
			onKeySpawn(0);
			break;
		case CSKeyCode0:
			onKeySpawn(1);
			break;
		case CSKeyCodeF:
			_fullScreen = !_fullScreen;
			break;
		case CSKeyCodeP:
			{
				bool on = !Map::sharedMap()->isPaused();
				if (Map::sharedMap()->modeType() == GameModeTypeOnline) {
					PVPGateway::sharedGateway()->requestMatchPause(on);
				}
				else {
					Map::sharedMap()->pause(on);
				}
			}
			break;
	}
    */
}

void GameLayer::onKeyUp(int keyCode) {
    /*
	switch (keyCode) {
		case CSKeyCodeA:
		case CSKeyCodeLeftShift:
			if (_selectControl && _selectAttackersLayer && _selectAttackersLayer->state() == CSLayerStateFocus) {
				onTouchesEndedSelectAttackers(_selectAttackersLayer);
			}
			break;
	}
    */
}

void GameLayer::onKeyCommand(int i) {
    /*
	if (_playerUI && i < _subCommandLayers->count()) {
		CSLayer* layer = _subCommandLayers->objectAtIndex(i);

		if (layer->state() == CSLayerStateFocus) {
			onTouchesBeganCommand(layer);
			onTouchesEndedCommand(layer);
		}
	}
    */
}

void GameLayer::onKeySpawn(int i) {
    /*
	if (_playerUI && i < _spawnLayers->count()) {
		CSLayer* layer = _spawnLayers->objectAtIndex(i);

		if (layer->state() == CSLayerStateFocus) {
			onTouchesBeganSpawn(layer);
			onTouchesEndedSpawn(layer);
		}
	}
    */
}

bool GameLayer::layerContains(CSLayer* layer, const CSVector2& p) {
    CSRect rect = layer->bounds();
    layer->convertToParentSpace(&rect.origin, this);
    return rect.contains(p);
}

bool GameLayer::doCommand(const CommandIndex& index) {
    CommandReturn rtn = Map::sharedMap()->command(index, NULL, CommandCommitRun);

    switch (rtn.state) {
        case CommandStateUnabled:
            showCommandUnabled(index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
            return false;
        case CommandStateTrainUnabled:
            showCommandUnabled(index, rtn.parameter.train.msg, rtn.parameter.train.announce);
            return false;
        case CommandStateProduceUnabled:
            showCommandUnabled(index, rtn.parameter.produce.msg, rtn.parameter.produce.announce);
            return false;
        case CommandStateBuildUnabled:
            showCommandUnabled(index, rtn.parameter.build.msg, rtn.parameter.build.announce);
            return false;
        case CommandStateEnabled:
            return true;
        default:
            return false;
    }
}

void GameLayer::showCommandUnabled(const CommandIndex& index, const MessageIndex& msg, AnnounceIndex announce) {
    if (msg) {
        Map::sharedMap()->pushMessage(Message::message(Asset::sharedAsset()->command(index).icon, Asset::sharedAsset()->string(msg)));
    }
    if (announce != AnnounceNone) {
        Map::sharedMap()->announce(announce);
    }
}

void GameLayer::onTouchesBeganCommand(CSLayer* layer) {
    //오토타게팅제거 2022-07-11
    //if (Option::sharedOption()->autoTargetingCommand()) beginDoubleClickControl(layer);
    //else releaseDoubleClickControl();
    //releaseDoubleClickControl();

    //_numbering = false;
    
	CommandIndex index(layer->tagAsInt());

	Sound::play(SoundControlEffect, index.indices[0] == CommandSetMain ? SoundFxCommand_0 : SoundFxCommand_1, SoundPlayEffectEssential);

    switch (_state.code) {
        case GameStateTargetCommand:
            if (index == _state.context.targetCommand.index) return;
            break;
        case GameStateRangeCommand:
            if (index == _state.context.rangeCommand.index) return;
            break;
        case GameStateAngleCommand:
            if (index == _state.context.angleCommand.index) return;
            break;
        case GameStateLineCommand:
            if (index == _state.context.lineCommand.index) return;
            break;
        case GameStateBuildTargetCommand:
            if (index == _state.context.buildTargetCommand.index) return;
            break;
    }
    releaseState();

    CommandReturn rtn = Map::sharedMap()->command(index, NULL, CommandCommitNone);
    
    switch (rtn.state) {
        case CommandStateTarget:
            _state.code = GameStateTargetCommand;
            _state.context.targetCommand.index = index;
            _state.context.targetCommand.targeting = GameTargetingNone;
            _state.context.targetCommand.rtn = rtn.parameter.target;
            _state.context.targetCommand.destinations = new CSArray<Destination>();
            _state.context.targetCommand.destinations->addObject(Destination::destination(Map::sharedMap()->force()));
            _state.context.targetCommand.effects = new CSArray<Effect>();
            _state.context.targetCommand.units = NULL;
            _state.context.targetCommand.target = NULL;
            break;
        case CommandStateRange:
            _state.code = GameStateRangeCommand;
            _state.context.rangeCommand.index = index;
            _state.context.rangeCommand.targeting = GameTargetingNone;
            _state.context.rangeCommand.rtn = rtn.parameter.range;
            _state.context.rangeCommand.destinations = new CSArray<Destination>();
            _state.context.rangeCommand.destinations->addObject(Destination::destination(Map::sharedMap()->force(), rtn.parameter.range.range));
            _state.context.rangeCommand.effects = new CSArray<Effect>();
            _state.context.rangeCommand.units = NULL;
            break;
        case CommandStateAngle:
            _state.code = GameStateAngleCommand;
            _state.context.angleCommand.index = index;
            _state.context.angleCommand.targeting = GameTargetingNone;
            _state.context.angleCommand.rtn = rtn.parameter.angle;
            _state.context.angleCommand.destination = new Destination(Map::sharedMap()->force());
            _state.context.angleCommand.effects = new CSArray<Effect>();
            _state.context.angleCommand.units = NULL;
            break;
        case CommandStateLine:
            _state.code = GameStateLineCommand;
            _state.context.lineCommand.index = index;
            _state.context.lineCommand.targeting = GameTargetingNone;
            _state.context.lineCommand.rtn = rtn.parameter.line;
            _state.context.lineCommand.destination = new Destination(Map::sharedMap()->force());
            _state.context.lineCommand.effects = new CSArray<Effect>();
            _state.context.lineCommand.units = NULL;
            break;
        case CommandStateProduce:
            _state.code = GameStateProduceCommand;
            _state.context.produceCommand.index = index;
            _state.context.produceCommand.unit = rtn.parameter.produce.index;
            _state.context.produceCommand.seq = LongClickDuration * Map::sharedMap()->framePerSecond();
            break;
        case CommandStateTrain:
            _state.code = GameStateTrainCommand;
            _state.context.trainCommand.index = index;
            _state.context.trainCommand.rune = rtn.parameter.train.index;
			_state.context.trainCommand.confirmLayer = NULL;
            _state.context.trainCommand.seq = LongClickDuration * Map::sharedMap()->framePerSecond();
            break;
        case CommandStateBuild:
            if (rtn.parameter.build.target) {
                _state.code = GameStateBuildTargetCommand;
                _state.context.buildTargetCommand.index = index;
                _state.context.buildTargetCommand.targeting = GameTargetingNone;
                _state.context.buildTargetCommand.unit = rtn.parameter.build.index;
                _state.context.buildTargetCommand.target = rtn.parameter.build.target;
                _state.context.buildTargetCommand.frame = new Frame(Map::sharedMap()->force(), rtn.parameter.build.index, rtn.parameter.build.target, false);
                _state.context.buildTargetCommand.effect = retain(LightEffect::effect(_state.context.buildTargetCommand.frame, EffectDisplayBlink));
                Map::sharedMap()->showBuildingLocations(rtn.parameter.build.index);
            }
            else {
                _state.code = GameStateBuildCommand;
                _state.context.buildCommand.index = index;
                _state.context.buildCommand.unit = rtn.parameter.build.index;
            }
            break;
        case CommandStateEnabled:
            _state.code = GameStateCommand;
            _state.context.command.index = index;
            _state.context.command.all = rtn.parameter.enabled.all;
            break;
        default:
            _state.code = GameStateNone;
            break;
    }

    switch (rtn.state) {
        case CommandStateUnabled:
            showCommandUnabled(index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
            break;
        case CommandStateProduceUnabled:
            showCommandUnabled(index, rtn.parameter.produce.msg, rtn.parameter.produce.announce);
            break;
        case CommandStateBuildUnabled:
            showCommandUnabled(index, rtn.parameter.build.msg, rtn.parameter.build.announce);
            break;
		case CommandStateTrainUnabled:
            showCommandUnabled(index, rtn.parameter.train.msg, rtn.parameter.train.announce);
		case CommandStateTrainComplete:
			{
				const Force* force = Map::sharedMap()->force();
				if (force) {
					const Rune* rune = force->runeForIndex(rtn.parameter.train.index);
					if (rune) {
						CSVector2 p = layer->centerMiddle();
						layer->convertToViewSpace(&p);
						view()->addLayer(RuneAbilityLayer::layer(rune, p, false));
					}
				}
			}
			break;
        default:
            {
                CSString* msg = new CSString();

                const CommandData& data = Asset::sharedAsset()->command(index);

				const char* description = *data.description;

				if (description) {
					msg->append(description);
				}

                switch (rtn.state) {
					case CommandStateProduce:
						if (!description) {
							description = *Asset::sharedAsset()->unit(rtn.parameter.produce.index).description;
							if (description) msg->append(description);
						}
						break;
                    case CommandStateBuild:
						if (!description) {
							description = *Asset::sharedAsset()->unit(rtn.parameter.build.index).description;
							if (description) msg->append(description);
						}
                        if (!rtn.parameter.build.target) {
                            break;
                        }
                    case CommandStateTarget:
                    case CommandStateRange:
                    case CommandStateLine:
                    case CommandStateAngle:
                        {
                            const CSString* str = Asset::sharedAsset()->string(MessageSetGame, MessageGameDragTarget);
                            if (msg) {
                                msg->append("\n#color(00C0FFFF)");
                                msg->append(*str);
                            }
                        }
                        break;
                }
                if (msg->length()) {
                    Map::sharedMap()->pushMessage(Message::message(data.icon, msg));
                }
                msg->release();
            }
            break;
    }
}

void GameLayer::onTouchesMovedCommand(CSLayer* layer) {
    const CSTouch* touch = layer->touch();
    CSVector2 p = touch->point(this);
    CSVector2 tp = p;
    if (touch->isMoved()) tp.y -= TargetPointVerticalOffset;
    
    switch (_state.code) {
        case GameStateTargetCommand:
            {
                Destination* dest = _state.context.targetCommand.destinations->lastObject();
                if (!layerContains(layer, p)) {
                    _state.context.targetCommand.targeting = GameTargetingScreen;
                    Map::sharedMap()->setLightState(MapLightStateOff);

                    FPoint mp;
                    if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                        _state.context.targetCommand.point = tp;
                        dest->locate(mp);
                    }
                    else {
                        dest->unlocate();
                    }
                }
                else {
                    _state.context.targetCommand.targeting = GameTargetingReady;
                    Map::sharedMap()->setLightState(MapLightStateOn);
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    dest->unlocate();
                }
            }
            break;
        case GameStateRangeCommand:
            {
                Destination* dest = _state.context.rangeCommand.destinations->lastObject();
                if (!layerContains(layer, p)) {
                    _state.context.rangeCommand.targeting = GameTargetingScreen;
                    Map::sharedMap()->setLightState(MapLightStateOff);

                    FPoint mp;
                    if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                        _state.context.rangeCommand.point = tp;
                        dest->locate(mp);
                    }
                    else {
                        dest->unlocate();
                    }
                }
                else {
                    _state.context.rangeCommand.targeting = GameTargetingReady;
                    Map::sharedMap()->setLightState(MapLightStateOn);
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    dest->unlocate();
                }
            }
            break;
        case GameStateAngleCommand:
            if (!layerContains(layer, p)) {
                _state.context.angleCommand.targeting = GameTargetingScreen;
                Map::sharedMap()->setLightState(MapLightStateOff);

                FPoint mp;
                if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                    _state.context.angleCommand.point = tp;
                    _state.context.angleCommand.destination->locate(mp);
                }
                else {
                    _state.context.angleCommand.destination->unlocate();
                }
            }
            else {
                _state.context.angleCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOn);
                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;
                _state.context.angleCommand.destination->unlocate();
            }
            break;
        case GameStateLineCommand:
            if (!layerContains(layer, p)) {
                _state.context.lineCommand.targeting = GameTargetingScreen;
                Map::sharedMap()->setLightState(MapLightStateOff);

                FPoint mp;
                if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                    _state.context.lineCommand.point = tp;
                    _state.context.lineCommand.destination->locate(mp);
                }
                else {
                    _state.context.lineCommand.destination->unlocate();
                }
            }
            else {
                _state.context.lineCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOn);
                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;
                _state.context.lineCommand.destination->unlocate();
            }
            break;
        case GameStateBuildTargetCommand:
            {
                Frame* frame = _state.context.buildTargetCommand.frame;
                if (!layerContains(layer, p)) {
                    _state.context.buildTargetCommand.targeting = GameTargetingScreen;
                    //Map::sharedMap()->lightOff(true);
                    Map::sharedMap()->setLightState(_state.context.buildTargetCommand.target == CommandBuildTargetSpawn ? MapLightStateOffSpawn : MapLightStateOffGrid);
                    
                    FPoint mp;
                    if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                        const UnitData& data = frame->data();
                        
                        if (Object::compareType(data.type, ObjectMaskRefinery|ObjectMaskBase)) {
                            FPoint nmp = mp;
                            
                            if (Map::sharedMap()->locatablePosition(frame, data.type, nmp, data.collider, Map::sharedMap()->vision(), MapLocationEnabled) != MapLocationUnabled) {
                                frame->locate(nmp);
                                break;
                            }
                        }
                        if (Object::compareType(data.type, ObjectMaskTiled)) {
                            Map::tiledPosition(data.collider, mp);
                        }
                        IBounds bounds(mp, data.collider, false);
                        
                        if (bounds.src.x < 0 || bounds.src.y < 0 || bounds.dest.x >= Map::sharedMap()->terrain()->width() || bounds.dest.y >= Map::sharedMap()->terrain()->height()) {
                            frame->unlocate();
                        }
                        else {
                            frame->locate(mp);
                        }
                    }
                    else {
                        frame->unlocate();
                    }
                }
                else {
                    _state.context.buildTargetCommand.targeting = GameTargetingReady;
                    Map::sharedMap()->setLightState(MapLightStateOn);
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    frame->unlocate();
                }
            }
            break;
		case GameStateTrainCommand:
			releaseTrainCommandState();
			_state.code = GameStateNone;
			break;
		case GameStateCommand:
        case GameStateBuildCommand:
            _state.code = GameStateNone;
            break;
    }
}

void GameLayer::onTouchesEndedCommand(CSLayer* layer) {
    switch (_state.code) {
        case GameStateBuildTargetCommand:
            switch (_state.context.buildTargetCommand.targeting) {
                case GameTargetingScreen:
                    {
                        _state.context.buildTargetCommand.targeting = GameTargetingReady;

                        Frame* frame = _state.context.buildTargetCommand.frame;
                        
                        if (frame->isLocated()) {
                            const UnitData& data = frame->data();
                            
							MapLocation loc = Map::sharedMap()->buildable(frame, data.type, frame->point(), data.collider, Map::sharedMap()->vision(), frame->target);

                            if (loc != MapLocationUnabled) {
                                frame->setCollision(true);
                                
                                CommandReturn rtn = Map::sharedMap()->command(_state.context.buildTargetCommand.index, frame, CommandCommitRun);
                                
                                //TODO:네트워크 상에서 딜레이로 인해 실행할 수 없는데 실행할 수 있는 경우 있음, 일단 무시
                                
                                if (rtn.state == CommandStateUnabled) {
                                    showCommandUnabled(_state.context.buildTargetCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                                }
                            }
                            else {
                                showCommandUnabled(_state.context.buildTargetCommand.index, MessageIndex(MessageSetCommand, MessageCommandBuildUnabled), AnnounceBuildUnabled);

                                frame->unlocate();
                                break;
                            }
                        }
                        else {
                            Map::sharedMap()->camera()->stopScroll(true);
                            _mapScroll = CSVector2::Zero;
                            break;
                        }
                    }
                case GameTargetingReady:
                    releaseBuildTargetCommandState();
                    _state.code = GameStateNone;
                    break;
                case GameTargetingNone:
                    _state.context.buildTargetCommand.targeting = GameTargetingReady;
                    //Map::sharedMap()->lightOff(true);
                    Map::sharedMap()->setLightState(_state.context.buildTargetCommand.target == CommandBuildTargetSpawn ? MapLightStateOffSpawn : MapLightStateOffGrid);
                    break;
                
            }
            break;
        case GameStateTargetCommand:
            if (_state.context.targetCommand.targeting == GameTargetingScreen) {
                _state.context.targetCommand.targeting = GameTargetingReady;
                
                Destination* dest = _state.context.targetCommand.destinations->lastObject();

                if (dest->isLocated()) {
                    const Object* target = _state.context.targetCommand.target;
                    if (!target && !_state.context.targetCommand.rtn.condition) {
                        target = dest;
                    }
                    if (target) {
                        //CommandReturn rtn = Map::sharedMap()->command(_state.context.targetCommand.index, target, _state.context.targetCommand.stack ? CommandCommitStack : CommandCommitRun);
                        CommandReturn rtn = Map::sharedMap()->command(_state.context.targetCommand.index, target, CommandCommitRun);

                        //TODO:네트워크 상에서 딜레이로 인해 실행할 수 없는데 실행할 수 있는 경우 있음, 일단 무시

                        if (rtn.state == CommandStateUnabled) {
                            showCommandUnabled(_state.context.targetCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                        }
                    }
                    else {
                        dest->unlocate();
                        break;
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingCommand() && _state.context.targetCommand.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.targetCommand.targeting == GameTargetingNone)) {
                if (!doCommand(_state.context.targetCommand.index)) releaseDoubleClickControl();
            }
            */
            else if (_state.context.targetCommand.targeting == GameTargetingNone) {
                _state.context.targetCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseTargetCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateRangeCommand:
            if (_state.context.rangeCommand.targeting == GameTargetingScreen) {
                _state.context.rangeCommand.targeting = GameTargetingReady;

                Destination* dest = _state.context.rangeCommand.destinations->lastObject();

                if (dest->isLocated()) {
                    //CommandReturn rtn = Map::sharedMap()->command(_state.context.rangeCommand.index, dest, _state.context.rangeCommand.stack ? CommandCommitStack : CommandCommitRun);
                    CommandReturn rtn = Map::sharedMap()->command(_state.context.rangeCommand.index, dest, CommandCommitRun);

                    //TODO:네트워크 상에서 딜레이로 인해 실행할 수 없는데 실행할 수 있는 경우 있음, 일단 무시

                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.rangeCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingCommand() && _state.context.rangeCommand.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.rangeCommand.targeting == GameTargetingNone)) {
                if (!doCommand(_state.context.rangeCommand.index)) releaseDoubleClickControl();
            }
            */
            else if (_state.context.rangeCommand.targeting == GameTargetingNone) {
                _state.context.rangeCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseRangeCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateAngleCommand:
            if (_state.context.angleCommand.targeting == GameTargetingScreen) {
                _state.context.angleCommand.targeting = GameTargetingReady;

                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;

                if (_state.context.angleCommand.destination->isLocated()) {
                    CommandReturn rtn = Map::sharedMap()->command(_state.context.angleCommand.index, _state.context.angleCommand.destination, CommandCommitRun);

                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.angleCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingCommand() && _state.context.angleCommand.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.angleCommand.targeting == GameTargetingNone)) {
                if (!doCommand(_state.context.angleCommand.index)) releaseDoubleClickControl();
            }
            */
            else if (_state.context.angleCommand.targeting == GameTargetingNone) {
                _state.context.angleCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseAngleCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateLineCommand:
            if (_state.context.lineCommand.targeting == GameTargetingScreen) {
                _state.context.lineCommand.targeting = GameTargetingReady;

                if (_state.context.lineCommand.destination->isLocated()) {
                    CommandReturn rtn = Map::sharedMap()->command(_state.context.lineCommand.index, _state.context.lineCommand.destination, CommandCommitRun);

                    if (rtn.state == CommandStateUnabled) {
                        showCommandUnabled(_state.context.lineCommand.index, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingCommand() && _state.context.lineCommand.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.lineCommand.targeting == GameTargetingNone)) {
                if (!doCommand(_state.context.lineCommand.index)) releaseDoubleClickControl();
            }
            */
            else if (_state.context.lineCommand.targeting == GameTargetingNone) {
                _state.context.lineCommand.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseLineCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateCommand:
            doCommand(_state.context.command.index);
            _state.code = GameStateNone;
            break;
        case GameStateProduceCommand:
            doCommand(_state.context.produceCommand.index);
            _state.code = GameStateNone;
            break;
        case GameStateTrainCommand:
            if (!_state.context.trainCommand.confirmLayer) {
                const Force* force = Map::sharedMap()->force();
                if (force) {
                    const Rune* rune = force->runeForIndex(_state.context.trainCommand.rune);
                    if (rune) {
                        CSVector2 p = layer->centerMiddle();
                        layer->convertToViewSpace(&p);

                        RuneAbilityLayer* confirmLayer = new RuneAbilityLayer(rune, p, true);
                        confirmLayer->setOwner(this);
                        confirmLayer->OnConfirm.add([this](CSLayer* layer, bool yes) {
                            if (yes) doCommand(_state.context.trainCommand.index);
                            _state.code = GameStateNone;
                        });
                        _state.context.trainCommand.confirmLayer = confirmLayer;
                    }
                }
            }
            if (_state.context.trainCommand.confirmLayer) view()->addLayer(_state.context.trainCommand.confirmLayer);
            else _state.code = GameStateNone;
			break;
        case GameStateBuildCommand:
            doCommand(_state.context.buildCommand.index);
            _state.code = GameStateNone;
            break;
    }
}
void GameLayer::onTouchesCancelledCommand(CSLayer *layer) {
    switch (_state.code) {
        case GameStateTargetCommand:
            releaseTargetCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateRangeCommand:
            releaseRangeCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateAngleCommand:
            releaseAngleCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateLineCommand:
            releaseLineCommandState();
            _state.code = GameStateNone;
            break;
        case GameStateBuildTargetCommand:
            releaseBuildTargetCommandState();
            _state.code = GameStateNone;
            break;
		case GameStateTrainCommand:
			releaseTrainCommandState();
			_state.code = GameStateNone;
			break;
		case GameStateCommand:
        case GameStateProduceCommand:
        case GameStateBuildCommand:
            _state.code = GameStateNone;
            break;
    }
}
static void drawCommandBase(CSGraphics* graphics, float width, const ImageIndex& icon0, const CSString* name, const ImageIndex& icon1) {
	graphics->drawImage(Asset::sharedAsset()->image(icon0), CSVector3::Zero, CSAlignCenterMiddle);

	if (icon1) {
        graphics->drawImage(Asset::sharedAsset()->image(icon1), CSVector3::Zero, CSAlignCenterMiddle);
    }
    if (name) {
		graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
        graphics->setStrokeWidth(2);
        graphics->drawStringScaled(name, CSVector2(0, width * 0.2f), CSAlignCenterMiddle, width * 0.6f);
        graphics->setStrokeWidth(0);
    }
}
static void drawCommandBase(CSGraphics* graphics, float width, const ImageIndex& icon0, const CSString* name, const ImageIndex& icon1, float progress) {
	graphics->setColor(CSColor::DarkGray);
	graphics->drawImage(Asset::sharedAsset()->image(icon0), CSVector3::Zero, CSAlignCenterMiddle);
	graphics->setColor(CSColor::White);
	graphics->drawClockImage(Asset::sharedAsset()->image(icon0), progress, CSVector3::Zero, CSAlignCenterMiddle);

	if (icon1) {
        graphics->drawImage(Asset::sharedAsset()->image(icon1), CSVector3::Zero, CSAlignCenterMiddle);
    }
    if (name) {
		graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
        graphics->setStrokeWidth(2);
        graphics->drawStringScaled(name, CSVector2(0, width * 0.2f), CSAlignCenterMiddle, width * 0.6f);
        graphics->setStrokeWidth(0);
    }
}

static void drawCommandValue(CSGraphics* graphics, int resource0, int resource1, int population, int count) {
    float y = 24;
    
    graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
    graphics->setStrokeWidth(2);
    if (resource0) {
        graphics->setColor(CSColor(255, 192, 0, 255));
        graphics->drawDigit(resource0, false, CSVector2(32, y), CSAlignRightMiddle);
        y -= 17;
    }
    if (resource1) {
        graphics->setColor(CSColor(0, 192, 255, 255));
        graphics->drawDigit(resource1, false, CSVector2(32, y), CSAlignRightMiddle);
        y -= 17;
    }
    if (population) {
        graphics->setColor(CSColor(0, 255, 128, 255));
        graphics->drawDigit(population, false, CSVector2(32, y), CSAlignRightMiddle);
    }
	graphics->setFont(Asset::sharedAsset()->boldSmallFont);
	if (count) {
		graphics->setColor(CSColor::White);
		graphics->drawDigit(count, false, CSVector2(-28, 16), CSAlignCenterMiddle);
	}
    graphics->setStrokeWidth(0);
}

void GameLayer::onDrawCommand(CSLayer* layer, CSGraphics* graphics) {
    CommandIndex index(layer->tagAsInt());
    
    CommandReturn rtn = Map::sharedMap()->command(index, NULL, CommandCommitNone);
    
    bool selected;
	bool target;
    switch (_state.code) {
        case GameStateTargetCommand:
            selected = index == _state.context.targetCommand.index;
			target = selected;
            if (!selected) graphics->setSaturation(0);
            break;
        case GameStateRangeCommand:
            selected = index == _state.context.rangeCommand.index;
			target = selected;
            if (!selected) graphics->setSaturation(0);
            break;
        case GameStateAngleCommand:
            selected = index == _state.context.angleCommand.index;
			target = selected;
            if (!selected) graphics->setSaturation(0);
            break;
        case GameStateLineCommand:
            selected = index == _state.context.lineCommand.index;
			target = selected;
            if (!selected) graphics->setSaturation(0);
            break;
        case GameStateBuildTargetCommand:
            selected = index == _state.context.buildTargetCommand.index;
			target = selected;
            if (!selected) graphics->setSaturation(0);
            break;
		case GameStateTargetSpawn:
		case GameStateRangeSpawn:
		case GameStateBuildSpawn:
			selected = false;
			target = false;
			graphics->setSaturation(0);
			break;
        default:
            selected = layer->isTouching();
			target = true;
            break;
    }

	const CommandData& data = Asset::sharedAsset()->command(index);

	const CSString* name = data.name->value();

	if (!name) {
		switch (rtn.state) {
			case CommandStateBuildUnabled:
			case CommandStateBuild:
				name = Asset::sharedAsset()->unit(rtn.parameter.build.index).name->value();
				break;
			case CommandStateProduceUnabled:
			case CommandStateProduce:
				name = Asset::sharedAsset()->unit(rtn.parameter.produce.index).name->value();
				break;
			case CommandStateTrainUnabled:
			case CommandStateTrain:
			case CommandStateTrainComplete:
				name = Asset::sharedAsset()->rune(rtn.parameter.train.index).name->value();
				break;
		}
	}

	float width = 100;
	ImageIndex icon1 = ImageIndex::None;

	switch (index.indices[0]) {
		case CommandSetMain:
			width = 140;
			target = false;
			if (selected) icon1 = ImageIndex(ImageSetGame, ImageGameActivate_2);
			break;
		case CommandSetSub:
			target = false;
			if (selected) icon1 = ImageIndex(ImageSetGame, ImageGameActivate_1);
			break;
		default:
			name = NULL;
			if (selected) icon1 = ImageIndex(ImageSetGame, ImageGameActivate_1);
			break;
	}

	graphics->translate(layer->centerMiddle());
	graphics->scale(layer->width() / width);

	switch (rtn.state) {
        case CommandStateEmpty:
		case CommandStateUnabled:
            graphics->setColor(CSColor::Gray);
            drawCommandBase(graphics, width, data.icon, name, icon1);
            break;
        case CommandStateCooltime:
            drawCommandBase(graphics, width, data.icon, name, icon1, (rtn.parameter.cooltime.initial - rtn.parameter.cooltime.remaining) / rtn.parameter.cooltime.initial);

			graphics->setColor(CSColor::White);
            graphics->setStrokeWidth(2);
            graphics->setFont(Asset::sharedAsset()->boldExtraLargeFont);
            graphics->drawDigit(CSMath::ceil(rtn.parameter.cooltime.remaining), false, CSVector3::Zero, CSAlignCenterMiddle);
            break;
        case CommandStateProduceUnabled:
            graphics->setColor(CSColor::Gray);
        case CommandStateProduce:
            {

                const UnitData& subdata = Asset::sharedAsset()->unit(rtn.parameter.produce.index);
                const Force* force = Map::sharedMap()->force();
                ImageIndex icon0;

				if (force && subdata.skins) {
					icon0 = subdata.skins->objectAtIndex(force->unitSkin(rtn.parameter.produce.index)).commandIcon;
					if (!icon0) icon0 = data.icon;
				}
				else {
					icon0 = data.icon;
				}
				drawCommandBase(graphics, width, icon0, name, icon1);

                drawCommandValue(graphics, 
					subdata.resources[0], 
					subdata.resources[1], 
					subdata.population * (subdata.addition + 1),
					Map::sharedMap()->producingCount(rtn.parameter.produce.index)
				);
            }
            break;
        case CommandStateTrainUnabled:
			if (rtn.parameter.train.progress) {
				const RuneData& subdata = Asset::sharedAsset()->rune(rtn.parameter.train.index);
				
				if (subdata.progress) {
					drawCommandBase(graphics, width, data.icon, name, icon1, rtn.parameter.train.progress / subdata.progress);

					graphics->setColor(CSColor::White);
					graphics->setStrokeWidth(2);
					graphics->setFont(Asset::sharedAsset()->boldLargeFont);
					graphics->drawDigit(CSMath::ceil(subdata.progress - rtn.parameter.train.progress), false, CSVector3::Zero, CSAlignCenterMiddle);
					break;
				}
			}
			graphics->setColor(CSColor::Gray);
        case CommandStateTrain:
            drawCommandBase(graphics, width, data.icon, name, icon1);
            {
                const RuneData& subdata = Asset::sharedAsset()->rune(rtn.parameter.train.index);
                drawCommandValue(graphics, subdata.resources[0], subdata.resources[1], 0, 0);
            }
            break;
		case CommandStateTrainComplete:
			drawCommandBase(graphics, width, data.icon, name, icon1);
			graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameTrainComplete), CSVector3::Zero, CSAlignCenterMiddle);
			break;
        case CommandStateBuildUnabled:
            graphics->setColor(CSColor::Gray);
        case CommandStateBuild:
            {
                const UnitData& subdata = Asset::sharedAsset()->unit(rtn.parameter.build.index);
                const Force* force = Map::sharedMap()->force();
				ImageIndex icon0;
				
				if (force && subdata.skins) {
					icon0 = subdata.skins->objectAtIndex(force->unitSkin(rtn.parameter.build.index)).commandIcon;
					if (!icon0) icon0 = data.icon;
				}
				else {
					icon0 = data.icon;
				}
                drawCommandBase(graphics, width, icon0, name, icon1);
                
				drawCommandValue(graphics, 
					subdata.resources[0], 
					subdata.resources[1], 
					subdata.population, 
					Map::sharedMap()->unitCount(rtn.parameter.build.index)
				);
				if (target && rtn.parameter.build.target) {
					graphics->setColor(CSColor::White);
					graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCommandTarget), CSVector2(-26, -26), CSAlignCenterMiddle);
				}
            }
            break;
		case CommandStateTarget:
		case CommandStateRange:
		case CommandStateAngle:
		case CommandStateLine:
            //오토타게팅제거 2022-07-11
            /*
            if (_doubleClickLayer == layer) {
                float p = CSMath::max(1.0f - (float)_doubleClickWaiting / (DoubleClickAutoTargetDuration * Map::sharedMap()->framePerSecond()), 0.0f);
                drawCommandBase(graphics, width, data.icon, name, icon1, p);
            }
            else {
                drawCommandBase(graphics, width, data.icon, name, icon1);
            }
            */
            drawCommandBase(graphics, width, data.icon, name, icon1);

            graphics->setColor(CSColor::White);
            if (target) {
				graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCommandTarget), CSVector2(-26, -26), CSAlignCenterMiddle);
			}
			break;
		default:
            drawCommandBase(graphics, width, data.icon, name, icon1);
            break;
    }
}

void GameLayer::onTouchesBeganCancelCommand(CSLayer* layer) {
    releaseState();
    //releaseControl();
    
    uint tag = layer->tagAsInt();
    
    int type = tag & 3;
    
    switch (type) {
        case 0:
            _state.context.cancelProducingCommand.unit = UnitIndex(tag >> 2);
            _state.context.cancelProducingCommand.seq = LongClickDuration * Map::sharedMap()->framePerSecond();
            _state.code = GameStateCancelProducingCommand;
            break;
        default:
            CSAssert(false, "invalid type");
            break;
    }

	Sound::play(SoundControlEffect, SoundFxCommand_1, SoundPlayEffectEssential);
}
void GameLayer::onTouchesEndedCancelCommand(CSLayer* layer) {
    switch (_state.code) {
        case GameStateCancelProducingCommand:
            Map::sharedMap()->cancelProducingCommand(_state.context.cancelProducingCommand.unit);
            _state.code = GameStateNone;
            break;
    }
}
void GameLayer::onTouchesCancelledCancelCommand(CSLayer* layer) {
    switch (_state.code) {
        case GameStateCancelProducingCommand:
            Map::sharedMap()->cancelProducingCommand(_state.context.cancelProducingCommand.unit);
            _state.code = GameStateNone;
            break;
    }
}
void GameLayer::onDrawCancelCommand(CSLayer* layer, CSGraphics* graphics) {
	float scale = layer->width() / 40;

    graphics->drawImageScaled(Asset::sharedAsset()->image(ImageSetGame, ImageGameCancel_1), layer->centerMiddle(), scale, CSAlignCenterMiddle);
}

static const Spawn* getSpawn(int slot) {
    const Force* force = Map::sharedMap()->force();

    return force && slot < force->spawnCount() ? force->spawn(slot) : NULL;
}

void GameLayer::showSpawnUnabled(const Spawn* spawn, const MessageIndex& msg, AnnounceIndex announce) {
    if (msg) {
        Map::sharedMap()->pushMessage(Message::message(spawn ? spawn->smallIcon() : ImageIndex::None, Asset::sharedAsset()->string(msg)));
    }
    if (announce != AnnounceNone) {
        Map::sharedMap()->announce(announce);
    }
}

void GameLayer::onTouchesBeganSpawn(CSLayer* layer) {
    //오토타게팅제거 2022-07-11
    //if (Option::sharedOption()->autoTargetingSpawn()) beginDoubleClickControl(layer);
    //else releaseDoubleClickControl();
    //releaseDoubleClickControl();

    //_numbering = false;

	Sound::play(SoundControlEffect, SoundFxCommand_1, SoundPlayEffectEssential);

    int slot = layer->tagAsInt();
    
    switch (_state.code) {
        case GameStateTargetSpawn:
            if (_state.context.targetSpawn.slot == slot) return;
            break;
        case GameStateRangeSpawn:
            if (_state.context.rangeSpawn.slot == slot) return;
            break;
        case GameStateBuildSpawn:
            if (_state.context.buildSpawn.slot == slot) return;
            break;
    }
    
    releaseState();
    
    CommandReturn rtn = Map::sharedMap()->spawn(slot, NULL, SpawnCommitNone);
    
    switch (rtn.state) {
        case CommandStateTarget:
            _state.code = GameStateTargetSpawn;
            _state.context.targetSpawn.slot = slot;
            _state.context.targetSpawn.targeting = GameTargetingNone;
            _state.context.targetSpawn.rtn = rtn.parameter.target;
            _state.context.targetSpawn.destination = new Destination(Map::sharedMap()->force());
            _state.context.targetSpawn.effects = new CSArray<Effect>();
            _state.context.targetSpawn.target = NULL;
            break;
        case CommandStateRange:
            _state.code = GameStateRangeSpawn;
            _state.context.rangeSpawn.slot = slot;
            _state.context.rangeSpawn.targeting = GameTargetingNone;
            _state.context.rangeSpawn.rtn = rtn.parameter.range;
            _state.context.rangeSpawn.destination = new Destination(Map::sharedMap()->force(), rtn.parameter.range.range);
            _state.context.rangeSpawn.effects = new CSArray<Effect>();
            break;
        case CommandStateBuild:
            _state.code = GameStateBuildSpawn;
            _state.context.buildSpawn.slot = slot;
            _state.context.buildSpawn.targeting = GameTargetingNone;
            _state.context.buildSpawn.target = rtn.parameter.build.target;
            _state.context.buildSpawn.frame = new Frame(Map::sharedMap()->force(), rtn.parameter.build.index, rtn.parameter.build.target, false);
            _state.context.buildSpawn.effect = retain(LightEffect::effect(_state.context.buildSpawn.frame, EffectDisplayBlink));
            Map::sharedMap()->showBuildingLocations(rtn.parameter.build.index);
            break;
        case CommandStateEnabled:
            _state.code = GameStateSpawn;
            _state.context.spell.slot = slot;
            break;
        default:
            _state.code = GameStateNone;
            break;
    }
    
    
    const Spawn* spawn = getSpawn(slot);
    
    switch (rtn.state) {
        case CommandStateUnabled:
            showSpawnUnabled(spawn, rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
            break;
        default:
            {
                CSString* msg = new CSString();
                if (spawn) {
                    const UnitData& data = spawn->data();
                    const CSString* description = data.description->value();
                    if (description) {
                        msg->append(*description);
                    }
                }
                switch (rtn.state) {
                    case CommandStateTarget:
                    case CommandStateRange:
                    case CommandStateBuild:
                        {
                            const CSString* str = Asset::sharedAsset()->string(MessageSetGame, MessageGameDragTarget);
                            msg->append("\n#color(00C0FFFF)");
                            msg->append(*str);
                        }
                        break;
                }
                if (msg->length()) {
                    Map::sharedMap()->pushMessage(Message::message(spawn ? spawn->smallIcon() : ImageIndex::None, msg));
                }
                msg->release();
            }
            break;
    }
}

void GameLayer::onTouchesMovedSpawn(CSLayer* layer) {
    const CSTouch* touch = layer->touch();
    CSVector2 p = touch->point(this);
    CSVector2 tp = p;
    if (touch->isMoved()) tp.y -= TargetPointVerticalOffset;
    
    switch (_state.code) {
        case GameStateTargetSpawn:
            if (!layerContains(layer, p)) {
                _state.context.targetSpawn.targeting = GameTargetingScreen;
                Map::sharedMap()->setLightState(MapLightStateOff);

                FPoint mp;
                if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                    _state.context.targetSpawn.point = tp;
                    _state.context.targetSpawn.destination->locate(mp);
                }
                else {
                    _state.context.targetSpawn.destination->unlocate();
                }
            }
            else {
                _state.context.targetSpawn.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOn);
                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;
                _state.context.targetSpawn.destination->unlocate();
            }
            break;
        case GameStateRangeSpawn:
            if (!layerContains(layer, p)) {
                _state.context.rangeSpawn.targeting = GameTargetingScreen;
                Map::sharedMap()->setLightState(MapLightStateOff);
                
                FPoint mp;
                if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                    _state.context.rangeSpawn.point = tp;
                    _state.context.rangeSpawn.destination->locate(mp);
                }
                else {
                    _state.context.rangeSpawn.destination->unlocate();
                }
            }
            else {
                _state.context.rangeSpawn.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOn);
                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;
                _state.context.rangeSpawn.destination->unlocate();
            }
            break;
        case GameStateBuildSpawn:
            if (!layerContains(layer, p)) {
                _state.context.buildSpawn.targeting = GameTargetingScreen;
                //Map::sharedMap()->lightOff(true);
                Map::sharedMap()->setLightState(_state.context.buildSpawn.target == CommandBuildTargetSpawn ? MapLightStateOffSpawn : MapLightStateOffGrid);
                
                FPoint mp;
                if ((!touch->isMoved() || !checkTargetScroll(p)) && Map::sharedMap()->convertViewToMapSpace(tp, mp, true)) {
                    const UnitData& data = _state.context.buildSpawn.frame->data();
                    
                    if (Object::compareType(data.type, ObjectMaskRefinery|ObjectMaskBase)) {
                        FPoint nmp = mp;
                        
                        if (Map::sharedMap()->locatablePosition(_state.context.buildSpawn.frame, data.type, nmp, data.collider, Map::sharedMap()->vision(), MapLocationEnabled) != MapLocationUnabled) {
                            _state.context.buildSpawn.frame->locate(nmp);
                            break;
                        }
                    }
                    if (Object::compareType(data.type, ObjectMaskTiled)) {
                        Map::tiledPosition(data.collider, mp);
                    }
                    IBounds bounds(mp, data.collider, false);
                    
                    if (bounds.src.x < 0 || bounds.src.y < 0 || bounds.dest.x >= Map::sharedMap()->terrain()->width() || bounds.dest.y >= Map::sharedMap()->terrain()->height()) {
                        _state.context.buildSpawn.frame->unlocate();
                    }
                    else {
                        _state.context.buildSpawn.frame->locate(mp);
                    }
                }
                else {
                    _state.context.buildSpawn.frame->unlocate();
                }
            }
            else {
                _state.context.buildSpawn.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOn);
                Map::sharedMap()->camera()->stopScroll(true);
                _mapScroll = CSVector2::Zero;
                _state.context.buildSpawn.frame->unlocate();
            }
            break;
        case GameStateSpawn:
            _state.code = GameStateNone;
            break;
    }
}
void GameLayer::onTouchesEndedSpawn(CSLayer* layer) {
    switch (_state.code) {
        case GameStateTargetSpawn:
            if (_state.context.targetSpawn.targeting == GameTargetingScreen) {
                _state.context.targetSpawn.targeting = GameTargetingReady;

                if (_state.context.targetSpawn.destination->isLocated()) {
                    const Object* target = _state.context.targetSpawn.target;
                    if (!target && !_state.context.targetSpawn.rtn.condition) {
                        target = _state.context.targetSpawn.destination;
                    }
                    if (target) {
                        CommandReturn rtn = Map::sharedMap()->spawn(_state.context.targetSpawn.slot, target, SpawnCommitRun);

                        if (rtn.state == CommandStateUnabled) {
                            showSpawnUnabled(getSpawn(_state.context.targetSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                        }
                    }
                    else {
                        _state.context.targetSpawn.destination->unlocate();
                        break;
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingSpawn() && _state.context.targetSpawn.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.targetSpawn.targeting == GameTargetingNone)) {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.targetSpawn.slot, NULL, SpawnCommitRunAuto);

                switch (rtn.state) {
                    case CommandStateEnabled:
                        break;
                    case CommandStateUnabled:
                        showSpawnUnabled(getSpawn(_state.context.targetSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    default:
                        releaseDoubleClickControl();
                        break;
                }
            }
            */
            else if (_state.context.targetSpawn.targeting == GameTargetingNone) {
                _state.context.targetSpawn.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseTargetSpawnState();
            _state.code = GameStateNone;
            break;
        case GameStateRangeSpawn:
            if (_state.context.rangeSpawn.targeting == GameTargetingScreen) {
                _state.context.rangeSpawn.targeting = GameTargetingReady;

                if (_state.context.rangeSpawn.destination->isLocated()) {
                    CommandReturn rtn = Map::sharedMap()->spawn(_state.context.rangeSpawn.slot, _state.context.rangeSpawn.destination, SpawnCommitRun);

                    if (rtn.state == CommandStateUnabled) {
                        showSpawnUnabled(getSpawn(_state.context.rangeSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    }
                }
                else {
                    Map::sharedMap()->camera()->stopScroll(true);
                    _mapScroll = CSVector2::Zero;
                    break;
                }
            }
            //오토타게팅제거 2022-07-11
            /*
            else if (Option::sharedOption()->autoTargetingSpawn() && _state.context.rangeSpawn.rtn.condition && commitDoubleClickControl(layer, DoubleClickAutoTargetDuration, _state.context.rangeSpawn.targeting == GameTargetingNone)) {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.rangeSpawn.slot, NULL, SpawnCommitRunAuto);

                switch (rtn.state) {
                    case CommandStateEnabled:
                        break;
                    case CommandStateUnabled:
                        showSpawnUnabled(getSpawn(_state.context.rangeSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                    default:
                        releaseDoubleClickControl();
                        break;
                }
            }
            */
            else if (_state.context.rangeSpawn.targeting == GameTargetingNone) {
                _state.context.rangeSpawn.targeting = GameTargetingReady;
                Map::sharedMap()->setLightState(MapLightStateOff);
                break;
            }
            releaseRangeSpawnState();
            _state.code = GameStateNone;
            break;
        case GameStateBuildSpawn:
            switch (_state.context.buildSpawn.targeting) {
                case GameTargetingScreen:
                    {
                        _state.context.buildSpawn.targeting = GameTargetingReady;

                        Frame* frame = _state.context.buildSpawn.frame;
                        
                        if (frame->isLocated()) {
                            const UnitData& data = frame->data();
                            
                            if (Map::sharedMap()->buildable(frame, data.type, frame->point(), data.collider, Map::sharedMap()->vision(), frame->target) != MapLocationUnabled) {
                                frame->setCollision(true);
                                
                                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.buildSpawn.slot, frame, SpawnCommitRun);
                                
                                if (rtn.state == CommandStateUnabled) {
                                    showSpawnUnabled(getSpawn(_state.context.buildSpawn.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                                }
                            }
                            else {
                                showSpawnUnabled(getSpawn(_state.context.buildSpawn.slot), MessageIndex(MessageSetCommand, MessageCommandBuildUnabled), AnnounceBuildUnabled);

                                frame->unlocate();
                                break;
                            }
                        }
                        else {
                            Map::sharedMap()->camera()->stopScroll(true);
                            _mapScroll = CSVector2::Zero;
                            break;
                        }
                    }
                case GameTargetingReady:
                    releaseBuildSpawnState();
                    _state.code = GameStateNone;
                    break;
                case GameTargetingNone:
                    _state.context.buildSpawn.targeting = GameTargetingReady;
                    //Map::sharedMap()->lightOff(true);
                    Map::sharedMap()->setLightState(_state.context.buildSpawn.target == CommandBuildTargetSpawn ? MapLightStateOffSpawn : MapLightStateOffGrid);
                    break;
            }
            break;
        case GameStateSpawn:
            {
                CommandReturn rtn = Map::sharedMap()->spawn(_state.context.spell.slot, NULL, SpawnCommitRun);
                if (rtn.state == CommandStateUnabled) {
                    showSpawnUnabled(getSpawn(_state.context.spell.slot), rtn.parameter.unabled.msg, rtn.parameter.unabled.announce);
                }
                _state.code = GameStateNone;
            }
            break;
            
    }
}
void GameLayer::onTouchesCancelledSpawn(CSLayer *layer) {
    switch (_state.code) {
        case GameStateTargetSpawn:
            releaseTargetSpawnState();
            _state.code = GameStateNone;
            break;
        case GameStateRangeSpawn:
            releaseRangeSpawnState();
            _state.code = GameStateNone;
            break;
        case GameStateBuildSpawn:
            releaseBuildSpawnState();
            _state.code = GameStateNone;
            break;
        case GameStateSpawn:
            _state.code = GameStateNone;
            break;
    }
}

void GameLayer::onDrawSpawn(CSLayer* layer, CSGraphics* graphics) {
    int slot = layer->tagAsInt();
    
    const Spawn* spawn = getSpawn(slot);
    
    if (spawn) {
        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSpawnStand_0), layer->bounds());

        graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSpawnStand_1), CSVector2(layer->center(), layer->height() - 50), CSAlignCenterMiddle);

        bool enabled;
        switch (_state.code) {
			case GameStateTargetCommand:
			case GameStateRangeCommand:
			case GameStateAngleCommand:
			case GameStateLineCommand:
			case GameStateBuildTargetCommand:
                enabled = false;
				break;
            case GameStateTargetSpawn:
                enabled = _state.context.targetSpawn.slot == slot;
                break;
            case GameStateRangeSpawn:
                enabled = _state.context.rangeSpawn.slot == slot;
                break;
            case GameStateBuildSpawn:
                enabled = _state.context.buildSpawn.slot == slot;
                break;
            default:
                enabled = true;
                break;
        }
            
        CommandReturn rtn = Map::sharedMap()->spawn(slot, NULL, SpawnCommitNone);
        
        float progress = 1;

        switch (rtn.state) {
            case CommandStateEmpty:
            case CommandStateUnabled:
            case CommandStateProduceUnabled:
            case CommandStateTrainUnabled:
            case CommandStateBuildUnabled:
                enabled = false;
                break;
            case CommandStateCooltime:
                progress = (rtn.parameter.cooltime.initial - rtn.parameter.cooltime.remaining) / rtn.parameter.cooltime.initial;
                if (!rtn.parameter.cooltime.enabled) enabled = false;
                break;
        }
        graphics->translate(CSVector2(layer->center(), layer->height() - 50));

        if (_spawnAnimations[slot]) {
            if (!enabled) graphics->setColor(CSColor::Gray);

            if (progress < 1) {
                graphics->push();
                graphics->setColor(CSColor::Gray);
                _spawnAnimations[slot]->drawLayered(graphics, SpawnAnimationKey, false);
                graphics->setColor(CSColor::White);

                graphics->setStencilMode(CSStencilInclusive);
                graphics->drawRect(CSRect(-55, -100, 110, 120 * progress), true);
                graphics->setStencilMode(CSStencilNone);

                _spawnAnimations[slot]->drawLayered(graphics, SpawnAnimationKey, false);
                graphics->pop();
            }
            else _spawnAnimations[slot]->drawLayered(graphics, SpawnAnimationKey, false);

            graphics->setColor(CSColor::White);
        }
        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceGage_0 + (enabled && progress >= 1)), CSRect(-50, 19, 100, 26));

        graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceIcon), CSVector2(-15, 32), CSAlignCenterMiddle);

        graphics->setFont(Asset::sharedAsset()->boldSmallFont);
        if (enabled) {
            graphics->setStrokeWidth(1);
        }
        else {
            graphics->setColor(CSColor::Orange);
        }
        graphics->drawDigit(spawn->data().resources[0], false, CSVector2(15, 29), CSAlignCenterMiddle);
    }
}

void GameLayer::onDrawControl(CSLayer* layer, CSGraphics* graphics) {
    float h = layer->height();

    graphics->setFont(Asset::sharedAsset()->boldSmallFont);

    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameControlBackground), CSRect(0, h - 164, layer->width(), 164));

    const Force* force = Map::sharedMap()->force();

    if (force) {
        graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCommander_0 + force->index), CSVector2(0, h), CSAlignBottom);

        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameControlBorder), CSRect(ProjectionBaseX + 240, h - 170, 475, 150));

        const Card* nextCard = force->nextCard();

        if (nextCard) {
            graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameControlBorder), CSRect(ProjectionBaseX + 143, h - 150, 90, 130));
            graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSpawnStand_0), CSRect(ProjectionBaseX + 148, h - 145, 80, 80));
            graphics->drawImageScaled(Asset::sharedAsset()->image(ImageSetGame, ImageGameSpawnStand_1), CSVector2(ProjectionBaseX + 178, h - 105), 0.75f, CSAlignCenterMiddle);

            if (_spawnAnimations[4]) _spawnAnimations[4]->drawLayered(graphics, CSVector2(ProjectionBaseX + 188, h - 105), SpawnAnimationKey, false);

            graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceGage_0), CSRect(ProjectionBaseX + 153, h - 99, 70, 30));

            graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceIcon), CSVector2(ProjectionBaseX + 175, h - 84), CSAlignCenterMiddle);
            graphics->setColor(CSColor::Orange);
            graphics->drawDigit(nextCard->data().resources[0], false, CSVector2(ProjectionBaseX + 201, h - 87), CSAlignCenterMiddle);
            graphics->setColor(CSColor::White);

            graphics->setStrokeWidth(1);
            graphics->drawString("NEXT", CSVector2(ProjectionBaseX + 188, h - 48), CSAlignCenterMiddle);
            graphics->setStrokeWidth(0);
        }

        int resource = force->resource(0);

        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceGage_0), CSRect(ProjectionBaseX + 253, h - 60, 82, 30));
        graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceIcon), CSVector2(ProjectionBaseX + 280, h - 45), CSAlignCenterMiddle);

        graphics->setColor(CSColor::Orange);
        graphics->drawDigit(resource, false, CSVector2(ProjectionBaseX + 308, h - 48), CSAlignCenterMiddle);
        graphics->setColor(CSColor::White);

        for (int i = 0; i < 10; i++) {
            float x = ProjectionBaseX + 340 + 37 * i;

            graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceGage_0), CSRect(x, h - 57, 35, 26));
            if (i < resource) {
                graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceIcon), CSVector2(x + 17, h - 45), CSAlignCenterMiddle);
            }
            else if (i == resource) {
                float rate = force->resourceChargeTime() / Asset::sharedAsset()->gamePreference.resourceChargeTime;

                if (rate) {
                    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameResourceIcon), CSRect(x + 6, h - 56, 22 * rate, 22), CSRect(0, 0, 22 * rate, 22));
                }
            }
        }
    }
}

void GameLayer::onDrawStatus(CSLayer* layer, CSGraphics* graphics) {            //TODO
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameTimeBorder), CSVector2(ProjectionWidth * 0.5f, 60), CSAlignCenter);

    graphics->drawImage(Asset::sharedAsset()->image(ImageIndex(1, 1, 2)), CSRect(ProjectionBaseX + 30, 60, 80, 80));            //TODO
    graphics->drawImage(Asset::sharedAsset()->image(ImageIndex(1, 1, 2)), CSRect(ProjectionWidth - ProjectionBaseX - 110, 60, 80, 80));            //TODO

    float playerRate, enemyRate;
    Map::sharedMap()->healthRate(playerRate, enemyRate);

    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameHealthBorder), CSVector2(ProjectionBaseX + 115, 60));
    if (playerRate) {
        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameHealthGage_0), CSRect(ProjectionBaseX + 123, 68, 134 * playerRate, 23));
    }

    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameHealthBorder), CSVector2(ProjectionWidth - ProjectionBaseX - 115 - 150, 60));
    if (playerRate) {
        float w = 134 * enemyRate;
        graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameHealthGage_1), CSRect(ProjectionWidth - ProjectionBaseX - 123 - w, 68, w, 23));
    }

    graphics->setFont(Asset::sharedAsset()->boldSmallFont);
    graphics->setStrokeWidth(2);
    graphics->drawString("Player", CSVector2(ProjectionBaseX + 120, 115), CSAlignMiddle);
    graphics->drawString("Enemy", CSVector2(ProjectionWidth - ProjectionBaseX - 120, 115), CSAlignRightMiddle);
    int sec = Map::sharedMap()->playTime();
    graphics->drawString(CSString::cstringWithFormat("%02d:%02d", sec / 60, sec % 60), CSVector2(ProjectionWidth * 0.5f, 92), CSAlignCenterMiddle);
    graphics->setStrokeWidth(0);
}

void GameLayer::onDrawOffscreen(CSLayer *layer, CSGraphics *graphics) {
    graphics->setColor(CSColor::TranslucentBlack);
    graphics->drawRoundRect(layer->bounds(), 10, true);
    graphics->setColor(CSColor::White);
    graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameLoading), CSVector2(layer->center(), 25), CSAlignCenterMiddle);
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameProgress_0), CSRect(10, 65, layer->width() - 20, 20));
    float w = (layer->width() - 24) * Map::sharedMap()->offscreenProgress();
    graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameProgress_1), CSRect(12, 67, w, 16));
}

void GameLayer::onDrawOffscreenCancel(CSLayer *layer, CSGraphics *graphics) {
    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameClose), CSVector3::Zero);
}

void GameLayer::onTouchesEndedOffscreenCancel(CSLayer *layer) {
    Sound::play(SoundControlEffect, SoundFxClick, SoundPlayEffect);

    switch (Map::sharedMap()->modeType()) {
        case GameModeTypeOnline:
            showExit();
            break;
        case GameModeTypeReplay:
            Map::sharedMap()->stopReplayProgress();
            break;
    }
}

void GameLayer::onDrawTicker(CSLayer* layer, CSGraphics* graphics) {
    graphics->setColor(CSColor::TranslucentBlack);
    graphics->drawRoundRect(CSRect(-10, -10, layer->width() + 20, layer->height() + 20), 10, true);
}

void GameLayer::showExit() {
	MessageLayer* popupLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetGame, Map::sharedMap()->isPlaying() ? MessageGameExit : MessageGameExitWatching), NULL, MessageLayerTypeConfirmCancel);
	popupLayer->OnConfirm.add([](CSLayer* layer, bool yes) {
		if (yes) {
			Map::sharedMap()->end();
		}
	});
	view()->addLayerAsPopup(popupLayer);
}

void GameLayer::reconnectFail(bool result) {
    MessageLayer* popupLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetGame, MessageGameReconnectFail));
    popupLayer->setOwner(this);
	popupLayer->setShouldConfirm(true);
	popupLayer->OnConfirm.add([this, result](CSLayer* layer, bool yes) {
        if (result) {
            Map::sharedMap()->end();        //방에 재입장할 수 없는 경우는 네트워크가 살아 있는 경우이므로 결과처리를 함
        }
        else {
            view()->clearLayers(false);
            view()->addLayer(LobbyLayer::layer());      //네트워크 접속이 해제된 경우, 결과처리도 못할 수 있으므로 로비로 보냄
        }
    });
    view()->addLayerAsPopup(popupLayer);
}

void GameLayer::onPVPShutdown() {
    if (Map::sharedMap()->modeType() == GameModeTypeOnline && _state.code < GameStateEnd) {
        //releaseControl();
        
        MessageLayer* reconnectLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetGame, MessageGameReconnect), NULL, MessageLayerTypeConfirmCancel);
        reconnectLayer->setOwner(this);
		reconnectLayer->setShouldConfirm(true);
        reconnectLayer->OnConfirm.add([this](CSLayer* layer, bool yes) {
            if (yes) {
                PVPConnectLayer* connectLayer = PVPConnectLayer::layerForMatch(Account::sharedAccount()->lastMatchId, Map::sharedMap()->onlineFrame(), true);
                connectLayer->setOwner(this);
                connectLayer->OnComplete.add([this](CSLayer* layer, PVPConnectResult result) {
                    if (result == PVPConnectResultFail) {
                        reconnectFail(true);
                    }
                });
                view()->addLayer(connectLayer);
            }
            else {
                reconnectFail(false);
            }
        });
        view()->addLayerAsPopup(reconnectLayer);
    }
}

void GameLayer::onPVPPartyChat(const User* user, const CSString* msg) {
    /*
    if (_observerChatLayer) {
        addObserverChat(chat);
    }
    if (!_observerChatLayer || !_observerChatOpened) {
        Map::sharedMap()->chat(NULL, chat->user, chat->message, false);
    }
    */
    Map::sharedMap()->chat(NULL, user, msg, false);
}

void GameLayer::onPVPMatchCommand(int framePerStep, uint frame, float serverDelay, const void* data, uint length) {
    if (Map::sharedMap()->modeType() == GameModeTypeOnline) {
        Map::sharedMap()->write(framePerStep, frame, serverDelay, data, length);
    }
}

void GameLayer::onPVPMatchChat(const User* user, const CSString* msg) {
    /*
    if (_observerChatLayer) {
        addObserverChat(chat);
    }
    if (!_observerChatLayer || !_observerChatOpened) {
        Map::sharedMap()->chat(NULL, chat->user, chat->message, false);
    }
    */
    Map::sharedMap()->chat(NULL, user, msg, false);
}

void GameLayer::onPVPMatchPause(const User* user, bool on) {
	Map::sharedMap()->pause(on);

	uint rgba = 0xFFFFFFFF;
	for (int i = 0; i < MaxPlayers; i++) {
		const Force* force = Map::sharedMap()->force(i);

		if (force && force->player->userId == user->userId) {
			rgba = (uint)force->originColor();
			break;
		}
	}
	CSString* str = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameOnlinePause_0 + on), rgba, (const char*)*user->name);

	Map::sharedMap()->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), str, true));
}

/*
void GameLayer::onLobbyGameResult(const RewardSet* rewards) {
    WaitingLayer::hide(this);

    switch (Map::sharedMap()->type()) {
		case GameTypeNormal:
			view()->addLayer(GameResultMatchLayer::layer(Map::sharedMap()->state() == MapStateVictory, rewards));
			break;
		case GameTypeFreeForAll:
            view()->addLayer(GameResultFreeForAllLayer::layer(rewards));
            break;
		case GameTypeTutorial:
            view()->addLayer(GameResultTutorialLayer::layer(rewards));
			break;
		default:
			returnToLobby(view(), rewards);
			break;
    }
}
*/

/*
void GameLayer::onMemorySecretError() {
	if (!_memorySecretError && Map::sharedMap()->syncUpdating()) {		//업데이트 중에 발생한 오류가 아니라면 무시, check-value 쌍은 낮은 확률로 메인쓰레드에서 불일치할 수 있다.
		_memorySecretError = true;

		if (CSThread::mainThread()->isActive()) {
			doMemorySecretError();
		}
		else {
			CSDelegate0<void>* inv = CSDelegate0<void>::delegate([this]() {
				doMemorySecretError();
			});
			CSThread::mainThread()->start(CSTask::task(inv));
		}
	}
}

void GameLayer::doMemorySecretError() {
    if (_state.code != GameStateCheat) {
        releaseState();
        //releaseControl();
        _state.code = GameStateCheat;
        
        if (Map::sharedMap()->modeType() == GameModeTypeOnline) {
            GameRecord::expireMatch();
            
            if (Account::sharedAccount()->match()) {
                PVPGateway::sharedGateway()->requestMatchExit();
            }
            if (Account::sharedAccount()->party()) {
                PVPGateway::sharedGateway()->requestPartyExit();
            }
        }
        LobbyGateway::sharedGateway()->requestCheatLog(CheatActionGameMemory);
        
        MessageLayer* layer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetGame, MessageGameCheat));
        layer->setOwner(this);
		layer->setShouldConfirm(true);
        layer->OnConfirm.add([this](CSLayer* layer, bool yes) {
            LobbyGateway::sharedGateway()->requestLogout();
            PVPGateway::sharedGateway()->requestShutdown(true);
            
            view()->clearLayers(false);
            view()->addLayer(TitleLayer::layer());
        });
        view()->addLayerAsPopup(layer);
    }
}
*/
