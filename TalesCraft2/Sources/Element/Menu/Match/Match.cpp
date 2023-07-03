//
//  Match.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "Match.h"

#include "Account.h"

MatchSlot::MatchSlot(CSBuffer* buffer) :
    _player(buffer->readBoolean() ? Player::createWithBuffer(buffer, false) : NULL),
    control((GameControl)buffer->readByte()),
    alliance(buffer->readByte())
{

}
MatchSlot::~MatchSlot() {
    CSObject::release(_player);
}
MatchSlot::MatchSlot(const MatchSlot& other) {
    memcpy(this, &other, sizeof(MatchSlot));
    CSObject::retain(_player);
}
MatchSlot& MatchSlot::operator=(const MatchSlot& other) {
    CSObject::release(_player);
    memcpy(this, &other, sizeof(MatchSlot));
    CSObject::retain(_player);
    return *this;
}

Match::Match(CSBuffer* buffer) :
    matchId(buffer->readInt()),
    mode((GameOnlineMode)buffer->readByte()),
    levelCorrection((GameLevelCorrection)buffer->readByte()),
    maxCardLevel(buffer->readShort()),
    maxRuneLevel(buffer->readShort()),
    observerDelay(buffer->readShort()),
    mapIndex(buffer),
    message(retain(buffer->readString())),
    hostId(buffer->readLong()),
    state((MatchState)buffer->readByte()),
    randomSeed(buffer->readInt()),
    _slots(retain(buffer->readArray<MatchSlot>())),
    _observers(retain(buffer->readArrayWithParam<User>(false, false)))
{
    CSAssert(_slots, "invalid slot");
}
Match::~Match() {
    release(message);
    
    _slots->release();
    _observers->release();
}

bool Match::isJoined(int64 userId) const {
    foreach(const MatchSlot&, slot, _slots) {
        if (slot.player() && slot.player()->userId == userId) {
            return true;
        }
    }
    foreach(const User*, observer, _observers) {
        if (observer->userId == userId) {
            return true;
        }
    }
    return false;
}

bool Match::isHost() const {
    return hostId == Account::sharedAccount()->userId;
}

bool Match::canStart() const {
    if (state != MatchStateInit || !isHost()) {
        return false;
    }
    for (int i = 0; i < MaxAlliances; i++) {
        bool flag = true;
        foreach(const MatchSlot&, slot, _slots) {
            if (slot.alliance == i) {
                flag = false;
                if (slot.player()) {
                    flag = true;
                    break;
                }
            }
        }
        if (!flag) {
            return false;
        }
    }
    return true;
}

int Match::accountIndex() const {
    int64 userId = Account::sharedAccount()->userId;
    
    for (int i = 0; i < _slots->count(); i++) {
        const MatchSlot& slot = _slots->objectAtIndex(i);
        if (slot.control == GameControlUser && slot.player() && slot.player()->userId == userId) {
            return i;
        }
    }
    return -1;
}

void Match::swap(int fromIndex, int toIndex) {
    MatchSlot& fromSlot = _slots->objectAtIndex(fromIndex);
    MatchSlot& toSlot = _slots->objectAtIndex(toIndex);
    
    Player* player = fromSlot.player();
    GameControl control = fromSlot.control;
    
    fromSlot.player() = toSlot.player();
    fromSlot.control = toSlot.control;
    
    toSlot.player() = player;
    toSlot.control = control;
}

bool Match::isTeamPlaying() const {
    int index = accountIndex();
    
    if (index >= 0) {
        int allianceCount = 0;
        int alliance = _slots->objectAtIndex(index).alliance;
        for (int i = 0; i < _slots->count(); i++) {
            const MatchSlot& slot = _slots->objectAtIndex(i);
            if (slot.player() && slot.alliance == alliance) {
                allianceCount++;
            }
        }
        if (allianceCount >= 2) {
            return true;
        }
    }
    return false;
}

bool Match::isFreeForAll() const {
	int allianceCount[2] = {};
	foreach (const MatchSlot&, slot, _slots) {
		if (slot.alliance >= 2) {
			return true;
		}
		allianceCount[slot.alliance]++;
	}
	return allianceCount[0] != allianceCount[1];
}
