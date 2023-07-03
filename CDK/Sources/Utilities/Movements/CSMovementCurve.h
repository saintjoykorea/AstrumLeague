//
//  CSMovementCurve.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMovementCurve__
#define __CDK__CSMovementCurve__

#include "CSMovement.h"

#include "CSMath.h"

#include "CSArray.h"

class CSBuffer;

struct CSMovementCurvePoint {
    float time;
    float value;
    float valueVar;
    float leftAngle;
    float rightAngle;
    
    inline CSMovementCurvePoint(float time, float value, float valueVar, float leftAngle, float rightAngle) : time(time), value(value), valueVar(valueVar), leftAngle(leftAngle), rightAngle(rightAngle) {
        CSAssert(time >= 0.0f && time <= 1.0f, "invalid operation");
    }
};

struct CSMovementCurve : public CSMovement {
private:
    CSArray<CSMovementCurvePoint>* _points;
    mutable bool _sort;
public:
    inline CSMovementCurve() {
        _points = new CSArray<CSMovementCurvePoint>();
        _sort = false;
    }
    inline CSMovementCurve(const CSMovementCurve* other) {
        _points = new CSArray<CSMovementCurvePoint>(other->_points, true);
        _sort = other->_sort;
    }
    CSMovementCurve(CSBuffer* buffer);
private:
    inline ~CSMovementCurve() {
        _points->release();
    }
public:
    static CSMovementCurve* movementWithBuffer(CSBuffer* buffer);
private:
    static int sortPoint(const CSMovementCurvePoint& a, const CSMovementCurvePoint& b);
    
    inline void sortPoints() const {
        if (_sort) {
            _points->sort(&CSMovementCurve::sortPoint);
            _sort = false;
        }
    }
    
    inline float pointValue(const CSMovementCurvePoint& point, float random) const {
        return point.valueVar ? CSMath::lerp(point.value - point.valueVar, point.value + point.valueVar, random) : point.value;
    }
public:
    inline CSMovementType type() const override {
        return CSMovementTypeCurve;
    }
    
    float value(float t, float r) const override;
    float value(float t) const override;
    
    inline uint pointCount() const {
        return _points->count();
    }
    inline CSMovementCurvePoint& pointAtIndex(uint index) {
        _sort = true;
        return _points->objectAtIndex(index);
    }
    inline const CSMovementCurvePoint& pointAtIndex(uint index) const {
        return _points->objectAtIndex(index);
    }
    inline CSMovementCurvePoint& firstPoint() {
        _sort = true;
        return _points->objectAtIndex(0);
    }
    inline const CSMovementCurvePoint& firstPoint() const {
        return _points->objectAtIndex(0);
    }
    inline CSMovementCurvePoint& lastPoint() {
        _sort = true;
        return _points->lastObject();
    }
    inline const CSMovementCurvePoint& lastPoint() const {
        return _points->lastObject();
    }
    void addPoint(const CSMovementCurvePoint& point);
    inline void removePointAtIndex(uint index) {
        _points->removeObjectAtIndex(index);
    }
    void linearRotatePointAtIndex(uint index, bool left, bool right);
    inline void linearRotatePointAtIndex(uint index) {
        linearRotatePointAtIndex(index, true, true);
    }
    inline CSObject* copy() const override {
        return new CSMovementCurve(this);
    }
};

#endif /* __CDK__CSMovementCurve__ */
