//
//  GameLayer.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 6. 13..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef GameLayer_h
#define GameLayer_h

#include "Map.h"
#include "Frame.h"
#include "Destination.h"

#include "PVPConnectLayer.h"

enum GameState : byte {
    GameStateNone,
    GameStateScroll,
    GameStateZoom,
    GameStateCommand,
    GameStateTargetCommand,
    GameStateRangeCommand,
    GameStateAngleCommand,
    GameStateLineCommand,
    GameStateProduceCommand,
    GameStateCancelProducingCommand,
    GameStateTrainCommand,
    GameStateBuildCommand,
    GameStateBuildTargetCommand,
    GameStateSpawn,
    GameStateTargetSpawn,
    GameStateRangeSpawn,
    GameStateBuildSpawn,
    GameStateMenu,
    GameStateAuto,
    GameStateSpeech,
    GameStateOffscreen,
	GameStateEnd,
    GameStateCheat,
    GameStateDisposed
};
enum GameSelection : byte {
    GameSelectionNormal,
    GameSelectionAttackers,
    GameSelectionBarracks,
    GameSelectionLabors
};
enum GameTargeting : byte {
    GameTargetingNone,
    GameTargetingReady,
    GameTargetingScreen
};

class GameLayer : public CSLayer, public PVPGatewayDelegate {
private:
    struct {
        GameState code;

		union _context {
            struct {
                CommandIndex index;
                bool all;
            } command;
            struct {
                CommandIndex index;
                GameTargeting targeting;
                CommandReturnParameterTarget rtn;
                CSVector2 point;
                CSArray<Destination>* destinations;
                CSArray<Effect>* effects;
                const CSArray<Unit>* units;
				const Object* target;
            } targetCommand;
            struct {
                CommandIndex index;
                GameTargeting targeting;
                CommandReturnParameterRange rtn;
                CSVector2 point;
                CSArray<Destination>* destinations;
                CSArray<Effect>* effects;
                const CSArray<Unit>* units;
            } rangeCommand;
            struct {
                CommandIndex index;
                GameTargeting targeting;
                CommandReturnParameterAngle rtn;
                CSVector2 point;
                Destination* destination;
                CSArray<Effect>* effects;
                const CSArray<Unit>* units;
            } angleCommand;
            struct {
                CommandIndex index;
                GameTargeting targeting;
                CommandReturnParameterLine rtn;
                CSVector2 point;
                Destination* destination;
                CSArray<Effect>* effects;
                const CSArray<Unit>* units;
            } lineCommand;
            struct {
                CommandIndex index;
                UnitIndex unit;
                byte seq;
            } produceCommand;
            struct {
                UnitIndex unit;
                byte seq;
            } cancelProducingCommand;
            struct {
                CommandIndex index;
                RuneIndex rune;
				CSLayer* confirmLayer;
                byte seq;
            } trainCommand;
            struct {
                CommandIndex index;
                UnitIndex unit;
            } buildCommand;
            struct {
                CommandIndex index;
                GameTargeting targeting;
                UnitIndex unit;
                CommandBuildTarget target;
                Frame* frame;
                Effect* effect;
            } buildTargetCommand;
            struct {
                byte slot;
            } spell;
            struct {
                byte slot;
                GameTargeting targeting;
                CommandReturnParameterTarget rtn;
                CSVector2 point;
                Destination* destination;
                CSArray<Effect>* effects;
				const Object* target;
            } targetSpawn;
            struct {
                byte slot;
                GameTargeting targeting;
                CommandReturnParameterRange rtn;
                CSVector2 point;
                Destination* destination;
                CSArray<Effect>* effects;
            } rangeSpawn;
            struct {
                byte slot;
                GameTargeting targeting;
                CommandBuildTarget target;
                Frame* frame;
                Effect* effect;
            } buildSpawn;
            struct {
                const Speech* speech;
                ushort seq;
            } speech;
            
            _context() {}
        } context;
    } _state;
    
	CSVector2 _mapScroll;
	GameSelection _selection;
	byte _wheelWaiting;
    Animation* _spawnAnimations[5];
    CSLayer* _controlLayer;
    CSLayer* _statusLayer;
    //CSArray<CSLayer>* _commandLayers;
    CSArray<CSLayer>* _spawnLayers;
    //CSArray<CSLayer>* _emoticonLayers;
    //CSLayer* _menuOpenLayer;
    //CSLayer* _menuLayer;
    //CSTextField* _chatTextField;
    //CSButton* _chatSendButton;
    //CSLayer* _speechLayer;
    //CSLayer* _autoLayer;
    //CSLayer* _speedLayer;
    CSLayer* _offscreenLayer;
    CSTicker* _ticker;
    CSLayer* _touchCoverLayer;

    static GameLayer* __layer;
private:
    GameLayer();
    ~GameLayer();
    
    void dispose();
public:     //nullable
    static GameLayer* layerForOfflinePlay(GameOfflineMode mode, const MapIndex& index, const Player* player, const Player* otherPlayer, int ai);
    static GameLayer* layerForOnlinePlay(GameOnlineMode mode, bool resume);
    static GameLayer* layerForReplay(GameReplayMode mode, const char* subpath);
private:
    void onLink() override;
    void onUnlink() override;
    void onStateChanged() override;

