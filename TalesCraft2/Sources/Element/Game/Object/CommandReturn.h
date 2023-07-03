//
//  CommandReturn.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 5. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef CommandReturn_h
#define CommandReturn_h

#include "Asset.h"

enum CommandState : byte {
    CommandStateEmpty,
    CommandStateCooltime,
    CommandStateUnabled,
    CommandStateTarget,
    CommandStateRange,
    CommandStateAngle,
    CommandStateLine,
    CommandStateProduceUnabled,
    CommandStateProduce,
    CommandStateTrainUnabled,
    CommandStateTrain,
	CommandStateTrainComplete,
    CommandStateBuildUnabled,
    CommandStateBuild,
    CommandStateRetry,
    CommandStateEnabled
};

enum CommandEffect : byte {
    CommandEffectLine_0 = 1,
    CommandEffectLine_1 = 2,
    CommandEffectLine_2 = 4,
    CommandEffectLine_3 = 8,
    CommandEffectStroke = 16,
    CommandEffectBlink = 32
};

struct CommandReturnParameterTarget {
    float distance;
    const byte* condition;
	AnimationIndex destAnimation;
	AnimationIndex sourceAnimation;
    byte effect;
    bool all;
    
    bool operator==(const CommandReturnParameterTarget& other) const;
    
    inline bool operator!=(const CommandReturnParameterTarget& other) const {
        return !(*this == other);
    }
};

struct CommandReturnParameterRange {
    float distance;
    const byte* condition;
	float range;
	AnimationIndex destAnimation;
	AnimationIndex sourceAnimation;
    byte effect;
    bool all;
    
    bool operator==(const CommandReturnParameterRange& other) const;
    
    inline bool operator!=(const CommandReturnParameterRange& other) const {
        return !(*this == other);
    }
    
};

struct CommandReturnParameterAngle {
    const byte* condition;
	float range;
    float angle;
	AnimationIndex destAnimation;
	AnimationIndex sourceAnimation;
    byte effect;
    bool all;
    
    bool operator==(const CommandReturnParameterAngle& other) const;
    
    inline bool operator!=(const CommandReturnParameterAngle& other) const {
        return !(*this == other);
    }
    
};

enum CommandBuildTarget : byte {
    CommandBuildTargetNone,
    CommandBuildTargetAll,
    CommandBuildTargetVisible,
    CommandBuildTargetSpawn
};

struct CommandReturnParameterLine {
    const byte* condition;
	float range;
    float thickness;
	AnimationIndex destAnimation;
	AnimationIndex sourceAnimation;
    byte effect;
    bool all;
    
    bool operator==(const CommandReturnParameterLine& other) const;
    
    inline bool operator!=(const CommandReturnParameterLine& other) const {
        return !(*this == other);
    }
};

enum CommandStack : byte {
	CommandStackEnabled,
	CommandStackSingle,
	CommandStackRewind
};

enum CommandCommit {
	CommandCommitNone,
	CommandCommitOnce,
	CommandCommitRun,
	CommandCommitStack
};

struct CommandReturn {
    CommandState state;
    union _parameter {
        struct {
            MessageIndex msg;
			AnnounceIndex announce;
        } unabled;
        struct {
            float initial;
            float remaining;
            bool enabled;
        } cooltime;
        CommandReturnParameterTarget target;
        CommandReturnParameterRange range;
        CommandReturnParameterAngle angle;
        CommandReturnParameterLine line;
        struct {
            UnitIndex index;
            MessageIndex msg;
			AnnounceIndex announce;
        } produce;
        struct {
            RuneIndex index;
            MessageIndex msg;
			AnnounceIndex announce;
			float progress;
        } train;
        struct {
            UnitIndex index;
            MessageIndex msg;
			AnnounceIndex announce;
            CommandBuildTarget target;
        } build;
        struct {
            int remaining;
        } retry;
        struct {
			CommandStack stack;
            bool all;
        } enabled;
        
        inline _parameter() {
        
        }
    } parameter;
    
    bool operator==(const CommandReturn& other) const;
    
    inline bool operator!=(const CommandReturn& other) const {
        return !(*this == other);
    }
};


#endif /* CommandReturn_h */
