//
//  SurfaceRenderer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "SurfaceRenderer.h"

#include "Asset.h"

#include "Terrain.h"

SurfaceSlice::SurfaceSlice(int surfaceImageIndex, const CSMaterial& material, float scale, float rotation, const CSRootImage* light, uint indexCount) :
    surfaceImageIndex(surfaceImageIndex),
    material(material),
    scale(scale),
    rotation(rotation),
    light(CSObject::retain(light)),
    indexCount(indexCount)
{
    
}
SurfaceSlice::~SurfaceSlice() {
    light->release();
}

enum SurfaceVertexAttrib {
    SurfaceVertexAttribPosition,
    SurfaceVertexAttribNormal,
    SurfaceVertexAttribLightCoord
};
static const CSVertexLayout __SurfaceVertexLayouts[] = {
    CSVertexLayout(SurfaceVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, offsetof(SurfaceVertex, position)),
    CSVertexLayout(SurfaceVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, offsetof(SurfaceVertex, normal)),
    CSVertexLayout(SurfaceVertexAttribLightCoord, 2, GL_FLOAT, GL_FALSE, offsetof(SurfaceVertex, lightCoord))
};
const CSArray<CSVertexLayout>* SurfaceVertexLayouts = new CSArray<CSVertexLayout>(__SurfaceVertexLayouts, 3);

static const char* uniformNames[] = {
    "world",
    "viewProjection",
    "positionScale",
    "surfaceTexture",
	"surfaceOffset",
    "surfaceScale",
    "surfaceRotation",
    "lightTexture",
	"shadowVisible",
    "shadowTexture",
    "shadowOffset",
    "shadowScale",
    "eye",
    "color",
    "lightAmbient",
    "lightColor",
    "lightDirection",
    "materialDiffuse",
    "materialSpecular",
    "materialShininess",
    "materialEmission"
};
SurfaceRenderer::SurfaceRenderer() {
    const char* vs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/surface.vsh"));
    const char* fs = *CSString::stringWithContentOfFile(CSFile::bundlePath("terrain/surface.fsh"));
    
    CSShader* vertexShader;
    CSShader* fragmentShader;
    bool link;
    
    _baseProgram = new CSProgram();
    vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER);
    fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER);
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    _baseProgram->attach(vertexShader);
    _baseProgram->attach(fragmentShader);
    glBindAttribLocationCS(_baseProgram->object(), SurfaceVertexAttribPosition, "attrPosition");
    link = _baseProgram->link();
    CSAssert(link, "program link fail");
    for (int i = 0; i < BaseUniformCount; i++) {
        _baseUniforms[i] = glGetUniformLocationCS(_baseProgram->object(), uniformNames[i]);
    }
    _surfaceProgram = new CSProgram();
    
    vertexShader = CSShader::shader(vs, GL_VERTEX_SHADER, "Surface");
    fragmentShader = CSShader::shader(fs, GL_FRAGMENT_SHADER, "Surface");
    CSAssert(vertexShader && fragmentShader, "shader compile fail");
    _surfaceProgram->attach(vertexShader);
    _surfaceProgram->attach(fragmentShader);
    glBindAttribLocationCS(_surfaceProgram->object(), SurfaceVertexAttribPosition, "attrPosition");
    glBindAttribLocationCS(_surfaceProgram->object(), SurfaceVertexAttribNormal, "attrNormal");
    glBindAttribLocationCS(_surfaceProgram->object(), SurfaceVertexAttribLightCoord, "attrLightCoord");
    link = _surfaceProgram->link();
    CSAssert(link, "program link fail");
    for (int i = 0; i < SurfaceUniformCount; i++) {
        _surfaceUniforms[i] = glGetUniformLocationCS(_surfaceProgram->object(), uniformNames[i]);
    }
}

SurfaceRenderer::~SurfaceRenderer() {
    _baseProgram->release();
    _surfaceProgram->release();
}

