//
//  CSParticleShapeRect.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShapeRect.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSParticleShapeRect::CSParticleShapeRect(CSBuffer* buffer) : _range(buffer) {

}

void CSParticleShapeRect::issue(CSVector3& position, CSVector3& direction, bool shell) const {
    if (shell) {
        bool x = randInt(0, 1);
        bool neg = randInt(0, 1);
        
        if (x) {
            position = CSVector3(neg ? -_range.x : _range.x, randFloat(-_range.y, _range.y), 0.0f);
            direction = CSVector3(neg ? -1.0f : 1.0f, 0.0f, 0.0f);
        }
        else {
            position = CSVector3(randFloat(-_range.x, _range.x), neg ? -_range.y : _range.y, 0.0f);
            direction = CSVector3(0.0f, neg ? -1.0f : 1.0f, 0.0f);
        }
    }
    else {
        position = CSVector3(randFloat(-_range.x, _range.x), randFloat(-_range.y, _range.y), 0.0f);
        direction = CSVector3::UnitZ;
    }
}
