//
//  CSMesh.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 9..
//  Copyright © 2020년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMesh.h"

#include "CSBuffer.h"

static void deleteNodes(CSArray<CSMeshNode*>* nodes) {
    foreach(CSMeshNode*, node, nodes) {
        delete node;
    }
    nodes->release();
}

CSMesh* CSMesh::createWithBuffer(CSBuffer* buffer) {
    int dataLen = buffer->readInt();
    int nextPos = buffer->position() + dataLen;
    
    int nodeCount = buffer->readLength();
    
    CSArray<CSMeshNode*>* nodes = new CSArray<CSMeshNode*>(nodeCount);
    
    uint memory = 0;
    
    for (int i = 0; i < nodeCount; i++) {
        int pi = buffer->readInt();
        
        const CSMeshNode* pnode = pi >= 0 ? nodes->objectAtIndex(pi) : NULL;
        
        CSMeshNode* node = CSMeshNode::createWithBuffer(pnode, buffer, memory);
        
        if (!node) {
            deleteNodes(nodes);
            
            buffer->setPosition(nextPos);
            
            return NULL;
        }
        
        nodes->addObject(node);
    }
    
    CSMesh* mesh = new CSMesh();
    mesh->_lastFrame = buffer->readInt();
    mesh->_ticksPerFrame = buffer->readInt();
    mesh->_frameSpeed = buffer->readInt();
    mesh->_memory = memory;
    mesh->_nodes = nodes;
    
    CSAssert(nextPos == buffer->position(), "invalid data");
    
    return mesh;
}

CSMesh::~CSMesh() {
    deleteNodes(_nodes);
}

void CSMesh::draw(CSGraphics* graphics, const CSArray<CSMeshMaterial>* materials, float progress, bool shadow, CSResourceDelegate* resourceDelegate) const {
    float ticks = progress * _frameSpeed * _ticksPerFrame;
    
    CSMatrix world = graphics->world();
    
    foreach(CSMeshNode*, node, _nodes) {
        node->update(world, ticks);
    }
    
    graphics->push();
    
    if (shadow) {
        graphics->setCullMode(CSCullNone);
        graphics->setDepthMode(CSDepthNone);
        graphics->setBlendMode(CSBlendNone);
        graphics->setUsingShadow(true);
        graphics->setUsingLight(false);
        graphics->setColor(CSColor::Black);
        
        foreach(CSMeshNode*, node, _nodes) {
            if (!node->bone() && node->castShadow()) {
                const CSMeshMaterial* material = materials && node->material() < materials->count() ? &materials->objectAtIndex(node->material()) : NULL;
                if (!material || !material->usingOpacity) node->draw(graphics, NULL);
            }
        }
        
        if (materials) {
            graphics->setBlendMode(CSBlendNormal);
            
            foreach(CSMeshNode*, node, _nodes) {
                if (!node->bone() && node->castShadow()) {
                    const CSMeshMaterial* material = materials && node->material() < materials->count() ? &materials->objectAtIndex(node->material()) : NULL;
                    if (material && material->usingOpacity) {
                        graphics->setColor(CSColor(0.0f, 0.0f, 0.0f, material->material.diffuse.a));
                        
                        const CSRootImage* texture = NULL;
                        if (resourceDelegate && material->textureIndices()) {
                            texture = static_cast<const CSRootImage*>((*resourceDelegate)(CSResourceTypeImage, material->textureIndices()->pointer(), material->textureIndices()->count()));
                        }
                        node->draw(graphics, texture);
                    }
                }
            }
        }
    }
    else {
        graphics->setCullMode(CSCullBack);
        graphics->setDepthMode(CSDepthReadWrite);
        graphics->setBlendMode(CSBlendNone);
        
        if (materials) {
            foreach(CSMeshNode*, node, _nodes) {
                if (!node->bone()) {
                    const CSMeshMaterial* material = materials && node->material() < materials->count() ? &materials->objectAtIndex(node->material()) : NULL;
                    
                    const CSRootImage* texture = NULL;
                    if (material) {
                        if (material->usingOpacity) continue;
                        
                        graphics->setMaterial(material->material);
                        
                        if (resourceDelegate && material->textureIndices()) {
                            texture = static_cast<const CSRootImage*>((*resourceDelegate)(CSResourceTypeImage, material->textureIndices()->pointer(), material->textureIndices()->count()));
                        }
                    }
                    node->draw(graphics, texture);
                }
            }
            
            graphics->setDepthMode(CSDepthRead);
            graphics->setBlendMode(CSBlendNormal);
            
            foreach(CSMeshNode*, node, _nodes) {
                if (!node->bone()) {
                    const CSMeshMaterial* material = materials && node->material() < materials->count() ? &materials->objectAtIndex(node->material()) : NULL;
                    if (material && material->usingOpacity) {
                        graphics->setMaterial(material->material);
                        
                        const CSRootImage* texture = NULL;
                        if (resourceDelegate && material->textureIndices()) {
                            texture = static_cast<const CSRootImage*>((*resourceDelegate)(CSResourceTypeImage, material->textureIndices()->pointer(), material->textureIndices()->count()));
                        }
                        node->draw(graphics, texture);
                    }
                }
            }
        }
        else {
            foreach(CSMeshNode*, node, _nodes) {
                if (!node->bone()) {
                    node->draw(graphics, NULL);
                }
            }
        }
    }
    
    graphics->pop();
}
                         
