//
//  CSRenderProgram.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSRenderProgram__
#define __CDK__CSRenderProgram__

#include "CSRenderState.h"

#include "CSProgram.h"

struct CSCamera;

enum CSRendererUniform {
    CSRendererUniformViewProjection,
    CSRendererUniformEye,
    CSRendererUniformWorld,
    CSRendererUniformShadow,
    CSRendererUniformColor,
    CSRendererUniformTexture0,
    CSRendererUniformTexture1,
    CSRendererUniformLightness,
    CSRendererUniformBrightness,
    CSRendererUniformContrast,
    CSRendererUniformSaturation,
    CSRendererUniformAlphaTest,
    CSRendererUniformDepthBias,
    CSRendererUniformDepthBiasPerspective,
    CSRendererUniformConvolutionTextureOffset,
    CSRendererUniformConvolution,
    CSRendererUniformOffset,
    CSRendererUniformLightAmbient,
    CSRendererUniformLightColor,
    CSRendererUniformLightDirection,
    CSRendererUniformMaterialDiffuse,
    CSRendererUniformMaterialSpecular,
    CSRendererUniformMaterialShininess,
    CSRendererUniformMaterialEmission,
    CSRendererUniformCount
};

class CSRenderProgram : public CSProgram {
private:
    int _uniforms[CSRendererUniformCount];
    
public:
    inline CSRenderProgram() {
    
    }
private:
    inline ~CSRenderProgram() {
    
    }
public:
    static inline CSRenderProgram* program() {
        return autorelease(new CSRenderProgram());
    }
    
    inline bool link() {
        CSAssert(false, "invalid operation");
        return false;
    }
    bool link(const CSRenderState& state);
    
    void setCamera(const CSCamera& camera);
    void setWorld(const CSMatrix& world);
    void setColor(const CSColor& color);
    void setImage(const CSRootImage* image);
    void setLightness(float lightness);
    void setBrightness(float brightness);
    void setContrast(float contrast);
    void setSaturation(float saturation);
    void setAlphaTest(float alphaTest);
    void setDepthBias(float depthBias, const CSCamera& camera);
    void setConvolution(const CSConvolution& convolution);
    void setOffset(const CSVector2& offset);
    void setLight(const CSLight& light);
    void setMaterial(const CSMaterial& material);
};

#endif /* defined(__CDK__CSRenderProgram__) */

#endif
