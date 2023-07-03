//
//  CSTeamSpeak+callback.cpp
//  Full
//
//  Created by Kim Chan on 2018. 12. 12..
//  Copyright © 2018년 brgames. All rights reserved.
//

#define CDK_IMPL

#include "CSThread.h"

#include "CSTeamSpeak+callback.h"
#include "CSTeamSpeak.h"

static void _onConnectStatusChangeEvent(uint64 serverConnectionHandlerId, int status, uint error) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onConnectStatusChangeEvent(serverConnectionHandlerId, status, error);
    }
}
void onConnectStatusChangeEvent(uint64 serverConnectionHandlerId, int status, uint error) {
    CSLog("onConnectStatusChangeEvent:%" PRIu64 " status:%d error:%d", serverConnectionHandlerId, status, error);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onConnectStatusChangeEvent(serverConnectionHandlerId, status, error);
    }
    else {
        CSInvocation3<void, uint64, int, uint>* inv = new CSInvocation3<void, uint64, int, uint>(&_onConnectStatusChangeEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(status);
        inv->setParam2(error);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
static void _onNewChannelCreatedEvent(uint64 serverConnectionHandlerId, uint64 ChannelId, ushort invokerId) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onNewChannelCreatedEvent(serverConnectionHandlerId, ChannelId, invokerId);
    }
}
void onNewChannelCreatedEvent(uint64 serverConnectionHandlerId, uint64 channelId, uint64 channelParentId, ushort invokerId, const char* invokerName, const char* invokerUniqueIdentifier) {
    CSLog("onNewChannelCreatedEvent:%" PRIu64 " channelId:%" PRIu64 " channelParentId:%" PRIu64 " invokerId:%d invokerName:%s invokerUniqueIdentifier:%s", serverConnectionHandlerId, channelId, channelParentId, invokerId, invokerName, invokerUniqueIdentifier);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onNewChannelCreatedEvent(serverConnectionHandlerId, channelId, invokerId);
    }
    else {
        CSInvocation3<void, uint64, uint64, ushort>* inv = new CSInvocation3<void, uint64, uint64, ushort>(&_onNewChannelCreatedEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(channelId);
        inv->setParam2(invokerId);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
static void _onClientMoveEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 newChannelId) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onClientMoveEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
}
void onClientMoveEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility, const char* moveMessage) {
    CSLog("onClientMoveEvent:%" PRIu64 " clientId:%d oldChannelId:%" PRIu64 " newChannelId:%" PRIu64 " visibility:%d moveMessage:%s", serverConnectionHandlerId, clientId, oldChannelId, newChannelId, visibility, moveMessage);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onClientMoveEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
    else {
        CSInvocation3<void, uint64, ushort, uint64>* inv = new CSInvocation3<void, uint64, ushort, uint64>(&_onClientMoveEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(clientId);
        inv->setParam2(newChannelId);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
static void _onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 newChannelId) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onClientMoveSubscriptionEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
}
void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility) {
    CSLog("onClientMoveSubscriptionEvent:%" PRIu64 " clientId:%d oldChannelId:%" PRIu64 " newChannelId:%" PRIu64 " visibility:%d", serverConnectionHandlerId, clientId, oldChannelId, newChannelId, visibility);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onClientMoveSubscriptionEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
    else {
        CSInvocation3<void, uint64, ushort, uint64>* inv = new CSInvocation3<void, uint64, ushort, uint64>(&_onClientMoveSubscriptionEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(clientId);
        inv->setParam2(newChannelId);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
static void _onClientMoveTimeoutEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 newChannelId) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onClientMoveTimeoutEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
}
void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility, const char* timeoutMessage) {
    CSLog("onClientMoveTimeoutEvent:%" PRIu64 " clientId:%d oldChannelId:%" PRIu64 " newChannelId:%" PRIu64 " visibility:%d timeoutMessage:%s", serverConnectionHandlerId, clientId, oldChannelId, newChannelId, visibility, timeoutMessage);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onClientMoveTimeoutEvent(serverConnectionHandlerId, clientId, newChannelId);
    }
    else {
        CSInvocation3<void, uint64, ushort, uint64>* inv = new CSInvocation3<void, uint64, ushort, uint64>(&_onClientMoveTimeoutEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(clientId);
        inv->setParam2(newChannelId);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
static void _onTalkStatusChangeEvent(uint64 serverConnectionHandlerId, int status, ushort clientId) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onTalkStatusChangeEvent(serverConnectionHandlerId, status, clientId);
    }
}
void onTalkStatusChangeEvent(uint64 serverConnectionHandlerId, int status, int isReceivedWhisper, ushort clientId) {
    CSLog("onTalkStatusChangeEvent:%" PRIu64 " status:%d isReceivedWhisper:%d clientId:%d", serverConnectionHandlerId, status, isReceivedWhisper, clientId);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onTalkStatusChangeEvent(serverConnectionHandlerId, status, clientId);
    }
    else {
        CSInvocation3<void, uint64, int, ushort>* inv = new CSInvocation3<void, uint64, int, ushort>(&_onTalkStatusChangeEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(status);
        inv->setParam2(clientId);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}

static void _onServerErrorEvent(uint64 serverConnectionHandlerId, uint error) {
    CSTeamSpeak* teamSpeak = CSTeamSpeak::sharedTeamSpeak();
    if (teamSpeak) {
        teamSpeak->onServerErrorEvent(serverConnectionHandlerId, error);
    }
}
void onServerErrorEvent(uint64 serverConnectionHandlerId, const char* errorMessage, uint error, const char* returnCode, const char* extraMessage) {
    CSLog("onServerErrorEvent:%" PRIu64 " errorMessage:%s error:%d returnCode:%s extraMessage:%s", serverConnectionHandlerId, errorMessage, error, returnCode, extraMessage);
    
    CSThread* thread = CSThread::currentThread();
    if (thread && thread->isMainThread()) {
        _onServerErrorEvent(serverConnectionHandlerId, error);
    }
    else {
        CSInvocation2<void, uint64, uint>* inv = new CSInvocation2<void, uint64, uint>(&_onServerErrorEvent);
        inv->setParam0(serverConnectionHandlerId);
        inv->setParam1(error);
        CSTask* task = new CSTask(inv);
        CSThread::mainThread()->start(task);
        task->release();
        inv->release();
    }
}
