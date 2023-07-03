//
//  PingData.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 9. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef PingData_h
#define PingData_h

#include "AssetIndex.h"

struct PingData {
    ImageIndex icon;
    AnimationIndex animation;
    CSColor color;
    const CSLocaleString* message;
    
    PingData(CSBuffer* buffer);
    ~PingData();
    
    inline PingData(const PingData& other) {
        CSAssert(false, "invalid operation");
    }
    inline PingData& operator=(const PingData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* PingData_h */
