//
//  Map+stream.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 22..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "Destination.h"

#include "Frame.h"

#include "PVPGateway.h"

enum StreamOp {
    ST_Control,
    ST_Latency,
    ST_Command,
    ST_CancelBuildingCommand,
    ST_CancelProducingCommand,
    ST_CancelTrainingCommand,
    ST_UnloadCommand,
    ST_Spawn,
    ST_Step,
    ST_Chat = 11,
    ST_Emoticon,
    ST_Ping
};

void Map::writeUnits(CSBuffer *buffer, const CSArray<Unit> *units) {
    buffer->writeLength(units->count());
    foreach(const Unit*, unit, units) {
        buffer->writeInt(unit->key);
    }
}
void Map::writeTarget(CSBuffer* buffer, const Object* target) {
    if (!target) {
        buffer->writeByte(0);
    }
    else if (target->compareType(ObjectMaskUnit)) {
        buffer->writeByte(2);
        buffer->writeInt(static_cast<const Unit*>(target)->key);
    }
    else if (target->compareType(ObjectMaskResource)) {
        buffer->writeByte(3);
        buffer->writeInt(static_cast<const Resource*>(target)->key);
    }
    else if (target->compareType(ObjectMaskFrame)) {
        buffer->writeByte(4);
        
        const Frame* frame = static_cast<const Frame*>(target);
        buffer->writeByte(frame->force->index);
        frame->index.writeTo(buffer);
        
        const FPoint& p = frame->point();
        buffer->writeFixed(p.x);
        buffer->writeFixed(p.y);
        buffer->writeInt(p.slabId);
        
        buffer->writeByte(frame->target);
    }
    else {
        buffer->writeByte(1);
        
        const FPoint& p = target->point();
        buffer->writeFixed(p.x);
        buffer->writeFixed(p.y);
        buffer->writeInt(p.slabId);
        buffer->writeFloat(target->radius());
    }
}

void Map::writeControl(CSBuffer* buffer, GameControl control) {
    buffer->writeByte(ST_Control);
    buffer->writeByte(control);
}
void Map::writeLatency(CSBuffer* buffer, int framePerSecond) {
    buffer->writeByte(ST_Latency);
    buffer->writeByte(framePerSecond);
}

void Map::writeCommand(CSBuffer *buffer, const CSArray<Unit> *units, const CommandIndex& index, const Object *target, const FPoint* point) {
    buffer->writeByte(ST_Command);
    writeUnits(buffer, units);
    index.writeTo(buffer);
    writeTarget(buffer, target);
    buffer->writeBoolean(point != NULL);
    if (point) {
        buffer->writeFixed(point->x);
        buffer->writeFixed(point->y);
        buffer->writeInt(point->slabId);
    }
}
void Map::writeCancelBuildingCommand(CSBuffer *buffer, const Unit *unit) {
    buffer->writeByte(ST_CancelBuildingCommand);
    buffer->writeInt(unit->key);
}
void Map::writeCancelProducingCommand(CSBuffer* buffer, const CSArray<Unit>* units, const UnitIndex& index) {
    buffer->writeByte(ST_CancelProducingCommand);
    buffer->writeBoolean(true);
    writeUnits(buffer, units);
    index.writeTo(buffer);
}
void Map::writeCancelProducingCommand(CSBuffer* buffer, const Unit* unit, int slot) {
    buffer->writeByte(ST_CancelProducingCommand);
    buffer->writeBoolean(false);
    buffer->writeInt(unit->key);
    buffer->writeByte(slot);
}
void Map::writeCancelTrainingCommand(CSBuffer* buffer, const Unit* unit, int slot) {
    buffer->writeByte(ST_CancelTrainingCommand);
    buffer->writeInt(unit->key);
    buffer->writeByte(slot);
}
void Map::writeUnloadCommand(CSBuffer* buffer, const Unit* unit, int slot) {
    buffer->writeByte(ST_UnloadCommand);
    buffer->writeInt(unit->key);
    buffer->writeByte(slot);
}
void Map::writeSpawn(CSBuffer *buffer, int slot, const Object* target) {
    buffer->writeByte(ST_Spawn);
    buffer->writeByte(slot);
    writeTarget(buffer, target);
}
void Map::writeStep(CSBuffer* buffer, int op) {
    buffer->writeByte(ST_Step);
	buffer->writeByte(op);
}
void Map::writeEmoticon(CSBuffer* buffer, const EmoticonIndex& index) {
    buffer->writeByte(ST_Emoticon);
    index.writeTo(buffer);
}
void Map::writePing(CSBuffer* buffer, int index, const CSVector2& point) {
    buffer->writeByte(ST_Ping);
    buffer->writeShort(index);
    buffer->writeFloat(point.x);
    buffer->writeFloat(point.y);
}

