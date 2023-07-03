//
//  CommandReturn.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 5. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "CommandReturn.h"

#include "UnitBase.h"

bool CommandReturnParameterTarget::operator==(const CommandReturnParameterTarget& other) const {
    if (distance != other.distance || destAnimation != other.destAnimation || sourceAnimation != other.sourceAnimation || effect != other.effect || all != other.all) {
        return false;
    }
    if (other.condition) {
        if (condition) {
            int len0 = UnitBase::matchLength(condition);
            int len1 = UnitBase::matchLength(other.condition);
            return len0 == len1 && memcmp(condition, other.condition, len0) == 0;
        }
        else {
            return false;
        }
    }
    else {
        return other.condition == NULL;
    }
}

bool CommandReturnParameterRange::operator==(const CommandReturnParameterRange& other) const {
    if (distance != other.distance || range != other.range || destAnimation != other.destAnimation || sourceAnimation != other.sourceAnimation || effect != other.effect || all != other.all) {
        return false;
    }
    if (other.condition) {
        if (condition) {
            int len0 = UnitBase::matchLength(condition);
            int len1 = UnitBase::matchLength(other.condition);
            return len0 == len1 && memcmp(condition, other.condition, len0) == 0;
        }
        else {
            return false;
        }
    }
    else {
        return other.condition == NULL;
    }
}

bool CommandReturnParameterAngle::operator==(const CommandReturnParameterAngle& other) const {
    if (range != other.range || angle != other.angle || destAnimation != other.destAnimation || sourceAnimation != other.sourceAnimation || effect != other.effect || all != other.all) {
        return false;
    }
    if (other.condition) {
        if (condition) {
            int len0 = UnitBase::matchLength(condition);
            int len1 = UnitBase::matchLength(other.condition);
            return len0 == len1 && memcmp(condition, other.condition, len0) == 0;
        }
        else {
            return false;
        }
    }
    else {
        return other.condition == NULL;
    }
}

bool CommandReturnParameterLine::operator==(const CommandReturnParameterLine& other) const {
    if (range != other.range || thickness != other.thickness || destAnimation != other.destAnimation || sourceAnimation != other.sourceAnimation || effect != other.effect || all != other.all) {
        return false;
    }
    if (other.condition) {
        if (condition) {
            int len0 = UnitBase::matchLength(condition);
            int len1 = UnitBase::matchLength(other.condition);
            return len0 == len1 && memcmp(condition, other.condition, len0) == 0;
        }
        else {
            return false;
        }
    }
    else {
        return other.condition == NULL;
    }
}

bool CommandReturn::operator==(const CommandReturn& other) const {
    if (state == other.state) {
        switch (state) {
            case CommandStateEmpty:
            case CommandStateRetry:
            case CommandStateCooltime:
                return true;
            case CommandStateUnabled:
                return parameter.unabled.msg == other.parameter.unabled.msg && parameter.unabled.announce == other.parameter.unabled.announce;
            case CommandStateTarget:
                return parameter.target == other.parameter.target;
            case CommandStateRange:
                return parameter.range == other.parameter.range;
            case CommandStateAngle:
                return parameter.angle == other.parameter.angle;
            case CommandStateLine:
                return parameter.line == other.parameter.line;
            case CommandStateProduceUnabled:
                return parameter.produce.index == other.parameter.produce.index && parameter.produce.msg == other.parameter.produce.msg && parameter.produce.announce == other.parameter.produce.announce;
            case CommandStateProduce:
                return parameter.produce.index == other.parameter.produce.index;
            case CommandStateTrainUnabled:
                return parameter.train.index == other.parameter.train.index && parameter.train.msg == other.parameter.train.msg && parameter.train.announce == other.parameter.train.announce;
            case CommandStateTrain:
                return parameter.train.index == other.parameter.train.index;
            case CommandStateBuildUnabled:
                return parameter.build.index == other.parameter.build.index && parameter.build.msg == other.parameter.build.msg && parameter.build.target == other.parameter.build.target && parameter.build.announce == other.parameter.build.announce;
            case CommandStateBuild:
                return parameter.build.index == other.parameter.build.index && parameter.build.target == other.parameter.build.target;
            case CommandStateEnabled:
                return parameter.enabled.stack == other.parameter.enabled.stack && parameter.enabled.all == other.parameter.enabled.all;
        }
    }
    return false;
}
