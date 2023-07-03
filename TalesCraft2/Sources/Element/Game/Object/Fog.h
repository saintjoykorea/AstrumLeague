//
//  Fog.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 9. 3..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef Fog_h
#define Fog_h

#include "Object.h"

class Fog : public Object {
public:
    const byte alliance;
protected:
    inline Fog(int alliance) : alliance(alliance) {
    
    }
    inline virtual ~Fog() {
    
    }
public:
    inline bool isVisible(const Force* force) const override {
        return _located && force && force->alliance() == alliance;
    }
};

#endif /* Fog_h */
