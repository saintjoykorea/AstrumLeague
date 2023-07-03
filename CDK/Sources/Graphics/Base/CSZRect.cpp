//
//  CSZRect.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSZRect.h"

const CSZRect CSZRect::Zero(0, 0, 0, 0, 0);

CSZRect::CSZRect(CSBuffer* buffer) :
    origin(buffer),
    size(buffer)
{
}

CSZRect::CSZRect(const byte*& raw) :
    origin(raw),
    size(raw)
{
}

CSZRect& CSZRect::adjust(float x, float y) {
    if (size.width + x * 2 <= 0) {
        origin.x += size.width / 2;
        size.width = 0;
    }
    else {
        origin.x -= x;
        size.width += x * 2;
    }
    if (size.height + y * 2 <= 0) {
        origin.y += size.height / 2;
        size.height = 0;
    }
    else {
        origin.y -= y;
        size.height += y * 2;
    }
    return *this;
}
