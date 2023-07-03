//
//  TerrainData.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef TerrainData_h
#define TerrainData_h

#include "SurfaceRenderer.h"
#include "VisionRenderer.h"
#include "WaterRenderer.h"
#include "GridRenderer.h"

struct TerrainData {
    const CSArray<CSRootImage>* surfaceImages;
    const CSArray<CSRootImage>* waterImages;
    SurfaceRenderer* surfaceRenderer;
	VisionRenderer* visionRenderer;
    WaterRenderer* waterRenderer;
    GridRenderer* gridRenderer;
    
    TerrainData();
    ~TerrainData();
    
    TerrainData(const TerrainData&);
    TerrainData& operator=(const TerrainData&);
};

#endif /* TerrainData_h */
