//
//  CSParticleShapeCone.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShapeCone.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSParticleShapeCone::CSParticleShapeCone(CSBuffer* buffer) : _topRange(buffer->readFloat()), _bottomRange(buffer->readFloat()), _height(buffer->readFloat()) {

}

void CSParticleShapeCone::issue(CSVector3& position, CSVector3& direction, bool shell) const {
    float a = randFloat(-FloatPi, FloatPi);
    if (shell) {
        position = CSVector3(CSMath::cos(a), CSMath::sin(a), randFloat(0.0f, 1.0f));
    }
    else {
        float r = randFloat(0.0f, 1.0f);
        position = CSVector3(CSMath::cos(a) * r, CSMath::sin(a) * r, randFloat(0.0f, 1.0f));
    }
    
    CSVector3 top(position.x * _topRange, position.y * _topRange, _height);
    CSVector3 bottom(position.x * _bottomRange, position.y * _bottomRange, 0.0f);
    CSVector3::normalize(top - bottom, direction);
    float range = CSMath::lerp(_bottomRange, _topRange, position.z);
    position *= CSVector3(range, range, _height);
}
