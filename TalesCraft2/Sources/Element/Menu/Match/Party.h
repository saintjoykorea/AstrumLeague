//
//  Party.h
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef Party_h
#define Party_h

#include "User.h"

#include "AssetIndex.h"

enum PartyState : byte {
    PartyStateInit,
    PartyStateReady,
    PartyStateLink,
    PartyStatePlay
};

enum PartySlotState : byte {
	PartySlotStateNone,
	PartySlotStateReady,
	PartySlotStatePlay
};

struct PartySlot {
private:
    User* _player;
public:
	PartySlotState state;
    
    PartySlot(CSBuffer* buffer);
    ~PartySlot();
    
    PartySlot(const PartySlot& other);
    PartySlot& operator=(const PartySlot& other);
    
    inline User*& player() {
        return _player;
    }
    inline const User* player() const {
        return _player;
    }
};

class Party : public CSObject {
public:
    const CSString* partyId;
    GameOnlineMode mode;
    const MapIndex* mapIndex;
    const CSString* message;
    int64 hostId;
    PartyState state;
private:
	CSArray<PartySlot>* _slots;
public:
	byte teamPlayerCount;
    byte totalPlayerCount;
	byte matchingPlayerCount;
	double readyTimeElapsed;
public:
    Party(CSBuffer* buffer);
private:
    ~Party();
public:
    static inline Party* partyWithBuffer(CSBuffer* buffer) {
        return autorelease(new Party(buffer));
    }
    
    bool isHost() const;
    
    inline CSArray<PartySlot>* slots() {
        return _slots;
    }
    inline const CSArray<PartySlot>* slots() const {
        return _slots;
    }
    
    bool isJoined(int64 userId) const;

    int connectedPlayerCount() const;
    
    int accountIndex() const;

    bool canStart() const;

	inline int waitingTime() const {
		return CSTime::currentTime() - readyTimeElapsed;
	}
    
    void enter(int index, User* player);
    void exit(int index);
};

#endif /* Party_h */
