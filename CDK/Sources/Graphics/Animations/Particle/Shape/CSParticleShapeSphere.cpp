//
//  CSParticleShapeSphere.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShapeSphere.h"

#include "CSMatrix.h"

#include "CSBuffer.h"

#include "CSRandom.h"

CSParticleShapeSphere::CSParticleShapeSphere(CSBuffer* buffer) : _range(buffer->readFloat()) {

}
void CSParticleShapeSphere::issue(CSVector3& position, CSVector3& direction, bool shell) const {
    if (shell) {
        position = CSVector3(0.0f, 0.0f, _range);
    }
    else {
        position = CSVector3(0.0f, 0.0f, _range * randFloat(0.0f, 1.0f));
    }
    CSMatrix matrix = CSMatrix::rotationYawPitchRoll(randFloat(-FloatPi, FloatPi), randFloat(-FloatPi, FloatPi), 0.0f);
    CSVector3::transformCoordinate(position, matrix, position);
    CSVector3::normalize(position, direction);
}
