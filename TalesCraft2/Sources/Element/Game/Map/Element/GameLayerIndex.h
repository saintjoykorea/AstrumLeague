//
//  GameLayerIndex.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 10. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef GameLayerIndex_h
#define GameLayerIndex_h

#include "AssetIndex.h"

//TODO

enum GameLayerKey {
    GameLayerKeyMap,
    GameLayerKeyScreen,
    GameLayerKeyControl,
    GameLayerKeyNavigation,
    GameLayerKeySelectionTab,
    GameLayerKeySelectedUnit,
    GameLayerKeySelectedTarget,
    GameLayerKeySelectAttackers,
    GameLayerKeySelectBarracks,
    GameLayerKeySelectLabors,
    GameLayerKeyNumbering,
    GameLayerKeySetNumbering,
    GameLayerKeyMainCommand,
	GameLayerKeySubCommand,
    GameLayerKeyGroupCommand,
    GameLayerKeySpawn,
    GameLayerKeyStatus,
    GameLayerKeyTimer,
    GameLayerKeyMenu,
    GameLayerKeyAuto,
    GameLayerKeySpeed,
    GameLayerKeyMission,
    GameLayerKeyMessage,
    GameLayerKeyProducing,
    GameLayerKeyTraining,
    GameLayerKeyTransporting,
    GameLayerKeyWarning,
    GameLayerKeyMinimapExpand
};

typedef AssetIndex2 GameLayerIndex;

#endif /* GameLayerIndex_h */
