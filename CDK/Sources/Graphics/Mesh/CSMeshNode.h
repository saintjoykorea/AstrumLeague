//
//  CSMeshNode.h
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifndef CSMeshNode_h
#define CSMeshNode_h

#include "CSMeshAnimation.h"

#include "CSRootImage.h"

#include "CSVertexArray.h"

class CSMeshNode {
private:
    const CSMeshNode* _parent;
    const CSString* _name;
    CSMatrix _transform;
    CSMeshAnimation* _animation;
    bool _bone;
    bool _castShadow;
    bool _recvShadow;
    byte _material;
    CSMatrix _capture;
    CSVertexArray* _vertices;
private:
    inline CSMeshNode() {}
public:
    ~CSMeshNode();
    
    static CSMeshNode* createWithBuffer(const CSMeshNode* parent, CSBuffer* buffer, uint& memory);
    
    inline const CSMeshNode* parent() const {
        return _parent;
    }
    inline const CSString* name() const {
        return _name;
    }
    inline bool bone() const {
        return _bone;
    }
    inline bool castShadow() const {
        return _castShadow;
    }
    inline bool recvShadow() const {
        return _recvShadow;
    }
    inline int material() const {
        return _material;
    }
    
    void update(const CSMatrix& world, float ticks);
    void draw(CSGraphics* graphics, const CSRootImage* texture);
};

#endif /* CSMeshNode_h */
