//
//  Party.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 7. 18..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Party.h"

#include "Account.h"

PartySlot::PartySlot(CSBuffer* buffer) :
    _player(buffer->readBoolean() ? new User(buffer, false) : NULL),
    state((PartySlotState)buffer->readByte())
{
    
}
PartySlot::~PartySlot() {
    CSObject::release(_player);
}
PartySlot::PartySlot(const PartySlot& other) {
    memcpy(this, &other, sizeof(PartySlot));
    CSObject::retain(_player);
}
PartySlot& PartySlot::operator=(const PartySlot& other) {
    CSObject::release(_player);
    memcpy(this, &other, sizeof(PartySlot));
    CSObject::retain(_player);
    return *this;
}

Party::Party(CSBuffer* buffer) :
    partyId(retain(buffer->readString())),
    mode((GameOnlineMode)buffer->readByte()),
    mapIndex(buffer->readBoolean() ? new MapIndex(buffer) : NULL),
    message(retain(buffer->readString())),
    hostId(buffer->readLong()),
    state((PartyState)buffer->readByte()),
	_slots(retain(buffer->readArray<PartySlot>())),
	teamPlayerCount(buffer->readByte()),
    totalPlayerCount(buffer->readByte()),
	readyTimeElapsed(CSTime::currentTime())
{
	switch (state) {
		case PartyStateInit:
			matchingPlayerCount = 0;
			break;
		case PartyStateReady:
			matchingPlayerCount = _slots->count();
			break;
		case PartyStateLink:
		case PartyStatePlay:
			matchingPlayerCount = totalPlayerCount;
			break;
	}
}

Party::~Party() {
    partyId->release();
    
    if (mapIndex) delete mapIndex;
    
    release(message);
    
    _slots->release();
}

bool Party::isHost() const {
    return hostId == Account::sharedAccount()->userId;
}

bool Party::isJoined(int64 userId) const {
    foreach(const PartySlot&, slot, _slots) {
        if (slot.player() && slot.player()->userId == userId) {
            return true;
        }
    }
    return false;
}

int Party::connectedPlayerCount() const {
    int count = 0;
    foreach (const PartySlot&, slot, _slots) {
        if (slot.state != PartySlotStateNone) {
            count++;
        }
    }
    return count;
}

int Party::accountIndex() const {
    int64 userId = Account::sharedAccount()->userId;
    
    for (int i = 0; i < _slots->count(); i++) {
        const PartySlot& slot = _slots->objectAtIndex(i);
        if (slot.player() && slot.player()->userId == userId) {
            return i;
        }
    }
    return -1;
}

bool Party::canStart() const {
    if (state != PartyStateInit || !isHost()) {
        return false;
    }
    foreach (const PartySlot&, slot, _slots) {
        if (slot.state != PartySlotStateReady) {
            return false;
        }
    }
    return true;
}

void Party::enter(int index, User* player) {
    PartySlot& slot = _slots->objectAtIndex(index);
    retain(slot.player(), player);
    slot.state = state == PartyStatePlay ? PartySlotStatePlay : PartySlotStateReady;
}

void Party::exit(int index) {
    PartySlot& slot = _slots->objectAtIndex(index);
    release(slot.player());
    slot.state = PartySlotStateNone;
}
