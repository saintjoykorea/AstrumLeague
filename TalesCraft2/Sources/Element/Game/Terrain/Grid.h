//
//  Grid.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 11. 20..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef Grid_h
#define Grid_h

#include "Terrain.h"

class Grid {
private:
    CSTVertexArray<GridVertex>* _vertices;
public:
    Grid(const Terrain* terrain, const CSColor3* colors);
    ~Grid();
private:
    Grid(const Grid& other);
    Grid& operator =(const Grid& other);
public:
    void draw(CSGraphics* graphics, float alpha);
};

#endif /* Grid_h */
