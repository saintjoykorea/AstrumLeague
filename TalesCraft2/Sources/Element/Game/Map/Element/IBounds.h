//
//  IBounds.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef IBounds_h
#define IBounds_h

#include "IPoint.h"

struct IBounds {
    IPoint src;
    IPoint dest;
    
    inline IBounds() {
    
    }
    IBounds(const CSVector2& point, float range, bool intersect);
    IBounds(const FPoint& point, fixed range, bool intersect);
    IBounds(const IPoint& point, int range, bool intersect);
    
    inline bool operator ==(const IBounds& other) const {
        return src == other.src && dest == other.dest;
    }
    
    inline bool operator !=(const IBounds& other) const {
        return src != other.src || dest != other.dest;
    }
    
    inline bool contains(const IPoint& p) const {
        return p.x >= src.x && p.x <= dest.x && p.y >= src.y && p.y <= dest.y;
    }
    
    inline uint size() const {
        return (dest.x - src.x + 1) * (dest.y - src.y + 1);
    }
};

#endif /* IBounds_h */
