//
//  CSLight.h
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSLight__
#define __CDK__CSLight__

#include "CSVector3.h"
#include "CSColor3.h"

class CSBuffer;

struct CSLight {
public:
    float ambient;
    CSColor3 color;
    CSVector3 direction;

    inline CSLight() {
    
    }
    inline CSLight(float ambient, const CSColor3& color, const CSVector3& direction) : ambient(ambient), color(color), direction(direction) {
    
    }
    CSLight(CSBuffer* buffer);
    CSLight(const byte*& raw);
    
    inline bool operator ==(const CSLight& other) const {
        return memcmp(this, &other, sizeof(CSLight)) == 0;
    }
    inline bool operator !=(const CSLight& other) const {
        return !(*this == other);
    }
    
    static const CSLight Default;
};

#endif /* __CDK__CSLight__ */
