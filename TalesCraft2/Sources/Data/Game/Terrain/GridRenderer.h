//
//  GridRenderer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef GridRenderer_h
#define GridRenderer_h

#include "Application.h"

struct GridVertex {
    CSVector3 position;
    CSColor3 color;
};

enum GridUniforms {
    GridUniformWorld,
    GridUniformViewProjection,
    GridUniformAlpha,
    GridUniformCount
};

extern const CSArray<CSVertexLayout>* GridVertexLayouts;

class GridRenderer {
private:
    CSProgram* _program;
    int _uniforms[GridUniformCount];
public:
    GridRenderer();
    ~GridRenderer();
    
    void draw(CSGraphics* graphics, const CSTVertexArray<GridVertex>* arr, float alpha);
};

#endif /* GridRenderer_h */
