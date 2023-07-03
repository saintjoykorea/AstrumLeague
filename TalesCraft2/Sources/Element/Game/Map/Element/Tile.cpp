//
//  Tile.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Tile.h"

#include "Map.h"
#include "FogBuilding.h"
#include "FogResource.h"

Tile::Tile(CSBuffer* buffer, fixed altitude) :
    _objects(NULL),
    _altitude(altitude),
    _vision{},
    _fog{},
    _detection{}
{
    _attribute = (TileAttribute) buffer->readByte();

#ifdef UseNoVision
    for (int i = 0; i < MaxAlliances; i++) _vision[i] = fixed::One;
#endif
}

Tile::~Tile() {
    TileObject* current = _objects;
    
    while (current) {
        TileObject* next = current->_next;
        delete current;
        current = next;
    }
}

bool Tile::locate(Object* obj) {
    TileObject* insert = new TileObject();
    insert->_object = obj;
    insert->_next = _objects;
    _objects = insert;
    return relocate(obj);
}

bool Tile::unlocate(Object* obj) {
    TileObject** current = &_objects;
    
    while (*current) {
        if ((*current)->_object == obj) {
            TileObject* next = (*current)->_next;
            delete *current;
            *current = next;
            break;
        }
        current = &(*current)->_next;
    }
    return relocate(obj);
}

bool Tile::relocate(Object *obj) {
    return obj->color(isVisible(), isDetected()).a && updateMinimap();
}

typename Tile::ChangeVisionReturn Tile::getChangeVisionReturn(int alliance, bool detecting, fixed degree) {
    ChangeVisionReturn rtn = { false };
    
    const Force* force;
    if (Map::sharedMap()->isFocusing() && (force = Map::sharedMap()->force())) {
        if (force->alliance() == alliance) {
            if (_vision[alliance] < fixed::One) {
                rtn.visionUpdated = updateVision();
                rtn.minimapUpdated = updateMinimap();
            }
            else if (detecting && !_detection[alliance]) {
                rtn.minimapUpdated = updateMinimap();
            }
        }
    }
    else {
        if (_vision[alliance] < fixed::One) {
            bool updated = true;
            for (int i = 0; i < MaxAlliances; i++) {
                if (i != alliance && _vision[i] && _vision[i] >= _vision[alliance] + degree) {
                    updated = false;
                    break;
                }
            }
            if (updated) {
                rtn.visionUpdated = updateVision();
                rtn.minimapUpdated = updateMinimap();
            }
        }
    }
    return rtn;
}

typename Tile::ChangeVisionReturn Tile::changeVision(int alliance, bool detecting, bool visible, fixed degree) {
    if (visible) {
        if (!_vision[alliance]) {
            TileObject* current = _objects;
            
            while (current) {
                TileObject* next = current->_next;
                
                if (current->_object->compareType(ObjectMaskFog)) {
                    Fog* fog = static_cast<Fog*>(current->_object);
                    
                    if (fog->alliance == alliance) {
                        fog->unlocate();
                    }
                }
                current = next;
            }
        }
        ChangeVisionReturn rtn = getChangeVisionReturn(alliance, detecting, degree);
        
        _vision[alliance] += degree;
        if (_fog[alliance] < _vision[alliance]) {
            _fog[alliance] = _vision[alliance];
        }
        if (detecting) _detection[alliance]++;
        
        return rtn;
    }
    else if (_vision[alliance]) {
        _vision[alliance] -= degree;
        if (detecting) _detection[alliance]--;
        if (_vision[alliance] <= fixed::Zero) {
            _vision[alliance] = fixed::Zero;
            
            TileObject* current = _objects;
            
            while (current) {
                TileObject* next = current->_next;
                
                if (current->_object->compareType(ObjectMaskBuilding)) {
                    const Unit* building = static_cast<const Unit*>(current->_object);
                    
                    if (building->isAlive() && building->isDetected(alliance) && !Map::sharedMap()->isVisible(building->bounds(), alliance)) {
                        FogBuilding::fog(building, alliance);
                    }
                }
                else if (current->_object->compareType(ObjectMaskResource)) {
                    const Resource* resource = static_cast<const Resource*>(current->_object);
                    
                    if (resource->isDisplaying(alliance) && !Map::sharedMap()->isVisible(resource->bounds(), alliance)) {
                        FogResource::fog(resource, alliance);
                    }
                }
                current = next;
            }
        }
        return getChangeVisionReturn(alliance, detecting, fixed::Zero);
    }
    else {
        ChangeVisionReturn rtn = { false };
        
        return rtn;
    }
}

bool Tile::updateVision() {
    if (!_visionUpdated) {
        _visionUpdated = true;
        return true;
    }
    return false;
}

bool Tile::updateMinimap() {
    if (!_minimapUpdated) {
        _minimapUpdated = true;
        return true;
    }
    return false;
}

int Tile::luminance() const {
    fixed vision, fog;
    const Force* force;
    if (Map::sharedMap()->isFocusing() && (force = Map::sharedMap()->force())) {
        vision = _vision[force->alliance()];
        fog = _fog[force->alliance()];
    }
    else {
        vision = fixed::Zero;
        fog = fixed::Zero;
        
        for (int i = 0; i < MaxAlliances; i++) {
            if (vision < _vision[i]) {
                vision = _vision[i];
            }
            if (fog < _fog[i]) {
                fog = _fog[i];
            }
        }
    }
    int rtn;
    if (vision >= fixed::One) {
        rtn = 255;
    }
    else {
        if (fog >= fixed::One) {
            rtn = VisionLuminanceFog;
        }
        else {
            rtn = VisionLuminanceHidden;
			if (fog) {
				rtn += (VisionLuminanceFog - VisionLuminanceHidden) * (float)fog;
			}
        }
        if (vision) {
            rtn += (255 - rtn) * (float)vision;
        }
    }
    return rtn;
}

bool Tile::isVisible() const {
    const Force* force;
    if (Map::sharedMap()->isFocusing() && (force = Map::sharedMap()->force())) {
        return _vision[force->alliance()] != fixed::Zero;
    }
    for (int i = 0; i < MaxAlliances; i++) {
        if (_vision[i]) {
            return true;
        }
    }
    return false;
}

bool Tile::isDetected() const {
    const Force* force;

    return Map::sharedMap()->isFocusing() && (force = Map::sharedMap()->force()) ? _detection[force->alliance()] != 0 : true;
}

CSColor Tile::color(const IPoint& point) const {
    TileObject* current = _objects;
    
    bool visible = isVisible();
    bool detecting = isDetected();
    
    while (current) {
        const CSColor& color = current->_object->color(visible, detecting);
        
        if (color.a) {
            float d2 = ((CSVector2)current->_object->point()).distanceSquared(CSVector2(point.x + 0.5f, point.y + 0.5f));
            float c = (float)current->_object->collider() + 0.5f;
            
            if (d2 < c * c) {
                return color;
            }
        }
        current = current->_next;
    }
    return CSColor(0, 0, 0, 255 - luminance());
}
