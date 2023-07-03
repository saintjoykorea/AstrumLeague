//
//  Tile.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Tile_h
#define Tile_h

#include "Object.h"

enum TileAttribute : byte {
    TileAttributeHidden,
    TileAttributeUnabled,
    TileAttributeMove,
    TileAttributeBush,
    TileAttributeBuild
};

struct Tile;

struct TileObject {
private:
    Object* _object;
    TileObject* _next;
public:
    inline const Object* object() const {
        return _object;
    }
    inline const TileObject* next() const {
        return _next;
    }
    inline Object* object() {
        return _object;
    }
    inline TileObject* next() {
        return _next;
    }
    friend struct Tile;
};

struct Tile {
private:
    TileAttribute _attribute;
    bool _minimapUpdated;
    bool _visionUpdated;
    TileObject* _objects;
    fixed _altitude;
    fixed _fog[MaxAlliances];
    fixed _vision[MaxAlliances];
    ushort _detection[MaxAlliances];
public:
    Tile(CSBuffer* buffer, fixed altitude);
    ~Tile();
    
    inline Tile(const Tile&) {
        CSAssert(false, "invalid operation");
    }
    inline Tile& operator=(const Tile&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    inline TileAttribute attribute() const {
        return _attribute;
    }
    inline fixed altitude() const {
        return _altitude;
    }
    inline TileObject* objects() {
        return _objects;
    }
    inline const TileObject* objects() const {
        return _objects;
    }
    bool locate(Object* obj);
    bool unlocate(Object* obj);
    bool relocate(Object* obj);
    
    struct ChangeVisionReturn {
        bool minimapUpdated;
        bool visionUpdated;
    };
private:
    ChangeVisionReturn getChangeVisionReturn(int alliance, bool detecting, fixed degree);
public:
    ChangeVisionReturn changeVision(int alliance, bool detecting, bool visible, fixed degree);
    bool updateVision();
    bool updateMinimap();
    
    inline void rollbackMinimap() {
        _minimapUpdated = false;
    }
    
    inline void rollbackVision() {
        _visionUpdated = false;
    }
    
    bool isVisible() const;
    bool isDetected() const;
    
    inline bool isVisible(int alliance) const {
        return _vision[alliance];
    }
    
    inline bool isDetected(int alliance) const {
        return _detection[alliance] != 0;
    }
    
    int luminance() const;
    CSColor color(const IPoint& point) const;
};

#endif /* Tile_h */
