//
//  CSMovement.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 1..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMovement__
#define __CDK__CSMovement__

#include "CSObject.h"

class CSBuffer;

enum CSMovementType {
    CSMovementTypeConstant = 1,
    CSMovementTypeLinear,
    CSMovementTypeCurve
};

struct CSMovement : public CSObject {
protected:
    inline virtual ~CSMovement() {
    
    }
public:
    static CSMovement* createWithBuffer(CSBuffer* buffer);
    static CSMovement* movementWithBuffer(CSBuffer* buffer);
    
    virtual CSMovementType type() const = 0;
    
    virtual float value(float t) const = 0;
    virtual float value(float t, float r) const = 0;
};

#endif /* __CDK__Movement__ */
