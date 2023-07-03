//
//  BuffData.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 2..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef BuffData_h
#define BuffData_h

#include "AssetIndex.h"

#include "AbilityData.h"

struct BuffData {
    fixed maxStack;
    AbilityUnitData ability;
    const CSArray<byte, 2>* trigger;
    
    BuffData(CSBuffer* buffer);
    ~BuffData();
    
    inline BuffData(const BuffData& other) : ability(other.ability) {
        CSAssert(false, "invalid operation");
    }
    inline BuffData& operator=(const BuffData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};


#endif /* BuffData_h */
