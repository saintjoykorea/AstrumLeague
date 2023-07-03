//
//  IBounds.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "IBounds.h"

#include "Map.h"

IBounds::IBounds(const CSVector2& point, float range, bool intersect) {
    src.x = point.x - range;
    src.y = point.y - range;
    dest.x = CSMath::ceil(point.x + range);
    dest.y = CSMath::ceil(point.y + range);
    if (dest.x > src.x) {
        dest.x--;
    }
    if (dest.y > src.y) {
        dest.y--;
    }
    if (intersect) {
        if (src.x < 0) {
            src.x = 0;
        }
        if (src.y < 0) {
            src.y = 0;
        }
        int width = Map::sharedMap()->terrain()->width();
        int height = Map::sharedMap()->terrain()->height();
        if (dest.x >= width) {
            dest.x = width - 1;
        }
        if (dest.y >= height) {
            dest.y = height - 1;
        }
    }
}

IBounds::IBounds(const FPoint& point, fixed range, bool intersect) {
    src.x = point.x - range;
    src.y = point.y - range;
    dest.x = CSMath::ceil(point.x + range);
    dest.y = CSMath::ceil(point.y + range);
    if (dest.x > src.x) {
        dest.x--;
    }
    if (dest.y > src.y) {
        dest.y--;
    }
    if (intersect) {
        if (src.x < 0) {
            src.x = 0;
        }
        if (src.y < 0) {
            src.y = 0;
        }
        int width = Map::sharedMap()->terrain()->width();
        int height = Map::sharedMap()->terrain()->height();
        if (dest.x >= width) {
            dest.x = width - 1;
        }
        if (dest.y >= height) {
            dest.y = height - 1;
        }
    }
}

IBounds::IBounds(const IPoint& point, int range, bool intersect) {
    src.x = point.x - range;
    src.y = point.y - range;
    dest.x = point.x + range;
    dest.y = point.y + range;
    
    if (intersect) {
        if (src.x < 0) {
            src.x = 0;
        }
        if (src.y < 0) {
            src.y = 0;
        }
        int width = Map::sharedMap()->terrain()->width();
        int height = Map::sharedMap()->terrain()->height();
        if (dest.x >= width) {
            dest.x = width - 1;
        }
        if (dest.y >= height) {
            dest.y = height - 1;
        }
    }
}
