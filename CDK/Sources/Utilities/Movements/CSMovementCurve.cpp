//
//  CSMovementCurve.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMovementCurve.h"

#include "CSMath.h"
#include "CSBuffer.h"

CSMovementCurve::CSMovementCurve(CSBuffer* buffer) :
    _points(CSObject::retain(buffer->readArray<CSMovementCurvePoint>(false))),
    _sort(false)
{

}

CSMovementCurve* CSMovementCurve::movementWithBuffer(CSBuffer* buffer) {
    return autorelease(new CSMovementCurve(buffer));
}

int CSMovementCurve::sortPoint(const CSMovementCurvePoint& a, const CSMovementCurvePoint& b) {
    if (a.time < b.time) {
        return 1;
    }
    else if (a.time > b.time) {
        return -1;
    }
    else {
        return 0;
    }
}

float CSMovementCurve::value(float t, float random) const {
    sortPoints();
    
    if (_points->count() < 2 || t <= _points->objectAtIndex(0).time) {
        return pointValue(_points->objectAtIndex(0), random);
    }
    if (t >= _points->lastObject().time) {
        return pointValue(_points->lastObject(), random);
    }
    int i;
    for (i = 1; i < _points->count() - 1; i++) {
        const CSMovementCurvePoint& p = _points->objectAtIndex(i);
        
        if (p.time == t) {
            return pointValue(p, random);
        }
        else if (p.time > t) {
            break;
        }
    }
    const CSMovementCurvePoint& p0 = _points->objectAtIndex(i - 1);
    const CSMovementCurvePoint& p3 = _points->objectAtIndex(i);
    float d = p3.time - p0.time;
    
    float v0 = pointValue(p0, random);
    float v3 = pointValue(p3, random);
    if (p0.rightAngle >= FloatPiOverTwo || p3.leftAngle >= FloatPiOverTwo) {
        return CSMath::max(v0, v3);
    }
    if (p0.rightAngle <= -FloatPiOverTwo || p3.leftAngle <= -FloatPiOverTwo) {
        return CSMath::min(v0, v3);
    }
    float v1 = v0 + CSMath::tan(p0.rightAngle) * (d / 3.0f);
    float v2 = v3 - CSMath::tan(p3.leftAngle) * (d / 3.0f);
    
    t = (t - p0.time) / d;
    
    float rt = 1.0f - t;
    
    return (rt * rt * rt * v0) + (3.0f * rt * rt * t * v1) + (3.0f * rt * t * t * v2) + (t * t * t * v3);
}

float CSMovementCurve::value(float t) const {
    sortPoints();
    
    if (_points->count() < 2 || t <= _points->objectAtIndex(0).time) {
        return _points->objectAtIndex(0).value;
    }
    if (t >= _points->lastObject().time) {
        return _points->lastObject().value;
    }
    int i;
    for (i = 1; i < _points->count() - 1; i++) {
        const CSMovementCurvePoint& p = _points->objectAtIndex(i);
        
        if (p.time == t) {
            return p.value;
        }
        else if (p.time > t) {
            break;
        }
    }
    const CSMovementCurvePoint& p0 = _points->objectAtIndex(i - 1);
    const CSMovementCurvePoint& p3 = _points->objectAtIndex(i);
    float d = p3.time - p0.time;
    
    float v1 = p0.value + CSMath::tan(p0.rightAngle) * (d / 3.0f);
    float v2 = p3.value - CSMath::tan(p3.leftAngle) * (d / 3.0f);
    
    t = (t - p0.time) / d;
    
    float rt = 1.0f - t;
    
    float v = (rt * rt * rt * p0.value) + (3.0f * rt * rt * t * v1) + (3.0f * rt * t * t * v2) + (t * t * t * p3.value);
    
    return v;
}

void CSMovementCurve::addPoint(const CSMovementCurvePoint& point) {
    sortPoints();
    
    int i;
    for (i = 0; i < _points->count(); i++) {
        CSMovementCurvePoint& p = _points->objectAtIndex(i);
        
        if (p.time == point.time) {
            p.value = point.value;
            p.valueVar = point.valueVar;
            p.leftAngle = point.leftAngle;
            p.rightAngle = point.rightAngle;
        }
        else if (p.time > point.time) {
            break;
        }
    }
    _points->insertObject(i, point);
}

void CSMovementCurve::linearRotatePointAtIndex(uint index, bool left, bool right) {
    float a0 = 0.0f;
    float a1 = 0.0f;
    
    CSMovementCurvePoint& p = _points->objectAtIndex(index);
    if (index > 0) {
        const CSMovementCurvePoint& p0 = _points->objectAtIndex(index - 1);
        a0 = CSMath::atan2(p.value - p0.value, p.time - p0.time);
    }
    if (index < _points->count() - 1) {
        const CSMovementCurvePoint& p1 = _points->objectAtIndex(index + 1);
        a1 = CSMath::atan2(p1.value - p.value, p1.time - p.time);
    }
    if (index == 0) {
        a0 = a1;
    }
    else if (index == _points->count() - 1) {
        a1 = a0;
    }
    if (left && right) {
        p.leftAngle = p.rightAngle = (a0 + a1) * 0.5f;
    }
    else if (left) {
        p.leftAngle = a1;
    }
    else if (right) {
        p.rightAngle = a0;
    }
}
