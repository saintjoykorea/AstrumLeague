//
//  CommandData.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 26..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef CommandData_h
#define CommandData_h

#include "AssetIndex.h"

struct CommandData {
    ImageIndex icon;
    const CSLocaleString* name;
    const CSLocaleString* description;
    
    CommandData(CSBuffer* buffer);
    ~CommandData();
    
    inline CommandData(const CommandData&) {
        CSAssert(false, "invalid operation");
    }
    inline CommandData& operator=(const CommandData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

struct CommandUnitData {
    const CSArray<byte, 2>* commitTrigger;
    const CSArray<byte, 2>* actionTrigger;
    
    CommandUnitData(CSBuffer* buffer);
    ~CommandUnitData();
    
    inline CommandUnitData(const CommandUnitData&) {
        CSAssert(false, "invalid operation");
    }
    inline CommandUnitData& operator=(const CommandUnitData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* CommandData_h */
