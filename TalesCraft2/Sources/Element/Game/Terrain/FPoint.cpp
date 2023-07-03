//
//  FPoint.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2016. 2. 9..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "FPoint.h"

#include "IPoint.h"

const FPoint FPoint::Zero(fixed::Zero, fixed::Zero);

FPoint::FPoint(const IPoint& p) : x(p.x), y(p.y), slabId(0) {

}

void FPoint::normalize(const FPoint& vector, FPoint& result) {
    result = vector;
    if (result.x || result.y) {
        result.x <<= 8;
        result.y <<= 8;
        fixed length = result.length();
        if (length) {
            result /= length;
        }
    }
}
void FPoint::normalize() {
    if (x || y) {
        x <<= 8;
        y <<= 8;
        fixed length = this->length();
        if (length) {
            operator /=(length);
        }
    }
}

fixed FPoint::distanceSquaredToLine(const FPoint& p0, const FPoint& p1, FPoint* np) const {
    FPoint v = p1 - p0;
    
    fixed len2 = v.lengthSquared();
    
    if (!len2) {
        if (np) *np = p0;
        return distanceSquared(p0);
    }
    
    FPoint pv = *this - p0;
    
    fixed t = (pv.x * v.x + pv.y * v.y) / len2;
    
    FPoint p2;
    if (t > fixed::One) {
        p2 = p1;
    }
    else if (t > fixed::Zero) {
        p2 = p0 + v * t;
    }
    else {
        p2 = p0;
    }
    if (np) *np = p2;
    return distanceSquared(p2);
}

bool FPoint::intersectsLineToLine(const FPoint& p0, const FPoint& p1, const FPoint& p2, const FPoint& p3, FPoint* cp) {
    //=========================================================================================================
    //optimization code
    fixed x0 = p0.x < p1.x ? p0.x : p1.x;
    fixed x1 = p0.x > p1.x ? p0.x : p1.x;
    fixed x2 = p2.x < p3.x ? p2.x : p3.x;
    fixed x3 = p2.x > p3.x ? p2.x : p3.x;
    if ((x0 > x2 ? x0 : x2) > (x1 < x3 ? x1 : x3)) {
        return false;
    }
    fixed y0 = p0.y < p1.y ? p0.y : p1.y;
    fixed y1 = p0.y > p1.y ? p0.y : p1.y;
    fixed y2 = p2.y < p3.y ? p2.y : p3.y;
    fixed y3 = p2.y > p3.y ? p2.y : p3.y;
    if ((y0 > y2 ? y0 : y2) > (y1 < y3 ? y1 : y3)) {
        return false;
    }
    //=========================================================================================================
    
    FPoint v0 = p1 - p0;
    FPoint v1 = p3 - p2;
    fixed delta = v1.x * v0.y - v1.y * v0.x;
    if (!delta) {
        return false;
    }
    FPoint v2 = p2 - p0;
    fixed s = (v0.x * v2.y - v0.y * v2.x) / delta;
    if (s < fixed::Zero || s > fixed::One) {
        return false;
    }
    fixed t = (v1.x * v2.y - v1.y * v2.x) / delta;
    if (t < fixed::Zero || t > fixed::One) {
        return false;
    }
    if (cp) *cp = p0 + (p1 - p0) * t;
    return true;
}
bool FPoint::between(const FPoint& cp, const FPoint& p0, const FPoint& p1) const {
    FPoint a = p0 - cp;
    FPoint c = p1 - cp;
    FPoint b = *this - cp;
    
    fixed at = CSMath::atan2(a.y, a.x);
    fixed bt = CSMath::atan2(b.y, b.x);
    fixed ct = CSMath::atan2(c.y, c.x);
    
    if (ct > at) {
        return bt > ct || bt < at;
    }
    else {
        return bt < at && bt > ct;
    }
}

void FPoint::rotate(fixed a) {
    fixed cosq = CSMath::cos(a);
    fixed sinq = CSMath::sin(a);
    fixed rx = x * cosq - y * sinq;
    fixed ry = y * cosq + x * sinq;
    x = rx;
    y = ry;
}
