//
//  CSLight.cpp
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLight.h"

#include "CSBuffer.h"

const CSLight CSLight::Default(0.5f, CSColor3::White, CSVector3::normalize(CSVector3(1, 1, 1.4f)));

CSLight::CSLight(CSBuffer* buffer) :
    ambient(buffer->readFloat()),
    color(buffer, false),
    direction(buffer)
{
}

CSLight::CSLight(const byte*& raw) :
    ambient(readFloat(raw)),
    color(raw, false),
    direction(raw)
{
    
}
