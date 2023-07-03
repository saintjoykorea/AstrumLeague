//
//  CSParticleShapeHamisphere.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeHamisphere__
#define __CDK__CSParticleShapeHamisphere__

#include "CSParticleShape.h"

class CSParticleShapeHamisphere : public CSParticleShape {
private:
    float _range;
public:
    inline CSParticleShapeHamisphere() {
    
    }
    CSParticleShapeHamisphere(CSBuffer* buffer);
private:
    inline ~CSParticleShapeHamisphere() {
    
    }
public:
    static inline CSParticleShapeHamisphere* shape() {
        return autorelease(new CSParticleShapeHamisphere());
    }
    static inline CSParticleShapeHamisphere* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeHamisphere(buffer));
    }
    
    inline void setRange(float range) {
        _range = range;
    }
    inline float range() const {
        return _range;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeHamisphere;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeHamisphere__ */
