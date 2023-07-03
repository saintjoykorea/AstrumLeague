//
//  CSParticleShapeBox.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShapeBox.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSParticleShapeBox::CSParticleShapeBox(CSBuffer* buffer) : _range(buffer) {

}

void CSParticleShapeBox::issue(CSVector3& position, CSVector3& direction, bool shell) const {
    if (shell) {
        int plane = randInt(0, 2);
        bool neg = randInt(0, 1);
        
        switch (plane) {
            case 0:
                position = CSVector3(neg ? -_range.x : _range.x,
                                     randFloat(-_range.y, _range.y),
                                     randFloat(-_range.z, _range.z));
                direction = CSVector3(neg ? -1.0f : 1.0f, 0.0f, 0.0f);
                break;
            case 1:
                position = CSVector3(randFloat(-_range.x, _range.x),
                                     neg ? -_range.y : _range.y,
                                     randFloat(-_range.z, _range.z));
                direction = CSVector3(0.0f, neg ? -1.0f : 1.0f, 0.0f);
                break;
            default:
                position = CSVector3(randFloat(-_range.x, _range.x),
                                     randFloat(-_range.y, _range.y),
                                     neg ? -_range.z : _range.z);
                direction = CSVector3(0.0f, 0.0f, neg ? -1.0f : 1.0f);
                break;
        }
    }
    else {
        position = CSVector3(randFloat(-_range.x, _range.x),
                             randFloat(-_range.y, _range.y),
                             randFloat(-_range.z, _range.z));
		direction = CSVector3::UnitZ;
    }
}
