//
//  CSParticleShape.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 11..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticleShape.h"

#include "CSBuffer.h"

#include "CSParticleShapeSphere.h"
#include "CSParticleShapeHamisphere.h"
#include "CSParticleShapeCone.h"
#include "CSParticleShapeCircle.h"
#include "CSParticleShapeBox.h"
#include "CSParticleShapeRect.h"

CSParticleShape* CSParticleShape::createWithBuffer(CSBuffer* buffer) {
    switch (buffer->readByte()) {
        case CSParticleShapeTypeSphere:
            return new CSParticleShapeSphere(buffer);
        case CSParticleShapeTypeHamisphere:
            return new CSParticleShapeHamisphere(buffer);
        case CSParticleShapeTypeCone:
            return new CSParticleShapeCone(buffer);
        case CSParticleShapeTypeCircle:
            return new CSParticleShapeCircle(buffer);
        case CSParticleShapeTypeBox:
            return new CSParticleShapeBox(buffer);
        case CSParticleShapeTypeRect:
            return new CSParticleShapeRect(buffer);
    }
    return NULL;
}

