//
//  GameRecord.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 11. 23..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef GameRecord_h
#define GameRecord_h

#include "Application.h"

class GameRecord {
public:
    static uint resumeMatch();
    static bool recordMatch();
    static void expireMatch();
};

#endif /* GameRecord_h */
