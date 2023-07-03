//
//  CSParticleShapeCircle.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShapeCircle.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSParticleShapeCircle::CSParticleShapeCircle(CSBuffer* buffer) : _range(buffer) {

}

void CSParticleShapeCircle::issue(CSVector3& position, CSVector3& direction, bool shell) const {
    float a = randFloat(-FloatPi, FloatPi);
    if (shell) {
        position = CSVector3(CSMath::cos(a) * _range.x, CSMath::sin(a) * _range.y, 0.0f);
    }
    else {
        float r = randFloat(0.0f, 1.0f);
        position = CSVector3(CSMath::cos(a) * r * _range.x, CSMath::sin(a) * r * _range.y, 0.0f);
    }
    CSVector3::normalize(position, direction);
}