void Map::write(int framePerStep, uint frame, float serverDelay, const void *data, uint length) {
	//CSLog("write:%d, %d, %d", _mode.context.online.maxFrame, frame, framePerStep);		//TODO:DEBUG

    CSAssert(_mode.type == GameModeTypeOnline && frame >= _mode.context.online.maxFrame, "invalid operation");

    if (length) {
        synchronized(_mode.context.online.globalBuffer, GameLockMap) {
            _mode.context.online.globalBuffer->writeInt(frame);
            _mode.context.online.globalBuffer->write(data, length);
        }
    }
	_mode.context.online.framePerStep = framePerStep;
	_mode.context.online.maxFrame = frame + framePerStep;

    double currentTime = CSTime::currentTime();
    if (_mode.context.online.commandElapsed) {
        float clientDelay = currentTime - _mode.context.online.commandElapsed;
        float latency = serverDelay - clientDelay;
        float decrement = PVPLatencyDecrementPerSecond * framePerStep / _latency.framePerSecond;
        _mode.context.online.latency = CSMath::clamp(_mode.context.online.latency + latency - decrement, 0.0f, PVPMaxLatency);
        if (latency < 0) latency = -latency;
        if (_mode.context.online.currentLatency < latency) _mode.context.online.currentLatency = latency;
        _mode.context.online.totalLatency += latency;
        _mode.context.online.totalLatencyCount++;
#ifdef UsePVPTest
        _mode.context.online.stat.response += latency;
        CSLog("pvp command:server:%.3f client:%.3f latency:%.3f / %.3f", serverDelay, clientDelay, latency, _mode.context.online.latency);
#endif
    }
    _mode.context.online.commandElapsed = currentTime;
}

