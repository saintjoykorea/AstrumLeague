//
//  SurfaceRenderer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef SurfaceRenderer_h
#define SurfaceRenderer_h

#include "Application.h"

struct SurfaceVertex {
    CSVector3 position;
    CSVector3 normal;
    CSVector2 lightCoord;
};

struct SurfaceSlice {
    ushort surfaceImageIndex;
    CSMaterial material;
    float scale;
    float rotation;
    const CSRootImage* light;
    uint indexCount;
    
    SurfaceSlice(int surfaceImageIndex, const CSMaterial& material, float scale, float rotation, const CSRootImage* light, uint indexCount);
    ~SurfaceSlice();
    
    inline SurfaceSlice(const SurfaceSlice&) {
        CSAssert(false, "invalid operation");
    }
    inline SurfaceSlice& operator =(const SurfaceSlice&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

enum SurfaceUniforms {
    SurfaceUniformWorld,
    SurfaceUniformViewProjection,
    SurfaceUniformPositionScale,
    SurfaceUniformSurfaceTexture,
	SurfaceUniformSurfaceOffset,
    SurfaceUniformSurfaceScale,
    SurfaceUniformSurfaceRotation,
    SurfaceUniformLightTexture,
	SurfaceUniformShadowVisible,
    SurfaceUniformShadowTexture,
    SurfaceUniformShadowOffset,
    SurfaceUniformShadowScale,
    SurfaceUniformEye,
    SurfaceUniformColor,
    SurfaceUniformLightAmbient,
    SurfaceUniformLightColor,
    SurfaceUniformLightDirection,
    SurfaceUniformMaterialDiffuse,
    SurfaceUniformMaterialSpecular,
    SurfaceUniformMaterialShininess,
    SurfaceUniformMaterialEmission,
    SurfaceUniformCount
};
enum BaseUniforms {
    BaseUniformWorld,
    BaseUniformViewProjection,
    BaseUniformPositionScale,
    BaseUniformCount
};

extern const CSArray<CSVertexLayout>* SurfaceVertexLayouts;

class SurfaceRenderer {
private:
    CSProgram* _baseProgram;
    CSProgram* _surfaceProgram;
    int _baseUniforms[BaseUniformCount];
    int _surfaceUniforms[SurfaceUniformCount];
public:
    SurfaceRenderer();
    ~SurfaceRenderer();

    void draw(CSGraphics* graphics, const CSTVertexArray<SurfaceVertex>* arr, const CSArray<SurfaceSlice>* slices, const CSVector2& offset, const CSRootImage* shadow);
};

#endif /* SurfaceRenderer_h */
