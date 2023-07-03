//
//  ResourceData.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 4. 2..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef ResourceData_h
#define ResourceData_h

#include "AssetIndex.h"

struct ResourceLevelData {
    ushort amount;
    AnimationIndex animation;
    
    inline ResourceLevelData(CSBuffer* buffer) :
        amount(buffer->readShort()),
        animation(buffer)
    {
    
    }
    
    inline ResourceLevelData(const ResourceLevelData&) {
        CSAssert(false, "invalid operation");
    }
    inline ResourceLevelData& operator=(const ResourceLevelData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

struct ResourceData {
    ImageIndex icon;
    const CSLocaleString* name;
    fixed collider;
    float radius;
	float body;
	float top;
	byte gatherings;
	CSColor color;
    const CSArray<ResourceLevelData>* levels;
    
    ResourceData(CSBuffer* buffer);
    ~ResourceData();
    
    inline ResourceData(const ResourceData&) {
        CSAssert(false, "invalid operation");
    }
    inline ResourceData& operator=(const ResourceData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* ResourceData_h */