CSArray<Unit>* Map::readUnits(CSBuffer* buffer, const Force* force, uint endpos) {
    if (buffer->position() + 4 > endpos) {
        return NULL;
    }
    uint count = buffer->readLength();
    
    if (buffer->position() + 4 * count > endpos) {
        return NULL;
    }
    
    CSArray<Unit>* units = CSArray<Unit>::arrayWithCapacity(count);
    
    for (int i = 0; i < count; i++) {
        uint key = buffer->readInt();
        
        Unit* unit = findUnit(key);
#ifdef UseObserverPlayTest
        if (unit)
#else
        if (unit && unit->force() == force)
#endif
        {
            units->addObject(unit);
        }
#ifdef CS_CONSOLE_DEBUG
        else {
            CSWarnLog("invalid command unit:%" PRId64, force->player->userId);
        }
#endif
    }
    return units;
}
bool Map::readTarget(CSBuffer* buffer, const Force* force, uint endpos, Object*& target) {
    target = NULL;
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    switch (buffer->readByte()) {
        case 1:
            {
                if (buffer->position() + 24 > endpos) {
                    return false;
                }
				FPoint point(buffer);
				point.slabId = buffer->readInt();
                if (point.x < fixed::Zero || point.x >= _terrain->width() || point.y < fixed::Zero || point.y >= _terrain->height()) {
                    return false;
                }
                target = Destination::destination(force, buffer->readFloat());
                target->locate(point);
            }
            break;
        case 2:
            if (buffer->position() + 4 > endpos) {
                return false;
            }
            target = findUnit(buffer->readInt());
#ifdef CS_CONSOLE_DEBUG
            if (!target) {
                CSWarnLog("invalid target unit:%" PRId64, force->player->userId);
            }
#endif
            break;
        case 3:
            if (buffer->position() + 4 > endpos) {
                return false;
            }
            target = findResource(buffer->readInt());
#ifdef CS_CONSOLE_DEBUG
            if (!target) {
                CSWarnLog("invalid target resource:%" PRId64, force->player->userId);
            }
#endif
            break;
        case 4:
            {
                int targetForceIndex = buffer->readByte();
                if (targetForceIndex < 0 || targetForceIndex >= MaxPlayers) {
                    return false;
                }
                const Force* targetForce = _forces[targetForceIndex];
                if (!targetForce || buffer->position() + 25 > endpos) {
                    return false;
                }
                UnitIndex index(buffer);
                if (index.indices[0] >= Asset::sharedAsset()->units->count() || index.indices[1] >= Asset::sharedAsset()->units->objectAtIndex(index.indices[0])->count()) {
                    return false;
                }
				FPoint point(buffer);
				point.slabId = buffer->readInt();
                if (point.x < fixed::Zero || point.x >= _terrain->width() || point.y < fixed::Zero || point.y >= _terrain->height()) {
                    return false;
                }
                target = Frame::frame(targetForce, index, (CommandBuildTarget)buffer->readByte(), true);
                target->locate(point);
            }
            break;
    }
    return true;
}
bool Map::readControl(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    GameControl control = (GameControl)buffer->readByte();
    force->setControl(control);
    return true;
}
bool Map::readLatency(CSBuffer *buffer, uint endpos) {
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    int framePerSecond = buffer->readByte();
    if (framePerSecond < GameMinFramePerSecond || framePerSecond > GameMaxFramePerSecond) {
        return false;
    }
    _latency.framePerSecond = framePerSecond;
    _latency.frameDelayFloat = _latency.frameDelayFixed = fixed::One / _latency.framePerSecond;
    CSLog("receive frame per second:%d", _latency.framePerSecond);
    return true;
}
bool Map::readCommand(CSBuffer* buffer, Force* force, uint endpos) {
    CSArray<Unit>* units = readUnits(buffer, force, endpos);
    if (!units || buffer->position() + 4 > endpos) {
        return false;
    }
    CommandIndex index(buffer);
    Object* target;
    if (!readTarget(buffer, force, endpos, target)) {
        return false;
    }
    FPoint* point = NULL;
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    if (buffer->readBoolean()) {
        if (buffer->position() + 20 > endpos) {
            return false;
        }
		point = new FPoint(buffer);
		point->slabId = buffer->readInt();
        
        if (point->x < fixed::Zero || point->x >= _terrain->width() || point->y < fixed::Zero || point->y >= _terrain->height()) {
            delete point;
            
            return false;
        }
    }
    commandImpl(units, index, target, point, 0, CommandCommitRun);

    if (point) {
        delete point;
    }
    return true;
}
bool Map::readCancelBuildingCommand(CSBuffer *buffer, Force* force, uint endpos) {
    if (buffer->position() + 4 > endpos) {
        return false;
    }
    Unit* unit = findUnit(buffer->readInt());
    
#ifdef UseObserverPlayTest
    if (unit)
#else
    if (unit && unit->force() == force)
#endif
    {
        unit->cancelBuildingCommand();
    }
    return true;
}
bool Map::readCancelProducingCommand(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    if (buffer->readBoolean()) {
        CSArray<Unit>* units = readUnits(buffer, force, endpos);
        if (!units || buffer->position() + 4 > endpos) {
            return false;
        } 
        UnitIndex index(buffer);
        
        for (int i = MaxWorking - 1; i >= 0; i--) {
            foreach(Unit*, unit, units) {
                if (unit->cancelProducingCommand(index)) {
                    goto rtn;
                }
            }
        }
    }
    else {
        if (buffer->position() + 5 > endpos) {
            return false;
        }
        Unit* unit = findUnit(buffer->readInt());
        int slot = buffer->readByte();
        if (slot < 0 || slot >= MaxWorking) {
            return false;
        }
#ifdef UseObserverPlayTest
        if (unit)
#else
        if (unit && unit->force() == force)
#endif
        {
            unit->cancelProducingCommand(slot);
        }
    }
rtn:
    return true;
}
bool Map::readCancelTrainingCommand(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 5 > endpos) {
        return false;
    }
    Unit* unit = findUnit(buffer->readInt());
    int slot = buffer->readByte();
    if (slot < 0 || slot >= MaxWorking) {
        return false;
    }
#ifdef UseObserverPlayTest
    if (unit)
#else
    if (unit && unit->force() == force)
#endif
    {
        unit->cancelTrainingCommand(slot);
    }
    return true;
}
bool Map::readUnloadCommand(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 5 > endpos) {
        return false;
    }
    Unit* unit = findUnit(buffer->readInt());
    int slot = buffer->readByte();
    if (slot < 0 || slot >= MaxTransporting) {
        return false;
    }
#ifdef UseObserverPlayTest
    if (unit)
#else
    if (unit && unit->force() == force)
