//
//  CSMeshMaterial.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMeshMaterial.h"

#include "CSBuffer.h"

CSMeshMaterial::CSMeshMaterial() : material(CSMaterial::Default), usingOpacity(false), _textureIndices(NULL) {
    
}

CSMeshMaterial::CSMeshMaterial(const CSMeshMaterial& other) : material(other.material), usingOpacity(other.usingOpacity), _textureIndices(CSObject::retain(other._textureIndices)) {
    
}

CSMeshMaterial::CSMeshMaterial(CSBuffer* buffer) :
    material(buffer),
    usingOpacity(buffer->readBoolean()),
    _textureIndices(CSObject::retain(buffer->readArray<ushort>()))
{
    
}

CSMeshMaterial::~CSMeshMaterial() {
    if (_textureIndices) _textureIndices->release();
}

CSMeshMaterial& CSMeshMaterial::operator=(const CSMeshMaterial &other) {
    material = other.material;
    usingOpacity = other.usingOpacity;
    CSObject::retain(_textureIndices, other._textureIndices);
    return *this;
}
