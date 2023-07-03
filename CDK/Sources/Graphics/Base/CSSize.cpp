//
//  CSSize.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSSize.h"

#include "CSBuffer.h"

const CSSize CSSize::Zero(0, 0);

CSSize::CSSize(CSBuffer* buffer) :
    width(buffer->readFloat()),
    height(buffer->readFloat())
{
}

CSSize::CSSize(const byte*& raw) :
    width(readFloat(raw)),
    height(readFloat(raw))
{
}
