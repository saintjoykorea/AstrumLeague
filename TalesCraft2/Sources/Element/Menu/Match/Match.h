//
//  Match.h
//  TalesCraft2
//
//  Created by 김찬 on 2014. 12. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef Match_h
#define Match_h

#include "Player.h"

#include "Asset.h"

enum MatchState : byte  {
    MatchStateInit,
    MatchStateSync,
    MatchStatePlay,
	MatchStatePause,
    MatchStateEnd
};

struct MatchSlot {
private:
    Player* _player;
public:
    GameControl control;
    byte alliance;
    
    MatchSlot(CSBuffer* buffer);
    ~MatchSlot();
    
    MatchSlot(const MatchSlot& other);
    MatchSlot& operator=(const MatchSlot& other);
    
    inline Player*& player() {
        return _player;
    }
    inline const Player* player() const {
        return _player;
    }
};

class Match : public CSObject {
public:
    uint matchId;
    GameOnlineMode mode;
    GameLevelCorrection levelCorrection;
    ushort maxCardLevel;
    ushort maxRuneLevel;
    ushort observerDelay;
    MapIndex mapIndex;
    const CSString* message;
    int64 hostId;
    MatchState state;
    uint randomSeed;
    int64 timestamp;
private:
    CSArray<MatchSlot>* _slots;
    CSArray<User>* _observers;
public:
	Match(CSBuffer* buffer);
private:
    ~Match();
public:
    static inline Match* matchWithBuffer(CSBuffer* buffer) {
        return autorelease(new Match(buffer));
    }
    
    inline CSArray<MatchSlot>* slots() {
        return _slots;
    }
    inline const CSArray<MatchSlot>* slots() const {
        return _slots;
    }
    inline CSArray<User>* observers() {
        return _observers;
    }
    inline const CSArray<User>* observers() const {
        return _observers;
    }
    inline const MapThumb* map() const {
        return Asset::sharedAsset()->map(mapIndex);
    }
    
    bool isJoined(int64 userId) const;
    
    bool isHost() const;
    
    bool canStart() const;
    
    int accountIndex() const;
    
    void swap(int fromIndex, int toIndex);
    
    bool isTeamPlaying() const;

	bool isFreeForAll() const;
};

#endif /* Match_h */
