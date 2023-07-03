//
//  Area.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 29..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef Area_h
#define Area_h

#include "IPoint.h"
#include "FPoint.h"

struct Area {
    FPoint point;
    fixed range;
    fixed frange2;
    fixed irange2;
    const CSArray<FPoint>* buildingPoints;
    const CSArray<FPoint>* rallyPoints;
    
    Area(CSBuffer* buffer);
    ~Area();
    
    inline Area(const Area& other) {
        CSAssert(false, "invalid operation");
    }
    inline Area& operator=(const Area&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    
    bool contains(const FPoint& point) const;
    bool contains(const IPoint& point) const;
};

#endif /* Area_h */
