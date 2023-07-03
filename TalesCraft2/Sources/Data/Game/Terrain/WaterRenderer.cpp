//
//  WaterRenderer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 4. 20..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "WaterRenderer.h"

#include "Terrain.h"

enum WaterVertexAttrib {
    WaterVertexAttribPosition,
    WaterVertexAttribAltitude
};
static const CSVertexLayout __WaterVertexLayouts[] = {
    CSVertexLayout(WaterVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, offsetof(WaterVertex, position)),
    CSVertexLayout(WaterVertexAttribAltitude, 1, GL_FLOAT, GL_FALSE, offsetof(WaterVertex, altitude))
};
const CSArray<CSVertexLayout>* WaterVertexLayouts = new CSArray<CSVertexLayout>(__WaterVertexLayouts, 2);

static const char* uniformNames[] = {
    "viewProjection",
    "world",
    "positionScale",
    "perturbTexture",
    "perturbWeight",
    "perturbScale",
    "perturbFlowForward",
    "perturbFlowCross",
    "foamTexture",
    "foamWeight",
    "foamScale",
    "foamFlowForward",
    "foamFlowCross",
    "foamShallowDepth",
    "waveProgress",
    "waveWidth",
    "waveHeight",
    "depthMax",
    "shallowColor",
    "deepColor",
    "specPower",
    "lightDirection",
    "lightColor",
    "eye",
    "destTexture",
    "depthTexture",
    "near",
    "far",
    "visionTexture",
    "visionScale",
    "resolution"
};

WaterRenderer::WaterRenderer() {
    _vertexShaders = new CSDictionary<uint, CSShader>();
    _fragmentShaders = new CSDictionary<uint, CSShader>();
    _programs = new CSDictionary<uint, Program>();
}

WaterRenderer::~WaterRenderer() {
    _vertexShaders->release();
    _fragmentShaders->release();
    _programs->release();
}

bool WaterRenderer::Program::link() {
	glBindAttribLocationCS(_object, WaterVertexAttribPosition, "attrPosition");
	glBindAttribLocationCS(_object, WaterVertexAttribAltitude, "attrAltitude");

	if (CSProgram::link()) {
		for (int i = 0; i < WaterUniformCount; i++) {
			uniforms[i] = glGetUniformLocationCS(_object, uniformNames[i]);
		}
		return true;
	}
	return false;
}

static void putToKey(uint& key, int value, int count) {
    key *= count;
    key += value;
}

static uint vertexShaderKey(const Water& water) {
    uint key = 0;
    putToKey(key, water.waveWidth && water.waveHeight, 2);
    putToKey(key, water.perturbImage != NULL, 2);
    putToKey(key, water.foamImage != NULL, 2);
    putToKey(key, water.distortion, 2);
    putToKey(key, water.specWeight != 0.0f, 2);
    putToKey(key, (water.foamImage && water.foamShallowDepth) || water.depthMax, 2);
    return key;
}

static uint fragmentShaderKey(const Water& water) {
    uint key = 0;
    putToKey(key, water.waveWidth && water.waveHeight, 2);
    putToKey(key, water.perturbImage != NULL, 2);
    putToKey(key, water.foamImage != NULL, 2);
    putToKey(key, water.crossSpeed != 0.0f, 2);
    putToKey(key, water.depthMax != 0.0f, 2);
    putToKey(key, water.distortion, 2);
    putToKey(key, water.specWeight != 0.0f, 2);
	putToKey(key, water.foamImage && water.foamShallowDepth, 2);
    return key;
}

static const CSString* vertexShaderDefines(const Water& water) {
    CSString* str = CSString::string();
	str->append("UsingVision;");

    if (water.waveWidth && water.waveHeight) {
        str->append("UsingWave;");
    }
    if (water.perturbImage) {
        str->append("UsingPerturb;");
    }
    if (water.foamImage) {
        str->append("UsingFoam;");
    }
    if (water.distortion) {
        str->append("UsingDistortion;");
    }
    if (water.specWeight) {
        str->append("UsingSpecular;");
    }
    if ((water.foamImage && water.foamShallowDepth) || water.depthMax) {
        str->append("UsingDepth;");
    }
    return str;
}

static uint programKey(const Water& water) {
    return fragmentShaderKey(water);
}

