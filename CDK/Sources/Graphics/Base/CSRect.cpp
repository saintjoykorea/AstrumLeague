//
//  CSRect.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRect.h"

#include "CSZRect.h"

const CSRect CSRect::Zero(0, 0, 0, 0);

CSRect::CSRect(CSBuffer* buffer) :
    origin(buffer),
    size(buffer)
{
}

CSRect::CSRect(const byte*& raw) :
    origin(raw),
    size(raw)
{
}

CSRect& CSRect::intersect(const CSRect& rect) {
    origin.x = CSMath::max(left(), rect.left());
    origin.y = CSMath::max(top(), rect.top());
    size.width = CSMath::max(CSMath::min(right(), rect.right()) - origin.x, 0.0f);
    size.height = CSMath::max(CSMath::min(bottom(), rect.bottom()) - origin.y, 0.0f);
    return *this;
}

CSRect& CSRect::adjust(float x, float y) {
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

CSRect::operator CSZRect() const {
    return CSZRect(origin, size);
}

