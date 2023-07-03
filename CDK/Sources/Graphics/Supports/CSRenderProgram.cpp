//
//  CSRenderProgram.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSRenderProgram.h"

#include "CSOpenGL.h"
#include "CSString.h"
#include "CSFile.h"

static const char* uniformNames[] = {
    "viewProjection",
    "eye",
    "world",
    "shadow",
    "color",
    "texture0",
    "texture1",
    "lightness",
    "brightness",
    "contrast",
    "saturation",
    "alphaTest",
    "depthBias",
    "depthBiasPerspective",
    "convolutionTextureOffset",
    "convolution",
    "offset",
    "lightAmbient",
    "lightColor",
    "lightDirection",
    "materialDiffuse",
    "materialSpecular",
    "materialShininess",
    "materialEmission"
};

bool CSRenderProgram::link(const CSRenderState& state) {
    glBindAttribLocationCS(_object, CSVertexAttribPosition, "attrPosition");
    if (state.color != CSRenderStateColorUniformOnly) {
        glBindAttribLocationCS(_object, CSVertexAttribColor, "attrColor");
    }
    if (state.image) {
        glBindAttribLocationCS(_object, CSVertexAttribTextureCoord, "attrTextureCoord");
    }
    if (state.usingLight) {
        glBindAttribLocationCS(_object, CSVertexAttribNormal, "attrNormal");
    }
    if (CSProgram::link()) {
        for (int i = 0; i < CSRendererUniformCount; i++) {
            _uniforms[i] = glGetUniformLocationCS(_object, uniformNames[i]);
        }
        return true;
    }
    
    return false;
}

void CSRenderProgram::setCamera(const CSCamera& camera) {
    if (_uniforms[CSRendererUniformViewProjection] >= 0) {
        glUniformMatrix4fvCS(_uniforms[CSRendererUniformViewProjection], 1, 0, camera.viewProjection());
    }
    if (_uniforms[CSRendererUniformEye] >= 0) {
        glUniform3fvCS(_uniforms[CSRendererUniformEye], 1, camera.position());
    }
}
void CSRenderProgram::setWorld(const CSMatrix& world) {
    if (_uniforms[CSRendererUniformWorld] >= 0) {
        glUniformMatrix4fvCS(_uniforms[CSRendererUniformWorld], 1, 0, world);
    }
}
void CSRenderProgram::setColor(const CSColor& color) {
    if (_uniforms[CSRendererUniformColor] >= 0) {
        glUniform4fvCS(_uniforms[CSRendererUniformColor], 1, color);
    }
}
void CSRenderProgram::setImage(const CSRootImage* image) {
    uint texture0 = image ? image->texture0() : 0;
    if (_uniforms[CSRendererUniformTexture0] >= 0) {
        glActiveTextureCS(GL_TEXTURE1);
        glBindTextureCS(GL_TEXTURE_2D, texture0);
        glUniform1iCS(_uniforms[CSRendererUniformTexture0], 1);
    }
    uint texture1 = image ? image->texture1() : 0;
    if (_uniforms[CSRendererUniformTexture1] >= 0) {
        glActiveTextureCS(GL_TEXTURE2);
        glBindTextureCS(GL_TEXTURE_2D, texture1);
        glUniform1iCS(_uniforms[CSRendererUniformTexture1], 2);
    }
    if (_uniforms[CSRendererUniformConvolutionTextureOffset] >= 0 && image) {
        CSSize textureSize = image->textureSize();
        
        glUniform2fCS(_uniforms[CSRendererUniformConvolutionTextureOffset], 1.0f / textureSize.width, 1.0f / textureSize.height);
    }
}
void CSRenderProgram::setLightness(float lightness) {
    if (_uniforms[CSRendererUniformLightness] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformLightness], lightness);
    }
}
void CSRenderProgram::setBrightness(float brightness) {
    if (_uniforms[CSRendererUniformBrightness] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformBrightness], brightness);
    }
}
void CSRenderProgram::setContrast(float contrast) {
    if (_uniforms[CSRendererUniformContrast] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformContrast], contrast);
    }
}
void CSRenderProgram::setSaturation(float saturation) {
    if (_uniforms[CSRendererUniformSaturation] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformSaturation], saturation);
    }
}
void CSRenderProgram::setAlphaTest(float alphaTest) {
    if (_uniforms[CSRendererUniformAlphaTest] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformAlphaTest], alphaTest);
    }
}
void CSRenderProgram::setDepthBias(float depthBias, const CSCamera& camera) {
    if (_uniforms[CSRendererUniformDepthBias] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformDepthBias], depthBias);
    }
    if (_uniforms[CSRendererUniformDepthBiasPerspective] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformDepthBiasPerspective], depthBias * (1.0f + camera.znear() / camera.zfar()));
    }
}
void CSRenderProgram::setConvolution(const CSConvolution& convolution) {
    if (_uniforms[CSRendererUniformConvolution] >= 0) {
        glUniform1fvCS(_uniforms[CSRendererUniformConvolution], convolution.length(), convolution);
    }
}
void CSRenderProgram::setOffset(const CSVector2& offset) {
    if (_uniforms[CSRendererUniformOffset] >= 0) {
        glUniform2fvCS(_uniforms[CSRendererUniformOffset], 1, offset);
    }
}
void CSRenderProgram::setLight(const CSLight& light) {
    if (_uniforms[CSRendererUniformLightAmbient] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformLightAmbient], light.ambient);
    }
    if (_uniforms[CSRendererUniformLightColor] >= 0) {
        glUniform3fvCS(_uniforms[CSRendererUniformLightColor], 1, light.color);
    }
    if (_uniforms[CSRendererUniformLightDirection] >= 0) {
        glUniform3fvCS(_uniforms[CSRendererUniformLightDirection], 1, light.direction);
    }
    if (_uniforms[CSRendererUniformShadow] >= 0) {
        CSMatrix shadow = CSPlane::Ground.shadow(CSVector4(light.direction, 0));
        
        glUniformMatrix4fvCS(_uniforms[CSRendererUniformShadow], 1, 0, shadow);
    }
}
void CSRenderProgram::setMaterial(const CSMaterial& material) {
    if (_uniforms[CSRendererUniformMaterialDiffuse] >= 0) {
        glUniform4fvCS(_uniforms[CSRendererUniformMaterialDiffuse], 1, material.diffuse);
    }
    if (_uniforms[CSRendererUniformMaterialSpecular] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformMaterialSpecular], material.specular);
    }
    if (_uniforms[CSRendererUniformMaterialShininess] >= 0) {
        glUniform1fCS(_uniforms[CSRendererUniformMaterialShininess], material.shininess);
    }
    if (_uniforms[CSRendererUniformMaterialEmission] >= 0) {
        glUniform3fvCS(_uniforms[CSRendererUniformMaterialEmission], 1, material.emission);
    }
}

