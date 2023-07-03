//
//  CSRenderBounds.cpp
//  CDK
//
//  Created by Kim Chan on 2018. 2. 23..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRenderBounds.h"

CSRenderBounds CSRenderBounds::None(2, -2, 2, -2);
CSRenderBounds CSRenderBounds::Full(-1, 1, -1, 1);

CSRenderBounds::CSRenderBounds(float left, float right, float top, float bottom) : left(left), right(right), top(top), bottom(bottom) {
    
}

void CSRenderBounds::clip() {
    if (left < -1) {
        left = -1;
    }
    if (right > 1) {
        right = 1;
    }
    if (top < -1) {
        top = -1;
    }
    if (bottom > 1) {
        bottom = 1;
    }
}

void CSRenderBounds::append(float x, float y) {
    if (left > x) {
        left = x;
    }
    if (right < x) {
        right = x;
    }
    if (top > y) {
        top = y;
    }
    if (bottom < y) {
        bottom = y;
    }
}

void CSRenderBounds::append(const CSRenderBounds &other) {
    if (left > other.left) {
        left = other.left;
    }
    if (right < other.right) {
        right = other.right;
    }
    if (top > other.top) {
        top = other.top;
    }
    if (bottom < other.bottom) {
        bottom = other.bottom;
    }
}

bool CSRenderBounds::contains(const CSRenderBounds &other) const {
    return CSMath::max(other.left, left) <= CSMath::min(other.right, right) && CSMath::max(other.top, top) <= CSMath::min(other.bottom, bottom);
}

bool CSRenderBounds::onScreen() const {
    return left <= 1 && right >= -1 && top <= 1 && bottom >= -1;
}

bool CSRenderBounds::fullScreen() const {
    return left <= -1 && right >= 1 && top <= -1 && bottom >= 1;
}
