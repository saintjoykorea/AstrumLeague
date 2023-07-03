//
//  Map+AI.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 25..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "VirtualUnit.h"
#include "Destination.h"
#include "Frame.h"

/*
#define AILogForceIndex 0

#ifdef UseAITriggerTest
# ifdef AILogForceIndex
#  define AILog(...);  if (force->index == AILogForceIndex) CSLog(__VA_ARGS__);
# else
#  define AILog(...);  CSLog(__VA_ARGS__);
# endif
#define AIStopWatchStart()  CSStopWatchStart()
#define AIStopWatchElapsed(...)  CSStopWatchElapsed(__VA_ARGS__)
#else
#define AILog(...);
#define AIStopWatchStart();
#define AIStopWatchElapsed(...);
#endif

#define AIUnitValuationTime     30

static const fixed AIMinUnitValuationRateByResource = (fixed)10 / 100;

static const fixed AIAttackDetectorDecreaseRate = (fixed)50 / 100;

#define AIWaitResourcePriorityProduce   0
#define AIWaitResourcePrioritySpawn     1
#define AIWaitResourcePriorityTraining  2
#define AIWaitResourcePriorityBuild     3

void Map::atCall(Force* force, AI::Thread* thread, const byte*& cursor) {
    int func;
    if (readBoolean(cursor)) {
        func = force->ai()->value(readShort(cursor));
        CSAssert(func >= 0 && func < Asset::sharedAsset()->aiTriggers->count(), "invalid operation");
    }
    else {
        func = readShort(cursor);
    }
    bool isNew = readBoolean(cursor);
    
    thread->current->ip++;
    
    if (isNew) {
        force->ai()->threads->addObject(new AI::Thread(++force->ai()->threadId, func, thread->current->area));
    }
    else {
        AI::Thread::Stack* stack = new AI::Thread::Stack(func, thread->current->area);
        stack->prev = thread->current;
        thread->current = stack;
    }
}

const CSArray<Force>* Map::atValueForces(Force* force, const byte*& cursor) {
    int alliance = readByte(cursor);
    
    CSArray<Force>* targetForces = NULL;
    
    switch (alliance) {
        case 0:
            targetForces = CSArray<Force>::arrayWithObjects(&force, 1);
            break;
        case 1:
            targetForces = CSArray<Force>::arrayWithCapacity(MaxPlayers);
            for (int i = 0; i < MaxPlayers; i++) {
                if (_forces[i] && _forces[i]->alliance() == force->alliance()) {
                    targetForces->addObject(_forces[i]);
                }
            }
            break;
        case 2:
            targetForces = CSArray<Force>::arrayWithCapacity(MaxPlayers);
            for (int i = 0; i < MaxPlayers; i++) {
                if (_forces[i] && _forces[i]->alliance() != force->alliance()) {
                    targetForces->addObject(_forces[i]);
                }
            }
            break;
        case 3:
            targetForces = CSArray<Force>::arrayWithCapacity(MaxPlayers);
            for (int i = 0; i < MaxPlayers; i++) {
                if (_forces[i]) {
                    targetForces->addObject(_forces[i]);
                }
            }
            break;
    }
    return targetForces;
}

fixed Map::atValueUnitLevel(Force* force, const byte*& cursor) {
    const CSArray<Force>* targetForces = atValueForces(force, cursor);
    UnitIndex index(cursor);
    
    int level = 0;
    foreach(const Force*, targetForce, targetForces) {
        int l = targetForce->unitLevel(index);
        if (l > level) {
            level = l;
        }
    }
    if (readBoolean(cursor)) {
        int maxLevel = Asset::sharedAsset()->gamePreference.maxCardLevel;
        
        return (fixed)level / maxLevel;
    }
    else {
        return (fixed)level;
    }
}

fixed Map::atValueRuneLevel(Force* force, const byte*& cursor) {
    const CSArray<Force>* targetForces = atValueForces(force, cursor);
    RuneIndex index(cursor);
    
    int level = 0;
    foreach(const Force*, targetForce, targetForces) {
        const Rune* rune = targetForce->runeForIndex(index);
        if (rune && rune->level > level) {
            level = rune->level;
        }
    }
    return readBoolean(cursor) ? (fixed)level / Asset::sharedAsset()->gamePreference.maxRuneLevel : (fixed)level;
}

int Map::atValueUnitCount(Force* force, const byte*& cursor) {
    const byte* condition = cursor;
    cursor += UnitBase::matchLength(cursor);
    int count = 0;
    foreach(const Unit*, unit, _units) {
        if (unit->match(force, condition)) {
            count++;
        }
    }
    return count;
}

int Map::atValueAreaUnitCount(Force* force, const Area* area, const byte*& cursor) {
    const byte* condition = cursor;
    cursor += UnitBase::matchLength(cursor);
    int count = 0;
    if (area) {
        foreach(const Unit*, unit, _units) {
            if (unit->isLocated() && area->contains(unit->point()) && unit->match(force, condition)) {
                count++;
            }
        }
    }
    return count;
}

fixed Map::atValueResource(Force* force, const byte*& cursor) {
    const CSArray<Force>* targetForces = atValueForces(force, cursor);
    int kindMask = readByte(cursor);
    bool gathering = readBoolean(cursor);
    fixed amount = fixed::Zero;
    
    foreach(const Force*, targetForce, targetForces) {
        if (gathering) {
            if (kindMask & 1) {
                amount += targetForce->gathering(0);
            }
            if (kindMask & 2) {
                amount += targetForce->gathering(1);
            }
        }
        else {
            if (kindMask & 1) {
                amount += targetForce->resource(0);
            }
            if (kindMask & 2) {
                amount += targetForce->resource(1);
            }
        }
    }
    return amount;
}

fixed Map::atValueAreaResource(Force* force, const Area* area, const byte*& cursor) {
    static const uint resourceMasks[] = {
        ObjectMaskMineral,
        ObjectMaskGas,
        ObjectMaskResource
    };
    uint resourceMask = resourceMasks[readByte(cursor)];
    int type = readByte(cursor);
    
    if (!area) {
        return fixed::Zero;
    }
    fixed f0 = fixed::Zero;
    fixed f1 = fixed::Zero;
    if (area) {
        foreach(const Resource*, resource, _resources) {
            if (resource->compareType(resourceMask) && resource->isLocated() && area->contains(resource->point())) {
                switch (type) {
                    case 0:
                        f0 += fixed::One;
                        break;
                    case 1:
                        f0 += resource->remaining();
                        break;
                    case 3:
                        f1 += resource->data().gatherings;
                    case 2:
                        f0 += resource->gatherings();
                        break;
                }
            }
        }
    }
    
    return type == 3 ? (f1 ? f0 / f1 : fixed::One) : f0;
}

fixed Map::atValueUnitChannelResourceRate(Force* force, const byte*& cursor) {
    UnitIndex index(cursor);
    UnitIndex source(cursor);
    int kindMask = readByte(cursor);
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    int channelCount = 0;
    foreach(const Unit*, unit, _units) {
        if (unit->force() == force && unit->index() == source) {
            channelCount++;
        }
    }
    
    fixed rate = fixed::Zero;
    
    if (channelCount) {
        if ((kindMask & 1) && data.resources[0]) {
            fixed resource = force->resource(0) + force->gathering(0) * data.progress;
            
            rate = resource ? CSMath::min(data.resources[0] * channelCount / resource, fixed::One) : fixed::One;
        }
        if ((kindMask & 2) && data.resources[1]) {
            fixed resource = force->resource(1) + force->gathering(1) * data.progress;
            
            rate = resource ? CSMath::clamp(data.resources[1] * channelCount / resource, rate, fixed::One) : fixed::One;
        }
    }
    return rate;
}

fixed Map::atValueUnitResource(Force* force, const byte*& cursor) {
    UnitIndex index(cursor);
    int kindMask = readByte(cursor);
    bool rating = readBoolean(cursor);
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    if (rating) {
        fixed rate = fixed::Zero;
        if ((kindMask & 1) && data.resources[0]) {
            fixed resource = force->resource(0) + force->gathering(0) * data.progress;
            
            rate = resource ? CSMath::min(data.resources[0] / resource, fixed::One) : fixed::One;
        }
        if ((kindMask & 2) && data.resources[1]) {
            fixed resource = force->resource(1) + force->gathering(1) * data.progress;
            
            rate = resource ? CSMath::clamp(data.resources[1] / resource, rate, fixed::One) : fixed::One;
        }
        return rate;
    }
    else {
        int amount = 0;
        if (kindMask & 1) {
            amount += data.resources[0];
        }
        if (kindMask & 2) {
            amount += data.resources[1];
        }
        return (fixed)amount;
    }
}

fixed Map::atValueTrainingResource(Force* force, const byte*& cursor) {
    RuneIndex index(cursor);
    int kindMask = readByte(cursor);
    bool rating = readBoolean(cursor);
    
    const RuneData& data = Asset::sharedAsset()->rune(index);

    if (rating) {
        fixed rate = fixed::One;
        if ((kindMask & 1) && data.resources[0] && force->resource(0) < data.resources[0]) {
            if (!force->gathering(0)) {
                rate = fixed::Zero;
            }
            else {
                rate = CSMath::min((force->resource(0) + force->gathering(0)) / data.resources[0], fixed::One);
            }
        }
        if ((kindMask & 2) && data.resources[1] && force->resource(1) < data.resources[1]) {
            if (!force->gathering(1)) {
                rate = fixed::Zero;
            }
            else {
                rate = CSMath::min((force->resource(1) + force->gathering(1)) / data.resources[1], rate);
            }
        }
        return rate;
    }
    else {
        int amount = 0;
        if (kindMask & 1) {
            amount += data.resources[0];
        }
        if (kindMask & 2) {
            amount += data.resources[1];
        }
        return (fixed)amount;
    }
}

int Map::atValueUnitChannelCount(Force* force, const byte*& cursor) {
    UnitIndex index(cursor);
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    int count = 0x7fffffff;
    
    if (data.resources[0]) {
        count = (int)((force->resource(0) + force->gathering(0) * data.progress) / data.resources[0]);
    }
    if (data.resources[1]) {
        count = CSMath::min((int)((force->resource(1) + force->gathering(1) * data.progress) / data.resources[1]), count);
    }
    return count;
}

fixed Map::atValueUnitChannelRate(Force* force, const byte*& cursor) {
    int needCount = atValueUnitChannelCount(force, cursor);
    
    UnitIndex index(cursor);
    
    if (needCount <= 0) {
        return fixed::Zero;
    }
    
    int channelCount = 0;
    foreach(const Unit*, unit, _units) {
        if (unit->force() == force && unit->index() == index) {
            channelCount++;
        }
    }
    return channelCount < needCount ? (fixed)(needCount - channelCount) / needCount : fixed::Zero;
}

fixed Map::atValueValuationRate(Force* force, const byte*& cursor) {
    CSArray<const Unit*>* alliances = new CSArray<const Unit*>();
    CSArray<const Unit*>* enemies = new CSArray<const Unit*>();
    
    foreach(const Unit*, unit, _units) {
        if (unit->force()) {
            if (unit->force()->alliance() == force->alliance()) {
                alliances->addObject(unit);
            }
            else {
                enemies->addObject(unit);
            }
        }
    }
    if (alliances->count()) {
        if (!enemies->count()) {
            alliances->release();
            enemies->release();
            
            return fixed::One;
        }
    }
    else {
        fixed rtn = enemies->count() ? fixed::Zero : fixed::Half;

        alliances->release();
        enemies->release();

        return rtn;
    }
    fixed allianceValuation = fixed::Zero;
    foreach(const Unit*, alliance, alliances) {
        fixed valuation = fixed::Zero;
        foreach(const Unit*, enemy, enemies) {
            valuation += _valuations.valuation(alliance->index(), alliance->level, enemy->index(), enemy->level);
        }
        valuation /= enemies->count();
        allianceValuation += valuation;
    }
    fixed totalValuation = allianceValuation;
    foreach(const Unit*, enemy, enemies) {
        fixed valuation = fixed::Zero;
        foreach(const Unit*, alliance, alliances) {
            valuation += _valuations.valuation(enemy->index(), enemy->level, alliance->index(), alliance->level);
        }
        valuation /= alliances->count();
        totalValuation += valuation;
    }
    alliances->release();
    enemies->release();
    return totalValuation ? allianceValuation / totalValuation : fixed::Half;
}

fixed Map::atValueAreaValuationRate(Force* force, const Area* area, const byte*& cursor) {
    if (!area) {
        return fixed::Zero;
    }
    CSArray<const Unit*>* alliances = new CSArray<const Unit*>();
    CSArray<const Unit*>* enemies = new CSArray<const Unit*>();
    foreach(const Unit*, unit, _units) {
        if (unit->force() && unit->isLocated() && area->contains(unit->point())) {
            if (unit->force()->alliance() == force->alliance()) {
                alliances->addObject(unit);
            }
            else {
                enemies->addObject(unit);
            }
        }
    }
    if (alliances->count()) {
        if (!enemies->count()) {
            alliances->release();
            enemies->release();
            
            return fixed::One;
        }
    }
    else {
        fixed rtn = enemies->count() ? fixed::Zero : fixed::Half;
        
        alliances->release();
        enemies->release();
        
        return rtn;
    }
    fixed allianceValuation = fixed::Zero;
    foreach(const Unit*, alliance, alliances) {
        fixed valuation = fixed::Zero;
        foreach(const Unit*, enemy, enemies) {
            valuation += _valuations.valuation(alliance->index(), alliance->level, enemy->index(), enemy->level);
        }
        valuation /= enemies->count();
        allianceValuation += valuation;
    }
    fixed totalValuation = allianceValuation;
    foreach(const Unit*, enemy, enemies) {
        fixed valuation = fixed::Zero;
        foreach(const Unit*, alliance, alliances) {
            valuation += _valuations.valuation(enemy->index(), enemy->level, alliance->index(), alliance->level);
        }
        valuation /= alliances->count();
        totalValuation += valuation;
    }
    alliances->release();
    enemies->release();
    return totalValuation ? allianceValuation / totalValuation : fixed::One;
}

fixed Map::atValueUnitValuationRate(Force* force, const byte*& cursor) {
    bool deck = readBoolean(cursor);
    UnitIndex index(cursor);
    
    if (deck && !force->unitCount(index, false)) {
        return fixed::Zero;
    }
    
    int level = force->unitLevel(index);
    
    fixed summonRate = fixed::Zero;
    const UnitData& data = Asset::sharedAsset()->unit(index);
    if (data.resources[0]) {
        summonRate = (force->resource(0) + force->gathering(0) * AIUnitValuationTime) / data.resources[0];
    }
    if (data.resources[1]) {
        summonRate = CSMath::min((force->resource(1) + force->gathering(1) * AIUnitValuationTime) / data.resources[1], summonRate);
    }
    summonRate += AIMinUnitValuationRateByResource;
    
    fixed enemyValuation = fixed::Zero;
    fixed allianceValuation = fixed::Zero;
    foreach(const Unit*, unit, _units) {
        if (unit->force() && unit->force()->alliance() != force->alliance()) {
            allianceValuation += _valuations.valuation(index, level, unit->index(), unit->level);
            enemyValuation += _valuations.valuation(unit->index(), unit->level, index, level);
        }
    }    
	allianceValuation *= summonRate;
    
    fixed valuationRate = allianceValuation ? allianceValuation / (allianceValuation + enemyValuation) : fixed::Zero;
    
    return valuationRate;
}

fixed Map::atValueAreaEmptyRate(Force* force, const Area* area, const byte*& cursor) {
    if (!area) {
        return fixed::Zero;
    }
    int fillSize = 0;
    int areaSize = 0;
    
    static const uint fillMask = ObjectMaskLandmark | ObjectMaskMineral | ObjectMaskGas;
    
    IBounds bounds(area->point, area->range, true);
    IPoint p;
    for (p.y = bounds.src.y; p.y <= bounds.dest.y; p.y++) {
        for (p.x = bounds.src.x; p.x <= bounds.dest.x; p.x++) {
            if (area->contains(p)) {
                const Tile& tile = this->tile(p);
                if (tile.attribute() == TileAttributeBuild) {
                    synchronized(_phaseLock, GameLockMap) {
                        for (const TileObject* current = tile.objects(); current; current = current->next()) {
                            if (current->object()->compareType(fillMask)) {
                                fillSize++;
                                break;
                            }
                        }
                    }
                    areaSize++;
                }
            }
        }
    }
    return areaSize ? fixed::One - (fixed)fillSize / areaSize : fixed::Zero;
}

fixed Map::atValueAreaCostRate(Force* force, const Area* area, const byte*& cursor) {
    if (!area) {
        return fixed::Zero;
    }
    int totalCost = 0;
    int areaCost = 0;
    
    foreach(const Unit*, unit, _units) {
        if (unit->force() == force) {
            const UnitData& data = unit->data();
            int cost = data.resources[0] + data.resources[1];
            if (unit->isLocated() && area->contains(unit->point())) {
                areaCost += cost;
            }
            totalCost += cost;
        }
    }
    return totalCost ? (fixed)areaCost / totalCost : fixed::One;
}

fixed Map::atValueBattleRate(Force* force, const byte*& cursor) {
    int totalCount = 0;
    int damageCount = 0;
    foreach(const Unit*, unit, _units) {
        if (unit->isVisible(force->alliance())) {
            totalCount++;
            if (unit->isDamaging()) {
                damageCount++;
            }
        }
    }
    return totalCount ? (fixed)damageCount / totalCount : fixed::Zero;
}

fixed Map::atValueAreaBattleRate(Force* force, const Area* area, const byte*& cursor) {
    if (!area) {
        return fixed::Zero;
    }
    int totalCount = 0;
    int damageCount = 0;
    foreach(const Unit*, unit, _units) {
        if (unit->isVisible(force->alliance()) && area->contains(unit->point())) {
            totalCount++;
            if (unit->isDamaging()) {
                damageCount++;
            }
        }
    }
    return totalCount ? (fixed)damageCount / totalCount : fixed::Zero;
}

void Map::atValue(Force* force, AI::Thread* thread, const byte*& cursor) {
    int valueIndex = readShort(cursor);
    
    int combineCount = readLength(cursor);
    
    for (int i = 0; i < combineCount; i++) {
        fixed value = fixed::Zero;
        
        int valueOp = readByte(cursor);
        
        switch (valueOp) {
            case 0:
                value = readFixed(cursor);
                break;
            case 1:
				{
					fixed min = readFixed(cursor);
					fixed max = readFixed(cursor);
					value = _random->nextFixed(min, max);
				}
                break;
            case 2:
                value = force->ai()->value(readShort(cursor));
                break;
            case 3:
                if (readBoolean(cursor)) {
                    value = (fixed)force->ai()->level / AILevelMax;
                }
                else {
                    value = force->ai()->level;
                }
                break;
            case 4:
                value = atValueUnitLevel(force, cursor);
                break;
            case 5:
                value = atValueRuneLevel(force, cursor);
                break;
            case 6:
                value = atValueUnitCount(force, cursor);
                break;
            case 7:
                value = atValueAreaUnitCount(force, thread->current->area, cursor);
                break;
            case 8:
                value = atValueResource(force, cursor);
                break;
            case 9:
                value = atValueAreaResource(force, thread->current->area, cursor);
                break;
            case 10:
                value = atValueUnitResource(force, cursor);
                break;
            case 11:
                value = atValueUnitChannelResourceRate(force, cursor);
                break;
            case 12:
                value = atValueTrainingResource(force, cursor);
                break;
            case 13:
                value = atValueUnitChannelCount(force, cursor);
                break;
            case 14:
                value = atValueUnitChannelRate(force, cursor);
                break;
            case 15:
                value = force->population();
                break;
            case 16:
                value = force->supply();
                break;
            case 17:
                value = atValueValuationRate(force, cursor);
                break;
            case 18:
                value = atValueAreaValuationRate(force, thread->current->area, cursor);
                break;
            case 19:
                value = atValueUnitValuationRate(force, cursor);
                break;
            case 20:
                value = atValueAreaEmptyRate(force, thread->current->area, cursor);
                break;
            case 21:
                value = atValueAreaCostRate(force, thread->current->area, cursor);
                break;
            case 22:
                value = atValueBattleRate(force, cursor);
                break;
            case 23:
                value = atValueAreaBattleRate(force, thread->current->area, cursor);
                break;
        }
        int combineOp = readByte(cursor);
        
        switch (combineOp) {
            case 0:
                force->ai()->value(valueIndex) = value;
                break;
            case 1:
                force->ai()->value(valueIndex) += value;
                break;
            case 2:
                force->ai()->value(valueIndex) -= value;
                break;
            case 3:
                force->ai()->value(valueIndex) *= value;
                break;
            case 4:
                {
					fixed& dest = force->ai()->value(valueIndex);
                    fixed div0 = readFixed(cursor);
					if (dest) dest = value ? dest / value : div0;
                }
                break;
        }
        
        AILog("atValue (%d) valueOp:%d value:%.3f combineOp:%d dest:%.3f", valueIndex, valueOp, (float)value, combineOp, (float)force->ai()->value(valueIndex));
    }

    thread->current->ip++;
}

void Map::atRateValue(Force* force, AI::Thread* thread, const byte*& cursor) {
    int valueIndex = readShort(cursor);
    fixed ss = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    fixed se = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    fixed ds = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    fixed de = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    
    fixed value = force->ai()->value(valueIndex);
    
    if (ss == se) {
        value = fixed::Half;
    }
    else {
        value = CSMath::clamp(value, CSMath::min(ss, se), CSMath::max(ss, se));
        
        value = (value - ss) / (se - ss);
    }
    
    force->ai()->value(valueIndex) = ds + value * (de - ds);
    
    AILog("atRateValue (%d) [%.3f~%.3f]=>[%.3f~%.3f] : %.3f", valueIndex, (float)ss, (float)se, (float)ds, (float)de, (float)force->ai()->value(valueIndex));
    
    thread->current->ip++;
}

void Map::atCompareValue(Force* force, AI::Thread* thread, const byte*& cursor) {
    int valueIndex = readShort(cursor);
    fixed value = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    int compareOp = readByte(cursor);
    int ip1 = readShort(cursor);
    
    bool flag;
    
    switch (compareOp) {
        case 0:
            flag = force->ai()->value(valueIndex) == value;
            break;
        case 1:
            flag = force->ai()->value(valueIndex) < value;
            break;
        case 2:
            flag = force->ai()->value(valueIndex) <= value;
            break;
        case 3:
            flag = force->ai()->value(valueIndex) > value;
            break;
        case 4:
            flag = force->ai()->value(valueIndex) >= value;
            break;
    }
    AILog("atCompareValue (%d) compare:%d value:%.3f", valueIndex, compareOp, (float)value);
    
    thread->current->ip = flag ? thread->current->ip + 1 : ip1;
}
void Map::atValueFunc(Force* force, AI::Thread* thread, const byte*& cursor) {
    int valueIndex = readShort(cursor);
    int func = readShort(cursor);
    
    force->ai()->value(valueIndex) = func;
    
    AILog("atValueFunc (%d) func:%d", valueIndex, func);
    thread->current->ip++;
}
void Map::atIsUnitEnabled(Force* force, AI::Thread* thread, const byte*& cursor) {
    UnitIndex index(cursor);
    int ip1 = readShort(cursor);
    AILog("atIsUnitEnabled");
    
    thread->current->ip = force->unitCount(index, false) ? thread->current->ip + 1 : ip1;
}
void Map::atIsTrainingEnabled(Force* force, AI::Thread* thread, const byte*& cursor) {
    RuneIndex index(cursor);
    int ip1 = readShort(cursor);
    AILog("atIsTrainingEnabled");
    
    thread->current->ip = force->trainingState(index) == TrainingStateReady ? thread->current->ip + 1 : ip1;
}
void Map::atRandom(Force* force, AI::Thread* thread, const byte*& cursor) {
    fixed rate = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    int ip1 = readShort(cursor);
    AILog("atRandom:%.3f", (float)rate);
	bool result = _random->nextFixed(fixed::Epsilon, fixed::One) <= rate;
	thread->current->ip = result ? thread->current->ip + 1 : ip1;
}
void Map::atRandomWeights(Force* force, AI::Thread* thread, const byte*& cursor) {
    struct RandomWeight {
        fixed weight;
        ushort ip1;
    };
    int count = readLength(cursor);
    CSArray<RandomWeight>* weights = new CSArray<RandomWeight>(count);
    fixed totalWeight = fixed::Zero;
    
    for (int i = 0; i < count; i++) {
        fixed w = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
        int ip1 = readShort(cursor);
        
        if (w > fixed::Zero) {
            AILog("atRandomWeights:%d, %.3f", i, (float)w);
            
            RandomWeight& weight = weights->addObject();
            weight.weight = w;
            weight.ip1 = ip1;
            
            totalWeight += w;
        }
    }
    int ip1 = readShort(cursor);
    if (totalWeight) {
        fixed r = _random->nextFixed(fixed::Epsilon, totalWeight);
        totalWeight = fixed::Zero;
        for (int i = 0; i < count; i++) {
            const RandomWeight& weight = weights->objectAtIndex(i);
            
            totalWeight += weight.weight;
            if (r <= totalWeight) {
                AILog("atRandomWeights select:%d", i);
                thread->current->ip = weight.ip1;
                break;
            }
        }
    }
    else {
        AILog("atRandomWeights none");
        thread->current->ip = ip1;
    }
    weights->release();
}
void Map::atValueFuncRandomWeights(Force* force, AI::Thread* thread, const byte*& cursor) {
    struct RandomWeight {
        fixed weight;
        ushort func;
    };
    int valueIndex = readShort(cursor);
    int count = readLength(cursor);
    CSArray<RandomWeight>* weights = new CSArray<RandomWeight>(count);
    fixed totalWeight = fixed::Zero;
    
    for (int i = 0; i < count; i++) {
        fixed w = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
        int func = readShort(cursor);
        
        if (w > fixed::Zero) {
            AILog("atValueFuncRandomWeights:%d, %.3f", i, (float)w);
            
            RandomWeight& weight = weights->addObject();
            weight.weight = w;
            weight.func = func;
            
            totalWeight += w;
        }
    }
    int ip1 = readShort(cursor);
    if (totalWeight) {
        fixed r = _random->nextFixed(fixed::Epsilon, totalWeight);
        totalWeight = fixed::Zero;
        for (int i = 0; i < count; i++) {
            const RandomWeight& weight = weights->objectAtIndex(i);
            
            totalWeight += weight.weight;
            if (r <= totalWeight) {
                AILog("atValueFuncRandomWeights select(%d):%d, %d", valueIndex, i, weight.func);
                force->ai()->value(valueIndex) = weight.func;
                break;
            }
        }
        thread->current->ip++;
    }
    else {
        AILog("atValueFuncRandomWeights none(%d)", valueIndex);
        thread->current->ip = ip1;
    }
    weights->release();
}

void Map::atCombatAttack(Force* force, fixed attackRate) {
    struct CombatUnit {
        Unit* unit;
        fixed valuation;
        uint valuationCount;
        
        inline CombatUnit(Unit* unit) : unit(unit), valuation(fixed::Zero), valuationCount(0) {
            
        }
    };
    
    CSArray<Unit*>* alliances = new CSArray<Unit*>();
    CSArray<Unit*>* enemies = new CSArray<Unit*>();
    
    foreach(Unit*, unit, _units) {
        if (unit->force() && unit->isLocated()) {
            if (unit->force()->alliance() == force->alliance()) {
                if (unit->compareType(ObjectMaskWeapon)) {
                    alliances->addObject(unit);
                }
            }
            else {
                enemies->addObject(unit);
            }
        }
    }
    CSArray<CombatUnit>* localAlliances = new CSArray<CombatUnit>();
    CSArray<CombatUnit>* localEnemies = new CSArray<CombatUnit>();
    CSArray<Unit>* commandUnits = new CSArray<Unit>();
    
    while (enemies->count() && alliances->count()) {
        int index = -1;
        fixed distance2 = fixed::Max;
        for (int i = 0; i < enemies->count(); i++) {
            Unit* enemy = enemies->objectAtIndex(i);
            foreach(Unit*, alliance, alliances) {
                fixed d2 = enemy->point().distanceSquared(alliance->point());
				if (d2 < distance2) {
                    index = i;
                    distance2 = d2;
                }
            }
        }
        Unit* firstEnemy = enemies->objectAtIndex(index);
        new (&localEnemies->addObject()) CombatUnit(firstEnemy);
        enemies->removeObjectAtIndex(index);
        
        const Area* invasionArea = NULL;
        foreach(const Area*, area, force->ai()->areas) {
            if (area->contains(firstEnemy->point())) {
                invasionArea = area;
                break;
            }
        }
        bool enemyCloaking = firstEnemy->isCloaking();
        bool allianceDetecting = false;
        fixed maxRate = fixed::Zero;
        
        while (alliances->count()) {
            index = -1;
            distance2 = fixed::Max;
            
            int appliedAllianceCount = 0;
            int order = 0;
            
            for (int i = 0; i < alliances->count(); i++) {
                Unit* alliance = alliances->objectAtIndex(i);
                
                if (alliance->compareType(ObjectMaskAir) || (invasionArea && firstEnemy->compareType(ObjectMaskAir)) || hasPath(alliance->point(), firstEnemy->point())) {
                    int currentOrder;
                    
                    bool isLabor = alliance->compareType(ObjectMaskLabor) && !alliance->isDetecting();
                    
                    if (invasionArea) {
                        if (invasionArea->contains(alliance->point())) {
                            currentOrder = isLabor ? 1 : 2;
                        }
                        else if (isLabor) {
                            continue;
                        }
                        else {
                            currentOrder = 0;
                        }
                    }
                    else if (isLabor) {
                        continue;
                    }
                    else {
                        currentOrder = 2;
                    }
                    if (currentOrder >= order) {
                        fixed d2 = alliance->point().distanceSquared(firstEnemy->point());
                        if (currentOrder > order || d2 < distance2) {
                            index = i;
                            distance2 = d2;
                            order = currentOrder;
                        }
                    }
                    appliedAllianceCount++;
                }
            }
            if (index == -1) {
                break;
            }
            Unit* firstAlliance = alliances->objectAtIndex(index);
            new (&localAlliances->addObject()) CombatUnit(firstAlliance);
            alliances->removeObjectAtIndex(index);
            
            {
                int i = 0;
                while (i < enemies->count()) {
                    Unit* enemy = enemies->objectAtIndex(i);
                    fixed d2 = enemy->point().distanceSquared(firstEnemy->point());
                    if (d2 <= distance2) {
                        new (&localEnemies->addObject()) CombatUnit(enemy);
                        if (!enemyCloaking && enemy->isCloaking()) {
                            enemyCloaking = true;
                        }
                        enemies->removeObjectAtIndex(i);
                    }
                    else {
                        i++;
                    }
                }
            }
            
            if (!allianceDetecting && firstAlliance->isDetecting()) {
                allianceDetecting = true;
                
                if (enemyCloaking) {
                    foreach(CombatUnit&, alliance, localAlliances) {
                        for (int i = 0; i < alliance.valuationCount; i++) {
                            const Unit* enemy = localEnemies->objectAtIndex(i).unit;
                            if (enemy->isCloaking()) {
                                alliance.valuation += _valuations.valuation(alliance.unit->index(), alliance.unit->level, enemy->index(), enemy->level);
                            }
                        }
                    }
                }
            }
            
            fixed allianceCountRate = (fixed)localAlliances->count() / (appliedAllianceCount + localAlliances->count() - 1);
            fixed enemyCountRate = (fixed)localEnemies->count() / (enemies->count() + localEnemies->count());
            
            AILog("atCombatAttack countRate:%.3f(%d, %d), %.3f", (float)allianceCountRate, appliedAllianceCount, localAlliances->count(), (float)enemyCountRate);
            
            if (allianceCountRate >= enemyCountRate) {
                fixed totalEnemyValuation = fixed::Zero;
                foreach(CombatUnit&, enemy, localEnemies) {
                    for (int i = enemy.valuationCount; i < localAlliances->count(); i++) {
                        const Unit* alliance = localAlliances->objectAtIndex(i).unit;
                        enemy.valuation += _valuations.valuation(enemy.unit->index(), enemy.unit->level, alliance->index(), alliance->level);
                    }
                    enemy.valuationCount = localAlliances->count();

                    totalEnemyValuation += enemy.valuation / enemy.valuationCount;
                }

                commandUnits->removeAllObjects();
                fixed totalAllianceValuation = fixed::Zero;
                fixed detectorRate = fixed::One;
                
                foreach(CombatUnit&, alliance, localAlliances) {
                    for (int i = alliance.valuationCount; i < localEnemies->count(); i++) {
                        const Unit* enemy = localEnemies->objectAtIndex(i).unit;
                        if (allianceDetecting || !enemyCloaking || !enemy->isCloaking()) {
                            alliance.valuation += _valuations.valuation(alliance.unit->index(), alliance.unit->level, enemy->index(), enemy->level);
                        }
                    }
                    alliance.valuationCount = localEnemies->count();

                    totalAllianceValuation += alliance.valuation / alliance.valuationCount;

                    if (alliance.unit->force() == force) {
                        bool flag = alliance.valuation > fixed::Zero;
                        if (alliance.unit->isDetecting()) {
                            if (!flag && _random->nextFixed(fixed::Epsilon, fixed::One) <= detectorRate) {
                                flag = true;
                            }
                            detectorRate *= AIAttackDetectorDecreaseRate;
                        }
                        if (flag) {
                            commandUnits->addObject(alliance.unit);
                        }
                    }
                }
                if (commandUnits->count() && totalEnemyValuation + totalAllianceValuation) {
                    fixed rate = totalAllianceValuation / (totalEnemyValuation + totalAllianceValuation);
                    if (!invasionArea) {
                        rate *= attackRate;
                    }
                    AILog("atCombatAttack:%.3f alliance:%d(%.3f) enemy:%d(%.3f)", (float)rate, localAlliances->count(), (float)totalAllianceValuation, localEnemies->count(), (float)totalEnemyValuation);
                    
                    if (rate > maxRate) {
                        AILog("atCombatAttack check");
                        
                        maxRate = rate;
                        
                        if (_random->nextFixed(fixed::Epsilon, fixed::One) <= rate) {
                            AILog("atCombatAttack commit:%d", commandUnits->count());
                            
                            if (enemyCloaking && !allianceDetecting) {
                                foreach(const CombatUnit&, enemy, localEnemies) {
                                    if (!enemy.unit->isCloaking()) {
                                        commandImpl(commandUnits, CommandIndex(CommandSetTarget, 0), NULL, &enemy.unit->point(), 0, CommandCommitRun);
                                        break;
                                    }
                                }
                            }
                            else {
                                commandImpl(commandUnits, CommandIndex(CommandSetTarget, 0), NULL, &firstEnemy->point(), 0, CommandCommitRun);
                            }
                            break;
                        }
                    }
                }
            }
        }
        localAlliances->removeAllObjects();
        localEnemies->removeAllObjects();
    }
    localAlliances->release();
    localEnemies->release();
    commandUnits->release();
    alliances->release();
    enemies->release();
}

void Map::atCombatRetreat(Force* force, fixed retreatRate) {
    CSArray<Unit*>* enemies = new CSArray<Unit*>();
    CSArray<Unit*>* alliances = new CSArray<Unit*>();
    foreach(Unit*, unit, _units) {
        if (unit->force() && unit->isLocated()) {
            if (unit->force()->alliance() == force->alliance()) {
                alliances->addObject(unit);
            }
            else {
                enemies->addObject(unit);
            }
        }
    }
    CSArray<Unit*>* localAlliances = new CSArray<Unit*>();
    CSArray<Unit*>* localEnemies = new CSArray<Unit*>();
    
    while (alliances->count() && enemies->count()) {
        Unit* firstAlliance = alliances->lastObject();
        
        localAlliances->addObject(firstAlliance);
        alliances->removeLastObject();
        
        int i = 0;
        while (i < enemies->count()) {
            Unit* enemy = enemies->objectAtIndex(i);
            fixed d2 = enemy->point().distanceSquared(firstAlliance->point());
            fixed c = firstAlliance->collider() + enemy->collider() + firstAlliance->sight();
            
            if (d2 <= c * c) {
                localEnemies->addObject(enemy);
                enemies->removeObjectAtIndex(i);
            }
            else {
                i++;
            }
        }
        if (localEnemies->count()) {
            i = 0;
            while (i < alliances->count()) {
                Unit* alliance = alliances->objectAtIndex(i);
                fixed d2 = alliance->point().distanceSquared(firstAlliance->point());
                fixed c = firstAlliance->collider() + alliance->collider() + firstAlliance->sight();
                
                if (d2 <= c * c) {
                    localAlliances->addObject(alliance);
                    alliances->removeObjectAtIndex(i);
                }
                else {
                    i++;
                }
            }
            fixed totalAllianceValuation = fixed::Zero;
            fixed totalEnemyValuation = fixed::Zero;
            
            foreach(Unit*, alliance, localAlliances) {
                fixed valuation = fixed::Zero;
                foreach(Unit*, enemy, localEnemies) {
                    valuation += _valuations.valuation(alliance->index(), alliance->level, enemy->index(), enemy->level);
                }
                valuation /= localEnemies->count();
                totalAllianceValuation += valuation;
            }
            
            foreach(Unit*, enemy, localEnemies) {
                fixed valuation = fixed::Zero;
                foreach(Unit*, alliance, localAlliances) {
                    valuation += _valuations.valuation(enemy->index(), enemy->level, alliance->index(), alliance->level);
                }
                valuation /= localAlliances->count();
                totalEnemyValuation += valuation;
            }
            if (totalEnemyValuation + totalAllianceValuation) {
                fixed rate = (totalEnemyValuation / (totalEnemyValuation + totalAllianceValuation)) * retreatRate;
                
                AILog("atCombatRetreat:%.3f alliance:%d(%.3f) enemy:%d(%.3f)", (float)rate, localAlliances->count(), (float)totalAllianceValuation, localEnemies->count(), (float)totalEnemyValuation);
                
                if (_random->nextFixed(fixed::Epsilon, fixed::One) <= rate) {
                    AILog("atCombatRetreat commit");
                    
                    CSArray<Unit>* units = CSArray<Unit>::arrayWithCapacity(localAlliances->count());
                    foreach(Unit*, alliance, localAlliances) {
                        if (alliance->force() == force && alliance->compareType(ObjectMaskWeapon)) {
                            units->addObject(alliance);
                        }
                    }
                    if (units->count()) {
                        FPoint p;
                        if (targetToRetreat(units->objectAtIndex(0), p)) {
                            Destination* target = Destination::destination(force);
                            target->locate(p);
                            
                            commandImpl(units, CommandIndex(CommandSetTarget, 1), target, NULL, 0, CommandCommitRun);
                        }
                    }
                }
            }
            localEnemies->removeAllObjects();
        }
        localAlliances->removeAllObjects();
    }
    alliances->release();
    enemies->release();
    localAlliances->release();
    localEnemies->release();
}

void Map::atCombat(Force* force, AI::Thread* thread, const byte*& cursor) {
    fixed attackRate = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    fixed retreatRate = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    
    atCombatRetreat(force, attackRate);
    atCombatAttack(force, retreatRate);
    thread->current->ip++;
    
    //TODO:기습유닛을 활용하는 부분, 미구현
}

bool Map::atWaitResource(Force* force, AI::Thread* thread, int resource0, int resource1, int priority, fixed remaining) {
    AI* ai = force->ai();
    
    if (!resource0 && !resource1) {
        AILog("atWaitResource (no resource)");
        if (ai->resourceWaiting.thread == thread) {
            ai->resourceWaiting.remaining = fixed::Zero;
            ai->resourceWaiting.thread = NULL;
            ai->resourceWaiting.priority = 0;
        }
        return false;
    }
    if (ai->resourceWaiting.thread && ai->resourceWaiting.thread != thread && ai->resourceWaiting.priority >= priority) {
        AILog("atWaitResource (waiting from:%d - %d)", ai->resourceWaiting.thread->current->func, ai->resourceWaiting.thread->current->ip);
        return true;
    }
    if (ai->resourceWaiting.remaining) {
        AILog("atWaitResource (remaining)");
        return true;
    }
    if (resource0) {
        if (force->resource(0) < resource0) {
            if (force->gathering(0)) {
                AILog("atWaitResource (fail 0)");
                ai->resourceWaiting.thread = thread;
                ai->resourceWaiting.priority = priority;
                return true;
            }
            else {
                AILog("atWaitResource (no gathering 0)");
                if (ai->resourceWaiting.thread == thread) {
                    ai->resourceWaiting.thread = NULL;
                    ai->resourceWaiting.priority = 0;
                }
                return false;
            }
        }
    }
    if (resource1) {
        if (force->resource(1) < resource1) {
            if (force->gathering(1)) {
                AILog("atWaitResource (fail 1)");
                ai->resourceWaiting.thread = thread;
                ai->resourceWaiting.priority = priority;
                return true;
            }
            else {
                AILog("atWaitResource (no gathering 1)");
                if (ai->resourceWaiting.thread == thread) {
                    ai->resourceWaiting.thread = NULL;
                    ai->resourceWaiting.priority = 0;
                }
                return false;
            }
        }
    }
    AILog("atWaitResource (success)");
    if (ai->resourceWaiting.thread == thread) {
        ai->resourceWaiting.thread = NULL;
        ai->resourceWaiting.priority = 0;
    }
    ai->resourceWaiting.remaining = remaining;
    return false;
}

void Map::atWaitResourceRelease(Force* force, AI::Thread* thread) {
    AI* ai = force->ai();
    
    if (ai->resourceWaiting.thread == thread) {
        ai->resourceWaiting.thread = NULL;
        ai->resourceWaiting.remaining = fixed::Zero;
        ai->resourceWaiting.priority = 0;
    }
}

Object* Map::atCommandTarget(Force* force, AI::Thread* thread, const CSArray<Unit>* units, const byte* condition) {
    Object* target = NULL;
    
    if (condition) {
        fixed targetDistance2 = fixed::Max;
        foreach(Unit*, dest, _units) {
            if (dest->isLocated() && dest->match(force, condition)) {
                foreach(const Unit*, src, units) {
                    fixed d2 = src->point().distanceSquared(dest->point());
                    if (d2 < targetDistance2) {
                        target = dest;
                        targetDistance2 = d2;
                    }
                }
            }
        }
    }
	else if (thread->current->area) {
		const CSArray<FPoint>* ps = thread->current->area->rallyPoints;
		
		if (ps) {
			const FPoint& p = ps->objectAtIndex(_random->nextInt(0, ps->count() - 1));
			target = Destination::destination(force);
			target->locate(p);
		}
	}
    
    return target;
}

bool Map::atCommand(Force* force, AI::Thread* thread, const byte*& cursor) {
    int areaOp = readByte(cursor);
    const byte* condition = cursor;
    cursor += UnitBase::matchLength(condition);
    int count = readBoolean(cursor) ? (int)force->ai()->value(readShort(cursor)) : readByte(cursor);
    CommandIndex index(cursor);
    const byte* targetCondition = NULL;
    int targetType = readByte(cursor);      //0:auto 1:build defense 2:target 3:target point
    if (targetType >= 2) {
        targetCondition = cursor;
        cursor += UnitBase::matchLength(targetCondition);
    }
    if (areaOp == 1 && !thread->current->area) {
        atWaitResourceRelease(force, thread);
        thread->current->ip++;
        return false;
    }
    
    CSArray<Unit>* units = CSArray<Unit>::array();
    foreach(Unit*, unit, _units) {
        if (unit->force() == force && unit->isLocated()) {
            switch (areaOp) {
                case 1:
                    if (!thread->current->area->contains(unit->point())) {
                        continue;
                    }
                    break;
                case 2:
                    {
                        bool flag = false;
                        foreach(const Area*, area, force->ai()->areas) {
                            if (area->contains(unit->point())) {
                                flag = true;
                                break;
                            }
                        }
                        if (flag) {
                            continue;
                        }
                    }
                    break;
            }
            if (unit->match(force, condition)) {
                units->addObject(unit);
            }
        }
    }

    CommandReturn rtn;
    rtn.state = CommandStateEmpty;

    fixed estimation = fixed::Zero;
    
    int i = 0;
    while (i < units->count()) {
        Unit* unit = units->objectAtIndex(i);
        
        CommandReturn crtn = unit->command(index, NULL, estimation, 0, 0, false);
        
        if (rtn.state < crtn.state) {
            rtn = crtn;
        }
        switch (crtn.state) {
			case CommandStateEmpty:
			case CommandStateCooltime:
			case CommandStateUnabled:
				units->removeObjectAtIndex(i);
				break;
			default:
                i++;
                break;
        }
    }
    
    AILog("atCommand ready index:(%d, %d) count:%d code:%d", index.indices[0], index.indices[1], units->count(), rtn.state);
    
    switch (rtn.state) {
        case CommandStateProduceUnabled:
        case CommandStateProduce:
            {
                const UnitData& data = Asset::sharedAsset()->unit(rtn.parameter.produce.index);
                if (atWaitResource(force, thread, data.resources[0], data.resources[1], AIWaitResourcePriorityProduce, fixed::Zero)) {
                    return true;
                }
            }
            break;
        case CommandStateBuildUnabled:
        case CommandStateBuild:
            {
                const UnitData& data = Asset::sharedAsset()->unit(rtn.parameter.build.index);
                if (atWaitResource(force, thread, data.resources[0], data.resources[1], AIWaitResourcePriorityBuild, fixed::One)) {     //실제로 건설을 해서 자원소모를 하기까지 다른 자원 소모 대기
                    return true;
                }
            }
            break;
        case CommandStateTrainUnabled:
        case CommandStateTrain:
            {
                const RuneData& data = Asset::sharedAsset()->rune(rtn.parameter.train.index);
                if (atWaitResource(force, thread, data.resources[0], data.resources[1], AIWaitResourcePriorityTraining, fixed::Zero)) {
                    return true;
                }
            }
            break;
        default:
            atWaitResourceRelease(force, thread);
            break;
    }

    if (!units->count()) {
        thread->current->ip++;
        return false;
    }
    
    Object* target = NULL;
    
    switch (rtn.state) {
        case CommandStateTarget:
            if (!(target = atCommandTarget(force, thread, units, targetCondition ? targetCondition : rtn.parameter.target.condition))) {
                thread->current->ip++;
                return false;
            }
            break;
        case CommandStateRange:
            if (!(target = atCommandTarget(force, thread, units, targetCondition ? targetCondition : rtn.parameter.range.condition))) {
                thread->current->ip++;
                return false;
            }
            break;
        case CommandStateAngle:
            if (!(target = atCommandTarget(force, thread, units, targetCondition ? targetCondition : rtn.parameter.angle.condition))) {
                thread->current->ip++;
                return false;
            }
            break;
        case CommandStateLine:
            if (!(target = atCommandTarget(force, thread, units, targetCondition ? targetCondition : rtn.parameter.line.condition))) {
                thread->current->ip++;
                return false;
            }
            break;
        case CommandStateBuild:
            if (rtn.parameter.build.target) {
                FPoint point;
                
                if (targetCondition) {
                    if (!(target = atCommandTarget(force, thread, units, targetCondition))) {
                        thread->current->ip++;
                        return false;
                    }
                    point = target->point();
                    
                    const UnitData& data = Asset::sharedAsset()->unit(rtn.parameter.build.index);
                    
                    if (locatablePosition(data.type, point, data.collider, NULL, MapLocationEnabled) == MapLocationUnabled) {
                        thread->current->ip++;
                        return false;
                    }
                }
                else if (targetType == 1) {
                    if (!thread->current->area) {
                        thread->current->ip++;
                        return false;
                    }
                    point = FPoint::Zero;
                    int count = 0;
                    foreach(const Resource*, resource, _resources) {
                        if (resource->isLocated() && resource->amount && thread->current->area->contains(resource->point())) {
                            point += resource->point();
                            count++;
                        }
                    }
                    if (!count) {
                        thread->current->ip++;
                        return false;
                    }
                    point /= count;
                    
                    const UnitData& data = Asset::sharedAsset()->unit(rtn.parameter.build.index);
                    
                    if (locatablePosition(data.type, point, data.collider, NULL, MapLocationEnabled) == MapLocationUnabled) {
                        thread->current->ip++;
                        return false;
                    }
                }
                else {
                    if (!thread->current->area || !buildingPosition(force, rtn.parameter.build.index, *thread->current->area, point)) {
                        thread->current->ip++;
                        return false;
                    }
                }
                target = Frame::frame(force, rtn.parameter.build.index, rtn.parameter.build.target, true);
                target->locate(point);
            }
            break;
        case CommandStateEnabled:
            if (targetCondition) {
                target = atCommandTarget(force, thread, units, targetCondition);
                if (target) {
                    if (targetType == 3) {
                        const FPoint& point = target->point();
                        target = Destination::destination(force);
                        target->locate(point);
                    }
                }
                else {
                    thread->current->ip++;
                    return false;
                }
            }
            break;
    }
    
    if (count && count < units->count()) {
        if (target) {
            for (int i = 0; i < count; i++) {
                int swap = i;
                fixed distance2 = units->objectAtIndex(i)->point().distanceSquared(target->point());
                
                for (int j = i + 1; j < units->count(); j++) {
                    Unit* unit = units->objectAtIndex(j);
                    fixed d2 = unit->point().distanceSquared(target->point());
                    if (d2 < distance2) {
                        distance2 = d2;
                        swap = j;
                    }
                }
                if (swap != i) {
                    Unit* unit = units->objectAtIndex(swap);
                    units->removeObjectAtIndex(swap);
                    units->insertObject(i, unit);
                }
            }
        }
        while (units->count() > count) {
            units->removeLastObject();
        }
    }
    rtn = commandImpl(units, index, target, NULL, 0, CommandCommitOnce);

    AILog("atCommand commit index:(%d, %d) count:%d code:%d", index.indices[0], index.indices[1], units->count(), rtn.state);
    
    thread->current->ip++;
    return false;
}
bool Map::atSpawn(Force* force, AI::Thread* thread, const byte*& cursor) {
    UnitIndex index(cursor);
    const byte* targetCondition = NULL;
    
    if (readBoolean(cursor)) {
        targetCondition = cursor;
        cursor += UnitBase::matchLength(targetCondition);
    }
    Spawn* spawn = force->spawn(index);
    
    int commit = 2;
    bool all = false;
    
    if (spawn) {
        CommandReturn rtn = spawn->run(NULL, false);
        
        switch (rtn.state) {
            case CommandStateEmpty:
            case CommandStateCooltime:
                commit = 0;
                break;
            case CommandStateUnabled:
                commit = 1;
                break;
            case CommandStateTarget:
                if (!targetCondition) {
                    targetCondition = rtn.parameter.target.condition;
                }
                all = rtn.parameter.target.all;
                if (!targetCondition) {
                    commit = 0;
                }
                break;
            case CommandStateRange:
                if (!targetCondition) {
                    targetCondition = rtn.parameter.range.condition;
                }
                all = rtn.parameter.range.all;
                if (!targetCondition) {
                    commit = 0;
                }
                break;
        }
    }
    else {
        commit = 0;
    }

    if (commit) {
        const UnitData& data = Asset::sharedAsset()->unit(index);
        if (atWaitResource(force, thread, data.resources[0], data.resources[1], AIWaitResourcePrioritySpawn, fixed::Zero)) {
            return true;
        }
        if (commit == 2) {
            Object* target = NULL;
            
            if (targetCondition) {
                CSArray<Unit*>* targets = CSArray<Unit*>::array();
                foreach(Unit*, unit, _units) {
                    if ((all ? unit->isLocated() : unit->isVisible(force->alliance())) && unit->match(force, targetCondition)) {
                        targets->addObject(unit);
                    }
                }
                if (!targets->count()) {
                    goto rtn;
                }
                target = targets->objectAtIndex(_random->nextInt(0, targets->count() - 1));
            }
            spawn->run(target, true);
        }
    }
    else {
        atWaitResourceRelease(force, thread);
    }
rtn:
    thread->current->ip++;
    return false;
}

void Map::atChangeArea(Force* force, AI::Thread* thread, const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (thread->current->area) {
        AILog("atChangeArea next");
        
        int index = force->ai()->areas->indexOfObjectIdenticalTo(thread->current->area) + 1;
        if (index == CSNotFound || index >= force->ai()->areas->count()) {
            thread->current->area = NULL;
            thread->current->ip = ip1;
        }
        else {
            thread->current->area = force->ai()->areas->objectAtIndex(index);
            thread->current->ip++;
        }
    }
    else if (force->ai()->areas->count()) {
        AILog("atChangeArea first");
        
        thread->current->area = force->ai()->areas->objectAtIndex(0);
        thread->current->ip++;
    }
    else {
        AILog("atChangeArea none");
        
        thread->current->ip = ip1;
    }
}

void Map::atNewArea(Force* force, AI::Thread* thread, const byte*& cursor) {
    int order = readByte(cursor);
    int ip1 = readShort(cursor);
    
    thread->current->area = NULL;
    
    switch (order) {
        case 0:
            {
                fixed distance = fixed::Max;
                foreach(const Area&, area, _areas) {
                    if (!force->ai()->areas->containsObjectIdenticalTo(&area)) {
                        fixed d = _navMesh->distance(force->point, area.point);     //가까운 영역을 확보할 때는 지상이동거리를 기준으로 한다.
                        
                        if (d < distance) {
                            thread->current->area = &area;
                            distance = d;
                        }
                    }
                }
            }
            break;
        case 1:
            {
                fixed distance2 = fixed::Zero;
                foreach(const Area&, area, _areas) {
                    if (!force->ai()->areas->containsObjectIdenticalTo(&area) && hasPath(force->point, area.point)) {
                        fixed enemyDistance2 = fixed::Max;
                        for (int i = 0; i < MaxPlayers; i++) {
                            if (_forces[i] && _forces[i]->alliance() != force->alliance()) {
                                fixed d2 = _forces[i]->point.distanceSquared(area.point);
                                if (d2 < enemyDistance2) {
                                    enemyDistance2 = d2;
                                }
                            }
                        }
                        if (enemyDistance2 > distance2) {
                            thread->current->area = &area;
                            distance2 = enemyDistance2;
                        }
                    }
                }
            }
            break;
    }
    if (thread->current->area) {
        AILog("atNewArea success:%d", order);
    }
    else {
        AILog("atNewArea fail:%d", order);
    }
    
    thread->current->ip = thread->current->area ? thread->current->ip + 1 : ip1;
}

void Map::atGiveupArea(Force* force, AI::Thread* thread, const byte*& cursor) {
    int ip1 = readShort(cursor);
    
    if (!thread->current->area) {
        thread->current->ip = ip1;
        return;
    }
    synchronized(_phaseLock, GameLockMap) {
        for (const TileObject* current = tile(thread->current->area->point).objects(); current; current = current->next()) {
            if (current->object()->compareType(ObjectMaskBase)) {
                const Unit* unit = static_cast<const Unit*>(current->object());
                
                if (unit->force() == force) {
                    thread->current->ip = ip1;
                    return;
                }
            }
        }
    }
    
    int index = force->ai()->areas->indexOfObjectIdenticalTo(thread->current->area);
    force->ai()->areas->removeObjectAtIndex(index);
    if (index < force->ai()->areas->count()) {
        thread->current->area = force->ai()->areas->objectAtIndex(index);
        thread->current->ip++;
    }
    else {
        thread->current->area = NULL;
        thread->current->ip = ip1;
    }
    AILog("atGiveupArea");
}

void Map::atGiveup(Force* force, AI::Thread* thread, const byte*& cursor) {
    if (force->ai()->fully) {
        force->setControl(GameControlNone);
    }
    thread->current->ip++;
}

bool Map::atWait(Force* force, AI::Thread* thread, const byte*& cursor) {
    thread->waiting = readBoolean(cursor) ? force->ai()->value(readShort(cursor)) : readFixed(cursor);
    AILog("atWait:%.3f", (float)thread->waiting);
    thread->current->ip++;
    return thread->waiting != fixed::Zero;
}

bool Map::atStop(Force* force, AI::Thread* thread, const byte*& cursor) {
    thread->current->ip = readShort(cursor);
    return readBoolean(cursor);
}

void Map::atEmoticon(Force* force, AI::Thread* thread, const byte*& cursor) {
    EmoticonIndex index;
    
    switch (readByte(cursor)) {
        case 0:
            index = force->ai()->emoticonResponseIndex;
            break;
        case 1:
            index = EmoticonIndex(cursor);
            break;
        case 2:
            {
                fixed rate = readFixed(cursor);
                index = force->ai()->emoticonResponseIndex && _random->nextFixed(fixed::Zero, fixed::One) < rate ? force->ai()->emoticonResponseIndex : EmoticonIndex::None;
            }
            break;
    }
    force->ai()->emoticonResponseIndex = EmoticonIndex::None;
    
    if (force->ai()->fully && index) {
        emoticon(force, index);
    }
    thread->current->ip++;
}

enum AITriggerOp {
    AT_Call,
    AT_Value,
    AT_RateValue,
    AT_CompareValue,
    AT_ValueFunc,
    AT_IsUnitEnabled,
    AT_IsTrainingEnabled,
    AT_Random,
    AT_RandomWeights,
    AT_ValueFuncRandomWeights,
    AT_Combat,
    AT_Command,
    AT_Spawn,
    AT_ChangeArea,
    AT_NewArea,
    AT_GiveupArea,
    AT_Giveup,
    AT_Wait,
    AT_Stop,
    AT_Emoticon
};

#ifdef UseAITriggerTest
static const char* AITriggerOpNames[] = {
    "AT_Call",
    "AT_Value",
    "AT_RateValue",
    "AT_CompareValue",
    "AT_ValueFunc",
    "AT_IsUnitEnabled",
    "AT_IsTrainingEnabled",
    "AT_Random",
    "AT_RandomWeights",
    "AT_ValueFuncRandomWeights",
    "AT_Combat",
    "AT_Command",
    "AT_Spawn",
    "AT_ChangeArea",
    "AT_NewArea",
    "AT_GiveupArea",
    "AT_Giveup",
    "AT_Wait",
    "AT_Stop",
    "AT_Emoticon"
};
#endif

bool Map::updateAI(Force* force, AI::Thread* thread) {
    if (thread->waiting) {
        thread->waiting -= _latency.frameDelayFixed;
        if (thread->waiting > fixed::Zero) {
            return true;
        }
        thread->waiting = fixed::Zero;
    }
    while (thread->current) {
        const CSArray<byte, 2>* triggerSet;
        
        while ((triggerSet = Asset::sharedAsset()->aiTriggers->objectAtIndex(thread->current->func)) && thread->current->ip < triggerSet->count()) {
            const CSArray<byte, 1>* trigger = triggerSet->objectAtIndex(thread->current->ip);
            
            const byte* cursor = trigger->pointer();
            
            AITriggerOp opCode = (AITriggerOp)readByte(cursor);
            
            AILog("ai Trigger (thread:%d) (func:%d) (ip:%d) (%s)", thread->threadId, thread->current->func, thread->current->ip, AITriggerOpNames[opCode]);
            
            bool stop = false;
            
            switch(opCode) {
                case AT_Call:
                    atCall(force, thread, cursor);
                    break;
                case AT_Value:
                    atValue(force, thread, cursor);
                    break;
                case AT_RateValue:
                    atRateValue(force, thread, cursor);
                    break;
                case AT_CompareValue:
                    atCompareValue(force, thread, cursor);
                    break;
                case AT_ValueFunc:
                    atValueFunc(force, thread, cursor);
                    break;
                case AT_IsUnitEnabled:
                    atIsUnitEnabled(force, thread, cursor);
                    break;
                case AT_IsTrainingEnabled:
                    atIsTrainingEnabled(force, thread, cursor);
                    break;
                case AT_Random:
                    atRandom(force, thread, cursor);
                    break;
                case AT_RandomWeights:
                    atRandomWeights(force, thread, cursor);
                    break;
                case AT_ValueFuncRandomWeights:
                    atValueFuncRandomWeights(force, thread, cursor);
                    break;
                case AT_Combat:
                    atCombat(force, thread, cursor);
                    break;
                case AT_Command:
                    if (atCommand(force, thread, cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Spawn:
                    if (atSpawn(force, thread, cursor)) {
                        stop = true;
                    }
                    break;
                case AT_ChangeArea:
                    atChangeArea(force, thread, cursor);
                    break;
                case AT_NewArea:
                    atNewArea(force, thread, cursor);
                    break;
                case AT_GiveupArea:
                    atGiveupArea(force, thread, cursor);
                    break;
                case AT_Giveup:
                    atGiveup(force, thread, cursor);
                    break;
                case AT_Wait:
                    if (atWait(force, thread, cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Stop:
                    if (atStop(force, thread, cursor)) {
                        stop = true;
                    }
                    break;
                case AT_Emoticon:
                    atEmoticon(force, thread, cursor);
                    break;
            }
#ifdef CS_ASSERT_DEBUG
            int pos = (uint64)cursor - (uint64)trigger->pointer();
            int len = trigger->count();
            CSAssert(pos == len, "ai trigger error:%d,%d,%d", opCode, pos, len);
#endif
            if (stop) {
                return true;
            }
        }
        AI::Thread::Stack* prev = thread->current->prev;
        delete thread->current;
        thread->current = prev;
    }
    return false;
}

void Map::updateAI(Force* force) {
	CSAssert(force->ai(), "invalid operation");

	if (!_running || (!force->isAlive() && force->ai()->fully)) {
        return;
    }
    if (force->ai()->lifetime) {
        force->ai()->lifetime -= _latency.frameDelayFixed;
        if (force->ai()->lifetime <= fixed::Zero) {
            force->ai()->lifetime = fixed::Zero;
            force->setControl(GameControlNone);
        }
    }
    if (force->ai()->resourceWaiting.remaining) {
        force->ai()->resourceWaiting.remaining = CSMath::max(force->ai()->resourceWaiting.remaining - _latency.frameDelayFixed, fixed::Zero);
    }
    foreach(const Area&, area, _areas) {
        if (!force->ai()->areas->containsObjectIdenticalTo(&area)) {
            synchronized(_phaseLock, GameLockMap) {
                for (const TileObject* current = tile(area.point).objects(); current; current = current->next()) {
                    if (current->object()->compareType(ObjectMaskBase)) {
                        const Unit* unit = static_cast<const Unit*>(current->object());
                        if (unit->force() == force) {
                            force->ai()->areas->addObject(&area);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    int i = 0;
    
    while (i < force->ai()->threads->count()) {
        AIStopWatchStart();
        
        AI::Thread* thread = force->ai()->threads->objectAtIndex(i);
        AILog("=========== AI force:%d process:%d time:%.3f ===========", force->index, i, _playTime);
        if (updateAI(force, thread)) {
            i++;
        }
        else {
            delete thread;
            force->ai()->threads->removeObjectAtIndex(i);
        }
        
        AIStopWatchElapsed("AI delay");
    }
}
*/

