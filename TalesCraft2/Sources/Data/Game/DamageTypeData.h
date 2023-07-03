//
//  DamageTypeData.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 5. 4..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef DamageTypeData_h
#define DamageTypeData_h

#include "AssetIndex.h"

struct DamageTypeData {
    ImageIndex icon;
    const CSLocaleString* name;
    
    DamageTypeData(CSBuffer* buffer);
    ~DamageTypeData();
    
    inline DamageTypeData(const DamageTypeData&) {
        CSAssert(false, "invalid operation");
    }
    inline DamageTypeData& operator=(const DamageTypeData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* DamageTypeData_h */
