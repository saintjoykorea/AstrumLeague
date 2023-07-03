//
//  Grid.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 11. 20..
//  Copyright © 2019년 brgames. All rights reserved.
//
#include "Grid.h"

Grid::Grid(const Terrain* terrain, const CSColor3* colors) {
    _vertices = CSTVertexArray<GridVertex>::create((terrain->width() + 1) * (terrain->height() + 1), 1, 4, false, false, GridVertexLayouts);
    _vertices->beginVertex();
    _vertices->beginIndex();
    
    for (int y = 0; y <= terrain->height(); y++) {
        for (int x = 0; x <= terrain->width(); x++) {
            GridVertex vertex;
            vertex.position = CSVector3(x, y, terrain->altitude(IPoint(x, y))) * TileSize;
            vertex.position.z += 0.5f;
            vertex.color = colors[y * (terrain->width() + 1) + x];
            _vertices->addVertex(vertex);
        }
    }
    for (int y = 0; y <= terrain->height(); y++) {
        for (int x = 0; x < terrain->width(); x++) {
            uint vi = y * (terrain->width() + 1) + x;
            _vertices->addIndex(vi + 0);
            _vertices->addIndex(vi + 1);
        }
    }
    for (int x = 0; x <= terrain->width(); x++) {
        for (int y = 0; y < terrain->height(); y++) {
            uint vi = y * (terrain->width() + 1) + x;
            _vertices->addIndex(vi + 0);
            _vertices->addIndex(vi + terrain->width() + 1);
        }
    }
    _vertices->transfer();
}

Grid::~Grid() {
    _vertices->release();
}

void Grid::draw(CSGraphics *graphics, float alpha) {
    Asset::sharedAsset()->terrain->gridRenderer->draw(graphics, _vertices, alpha);
}
