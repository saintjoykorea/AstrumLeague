//
//  AssetDefinitionAnimation.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 4. 6..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef AssetDefinitionAnimation_h
#define AssetDefinitionAnimation_h

#include "AssetIndex.h"

extern const AnimationGroupIndex AnimationSetCommon;

enum AnimationCommon {
    AnimationCommonLoading,
	AnimationCommonTitle
};

extern const AnimationGroupIndex AnimationSetGame;

enum AnimationGame {
    AnimationGameTargetCommand_0,
    AnimationGameTargetCommand_1,
    AnimationGameRallyCommand,
    AnimationGameMinimapEvent,
    AnimationGameDistortion,
	AnimationGamePing_0,
	AnimationGamePing_1,
	AnimationGamePing_2,
	AnimationGamePing_3,
	AnimationGameClouds_0,
	AnimationGameClouds_1,
	AnimationGameMatch,
	AnimationGameWin,
	AnimationGameLose,
	AnimationGameLobby,
	AnimationGameBattleButton
};

#endif /* AssetDefinitionAnimation_h */
