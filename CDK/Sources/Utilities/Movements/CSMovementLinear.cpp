//
//  CSMovementLinear.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMovementLinear.h"

#include "CSBuffer.h"

CSMovementLinear::CSMovementLinear(CSBuffer* buffer) : _startValue(buffer->readFloat()), _startValueVar(buffer->readFloat()), _endValue(buffer->readFloat()), _endValueVar(buffer->readFloat()) {

}

CSMovementLinear* CSMovementLinear::movementWithBuffer(CSBuffer* buffer) {
    return autorelease(new CSMovementLinear(buffer));
}
