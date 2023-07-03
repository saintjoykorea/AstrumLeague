//
//  CSMovementConstant.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMovementConstant__
#define __CDK__CSMovementConstant__

#include "CSMovement.h"

#include "CSMath.h"

struct CSMovementConstant : public CSMovement {
private:
    float _value;
    float _valueVar;
public:
    inline CSMovementConstant(float value, float valueVar) : _value(value), _valueVar(valueVar) {
    
    }
    inline CSMovementConstant(const CSMovementConstant* other) : _value(other->_value), _valueVar(other->_valueVar) {
    
    }
    CSMovementConstant(CSBuffer* buffer);
private:
    inline ~CSMovementConstant() {
    
    }
public:
    static CSMovementConstant* movementWithBuffer(CSBuffer* buffer);
    
    inline CSMovementType type() const override {
        return CSMovementTypeConstant;
    }
    
    inline float value(float t) const override {
        return _value;
    }
    inline float value(float t, float r) const override {
        return _valueVar ? CSMath::lerp(_value - _valueVar, _value + _valueVar, r) : _value;
    }
    
    inline void setValue(float value) {
        _value = value;
    }
    inline void setValueVar(float valueVar) {
        _valueVar = valueVar;
    }
    inline CSObject* copy() const override {
        return new CSMovementConstant(this);
    }
};

#endif /* __CDK__CSConstant__ */
