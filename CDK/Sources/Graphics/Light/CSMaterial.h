//
//  CSMaterial.h
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMaterial__
#define __CDK__CSMaterial__

#include "CSVector3.h"
#include "CSColor.h"
#include "CSColor3.h"

struct CSLight;
class CSBuffer;

struct CSMaterial {
public:
    CSColor diffuse;
    float specular;
    float shininess;
    CSColor3 emission;
    
    inline CSMaterial() {
    
    }
    inline CSMaterial(const CSColor& diffuse, float specular, float shininess, const CSColor3& emission) : diffuse(diffuse), specular(specular), shininess(shininess), emission(emission) {
    
    }
    CSMaterial(CSBuffer* buffer);
    CSMaterial(const byte*& raw);
    
    inline bool operator ==(const CSMaterial& other) const {
        return memcmp(this, &other, sizeof(CSMaterial)) == 0;
    }
    inline bool operator !=(const CSMaterial& other) const {
        return !(*this == other);
    }
    
    static const CSMaterial Default;
};

#endif /* __CDK__CSMaterial__ */