#endif
    {
        unit->unloadCommand(slot);
    }
    return true;
}
bool Map::readSpawn(CSBuffer *buffer, Force* force, uint endpos) {
    if (buffer->position() + 1 > endpos) {
        return false;
    }
    int slot = buffer->readByte();
    if (slot < 0) {
        return false;
    }
    Object* target;
    if (!readTarget(buffer, force, endpos, target)) {
        return false;
    }
    if (slot < force->spawnCount()) {
        if (force->spawn(slot)->run(target, true).state == CommandStateEnabled) {
            force->rollSpawn(slot);     //TODO:딜레이 있을 것.
        }
    }
    return true;
}
bool Map::readStep(CSBuffer* buffer, Force* force, uint endpos) {
	if (buffer->position() + 1 > endpos) {
		return false;
	}
	
	int op = buffer->readByte();

	switch (op) {
		case 0:
			if (_speech) {
				_speech->confirm = true;
			}
			break;
		case 1:
			_setting->skipCommit = true;
			break;
	}
	return true;
}
bool Map::readChat(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 2 > endpos) {
        return false;
    }
    int position = buffer->position();
    int length = buffer->readLength();
    if (buffer->position() + length + 1 > endpos) {
        buffer->setPosition(position);
        return false;
    }
    buffer->setPosition(position);
    
    const CSString* msg = buffer->readString();
    bool toAlliance = buffer->readBoolean();
    
    if (!toAlliance || !_force || _force->alliance() == force->alliance()) {
        chat(force, force->player, msg, toAlliance);
    }
    return true;
}
bool Map::readEmoticon(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 4 > endpos) {
        return false;
    }
    EmoticonIndex index(buffer);
    
    const CSArray<EmoticonData, 2>* arr = Asset::sharedAsset()->emoticons;
    if (!arr || index.indices[0] < 0 || index.indices[0] >= arr->count()) {
        return false;
    }
    
    const CSArray<EmoticonData>* subarr = Asset::sharedAsset()->emoticons->objectAtIndex(index.indices[0]);
    if (!subarr || index.indices[1] < 0 || index.indices[1] >= subarr->count()) {
        return false;
    }
    emoticon(force, index);
    return true;
}

bool Map::readPing(CSBuffer* buffer, Force* force, uint endpos) {
    if (buffer->position() + 10 > endpos) {
        return false;
    }
    
    int index = buffer->readShort();
    
    const CSArray<PingData>* arr = Asset::sharedAsset()->pings;
    if (!arr || index < 0 || index >= arr->count()) {
        return false;
    }
    
    CSVector2 point(buffer);
    
    ping(force, index, point);
    
    return true;
}

void Map::read(CSBuffer* buffer) {
    while (buffer->remaining() > 4) {
        uint frame = buffer->readInt();
        
        CSAssert(_frame <= frame, "invalid state");
        
        if (_frame >= frame) {
            for (; ;) {
                uint length = buffer->readLength();
                if (!length) {
                    break;
                }
                uint endpos = buffer->position() + length;
                
                int forceIndex = buffer->readByte();
                
                Force* force;
                if (forceIndex >= 0 && forceIndex < MaxPlayers && (force = _forces[forceIndex])) {
                    //======================================================================
                    //여기는 클라이언트에서 보내는 데이터이므로 신뢰할 수 없다. 모든 에러 체크 실시
                    
                    while (buffer->position() < endpos) {
                        StreamOp code = (StreamOp)buffer->readByte();
                        
                        SyncLog("Sync (%d) (Stream):%d", _frame, code);
                        
                        bool flag;
                        
                        switch (code) {
                            case ST_Control:
                                flag = readControl(buffer, force, endpos);
                                break;
                            case ST_Latency:
                                flag = readLatency(buffer, endpos);
                                break;
                            case ST_Command:
                                flag = readCommand(buffer, force, endpos);
                                break;
                            case ST_CancelBuildingCommand:
                                flag = readCancelBuildingCommand(buffer, force, endpos);
                                break;
                            case ST_CancelProducingCommand:
                                flag = readCancelProducingCommand(buffer, force, endpos);
                                break;
                            case ST_CancelTrainingCommand:
                                flag = readCancelTrainingCommand(buffer, force, endpos);
                                break;
                            case ST_UnloadCommand:
                                flag = readUnloadCommand(buffer, force, endpos);
                                break;
                            case ST_Spawn:
                                flag = readSpawn(buffer, force, endpos);
                                break;
                            case ST_Step:
								flag = readStep(buffer, force, endpos);
                                break;
                            case ST_Chat:
                                flag = readChat(buffer, force, endpos);
                                break;
                            case ST_Emoticon:
                                flag = readEmoticon(buffer, force, endpos);
                                break;
                            case ST_Ping:
                                flag = readPing(buffer, force, endpos);
                                break;
                            default:
                                flag = false;
                                break;
                        }
                        if (!flag) {
                            CSErrorLog("parse error:%d", code);
                            break;
                        }
                    }
                    //======================================================================
                }
                
                buffer->setPosition(endpos);
            }
        }
        else {
            buffer->setPosition(buffer->position() - 4);
            break;
        }
    }
}
