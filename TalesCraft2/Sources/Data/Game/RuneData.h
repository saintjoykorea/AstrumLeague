//
//  RuneData.h
//  TalesCraft2
//
//  Created by ChangSun on 2017. 1. 18..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef RuneData_h
#define RuneData_h

#include "AssetIndex.h"

#include "AbilityData.h"

struct RuneData {
    ImageIndex smallIcon;
	ImageIndex largeIcon;
    const CSLocaleString* name;
	const CSLocaleString* prefix;
    ushort resources[2];
    ushort progress;
    byte grade;
	byte tier;
	byte valuation;
	const CSArray<byte>* target;
    AbilityUnitData ability;
    
    RuneData(CSBuffer* buffer);
    ~RuneData();
    
    inline RuneData(const RuneData& other) : ability(other.ability) {
        CSAssert(false, "invalid operation");
    }
    inline RuneData& operator=(const RuneData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
	bool match(const UnitBase* unit) const;
};

#endif /* RuneData_h */
