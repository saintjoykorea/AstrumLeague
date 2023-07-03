//
//  CSParticleShapeRect.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeRect__
#define __CDK__CSParticleShapeRect__

#include "CSParticleShape.h"

class CSParticleShapeRect : public CSParticleShape {
private:
    CSVector2 _range;
public:
    inline CSParticleShapeRect() {
    
    }
    CSParticleShapeRect(CSBuffer* buffer);
private:
    inline ~CSParticleShapeRect() {
    
    }
public:
    static inline CSParticleShapeRect* shape() {
        return autorelease(new CSParticleShapeRect());
    }
    static inline CSParticleShapeRect* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeRect(buffer));
    }
    
    inline void setRange(const CSVector2& range) {
        _range = range;
    }
    inline const CSVector2& range() const {
        return _range;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeRect;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeRect__ */
