//
//  CSMovementLinear.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMovementLinear__
#define __CDK__CSMovementLinear__

#include "CSMovement.h"

#include "CSMath.h"

struct CSMovementLinear : public CSMovement {
private:
    float _startValue;
    float _startValueVar;
    float _endValue;
    float _endValueVar;
public:
    inline CSMovementLinear(float startValue, float startValueVar, float endValue, float endValueVar) : _startValue(startValue), _startValueVar(startValueVar), _endValue(endValue), _endValueVar(endValueVar) {
    
    }
    inline CSMovementLinear(const CSMovementLinear* other) : _startValue(other->_startValue), _startValueVar(other->_startValueVar), _endValue(other->_endValue), _endValueVar(other->_endValueVar) {
    
    }
    CSMovementLinear(CSBuffer* buffer);
private:
    inline ~CSMovementLinear() {
    
    }
public:
    static CSMovementLinear* movementWithBuffer(CSBuffer* buffer);
    
    inline CSMovementType type() const override {
        return CSMovementTypeLinear;
    }
    
    inline float value(float t) const override {
        return CSMath::lerp(_startValue, _endValue, t);
    }
    inline float value(float t, float r) const override {
        float s = _startValueVar ? CSMath::lerp(_startValue - _startValueVar, _startValue + _startValueVar, r) : _startValue;
        float e = _endValueVar ? CSMath::lerp(_endValue - _endValueVar, _endValue + _endValueVar, r) : _endValue;
        return CSMath::lerp(s, e, t);
    }
    
    inline void setStartValue(float startValue) {
        _startValue = startValue;
    }
    inline void setStartValueVar(float startValueVar) {
        _startValueVar = startValueVar;
    }
    inline void setEndValue(float endValue) {
        _endValue = endValue;
    }
    inline void setEndValueVar(float endValueVar) {
        _endValueVar = endValueVar;
    }
    inline CSObject* copy() const override {
        return new CSMovementLinear(this);
    }
};


#endif /* __CDK__CSMovementLinear__ */
