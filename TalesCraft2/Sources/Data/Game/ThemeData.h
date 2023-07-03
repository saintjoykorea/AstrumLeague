//
//  ThemeData.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 5. 24..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef ThemeData_h
#define ThemeData_h

#include "AssetIndex.h"

struct ThemeData {
    ImageIndex icon;
    const CSLocaleString* name;
    const CSDictionary<UnitIndex, ushort>* skins;
    
    ThemeData(CSBuffer* buffer);
    ~ThemeData();
    
    inline ThemeData(const ThemeData&) {
        CSAssert(false, "invalid operation");
    }
    inline ThemeData& operator=(const ThemeData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* ThemeData_h */