static constexpr int AIWaiting = 2;

void Map::updateAI(Force* force) {
    CSAssert(force->ai(), "invalid operation");

    if (!_running || (!force->isAlive() && force->ai()->fully)) {
        return;
    }
    if (force->ai()->lifetime) {
        force->ai()->lifetime -= _latency.frameDelayFixed;
        if (force->ai()->lifetime <= fixed::Zero) {
            force->ai()->lifetime = fixed::Zero;
            force->setControl(GameControlNone);
        }
    }
    if (force->ai()->waiting) {
        force->ai()->waiting--;
        return;
    }

    if (force->spawnCount()) {
        int slot = _random->nextInt(0, force->spawnCount() - 1);

        Spawn* spawn = force->spawn(slot);

        if (spawn) {
            CommandReturn rtn = spawn->run(NULL, false);

            const Object* target = NULL;

            switch (rtn.state) {
                case CommandStateTarget:
                    target = spawnTarget(spawn, rtn.parameter.target.condition);
                    break;
                case CommandStateRange:
                    target = spawnTarget(spawn, rtn.parameter.range.condition);
                    if (target) {
                        Destination* dest = Destination::destination(force);
                        dest->locate(target->point());
                        target = dest;
                    }
                    break;
                case CommandStateBuild:
                    synchronized(_phaseLock, GameLockMap) {
                        foreach(const Unit*, unit, _units) {
                            if (unit->force() == force && unit->compareType(ObjectMaskBase)) {
                                Frame* frame = Frame::frame(force, rtn.parameter.build.index, rtn.parameter.build.target, false);
                                FPoint point(_random->nextFixed(unit->point().x - SpawnWidth, unit->point().x + SpawnWidth), _random->nextFixed(unit->point().y - SpawnHeight, unit->point().y + SpawnHeight));
                                frame->locate(point);
                                target = frame;
                                break;
                            }
                        }
                    }
                    break;
            }
            if (target) {
                spawn->run(const_cast<Object*>(target), true);
            }
        }
    }
    force->ai()->waiting = _random->nextInt(0, AIWaiting * _latency.framePerSecond);
}
