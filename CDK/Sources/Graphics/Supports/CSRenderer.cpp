//
//  CSRenderer.m
//  CDK
//
//  Created by 김찬 on 12. 8. 1..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRenderer.h"

#include "CSOpenGL.h"
#include "CSString.h"
#include "CSFile.h"

static void putToKey(uint& key, int value, int count) {
    key *= count;
    key += value;
}

static uint vertexShaderKey(const CSRenderState& state) {
    uint key = 0;
    putToKey(key, state.image != NULL, 2);
    putToKey(key, state.usingWorld, 2);
    putToKey(key, state.color, 3);
    putToKey(key, state.usingOffset, 2);
    putToKey(key, state.depthBias, 3);
    putToKey(key, state.usingLight, 2);
    putToKey(key, state.usingShadow, 2);
    return key;
}

static int textureType(const CSRootImage* image) {
    if (image) {
		switch (image->format() & 0xff) {
			case CSImageFormatRawA8:
				return 3;
			case CSImageFormatRawL8:
				return 4;
			case CSImageFormatRawL8A8:
				return 5;
			default:
				return image->texture1() ? 2 : 1;
		}
    }
    return 0;
}

static uint fragmentShaderKey(const CSRenderState& state) {
    CSAssert(state.convolutionWidth <= CSConvolutionMaxWidth, "not implemented yet");
    uint key = 0;
    putToKey(key, textureType(state.image), 6);
    putToKey(key, state.convolutionWidth, CSConvolutionMaxWidth + 1);
    putToKey(key, state.maskingMode, CSMaskingModeCount);
    putToKey(key, state.usingAlphaTest, 2);
    putToKey(key, state.usingLightness, 2);
    putToKey(key, state.usingBrightness, 2);
    putToKey(key, state.usingContrast, 2);
    putToKey(key, state.usingSaturation, 2);
    putToKey(key, state.usingLight, 2);
    putToKey(key, state.usingShadow, 2);
    return key;
}
static uint programKey(const CSRenderState& state) {
    uint key = fragmentShaderKey(state);
    putToKey(key, state.usingWorld, 2);
    putToKey(key, state.color, 3);
    putToKey(key, state.usingOffset, 2);
    putToKey(key, state.depthBias, 3);
    return key;
}

static const CSString* vertexShaderDefines(const CSRenderState& state) {
    CSString* str = CSString::string();
    if (state.image) {
        str->append("Texture;");
    }
    if (state.usingWorld) {
        str->append("World;");
    }
    
    str->appendFormat("Color    %d;", state.color);
    
    if (state.usingOffset) {
        str->append("Offset;");
    }
    switch (state.depthBias) {
        case CSRenderStateDepthBiasOrtho:
            str->append("DepthBiasOrtho;");
            break;
        case CSRenderStateDepthBiasPerspective:
            str->append("DepthBiasPerspective;");
            break;
    }
    if (state.usingLight) {
        str->append("Light;");
    }
    if (state.usingShadow) {
        str->append("Shadow;");
    }
    return str;
}
static const CSString* fragmentShaderDefines(const CSRenderState& state) {
    CSString* str = CSString::string();
    if (state.image) {
        str->appendFormat("Texture   %d;", textureType(state.image));
    }
    if (state.convolutionWidth) {
        str->appendFormat("Convolution   %d;", state.convolutionWidth);
    }
    static const char* shaderMaskingModeDefines[] = {
        "MaskingModeNone",
        "MaskingModeRed",
        "MaskingModeGreen",
        "MaskingModeBlue",
        "MaskingModeAlpha"
    };
    str->append(shaderMaskingModeDefines[state.maskingMode]);
    str->append(";");
    if (state.usingAlphaTest) {
        str->append("AlphaTest;");
    }
    if (state.usingLightness) {
        str->append("Lightness;");
    }
    if (state.usingBrightness) {
        str->append("Brightness;");
    }
    if (state.usingContrast) {
        str->append("Contrast;");
    }
    if (state.usingSaturation) {
        str->append("Saturation;");
    }
    if (state.usingLight) {
        str->append("Light;");
    }
    if (state.usingShadow) {
        str->append("Shadow;");
    }
    return str;
}

CSRenderer::CSRenderer() {
    _vertexShaders = new CSDictionary<uint, CSShader>();
    _fragmentShaders = new CSDictionary<uint, CSShader>();
    _programs = new CSDictionary<uint, CSRenderProgram>();
}

CSRenderer::~CSRenderer() {
    _vertexShaders->release();
    _fragmentShaders->release();
    _programs->release();
}

CSRenderProgram* CSRenderer::program(const CSRenderState& state) {
    uint pkey = programKey(state);
    
    CSRenderProgram* program = _programs->objectForKey(pkey);
    
    if (!program) {
        uint vkey = vertexShaderKey(state);
        
        CSShader* vertexShader = _vertexShaders->objectForKey(vkey);
        
        if (!vertexShader) {
            vertexShader = CSShader::create(*CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/graphics.vsh")), GL_VERTEX_SHADER, *vertexShaderDefines(state));
            CSAssert(vertexShader, "shader compile fail");
            _vertexShaders->setObject(vkey, vertexShader);
            vertexShader->release();
        }
        uint fkey = fragmentShaderKey(state);
        
        CSShader* fragmentShader = _fragmentShaders->objectForKey(fkey);
        
        if (!fragmentShader) {
            fragmentShader = CSShader::create(*CSString::stringWithContentOfFile(CSFile::bundlePath("graphics/graphics.fsh")), GL_FRAGMENT_SHADER, *fragmentShaderDefines(state));
            CSAssert(fragmentShader, "shader compile fail");
            _fragmentShaders->setObject(fkey, fragmentShader);
            fragmentShader->release();
        }
        program = new CSRenderProgram();
        program->attach(vertexShader);
        program->attach(fragmentShader);
        bool link = program->link(state);
        CSAssert(link, "program link fail");
        
        _programs->setObject(pkey, program);
        
        program->release();
        
        CSLog("program load:%d", pkey);
    }
    return program;
}