    void releaseTargetCommandState();
    void releaseRangeCommandState();
    void releaseAngleCommandState();
    void releaseLineCommandState();
	void releaseTrainCommandState();
    void releaseBuildTargetCommandState();
    void releaseTargetSpawnState();
    void releaseRangeSpawnState();
    void releaseBuildSpawnState();
    void releaseMenuState();
    void releaseSpeechState();
    void releaseState();
    
    bool updatePlaySpeech();
    void updatePlay();
    void updateState();
    CSLayer* createCommandLayer(const CSRect& frame, const CommandIndex& index);
    CSLayer* getCommandLayer(const CommandIndex& index);
    void updateDisplayCommand(CSLayer* layer, const CommandReturn& rtn);
    CSLayer* createSpawnLayer(const CSRect& frame, int slot);
    CSLayer* getSpawnLayer(int slot);
	void updateDisplayMessages(const CSRect& frame);
    void updateDisplayMissions();
    void updateDisplay();
    bool checkTargetScroll(const CSVector2& point);

    void onTimeout() override;
    void onDraw(CSGraphics* graphics) override;
    void onDrawCover(CSGraphics* graphics) override;
    void onTouchesBegan() override;
    void onTouchesMoved() override;
    void onTouchesEnded() override;
    void onTouchesCancelled() override;
	void onWheel(float offset) override;
	void onKeyDown(int keyCode) override;
	void onKeyUp(int keyCode) override;
    void onBackKey() override;

	void onKeyCommand(int i);
	void onKeySpawn(int i);

    bool layerContains(CSLayer* layer, const CSVector2& p);

    bool doCommand(const CommandIndex& index);

    void showCommandUnabled(const CommandIndex& index, const MessageIndex& msg, AnnounceIndex announce);

	void onTouchesBeganCommand(CSLayer* layer);
    void onTouchesMovedCommand(CSLayer* layer);
    void onTouchesEndedCommand(CSLayer* layer);
    void onTouchesCancelledCommand(CSLayer* layer);
    void onDrawCommand(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesBeganCancelCommand(CSLayer* layer);
    void onTouchesEndedCancelCommand(CSLayer* layer);
    void onTouchesCancelledCancelCommand(CSLayer* layer);
    void onDrawCancelCommand(CSLayer* layer, CSGraphics* graphics);
    
    void showSpawnUnabled(const Spawn* spawn, const MessageIndex& msg, AnnounceIndex announce);

	void onTouchesBeganSpawn(CSLayer* layer);
    void onTouchesMovedSpawn(CSLayer* layer);
    void onTouchesEndedSpawn(CSLayer* layer);
    void onTouchesCancelledSpawn(CSLayer* layer);
    void onDrawSpawn(CSLayer* layer, CSGraphics* graphics);
    
    void onDrawControl(CSLayer* layer, CSGraphics* graphics);
    void onDrawStatus(CSLayer* layer, CSGraphics* graphics);
    /*
    void onTimeoutMessage(CSLayer* layer);
    void onDrawMessage(CSLayer* layer, CSGraphics* graphics);
    void onDrawEmoticonAnimation(CSLayer* layer, CSGraphics* graphics);
    void onDrawMission(CSLayer* layer, CSGraphics* graphics);
    
    void onDrawStatus(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesBeganMenuOpen(CSLayer* layer);
    void onTouchesEndedMenuOpen(CSLayer* layer);
    void onDrawMenuOpen(CSLayer* layer, CSGraphics* graphics);
    void onDrawMenu(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesEndedChatBlock(CSLayer* layer);
    void onDrawChatBlock(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedEmoticon(CSLayer* layer);
    void onDrawEmoticon(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedChatTarget(CSLayer* layer);
    void onDrawChatTarget(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesEndedChatSend(CSLayer* layer);
    void onDrawChatSend(CSLayer* layer, CSGraphics* graphics);
    
    void onTextReturnChat(CSTextField* textField);
    
    void onTouchesEndedOptionOpen(CSLayer* layer);
    void onUnlinkOption(CSLayer* layer);
    void onDrawOptionOpen(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedExitOpen(CSLayer* layer);
    void onDrawExitOpen(CSLayer* layer, CSGraphics* graphics);
#if defined(UseTeamSpeak) || defined(UseTCGME)
    void onTouchesEndedVoiceChat(CSLayer* layer);
    void onDrawVoiceChat(CSLayer* layer, CSGraphics* graphics);
#endif
    void onDrawSpeech(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesEndedAuto(CSLayer* layer);
    void onDrawAuto(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesEndedSpeed(CSLayer* layer);
    void onDrawSpeed(CSLayer* layer, CSGraphics* graphics);
    
    void onTouchesEndedVision(CSLayer* layer);
    void onDrawVision(CSLayer* layer, CSGraphics* graphics);
    */
    
    void onDrawOffscreen(CSLayer* layer, CSGraphics* graphics);
    void onDrawOffscreenCancel(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedOffscreenCancel(CSLayer* layer);
    
    void onDrawTicker(CSLayer* layer, CSGraphics* graphics);
    
    void showExit();
    
    void reconnectFail(bool result);
    
    void onPVPShutdown() override;
    void onPVPPartyChat(const User* user, const CSString* msg) override;
    void onPVPMatchCommand(int framePerStep, uint frame, float serverDelay, const void* data, uint length) override;
    void onPVPMatchChat(const User* user, const CSString* msg) override;
	void onPVPMatchPause(const User* user, bool on) override;

    //void onMemorySecretError();
	//void doMemorySecretError();
};

#endif /* GameLayer_h */
