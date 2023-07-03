//
//  Player.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 27..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef Player_h
#define Player_h

#include "User.h"

#include "ForceBase.h"

class Player : public User, public ForceBase {
public:
    ThemeIndex deckTheme;
protected:
    Player() = default;
    Player(CSBuffer* buffer, bool dummy);
    Player(const CSJSONObject* json);
    virtual ~Player() = default;
public:
    static Player* createForTraining();
    static Player* createWithBuffer(CSBuffer* buffer, bool dummy);
    static Player* createWithJSON(const CSJSONObject* json);
    
    static Player* playerForTraining();
    static Player* playerWithBuffer(CSBuffer* buffer, bool dummy);
    static Player* playerWithJSON(const CSJSONObject* json);
    
    void writeTo(CSBuffer* buffer, bool containsDeck) const;
};

#endif /* Player_h */

