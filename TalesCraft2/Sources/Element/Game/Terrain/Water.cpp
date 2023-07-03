//
//  Water.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 4. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Water.h"

#include "Terrain.h"

static const CSRootImage* readImage(CSBuffer* buffer) {
    int imageIndexLen = buffer->readLength();
    switch (imageIndexLen) {
        case 0:
            return NULL;
        case 1:
            return Asset::sharedAsset()->terrain->waterImages->objectAtIndex(buffer->readShort());
        default:
            CSAssert(false, "invalid data");
            return NULL;
    }
}

Water::Water(CSBuffer* buffer, const Terrain* terrain) {
    perturbImage = CSObject::retain(readImage(buffer));
    perturbWeight = buffer->readFloat();
    perturbScale = buffer->readFloat();
    foamImage = CSObject::retain(readImage(buffer));
    foamWeight = buffer->readFloat();
    foamScale = buffer->readFloat();
    foamShallowDepth = buffer->readFloat();
    angle = buffer->readFloat();
    forwardSpeed = buffer->readFloat();
    crossSpeed = buffer->readFloat();
    waveWidth = buffer->readFloat();
    waveHeight = buffer->readFloat();
    depthMax = buffer->readFloat();
    shallowColor = CSColor(buffer, true);
    deepColor = CSColor(buffer, true);
    specPower = buffer->readFloat();
    specWeight = buffer->readFloat();
    distortion = buffer->readBoolean();
    
    int quadCount = buffer->readInt();
    CSTVertexArray<WaterVertex>* vertices = CSTVertexArray<WaterVertex>::create(quadCount, 4, 6, false, false, WaterVertexLayouts);
    vertices->beginVertex();
    vertices->beginIndex();
    for (int i = 0; i < quadCount; i++) {
        int x = buffer->readByte() & 0xff;
        int y = buffer->readByte() & 0xff;
        float z = buffer->readFloat();
        
        vertices->addIndex(vertices->vertexCount());
        vertices->addIndex(vertices->vertexCount() + 1);
        vertices->addIndex(vertices->vertexCount() + 2);
        vertices->addIndex(vertices->vertexCount() + 1);
        vertices->addIndex(vertices->vertexCount() + 3);
        vertices->addIndex(vertices->vertexCount() + 2);
        
        WaterVertex vs[4];
        vs[0].position = CSVector3(x, y, z);
        vs[1].position = CSVector3(x + 1, y, z);
        vs[2].position = CSVector3(x, y + 1, z);
        vs[3].position = CSVector3(x + 1, y + 1, z);
        
        vs[0].altitude = (float)terrain->altitude(IPoint(x, y));
        vs[1].altitude = (float)terrain->altitude(IPoint(x + 1, y));
        vs[2].altitude = (float)terrain->altitude(IPoint(x, y + 1));
        vs[3].altitude = (float)terrain->altitude(IPoint(x + 1, y + 1));
        
        vertices->addVertex(vs[0]);
        vertices->addVertex(vs[1]);
        vertices->addVertex(vs[2]);
        vertices->addVertex(vs[3]);
    }
    vertices->transfer();
    
    this->vertices = vertices;
}
Water::~Water() {
    CSObject::release(perturbImage);
    CSObject::release(foamImage);
    vertices->release();
}
