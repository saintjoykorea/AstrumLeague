//
//  CSMaterial.cpp
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMaterial.h"

#include "CSBuffer.h"

const CSMaterial CSMaterial::Default(CSColor::White, 0.2f, 6.0f, CSColor3::Black);

CSMaterial::CSMaterial(CSBuffer* buffer) :
    diffuse(buffer, false),
    specular(buffer->readFloat()),
    shininess(buffer->readFloat()),
    emission(buffer, false)
{
}

CSMaterial::CSMaterial(const byte*& raw) :
    diffuse(raw, false),
    specular(readFloat(raw)),
    shininess(readFloat(raw)),
    emission(raw, false)
{
}
