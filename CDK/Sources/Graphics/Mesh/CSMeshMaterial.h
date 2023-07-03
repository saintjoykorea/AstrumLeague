//
//  CSMeshMaterial.h
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifndef CSMeshMaterial_h
#define CSMeshMaterial_h

#include "CSGraphics.h"

class CSBuffer;

struct CSMeshMaterial {
    CSMaterial material;
    bool usingOpacity;
private:
    const CSArray<ushort>* _textureIndices;
public:
    CSMeshMaterial();
    CSMeshMaterial(CSBuffer* buffer);
    CSMeshMaterial(const CSMeshMaterial& other);
    ~CSMeshMaterial();
    
    CSMeshMaterial& operator =(const CSMeshMaterial& other);
    
    inline void setTextureIndices(const CSArray<ushort>* textureIndices) {
        CSObject::retain(_textureIndices, textureIndices);
    }
    inline const CSArray<ushort>* textureIndices() const {
        return _textureIndices;
    }
};

#endif /* CSMeshMaterial_h */
