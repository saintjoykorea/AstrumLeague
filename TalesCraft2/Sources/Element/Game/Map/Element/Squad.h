//
//  Squad.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Squad_h
#define Squad_h

#include "Unit.h"

struct Squad {
    UnitIndex index;
    FPoint* point;
	fixed range;
    Force* force;
    ushort level;
    ushort skin;
    CSArray<Unit, 1, false>* units;
    CSArray<const Growup>* growups;
    const CSString* name;
    CSArray<AbilityRevision>* abilities;
	fixed scaleBase;
	fixed scaleUp;
    
    Squad(CSBuffer* buffer);
    ~Squad();
    
    inline Squad(const Squad&) {
        CSAssert(false, "invalid operation");
    }
    inline Squad& operator =(const Squad&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* Squad_h */