static const CSString* fragmentShaderDefines(const Water& water) {
    CSString* str = CSString::string();
	str->append("UsingVision;");

    if (water.waveWidth && water.waveHeight) {
        str->append("UsingWave;");
    }
    if (water.perturbImage) {
        str->append("UsingPerturb;");
    }
    if (water.foamImage) {
        str->append("UsingFoam;");
        if (water.foamShallowDepth) {
            str->append("UsingFoamShallowDepth;");
        }
    }
    if (water.crossSpeed) {
        str->append("UsingCross;");
    }
    if (water.depthMax) {
        str->append("UsingDepthMax;");
    }
    if (water.distortion) {
        str->append("UsingDistortion;");
    }
    if (water.specWeight) {
        str->append("UsingSpecular;");
    }
    if ((water.foamImage && water.foamShallowDepth) || water.depthMax) {
        str->append("UsingDepth;");
    }
    return str;
}

typename WaterRenderer::Program* WaterRenderer::program(const Water& water) {
    uint pkey = programKey(water);
    
    Program* program = _programs->objectForKey(pkey);
    
    if (!program) {
        uint vkey = vertexShaderKey(water);
        
        CSShader* vertexShader = _vertexShaders->objectForKey(vkey);
        
        if (!vertexShader) {
            vertexShader = CSShader::create(*CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/water.vsh")), GL_VERTEX_SHADER, *vertexShaderDefines(water));
            CSAssert(vertexShader, "shader compile fail");
            _vertexShaders->setObject(vkey, vertexShader);
            vertexShader->release();
        }
        uint fkey = fragmentShaderKey(water);
        
        CSShader* fragmentShader = _fragmentShaders->objectForKey(fkey);
        
        if (!fragmentShader) {
            fragmentShader = CSShader::create(*CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/water.fsh")), GL_FRAGMENT_SHADER, *fragmentShaderDefines(water));
            CSAssert(fragmentShader, "shader compile fail");
            _fragmentShaders->setObject(fkey, fragmentShader);
            fragmentShader->release();
        }
        program = new Program();
        program->attach(vertexShader);
        program->attach(fragmentShader);
        bool link = program->link();
        CSAssert(link, "program link fail");
        
        _programs->setObject(pkey, program);
        
        program->release();
        
        CSLog("program load:%d", pkey);
    }
    return program;
}

void WaterRenderer::draw(CSGraphics* graphics, const CSArray<Water>* waters, const CSRootImage* vision, float progress) {
	const CSRootImage* screen = NULL;
	graphics->commit();
	graphics->target()->swap(true);
	screen = graphics->target()->screen(true);

    graphics->push();
    graphics->setCullMode(CSCullBack);
    graphics->setBlendMode(CSBlendNone);
    graphics->setDepthMode(CSDepthRead);
    graphics->commit();
    
    bool usingDepthTexture = false;
    
    foreach(const Water&, water, waters) {
        Program* program = this->program(water);
        
        bool nextUsingDepthTexture = program->uniforms[WaterUniformDepthTexture] >= 0;

        //일부 안드로이드에서 뎁스버퍼가 프레임버퍼에 붙어 있는 상태로 뎁스버퍼의 값을 읽을 수 없다. 따라서 쉐이더에서 뎁스테스팅을 하는 식으로 처리했다.
        if (usingDepthTexture != nextUsingDepthTexture) {
            usingDepthTexture = nextUsingDepthTexture;
            if (usingDepthTexture) {
                glDisableCS(GL_DEPTH_TEST);
				glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            }
            else {
                glEnableCS(GL_DEPTH_TEST);
				glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, graphics->target()->depthStencilTexture(), 0);
            }
        }
        glUseProgramCS(program->object());
        
        glUniformMatrix4fvCS(program->uniforms[WaterUniformViewProjection], 1, 0, graphics->camera().viewProjection());
        glUniformMatrix4fvCS(program->uniforms[WaterUniformWorld], 1, 0, graphics->world());
        
        glUniform1fCS(program->uniforms[WaterUniformPositionScale], TileSize);
        
		glActiveTextureCS(GL_TEXTURE2);
		glBindTextureCS(GL_TEXTURE_2D, screen->texture0());
		glUniform1iCS(program->uniforms[WaterUniformDestTexture], 2);
		glUniform2fCS(program->uniforms[WaterUniformResolution], graphics->target()->width(), graphics->target()->height());
        
        CSVector2 forward(progress * water.forwardSpeed * CSMath::sin(water.angle), progress * water.forwardSpeed * CSMath::cos(water.angle));
        CSVector2 cross(progress * water.crossSpeed * -CSMath::cos(water.angle), progress * water.crossSpeed * CSMath::sin(water.angle));
        
        if (water.perturbImage) {
            glActiveTextureCS(GL_TEXTURE0);
            glBindTextureCS(GL_TEXTURE_2D, water.perturbImage->texture0());
            glUniform1iCS(program->uniforms[WaterUniformPerturbTexture], 0);
            glUniform1fCS(program->uniforms[WaterUniformPerturbWeight], water.perturbWeight);
            glUniform2fCS(program->uniforms[WaterUniformPerturbScale],
                          (float)TileSize / (water.perturbScale * water.perturbImage->textureWidth()),
                          (float)TileSize / (water.perturbScale * water.perturbImage->textureHeight()));
            glUniform2fCS(program->uniforms[WaterUniformPerturbFlowForward],
                          CSMath::frac(forward.x / water.perturbImage->textureWidth()),
                          CSMath::frac(forward.y / water.perturbImage->textureHeight()));
            if (water.crossSpeed) {
                glUniform2fCS(program->uniforms[WaterUniformPerturbFlowCross],
                              CSMath::frac(cross.x / water.perturbImage->textureWidth()),
                              CSMath::frac(cross.y / water.perturbImage->textureHeight()));
            }
        }
        if (water.foamImage) {
            glActiveTextureCS(GL_TEXTURE1);
            glBindTextureCS(GL_TEXTURE_2D, water.foamImage->texture0());
            glUniform1iCS(program->uniforms[WaterUniformFoamTexture], 1);
            glUniform1fCS(program->uniforms[WaterUniformFoamWeight], water.foamWeight);
            glUniform2fCS(program->uniforms[WaterUniformFoamScale],
                          (float)TileSize / (water.foamScale * water.foamImage->textureWidth()),
                          (float)TileSize / (water.foamScale * water.foamImage->textureHeight()));
            glUniform2fCS(program->uniforms[WaterUniformFoamFlowForward],
                          CSMath::frac(forward.x / water.foamImage->textureWidth()),
                          CSMath::frac(forward.y / water.foamImage->textureHeight()));
            if (water.crossSpeed) {
                glUniform2fCS(program->uniforms[WaterUniformFoamFlowCross],
                              CSMath::frac(cross.x / water.foamImage->textureWidth()),
                              CSMath::frac(cross.y / water.foamImage->textureHeight()));
            }
            if (water.foamShallowDepth) {
                glUniform1fCS(program->uniforms[WaterUniformFoamShallowDepth], water.foamShallowDepth);
            }
        }
        if (water.waveWidth && water.waveHeight) {
            glUniform1fCS(program->uniforms[WaterUniformWaveProgress], progress);
            glUniform1fCS(program->uniforms[WaterUniformWaveWidth], water.waveWidth);
            glUniform1fCS(program->uniforms[WaterUniformWaveHeight], water.waveHeight);
        }
        
        glUniform4fvCS(program->uniforms[WaterUniformShallowColor], 1, graphics->color() * water.shallowColor);
        
        if (water.depthMax) {
            glUniform4fvCS(program->uniforms[WaterUniformDeepColor], 1, graphics->color() * water.deepColor);
            glUniform1fCS(program->uniforms[WaterUniformDepthMax], water.depthMax);
        }
        if (usingDepthTexture) {
            glActiveTextureCS(GL_TEXTURE3);
            glBindTextureCS(GL_TEXTURE_2D, graphics->target()->depthStencilTexture());
            glUniform1iCS(program->uniforms[WaterUniformDepthTexture], 3);
            glUniform1fCS(program->uniforms[WaterUniformNear], graphics->camera().znear());
            glUniform1fCS(program->uniforms[WaterUniformFar], graphics->camera().zfar());
        }
        if (water.specWeight) {
            glUniform1fCS(program->uniforms[WaterUniformSpecPower], water.specPower);
            glUniform3fvCS(program->uniforms[WaterUniformLightDirection], 1, graphics->light().direction);
            glUniform3fvCS(program->uniforms[WaterUniformLightColor], 1, graphics->light().color * water.specWeight);
            glUniform3fvCS(program->uniforms[WaterUniformEye], 1, graphics->camera().position());
        }
		glActiveTextureCS(GL_TEXTURE4);
		glBindTextureCS(GL_TEXTURE_2D, vision->texture0());
		glUniform1iCS(program->uniforms[WaterUniformVisionTexture], 4);
		glUniform2fCS(program->uniforms[WaterUniformVisionScale], 1.0f / vision->width(), 1.0f / vision->height());

        water.vertices->render(GL_TRIANGLES);
    }
    if (usingDepthTexture) {
        glEnableCS(GL_DEPTH_TEST);
		glFramebufferTexture2DCS(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, graphics->target()->depthStencilTexture(), 0);
    }
    graphics->pop();
}
