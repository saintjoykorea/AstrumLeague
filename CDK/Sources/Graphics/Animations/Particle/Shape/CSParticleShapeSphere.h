//
//  CSParticleShapeSphere.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeSphere__
#define __CDK__CSParticleShapeSphere__

#include "CSParticleShape.h"

class CSParticleShapeSphere : public CSParticleShape {
private:
    float _range;
public:
    inline CSParticleShapeSphere() {
    
    }
    CSParticleShapeSphere(CSBuffer* buffer);
private:
    inline ~CSParticleShapeSphere() {
    
    }
public:
    static inline CSParticleShapeSphere* shape() {
        return autorelease(new CSParticleShapeSphere());
    }
    static inline CSParticleShapeSphere* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeSphere(buffer));
    }
    
    inline void setRange(float range) {
        _range = range;
    }
    inline float range() const {
        return _range;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeSphere;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeSphere__ */
