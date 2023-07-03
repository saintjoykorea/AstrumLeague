//
//  Terrain.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 8..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Terrain.h"

const fixed EpsilonCollider(fixed::Raw, 256);

Terrain::Terrain(CSBuffer* buffer) {
    _width = buffer->readByte() & 0xff;
    _height = buffer->readByte() & 0xff;
    _thumbnail.image = CSRootImage::createWithBuffer(buffer);
    _thumbnail.left = buffer->readByte();
    _thumbnail.right = buffer->readByte();
    _thumbnail.top = buffer->readByte();
    _thumbnail.bottom = buffer->readByte();
    _ambient = CSColor(buffer, true);
    _light = CSLight(buffer);
    loadAltitude(buffer);
    loadSurface(buffer);
    loadShadow();
    _waters = CSObject::retain(buffer->readArrayWithParam<Water>(this));
    _progress = 0;
}

Terrain::~Terrain() {
    _thumbnail.image->release();
    free(_altitudes);
    _surfaceVertices->release();
    _surfaceSlices->release();
    CSObject::release(_waters);
    _shadowGraphics->release();
}

void Terrain::loadAltitude(CSBuffer* buffer) {
    int altitudeDataLength = (_width + 1) * (_height + 1) * sizeof(fixed);
    _altitudes = (fixed*)fmalloc(altitudeDataLength);
    buffer->read(_altitudes, altitudeDataLength);
    
    //memset(_altitudes, 0, altitudeDataLength);          //TODO:DEBUG
}

void Terrain::loadSurface(CSBuffer* buffer) {
    CSStopWatchStart();
    
    int lightCount = buffer->readLength();
    CSRootImage** light = (CSRootImage**)alloca(lightCount * sizeof(CSRootImage*));
    
    for (int i = 0; i < lightCount; i++) {
        light[i] = CSRootImage::imageWithBuffer(buffer);
    }
    
	_surfaceOffset.x = buffer->readShort();
	_surfaceOffset.y = buffer->readShort();

	int totalQuadCount = buffer->readInt();
    
    _surfaceVertices = CSTVertexArray<SurfaceVertex>::create(totalQuadCount, 4, 6, false, false, SurfaceVertexLayouts);
    _surfaceSlices = new CSArray<SurfaceSlice>(1);
    _surfaceVertices->beginVertex();
    _surfaceVertices->beginIndex();

    int lightIndex = 0;
    int lightX = 0;
    int lightY = 0;
    
    int indexCount = 0;
    
    int materialCount = buffer->readLength();
    
    for (int i = 0; i < materialCount; i++) {
        int surfaceImageIndexLen = buffer->readLength();
        CSAssert(surfaceImageIndexLen == 1, "invalid data");
        int surfaceImageIndex = buffer->readShort();
        
        CSMaterial material(buffer);
        float scale = buffer->readFloat();
        float rotation = buffer->readFloat();
        
        int quadCount = buffer->readInt();
            
        for (int j = 0; j < quadCount; j++) {
            int x = buffer->readByte() & 0xff;
            int y = buffer->readByte() & 0xff;
            
            fixed altitudes[16];
            {
                int a = 0;
                for (int v = -1; v <= 2; v++) {
                    for (int h = - 1; h <= 2; h++) {
                        altitudes[a++] = altitude(IPoint(x + h, y + v));
                    }
                }
            }
            
            SurfaceVertex vertices[4];
            vertices[0].position = CSVector3(x, y, altitudes[5]);
            vertices[1].position = CSVector3(x + 1, y, altitudes[6]);
            vertices[2].position = CSVector3(x, y + 1, altitudes[9]);
            vertices[3].position = CSVector3(x + 1, y + 1, altitudes[10]);
            
            vertices[0].normal = normal(altitudes, 5);
            vertices[1].normal = normal(altitudes, 6);
            vertices[2].normal = normal(altitudes, 9);
            vertices[3].normal = normal(altitudes, 10);
                
            int lightWidth = light[lightIndex]->textureWidth();
            int lightHeight = light[lightIndex]->textureHeight();
            
            float lu = (float)(lightX + 1) / lightWidth;
            float ru = (float)(lightX + TileCell + 1) / lightWidth;
            float tv = (float)(lightY + 1) / lightHeight;
            float bv = (float)(lightY + TileCell + 1) / lightHeight;
            
            vertices[0].lightCoord = CSVector2(lu, tv);
            vertices[1].lightCoord = CSVector2(ru, tv);
            vertices[2].lightCoord = CSVector2(lu, bv);
            vertices[3].lightCoord = CSVector2(ru, bv);
            
            uint vi = _surfaceVertices->vertexCount();
            
            if (isZQuad(altitudes, 5)) {
                _surfaceVertices->addIndex(vi);
                _surfaceVertices->addIndex(vi + 1);
                _surfaceVertices->addIndex(vi + 2);
                _surfaceVertices->addIndex(vi + 1);
                _surfaceVertices->addIndex(vi + 3);
                _surfaceVertices->addIndex(vi + 2);
            }
            else {
                _surfaceVertices->addIndex(vi);
                _surfaceVertices->addIndex(vi + 1);
                _surfaceVertices->addIndex(vi + 3);
                _surfaceVertices->addIndex(vi);
                _surfaceVertices->addIndex(vi + 3);
                _surfaceVertices->addIndex(vi + 2);
            }
            
            _surfaceVertices->addVertex(vertices[0]);
            _surfaceVertices->addVertex(vertices[1]);
            _surfaceVertices->addVertex(vertices[2]);
            _surfaceVertices->addVertex(vertices[3]);
            
            indexCount += 6;
            
            lightX += (TileCell + 3);
            if (lightX + (TileCell + 3) > lightWidth) {
                lightX = 0;
                lightY += (TileCell + 3);
                
                if (lightY + (TileCell + 3) > lightHeight) {
                    new (&_surfaceSlices->addObject()) SurfaceSlice(surfaceImageIndex, material, scale, rotation, light[lightIndex], indexCount);
                    
                    lightY = 0;
                    lightIndex++;
                    
                    indexCount = 0;
                }
            }
        }
        if (indexCount) {
            new (&_surfaceSlices->addObject()) SurfaceSlice(surfaceImageIndex, material, scale, rotation, light[lightIndex], indexCount);
            indexCount = 0;
        }
    }

    _surfaceVertices->transfer();
    
    CSStopWatchElapsed("terrain load");
}

