//
//  Growup.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 12..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef Growup_h
#define Growup_h

#include "AssetIndex.h"

struct AbilityRevision {
    ushort index;
    ushort conditionLength;
    const byte* condition;
    fixed value;
    fixed diff;
};

enum DropExpOp : byte {
	DropExpOpNone,
	DropExpOpPopulation
};

struct DropUnit {
    UnitIndex index;
    ushort level;
    ushort skin;
    fixed rate;
    
    inline DropUnit(const byte*& cursor) :
        index(cursor),
        level(readShort(cursor)),
        skin(readShort(cursor)),
        rate(readFixed(cursor))
    {
    
    }
};

class Growup : public CSObject {
public:
    ushort maxLevel;
    ushort levelupExp;
	DropExpOp dropExpOp;
    ushort dropExpBase;
	ushort dropExpUp;
    const ushort rewardResources[2];
    const CSArray<DropUnit>* dropUnits;
    const AnimationIndex* levelupEffect;
    const AnimationIndex* dropEffect;
    
    Growup(const byte*& cursor);
private:
    ~Growup();
public:
    static inline Growup* growup(const byte*& cursor) {
        return autorelease(new Growup(cursor));
    }
};

#endif /* Growup_h */
