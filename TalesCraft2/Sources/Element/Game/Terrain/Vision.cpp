//
//  Vision.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 13..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Vision.h"

Vision::Vision(const Terrain* terrain) {
    _diffuse = CSScratch::create(CSImageFormatRawR8, terrain->width() + 1, terrain->height() + 1);
    _vertices = CSTVertexArray<CSVector3>::create(terrain->width() * terrain->height(), 4, 6, false, false, VisionVertexLayouts);
    
    _vertices->beginVertex();
    _vertices->beginIndex();
    
    for (int y = 0; y < terrain->height(); y++) {
        for (int x = 0; x < terrain->width(); x++) {
            fixed h0 = terrain->altitude(IPoint(x, y));
            fixed h1 = terrain->altitude(IPoint(x + 1, y));
            fixed h2 = terrain->altitude(IPoint(x, y + 1));
            fixed h3 = terrain->altitude(IPoint(x + 1, y + 1));
            
            CSVector3 vertices[4];
            vertices[0] = CSVector3(x, y, (float)h0);
            vertices[1] = CSVector3((x + 1), y, (float)h1);
            vertices[2] = CSVector3(x, (y + 1), (float)h2);
            vertices[3] = CSVector3((x + 1), (y + 1), (float)h3);
            
            uint vi = _vertices->vertexCount();
            
            if (CSMath::abs(h3 - h0) >= CSMath::abs(h2 - h1)) {
                _vertices->addIndex(vi);
                _vertices->addIndex(vi + 1);
                _vertices->addIndex(vi + 2);
                _vertices->addIndex(vi + 1);
                _vertices->addIndex(vi + 3);
                _vertices->addIndex(vi + 2);
            }
            else {
                _vertices->addIndex(vi);
                _vertices->addIndex(vi + 1);
                _vertices->addIndex(vi + 3);
                _vertices->addIndex(vi);
                _vertices->addIndex(vi + 3);
                _vertices->addIndex(vi + 2);
            }
            _vertices->addVertex(vertices[0]);
            _vertices->addVertex(vertices[1]);
            _vertices->addVertex(vertices[2]);
            _vertices->addVertex(vertices[3]);
        }
    }
    _vertices->transfer();
}
Vision::~Vision() {
    _vertices->release();
    _diffuse->release();
}

void Vision::draw(CSGraphics* graphics) {
    Asset::sharedAsset()->terrain->visionRenderer->draw(graphics, _vertices, _diffuse->image());
}
