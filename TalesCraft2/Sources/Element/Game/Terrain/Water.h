//
//  Water.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 4. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Water_h
#define Water_h

#include "WaterRenderer.h"

class Terrain;

struct Water {
    const CSRootImage* perturbImage;
    float perturbWeight;
    float perturbScale;
    const CSRootImage* foamImage;
    float foamWeight;
    float foamScale;
    float foamShallowDepth;
    float angle;
    float forwardSpeed;
    float crossSpeed;
    float waveWidth;
    float waveHeight;
    float depthMax;
    CSColor shallowColor;
    CSColor deepColor;
    float specPower;
    float specWeight;
    bool distortion;
    const CSTVertexArray<WaterVertex>* vertices;
    
    Water(CSBuffer* buffer, const Terrain* terrain);
    ~Water();
    
    inline Water(const Water&) {
        CSAssert(false, "invalid operation");
    }
    inline Water& operator=(const Water&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* Water_h */
