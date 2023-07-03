//
//  CSMovementConstant.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMovementConstant.h"

#include "CSBuffer.h"

CSMovementConstant::CSMovementConstant(CSBuffer* buffer) : _value(buffer->readFloat()), _valueVar(buffer->readFloat()) {

}
CSMovementConstant* CSMovementConstant::movementWithBuffer(CSBuffer* buffer) {
    return autorelease(new CSMovementConstant(buffer));
}