void SurfaceRenderer::draw(CSGraphics* graphics, const CSTVertexArray<SurfaceVertex>* arr, const CSArray<SurfaceSlice>* slices, const CSVector2& offset, const CSRootImage* shadow) {
    graphics->push();
    graphics->setCullMode(CSCullBack);
    
    graphics->setBlendMode(CSBlendNone);
    graphics->setDepthMode(CSDepthReadWrite);
    graphics->commit();
    glUseProgramCS(_baseProgram->object());
    glUniformMatrix4fvCS(_baseUniforms[BaseUniformWorld], 1, 0, graphics->world());
    glUniformMatrix4fvCS(_baseUniforms[BaseUniformViewProjection], 1, 0, graphics->camera().viewProjection());
    glUniform1fCS(_baseUniforms[BaseUniformPositionScale], TileSize);
    arr->setLayoutEnabled(SurfaceVertexAttribNormal, false);
    arr->setLayoutEnabled(SurfaceVertexAttribLightCoord, false);
    arr->render(GL_TRIANGLES);
    
    graphics->setBlendMode(CSBlendAdd);
    graphics->setDepthMode(CSDepthRead);
    graphics->commit();
    glUseProgramCS(_surfaceProgram->object());
    glUniformMatrix4fvCS(_surfaceUniforms[SurfaceUniformWorld], 1, 0, graphics->world());
    glUniformMatrix4fvCS(_surfaceUniforms[SurfaceUniformViewProjection], 1, 0, graphics->camera().viewProjection());
    glUniform1fCS(_surfaceUniforms[SurfaceUniformPositionScale], TileSize);
    glUniform3fvCS(_surfaceUniforms[SurfaceUniformEye], 1, graphics->camera().position());
    glUniform4fvCS(_surfaceUniforms[SurfaceUniformColor], 1, graphics->color());
    glUniform1fCS(_surfaceUniforms[SurfaceUniformLightAmbient], graphics->light().ambient);
    glUniform3fvCS(_surfaceUniforms[SurfaceUniformLightColor], 1, graphics->light().color);
    glUniform3fvCS(_surfaceUniforms[SurfaceUniformLightDirection], 1, graphics->light().direction);
    
	if (shadow) {
		glUniform1i(_surfaceUniforms[SurfaceUniformShadowVisible], 1);

		glActiveTextureCS(GL_TEXTURE2);
		glBindTextureCS(GL_TEXTURE_2D, shadow->texture0());
		glUniform1iCS(_surfaceUniforms[SurfaceUniformShadowTexture], 2);

		CSVector2 shadowOffset = (CSVector2)graphics->camera().target() * ShadowScale;
		static const float shadowRealScale = TileSize * ShadowScale;
		shadowOffset.x -= shadow->textureWidth() * 0.5f;
		shadowOffset.y -= shadow->textureHeight() * 0.5f;
		shadowOffset /= shadowRealScale;
		glUniform2fCS(_surfaceUniforms[SurfaceUniformShadowOffset], shadowOffset.x, shadowOffset.y);
		glUniform2fCS(_surfaceUniforms[SurfaceUniformShadowScale], shadowRealScale / shadow->textureWidth(), shadowRealScale / shadow->textureHeight());
	}
	else {
		glUniform1i(_surfaceUniforms[SurfaceUniformShadowVisible], 0);
	}

    arr->setLayoutEnabled(SurfaceVertexAttribNormal, true);
    arr->setLayoutEnabled(SurfaceVertexAttribLightCoord, true);

	glUniform2fCS(_surfaceUniforms[SurfaceUniformSurfaceOffset], offset.x, offset.y);

    uint indexOffset = 0;
    foreach(const SurfaceSlice&, slice, slices) {
        const CSRootImage* surfaceImage = Asset::sharedAsset()->terrain->surfaceImages->objectAtIndex(slice.surfaceImageIndex);
        
        glActiveTextureCS(GL_TEXTURE0);
        glBindTextureCS(GL_TEXTURE_2D, surfaceImage->texture0());
        glUniform1iCS(_surfaceUniforms[SurfaceUniformSurfaceTexture], 0);
        glUniform2fCS(_surfaceUniforms[SurfaceUniformSurfaceScale], (float)TileSize * slice.scale / surfaceImage->textureWidth(), (float)TileSize * slice.scale / surfaceImage->textureHeight());
        glUniform1fCS(_surfaceUniforms[SurfaceUniformSurfaceRotation], slice.rotation);
        
        glActiveTextureCS(GL_TEXTURE1);
        glBindTextureCS(GL_TEXTURE_2D, slice.light->texture0());
        glUniform1iCS(_surfaceUniforms[SurfaceUniformLightTexture], 1);
        
        glUniform4fvCS(_surfaceUniforms[SurfaceUniformMaterialDiffuse], 1, slice.material.diffuse);
        glUniform1fCS(_surfaceUniforms[SurfaceUniformMaterialSpecular], slice.material.specular);
        glUniform1fCS(_surfaceUniforms[SurfaceUniformMaterialShininess], slice.material.shininess);
        glUniform3fvCS(_surfaceUniforms[SurfaceUniformMaterialEmission], 1, slice.material.emission);
        
        arr->render(indexOffset, slice.indexCount, GL_TRIANGLES);
        
        indexOffset += slice.indexCount;
    }
    graphics->pop();
}

