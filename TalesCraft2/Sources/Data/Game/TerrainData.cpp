//
//  TerrainData.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "TerrainData.h"

#include "GameConfig.h"

TerrainData::TerrainData() {
    CSBuffer* buffer = CSBuffer::createWithContentOfFile(CSFile::findPath("terrain.xmf"), true);
    
    surfaceImages = CSObject::retain(buffer->readArrayFunc<CSRootImage>(CSRootImage::imageWithBuffer));
    waterImages = CSObject::retain(buffer->readArrayFunc<CSRootImage>(CSRootImage::imageWithBuffer));
    
    CSAssert(buffer->position() == buffer->length(), "invalid data");
    buffer->release();

    surfaceRenderer = new SurfaceRenderer();
	visionRenderer = new VisionRenderer();
    waterRenderer = new WaterRenderer();
    gridRenderer = new GridRenderer();
}

TerrainData::~TerrainData() {
    surfaceImages->release();
    waterImages->release();
    
    delete surfaceRenderer;
	delete visionRenderer;
    delete waterRenderer;
    delete gridRenderer;
}
