//
//  CSRenderer.h
//  CDK
//
//  Created by 김찬 on 2015. 8. 19..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSRenderState__
#define __CDK__CSRenderState__

#include "CSRenderTarget.h"
#include "CSColor.h"
#include "CSMatrix.h"
#include "CSBlendMode.h"
#include "CSMaskingMode.h"
#include "CSConvolution.h"
#include "CSLight.h"
#include "CSMaterial.h"
#include "CSCamera.h"
#include "CSVertex.h"

enum CSRenderStateColor : byte {
    CSRenderStateColorVertexOnly,
    CSRenderStateColorUniformOnly,
    CSRenderStateColorAll
};
enum CSRenderStateDepthBias : byte {
    CSRenderStateDepthBiasNone,
    CSRenderStateDepthBiasOrtho,
    CSRenderStateDepthBiasPerspective
};

struct CSRenderState {
    const CSRootImage* image;
    bool usingWorld;
    CSRenderStateColor color;
    bool usingOffset;
    byte convolutionWidth;
    CSMaskingMode maskingMode;
    CSBlendMode blendMode;
    CSRenderStateDepthBias depthBias;
    bool usingLightness;
    bool usingBrightness;
    bool usingContrast;
    bool usingSaturation;
    bool usingAlphaTest;
    bool usingLight;
    bool usingShadow;
};

#endif

#endif
