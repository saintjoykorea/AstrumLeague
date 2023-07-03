//
//  NavMesh.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef NavMesh_h
#define NavMesh_h

#include "Terrain.h"

class NavMesh;

class Path : public CSObject {
private:
    struct Node {
        FPoint point;
        FPoint bridgePoint0;
        FPoint bridgePoint1;
        uint slabId;
        sbyte dir;
        bool obstacle;
        
        Node(const FPoint& point, const FPoint& bridgePoint0, const FPoint& bridgePoint1, uint slabId, bool obstacle);
    };
    CSArray<Node>* _nodes;
	FPoint _lastPoint;
    uint _lastSlabId;
    bool _complete;
    uint _pointIndex;
public:
    Path(uint count, const FPoint& lastPoint, uint lastSlabId, bool complete);
private:
    ~Path();
public:
    static inline Path* path(uint capacity, const FPoint& lastPoint, uint lastSlabId, bool complete) {
        return autorelease(new Path(capacity, lastPoint, lastSlabId, complete));
    }
    inline bool isComplete() const {
        return _complete;
    }
    inline int pointCount() const {
        return _nodes->count();
    }
    const CSArray<FPoint>* points() const;
    inline void clear() {
        _nodes->removeAllObjects();
    }
    inline uint pointIndex() const {
        return _pointIndex;
    }
    const FPoint& point(const FPoint& p0, const FPoint& p1, bool updating, int* dir);

	inline const FPoint& lastPoint() const {
		return _lastPoint;
	}

    void rewind();
    
    friend class NavMesh;
};

class PathObject : public CSObject {
protected:
	FPoint _point;
	fixed _collider;
public:
    inline virtual ~PathObject() {
        
    }
	inline const FPoint& point() const {
		return _point;
	}
	inline fixed collider() const {
		return _collider;
	}
};

enum ProgressPathReturn {
    ProgressPathSuccess,
    ProgressPathFail,
    ProgressPathOut
};

class NavMesh {
private:
    struct Slab;
    
    struct Edge {
        FPoint point;
        Edge* next;
        Edge* prev;
        Slab* slab;
        bool open;
        
        Edge(const FPoint& point);
    };
    struct Bridge {
        Edge* edge0;
        Edge* edge1;
        Slab* slab0;
        Slab* slab1;
        fixed collider;
        fixed half;
        FPoint normal;
        
        Bridge(Edge* e0, Edge* e1, Slab* slab0, fixed collider);
        
        FPoint point0(fixed collider) const;
        FPoint point1(fixed collider) const;
        bool pass(fixed collider, const PathObject* target) const;
    };
    struct Slab {
        uint id;
        uint group;
        const PathObject* obstacle;
        CSArray<Edge*>* edges;
        CSArray<Bridge*>* bridges;
        fixed minx;
        fixed maxx;
        fixed miny;
        fixed maxy;
        bool marked;
        
        Slab(uint id, const PathObject* obstacle);
        ~Slab();
    };
    enum Grid : byte {
        GridClosed,
        GridIntersect,
        GridOpen
    };
    uint _width;
    uint _height;
    Grid* _grids;
    CSArray<Edge*>* _edges;
    CSArray<Edge*>* _lines;
    CSArray<Bridge*>* _bridges;
    CSArray<Slab*>* _slabs;
    uint _slabId;
    uint _slabGroup;

public:
    NavMesh(uint width, uint height, const bool* fills);
    ~NavMesh();
private:
    NavMesh(const NavMesh& other);
    NavMesh& operator =(const NavMesh& other);
private:
    inline Grid grid(const IPoint& p) const {
        return p.x >= 0 && p.x < _width && p.y >= 0 && p.y < _height ? _grids[p.y * _width + p.x] : GridClosed;
    }
    void init(const bool* fills);
    bool edgeIntersects(const Edge* e0, const Edge* e1) const;
    bool slabContains(const Slab* slab, const FPoint& p) const;
    bool slabsContains(const Edge* e) const;
    void groupSlabs(Slab* first, CSArray<Slab*>* slabs);
    void resolveSlabs();
    void resolveSlab(Edge* e0, Edge* e1);
    void removeSlab(Slab* slab);
    Slab* findSlab(uint slabId) const;
    Slab* findSlab(const FPoint& p) const;
public:
    bool locatable(const FPoint& p) const;
    void locateObstacle(const PathObject* obstacle);
    void unlocateObstacle(const PathObject* obstacle);
public:
    bool hasPath(const FPoint& p0, const FPoint& p1) const;
    fixed distance(const FPoint& p0, const FPoint& p1) const;
    
    struct Area {
        Slab* slab;
        Slab* nearSlab;
        FPoint nearPoint;
        FPoint nearBridgePoint0;
        FPoint nearBridgePoint1;
    };
	Area findArea(const FPoint& p, const PathObject* target, uint group) const;
private:
    ProgressPathReturn progressPath(const FPoint& p, CSArray<Path::Node>* nodes, int i) const;
public:
    ProgressPathReturn progressPath(const PathObject* obj, const PathObject* target, Path* path) const;
    Path* findPath(const PathObject* obj, const PathObject* target) const;
    
    void draw(CSGraphics* graphics, const Terrain* terrain);
    void draw(CSGraphics* graphics, float scale);
};

#endif /* NavMesh_h */
