//
//  CSParticleShapeCircle.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeCircle__
#define __CDK__CSParticleShapeCircle__

#include "CSParticleShape.h"

class CSParticleShapeCircle : public CSParticleShape {
private:
    CSVector2 _range;
public:
    inline CSParticleShapeCircle() {
    
    }
    CSParticleShapeCircle(CSBuffer* buffer);
private:
    inline ~CSParticleShapeCircle() {
    
    }
public:
    static inline CSParticleShapeCircle* shape() {
        return autorelease(new CSParticleShapeCircle());
    }
    static inline CSParticleShapeCircle* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeCircle(buffer));
    }
    
    inline void setRange(const CSVector2& range) {
        _range = range;
    }
    inline const CSVector2& range() const {
        return _range;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeCircle;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeCircle__ */
