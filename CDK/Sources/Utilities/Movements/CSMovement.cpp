//
//  CSMovement.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 6..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMovement.h"

#include "CSMovementConstant.h"
#include "CSMovementLinear.h"
#include "CSMovementCurve.h"

#include "CSBuffer.h"

CSMovement* CSMovement::createWithBuffer(CSBuffer* buffer) {
    switch (buffer->readByte()) {
        case CSMovementTypeConstant:
            return new CSMovementConstant(buffer);
        case CSMovementTypeLinear:
            return new CSMovementLinear(buffer);
        case CSMovementTypeCurve:
            return new CSMovementCurve(buffer);
    }
    return NULL;
}

CSMovement* CSMovement::movementWithBuffer(CSBuffer* buffer) {
    return autorelease(createWithBuffer(buffer));
}
