//
//  CSMesh.h
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 9..
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifndef CSMesh_h
#define CSMesh_h

#include "CSMeshNode.h"
#include "CSMeshMaterial.h"

#include "CSGraphics.h"

class CSMesh : public CSResource {
private:
    uint _lastFrame;
    uint _ticksPerFrame;
    uint _frameSpeed;
    uint _memory;
    CSArray<CSMeshNode*>* _nodes;
private:
    inline CSMesh() {}
    ~CSMesh();
public:
    static CSMesh* createWithBuffer(CSBuffer* buffer);
    
    static inline CSMesh* meshWithBuffer(CSBuffer* buffer) {
        return autorelease(createWithBuffer(buffer));
    }
    
    inline float progress() const {
        return _lastFrame * _frameSpeed;
    }
    
    inline CSResourceType resourceType() const override {
        return CSResourceTypeMesh;
    }
    
    void draw(CSGraphics* graphics, const CSArray<CSMeshMaterial>* materials, float progress, bool shadow, CSResourceDelegate* resourceDelegate) const;
    
    inline uint memory() const {
        return _memory;
    }
};

#endif /* CSMesh_h */
