//
//  CSTeamSpeak+callback.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 12. 12..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSTeamSpeak_callback_h
#define CSTeamSpeak_callback_h

#include "teamspeak/clientlib.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_definitions.h"

#include "CSTypes.h"

void onConnectStatusChangeEvent(uint64 serverConnectionHandlerId, int status, uint error);
void onNewChannelCreatedEvent(uint64 serverConnectionHandlerId, uint64 channelId, uint64 channelParentId, ushort invokerId, const char* invokerName, const char* invokerUniqueIdentifier);
void onClientMoveEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility, const char* moveMessage);
void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility);
void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerId, ushort clientId, uint64 oldChannelId, uint64 newChannelId, int visibility, const char* timeoutMessage);
void onTalkStatusChangeEvent(uint64 serverConnectionHandlerId, int status, int isReceivedWhisper, ushort clientId);
void onServerErrorEvent(uint64 serverConnectionHandlerId, const char* errorMessage, uint error, const char* returnCode, const char* extraMessage);

#endif /* CSTeamSpeak_callback_h */
