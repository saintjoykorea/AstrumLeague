//
//  CSParticleShapeCone.h
//  CDK
//
//  Created by 김찬 on 2016. 2. 3..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticleShapeCone__
#define __CDK__CSParticleShapeCone__

#include "CSParticleShape.h"

class CSParticleShapeCone : public CSParticleShape {
private:
    float _topRange;
    float _bottomRange;
    float _height;
public:
    inline CSParticleShapeCone() {
    
    }
    CSParticleShapeCone(CSBuffer* buffer);
private:
    inline ~CSParticleShapeCone() {
    
    }
public:
    static inline CSParticleShapeCone* shape() {
        return autorelease(new CSParticleShapeCone());
    }
    static inline CSParticleShapeCone* shapeWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticleShapeCone(buffer));
    }
    
    inline void setTopRange(float topRange) {
        _topRange = topRange;
    }
    inline float topRange() const {
        return _topRange;
    }
    inline void setBottomRange(float bottomRange) {
        _bottomRange = bottomRange;
    }
    inline float bottomRange() const {
        return _bottomRange;
    }
    inline void setHeight(float height) {
        _height = height;
    }
    inline float height() const {
        return _height;
    }
    
    inline CSParticleShapeType type() const override {
        return CSParticleShapeTypeCone;
    }
    
    void issue(CSVector3& position, CSVector3& direction, bool shell) const override;
};

#endif /* __CDK__CSParticleShapeCone__ */
