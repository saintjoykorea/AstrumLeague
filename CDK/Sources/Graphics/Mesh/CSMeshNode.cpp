//
//  CSMeshNode.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMeshNode.h"

#include "CSBuffer.h"

CSMeshNode* CSMeshNode::createWithBuffer(const CSMeshNode* parent, CSBuffer* buffer, uint& memory) {
    int faceCount = buffer->readLength();
    
    CSVertexArray* vertices = CSVertexArray::create(faceCount, 3, 3, false, false, CSVertexLayouts);
    
    if (!vertices) return NULL;
    
    vertices->beginVertex();
    vertices->beginIndex();
    for (int i = 0; i < faceCount; i++) {
		CSVertex vertex0(buffer);
		CSVertex vertex1(buffer);
		CSVertex vertex2(buffer);

        vertices->addVertex(vertex0);
        vertices->addVertex(vertex1);
        vertices->addVertex(vertex2);
    }
    for (int i = 0; i < vertices->vertexCount(); i++) {
        vertices->addIndex(i);
    }
    vertices->transfer();
    
    memory += faceCount * 3 * (sizeof(CSVertex) + 4);
    
    CSMeshNode* node = new CSMeshNode();
    node->_parent = parent;
    node->_name = CSObject::retain(buffer->readString());
    node->_transform = CSMatrix(buffer);
    node->_animation = buffer->readBoolean() ? new CSMeshAnimation(buffer) : NULL;
    node->_bone = buffer->readBoolean();
    node->_castShadow = buffer->readBoolean();
    node->_recvShadow = buffer->readBoolean();
    node->_material = buffer->readByte();
    node->_capture = CSMatrix::Identity;
    node->_vertices = vertices;
    return node;
}

CSMeshNode::~CSMeshNode() {
    _name->release();
    if (_animation) delete _animation;
    _vertices->release();
}

void CSMeshNode::update(const CSMatrix& world, float ticks) {
    if (_animation && _animation->hasRotation()) {
        CSMatrix::rotationQuaternion(_animation->rotation(ticks), _capture);
    }
    else {
        _capture = _transform;
    }
    if (_animation && _animation->hasPosition()) {
        _capture.setTranslationVector(_animation->position(ticks));
    }
    _capture *= _parent ? _parent->_capture : world;
}

void CSMeshNode::draw(CSGraphics* graphics, const CSRootImage* texture) {
    graphics->setWorld(_capture);
    graphics->drawVertices(texture, _vertices, GL_TRIANGLES);
}
