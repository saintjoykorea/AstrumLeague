//
//  AssetDefinitionString.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 4. 6..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef AssetDefinitionString_h
#define AssetDefinitionString_h

enum MessageSet {
    MessageSetCommon,
    MessageSetError,
    MessageSetGame,
    MessageSetCommand
};

enum MessageCommon {
    MessageCommonConfirm,
    MessageCommonCancel,
    MessageCommonFinishApplication,
    MessageCommonTimeSecond,
    MessageCommonReconnect,
    MessageCommonDeny,
    MessageCommonRemove,
    MessageCommonSave,
    MessageCommonMove,
	MessageCommonLvDot,
};

enum MessageError {
    MessageErrorDataFail,
    MessageErrorNetworkFail,
    MessageErrorNetworkUnstable,
    MessageErrorNetworkReconnected,
    MessageErrorNetworkReconnectConfirm,
    MessageErrorNetworkUnstableMoveTitle
};

enum MessageGame {
    MessageGameCommander,
    MessageGameUnit,
    MessageGameCommunity,
    MessageGameShop,
    MessageGameCardsCollected,
    MessageGameBattleDeck,
    MessageGameStartButton,
    MessageGameLoading,
    MessageGameDestroy,
    MessageGameGiveup,
	MessageGameReturn,
    MessageGameExit,
    MessageGameExitWatching,
    MessageGameReconnect,
    MessageGameReconnectFail,
    MessageGameChatToTeam,
    MessageGameChatBlock_0,
    MessageGameChatBlock_1,
    MessageGameDragTarget,
    MessageGameCheat,
    MessageGameOnlineSync,
    MessageGameOnlineSyncWaiting,
	MessageGameOnlinePause_0,
	MessageGameOnlinePause_1,
	MessageGameOnlinePause_2,
	MessageGameUndetectedAttack,
    MessageGameDefensePower,
    MessageGameAttackPower,
    MessageGameAIPlayer,
    MessageGameCardLevelLimited,
    MessageGameRuneLevelLimited
};

enum MessageCommand {
    MessageCommandNotEnoughMineral,
    MessageCommandNotEnoughGas,
    MessageCommandNotEnoughSupply,
    MessageCommandBuildUnabled,
    MessageCommandMoveUnabled,
    MessageCommandSummonUnabled,
    MessageCommandAttackFail,
    MessageCommandAdvanceFail,
    MessageCommandRetreatFail,
    MessageCommandInvalidTarget,
    MessageCommandProduceFull,
    MessageCommandCancelBuilding,
    MessageCommandDeactivated,
    MessageCommandTrainingComplete
};

#endif /* AssetDefinitionString_h */
