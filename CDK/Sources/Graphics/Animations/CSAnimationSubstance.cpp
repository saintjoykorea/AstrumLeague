//
//  CSAnimationSubstance.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimationSubstance.h"

#include "CSBuffer.h"

#include "CSSprite.h"
#include "CSParticle.h"
#include "CSTrail.h"
#include "CSChain.h"
#include "CSModel.h"

CSAnimationSubstance* CSAnimationSubstance::createWithBuffer(CSBuffer* buffer) {
    switch (buffer->readByte()) {
        case CSAnimationSubstanceTypeSprite:
            return new CSSprite(buffer);
        case CSAnimationSubstanceTypeParticle:
            return new CSParticle(buffer);
        case CSAnimationSubstanceTypeTrail:
            return new CSTrail(buffer);
        case CSAnimationSubstanceTypeChain:
            return new CSChain(buffer);
        case CSAnimationSubstanceTypeModel:
            return new CSModel(buffer);
    }
    return NULL;
}
