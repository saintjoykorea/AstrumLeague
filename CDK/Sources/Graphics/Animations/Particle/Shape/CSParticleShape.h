//
//  CSParticleShape.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShape__
#define __CDK__CSParticleShape__

#include "CSObject.h"

#include "CSVector3.h"

enum CSParticleShapeType {
    CSParticleShapeTypeSphere,
    CSParticleShapeTypeHamisphere,
    CSParticleShapeTypeCone,
    CSParticleShapeTypeCircle,
    CSParticleShapeTypeBox,
    CSParticleShapeTypeRect
};

class CSBuffer;

class CSParticleShape : public CSObject {
protected:
    inline virtual ~CSParticleShape() {
    
    }
public:
    static CSParticleShape* createWithBuffer(CSBuffer* buffer);
    static inline CSParticleShape* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(createWithBuffer(buffer));
    }
    
    virtual CSParticleShapeType type() const = 0;
    
    virtual void issue(CSVector3& position, CSVector3& direction, bool shell) const = 0;
};

#endif /* __CDK__CSParticleShape__ */
