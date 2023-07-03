//
//  CSParticleShapeBox.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeBox__
#define __CDK__CSParticleShapeBox__

#include "CSParticleShape.h"

class CSParticleShapeBox : public CSParticleShape {
private:
    CSVector3 _range;
public:
    inline CSParticleShapeBox() {
    
    }
    CSParticleShapeBox(CSBuffer* buffer);
private:
    inline ~CSParticleShapeBox() {
    
    }
public:
    static inline CSParticleShapeBox* shape() {
        return autorelease(new CSParticleShapeBox());
    }
    static inline CSParticleShapeBox* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeBox(buffer));
    }
    
    inline void setRange(const CSVector3& range) {
        _range = range;
    }
    inline const CSVector3& range() const {
        return _range;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeBox;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeBox__ */
