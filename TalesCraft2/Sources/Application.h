//
//  Application.h
//  TalesCraft2
//
//  Created by chan on 13. 4. 26..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Application_h
#define Application_h

#include "CDK.h"

#define ApplicationVersion      "0.0.1.0000"

#define AssetFileKey            "brtc"

#define BaseProjectionWidth     720
#define BaseProjectionHeight    1280

#define MaxAnimationWidth		960
#define MaxAnimationHeight		1560

extern float ProjectionWidth;
extern float ProjectionHeight;
extern float ProjectionBaseX;
extern float ProjectionBaseY;

#define TouchSensitivity		8

#define GameMinFramePerSecond       20
#define GameMaxFramePerSecond       40

#define GameReconnectWaitingTime    120

#define MaxPlayers              6
#define MaxAlliances            4

#define GameVersion             23
#define MinLoadRevision         71

#define SpawnSlots   			4
#define CardSlots               8
#define RuneTiers				3
#define RuneSlots               4
#define EmoticonSlots           6
#define MaxCardGrade	        3
#define MaxRuneGrade	        1

enum GameModeType : byte {
    GameModeTypeOnline,
    GameModeTypeReplay,
    GameModeTypeOffline
};
enum GameMode : byte {
    GameModeTestOnline = 0,     //online
    GameModeTestReplay = 20,     //replay
    GameModeTestOffline = 30,    //offline
    GameModeNone = 40
};
enum GameOnlineMode : byte {
    GameOnlineModeTest = GameModeTestOnline
};
enum GameOfflineMode : byte {
    GameOfflineModeTest = GameModeTestOffline
};
enum GameReplayMode : byte {
    GameReplayModeTest = GameModeTestReplay
};
enum GameType : byte {
    GameTypeNormal,
    GameTypeFreeForAll,
	GameTypeInstant,
	GameTypeTutorial
};
enum GameControl {
    GameControlNone,
    GameControlUser,
    GameControlAI_0,
	GameControlAI_1,
	GameControlAI_2,
	GameControlAI_3,
	GameControlAI_4,
	GameControlAITemporary,
    GameControlAuto
};
enum GameResult : byte {
    GameResultNone,
    GameResultVictory,
    GameResultDefeat,
    GameResultDraw
};
#define GameResultSkip	GameResultDraw

enum GameLevelCorrection {
    GameLevelCorrectionNone,
    GameLevelCorrectionMin,
    GameLevelCorrectionMax,
	GameLevelCorrectionAvg
};

#endif /* Application_h */