void Terrain::loadShadow() {
    CSRootImage* shadow = CSRootImage::imageWithRaw(NULL, CSImageFormatRawR8, 2048, 2048);
    _shadowGraphics = new CSGraphics(CSRenderTarget::target(shadow, false));
}

fixed Terrain::altitude(const IPoint &p) const {
    int x = CSMath::clamp(p.x, 0, (int)_width);
    int y = CSMath::clamp(p.y, 0, (int)_height);
    
    return _altitudes[y * (_width + 1) + x];
}

fixed Terrain::altitude(const FPoint &p) const {
    int x = p.x;
    int y = p.y;
    
    if (p.x <= fixed::Zero || p.x >= _width || p.y <= fixed::Zero || p.y >= _height || (p.x == x && p.y == y)) return altitude(IPoint(x, y));
    
    fixed xd = p.x - x;
    fixed yd = p.y - y;

    fixed h0 = altitude(IPoint(x, y));
    fixed h1 = altitude(IPoint(x + 1, y));
    fixed h2 = altitude(IPoint(x, y + 1));
    fixed h3 = altitude(IPoint(x + 1, y + 1));
    
    fixed a;
    
    if (CSMath::abs(h3 - h0) >= CSMath::abs(h2 - h1)) {
        if (xd <= fixed::One - yd) {
            a = h0 * (fixed::One - xd - yd) + h1 * xd + h2 * yd;
        }
        else {
            a = h1 * (fixed::One - yd) + h3 * (xd + yd - fixed::One) + h2 * (fixed::One - xd);
        }
    }
    else {
        if (xd <= yd) {
            a = h0 * (fixed::One - yd) + h3 * xd + h2 * (yd - xd);
        }
        else {
            a = h0 * (fixed::One - xd) + h1 * (xd - yd) + h3 * yd;
        }
    }
    return a;
}

bool Terrain::isZQuad(const fixed* altitudes, int i) const {
    return CSMath::abs(altitudes[i + 5] - altitudes[i]) >= CSMath::abs(altitudes[i + 4] - altitudes[i + 1]);
}

CSVector3 Terrain::normal(const fixed* altitudes, int i) const {
    IPoint ps[8];
    int count = 0;
    
    if (!isZQuad(altitudes, i - 5)) ps[count++] = IPoint(-1, -1);
    ps[count++] = IPoint(0, -1);

    if (isZQuad(altitudes, i - 4)) ps[count++] = IPoint(1, -1);
    ps[count++] = IPoint(1, 0);

    if (!isZQuad(altitudes, i)) ps[count++] = IPoint(1, 1);
    ps[count++] = IPoint(0, 1);

    if (isZQuad(altitudes, i - 1)) ps[count++] = IPoint(-1, 1);
    ps[count++] = IPoint(-1, 0);
    
    CSVector3 normal = CSVector3::Zero;
    for (int j = 0; j < count; j++) {
        const IPoint& p0 = ps[j];
        const IPoint& p1 = ps[(j + 1) % count];
        
        CSVector3 v0(p0.x, p0.y, (float)(altitudes[i + (p0.y * 4) + p0.x] - altitudes[i]));
        CSVector3 v1(p1.x, p1.y, (float)(altitudes[i + (p1.y * 4) + p1.x] - altitudes[i]));
        
        normal += CSVector3::cross(v0, v1);
    }
    normal.normalize();
    
    return normal;
}

void Terrain::update(float delta) {
    _progress += delta;
}

void Terrain::drawSurface(CSGraphics *graphics) {
	const CSRootImage* shadow = _shadowGraphics->target()->screen(false);

    Asset::sharedAsset()->terrain->surfaceRenderer->draw(graphics, _surfaceVertices, _surfaceSlices, _surfaceOffset, shadow);
}

void Terrain::drawWater(CSGraphics *graphics, const CSRootImage* vision) {
    if (_waters) {
        Asset::sharedAsset()->terrain->waterRenderer->draw(graphics, _waters, vision, _progress);
    }
}
