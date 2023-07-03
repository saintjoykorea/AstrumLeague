//
//  Terrain.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Terrain_h
#define Terrain_h

#include "Asset.h"

#include "Water.h"
#include "IPoint.h"
#include "FPoint.h"

#define TileSize        40
#define TileCell        8
#define CellSize        5
#define ShadowScale     0.5f

extern const fixed EpsilonCollider;

class Terrain {
public:
    struct Thumbnail {
        const CSImage* image;
        byte left;
        byte right;
        byte top;
        byte bottom;
    };
private:
    byte _width;
    byte _height;
    Thumbnail _thumbnail;
    CSColor _ambient;
    CSLight _light;
    fixed* _altitudes;
	CSVector2 _surfaceOffset;
    CSTVertexArray<SurfaceVertex>* _surfaceVertices;
    CSArray<SurfaceSlice>* _surfaceSlices;
    CSArray<Water>* _waters;
    CSGraphics* _shadowGraphics;
    float _progress;
public:
    Terrain(CSBuffer* buffer);
    ~Terrain();
private:
    Terrain(const Terrain& other);
    Terrain& operator =(const Terrain& other);
private:
    void loadAltitude(CSBuffer* buffer);
    void loadSurface(CSBuffer* buffer);
    void loadShadow();
public:
    inline int width() const {
        return _width;
    }
    inline int height() const {
        return _height;
    }
    inline const Thumbnail& thumbnail() const {
        return _thumbnail;
    }
    inline const CSColor& ambient() const {
        return _ambient;
    }
    inline const CSLight& light() const {
        return _light;
    }
    fixed altitude(const IPoint& p) const;
    fixed altitude(const FPoint& p) const;
private:
    bool isZQuad(const fixed* altitudes, int i) const;
    CSVector3 normal(const fixed* altitudes, int i) const;
public:
    void update(float delta);
    void drawSurface(CSGraphics* graphics);
    void drawWater(CSGraphics* graphics, const CSRootImage* vision);
    
    inline CSGraphics* shadowGraphics() {
        return _shadowGraphics;
    }
};

#endif /* Terrain_h */
