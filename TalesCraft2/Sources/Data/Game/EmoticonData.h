//
//  EmoticonData.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 9. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef EmoticonData_h
#define EmoticonData_h

#include "AssetIndex.h"

struct EmoticonData {
    ImageIndex icon;
    AnimationIndex animation;
    const CSLocaleString* message;
	const CSLocaleString* name;
    
    EmoticonData(CSBuffer* buffer);
    ~EmoticonData();
    
    inline EmoticonData(const EmoticonData& other) {
        CSAssert(false, "invalid operation");
    }
    inline EmoticonData& operator=(const EmoticonData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* EmoticonData_h */
