//
//  CSModel.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 9..
//  Copyright © 2020년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSModel.h"

#include "CSAnimation.h"

#include "CSBuffer.h"

CSModel::CSModel(CSBuffer* buffer) :
    _meshIndices(retain(buffer->readArray<ushort>())),
    _materials(retain(buffer->readArray<CSMeshMaterial>()))
{
    
}

CSModel::CSModel(const CSModel* other, bool capture) :
    _meshIndices(retain(other->_meshIndices)),
    _materials(retain(other->_materials))
{
    if (capture) _progress = other->_progress;
}

CSModel::~CSModel() {
    release(_meshIndices);
    release(_materials);
}

CSAnimationState CSModel::update(float delta, const CSCamera* camera, bool alive) {
    if (_meshIndices && _resourceDelegate) {
        const CSMesh* mesh = static_cast<const CSMesh*>((*_resourceDelegate)(CSResourceTypeMesh, _meshIndices->pointer(), _meshIndices->count()));
        
        if (mesh) {
            _progress += delta;
            if (_progress > mesh->progress()) _progress = 0;
        }
    }
    return CSAnimationStateNone;
}

void CSModel::draw(CSGraphics* graphics, bool shadow) {
    if (_meshIndices && _resourceDelegate) {
        const CSMesh* mesh = static_cast<const CSMesh*>((*_resourceDelegate)(CSResourceTypeMesh, _meshIndices->pointer(), _meshIndices->count()));
        
        if (mesh) {
            if (_parent) {
                CSMatrix capture;
                if (_parent->capture(_parent->progress(), &graphics->camera(), capture)) {
                    graphics->pushTransform();
                    graphics->transform(capture);
                    mesh->draw(graphics, _materials, _progress, shadow, _resourceDelegate);
                    graphics->popTransform();
                }
            }
            else {
                mesh->draw(graphics, _materials, _progress, shadow, _resourceDelegate);
            }
        }
    }
}

void CSModel::preload(CSResourceDelegate* delegate) const {
    if (!delegate) delegate = _resourceDelegate;
    if (delegate) {
        if (_meshIndices) (*delegate)(CSResourceTypeMesh, _meshIndices->pointer(), _meshIndices->count());
        
        foreach(const CSMeshMaterial&, material, _materials) {
            if (material.textureIndices()) {
                (*delegate)(CSResourceTypeImage, material.textureIndices()->pointer(), material.textureIndices()->count());
            }
        }
    }
}
