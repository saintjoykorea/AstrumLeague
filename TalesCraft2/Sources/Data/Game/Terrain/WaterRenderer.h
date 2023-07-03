//
//  WaterRenderer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 4. 20..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef WaterRenderer_h
#define WaterRenderer_h

#include "Application.h"

struct Water;

struct WaterVertex {
    CSVector3 position;
    float altitude;
};

enum WaterUniforms {
    WaterUniformViewProjection,
    WaterUniformWorld,
    WaterUniformPositionScale,
    WaterUniformPerturbTexture,
    WaterUniformPerturbWeight,
    WaterUniformPerturbScale,
    WaterUniformPerturbFlowForward,
    WaterUniformPerturbFlowCross,
    WaterUniformFoamTexture,
    WaterUniformFoamWeight,
    WaterUniformFoamScale,
    WaterUniformFoamFlowForward,
    WaterUniformFoamFlowCross,
    WaterUniformFoamShallowDepth,
    WaterUniformWaveProgress,
    WaterUniformWaveWidth,
    WaterUniformWaveHeight,
    WaterUniformDepthMax,
    WaterUniformShallowColor,
    WaterUniformDeepColor,
    WaterUniformSpecPower,
    WaterUniformLightDirection,
    WaterUniformLightColor,
    WaterUniformEye,
    WaterUniformDestTexture,
    WaterUniformDepthTexture,
    WaterUniformNear,
    WaterUniformFar,
    WaterUniformVisionTexture,
    WaterUniformVisionScale,
    WaterUniformResolution,
    WaterUniformCount
};

extern const CSArray<CSVertexLayout>* WaterVertexLayouts;

class WaterRenderer {
private:
    class Program : public CSProgram {
    public:
        int uniforms[WaterUniformCount];
        bool link();
    };
    CSDictionary<uint, CSShader>* _vertexShaders;
    CSDictionary<uint, CSShader>* _fragmentShaders;
    CSDictionary<uint, Program>* _programs;
public:
    WaterRenderer();
    ~WaterRenderer();
    
private:
    Program* program(const Water& water);
public:
    void draw(CSGraphics* graphics, const CSArray<Water>* waters, const CSRootImage* vision, float progress);
};

#endif /* WaterRenderer_h */
