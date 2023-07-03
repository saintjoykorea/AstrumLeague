//
//  GameConfig.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 4. 18..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef GameConfig_h
#define GameConfig_h

//#define UseCheatResource
//#define UseCheatProgress	5
//#define UseCheatDeck
//#define UseCheatInvincible

#define UseNoVision

//#define UseAlwaysWin
//#define UseFullAIPlayersTest
//#define UseObserverPlayTest
//#define UseMoveTest
//#define UsePathTest
//#define UseLeakTest
//#define UseSyncTest
//#define UseLoggerPath	"debug.txt"
//#define UsePVPTest
//#define UseBenchMarkTest
//#define UseAutoPerformanceTest
//#define UseMonitorStatusAlways
//#define UseMonitorStatusMenu

//#define UseCommandTriggerTest
//#define UseActionTriggerTest
//#define UseBuffTriggerTest
//#define UseSpellTriggerTest
//#define UseMapTriggerTest
//#define UseAITriggerTest

//#define UseVoiceChatToAll
//#define UseDynamicSecretTable

#ifndef DEBUG

#ifdef UseCheatResource
#pragma message("UseCheatResource is on on release build")
#endif

#ifdef UseCheatProgress
#pragma message("UseCheatProgress is on on release build")
#endif

#ifdef UseCheatDeck
#pragma message("UseCheatDeck is on on release build")
#endif

#ifdef UseNoVision
#pragma message("UseNoVision is on on release build")
#endif

#ifdef UseSingleTest
#pragma message("UseSingleTest is on on release build")
#endif

#ifdef UseAlwaysWin
#pragma message("UseAlwaysWin is on on release build")
#endif

#ifdef UseFullAIPlayersTest
#pragma message("UseFullAIPlayersTest is on on release build")
#endif

#ifdef UseObserverPlayTest
#pragma message("UseObserverPlayTest is on on release build")
#endif

#ifdef UseMoveTest
#pragma message("UseMoveTest is on on release build")
#endif

#ifdef UsePathTest
#pragma message("UsePathTest is on on release build")
#endif

#ifdef UseLeakTest
#pragma message("UseLeakTest is on on release build")
#endif

#ifdef UseSyncTest
#pragma message("UseSyncTest is on on release build")
#endif

#ifdef UseLoggerPath
#pragma message("UseLoggerPath is on on release build")
#endif

#ifdef UsePVPTest
#pragma message("UsePVPTest is on on release build")
#endif

#ifdef UseMonitorStatus
#pragma message("UseMonitorStatus is on on release build")
#endif

#ifdef UseCommandTriggerTest
#pragma message("UseCommandTriggerTest is on on release build")
#endif

#ifdef UseActionTriggerTest
#pragma message("UseActionTriggerTest is on on release build")
#endif

#ifdef UseBuffTriggerTest
#pragma message("UseBuffTriggerTest is on on release build")
#endif

#ifdef UseSpellTriggerTest
#pragma message("UseSpellTriggerTest is on on release build")
#endif

#ifdef UseMapTriggerTest
#pragma message("UseMapTriggerTest is on on release build")
#endif

#ifdef UseAITriggerTest
#pragma message("UseAITriggerTest is on on release build")
#endif

#endif

#endif /* GameConfig_h */
