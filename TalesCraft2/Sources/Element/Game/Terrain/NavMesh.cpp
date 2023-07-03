//
//  NavMesh.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 30..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "NavMesh.h"

Path::Node::Node(const FPoint& point, const FPoint& bridgePoint0, const FPoint& bridgePoint1, uint slabId, bool obstacle) :
    point(point),
    bridgePoint0(bridgePoint0),
    bridgePoint1(bridgePoint1),
    slabId(slabId),
    dir(0),
    obstacle(obstacle)
{

}

Path::Path(uint count, const FPoint& lastPoint, uint lastSlabId, bool complete) :
    _nodes(new CSArray<Node>(count)),
	_lastPoint(lastPoint),
    _lastSlabId(lastSlabId),
    _complete(complete),
    _pointIndex(count)
{

}

Path::~Path() {
    _nodes->release();
}

const CSArray<FPoint>* Path::points() const {
    CSArray<FPoint>* points = CSArray<FPoint>::arrayWithCapacity(_nodes->count() ? _nodes->count() : 1);
    foreach(const Node&, node, _nodes) {
        points->addObject(node.point);
    }
    return points;
}

const FPoint& Path::point(const FPoint& p0, const FPoint& p1, bool updating, int* dir) {
    if (_pointIndex > _nodes->count()) {
        _pointIndex = _nodes->count();
    }

    const FPoint* pp = &p1;
    int i = 0;
    
    while (i < _pointIndex) {
        bool flag = true;
        
        for (int j = i; j < _nodes->count(); j++) {
            const Node& node = _nodes->objectAtIndex(j);
            if (node.point == p0) {
                break;
            }
            if (!FPoint::intersectsLineToLine(p0, *pp, node.bridgePoint0, node.bridgePoint1)) {
                flag = false;
                break;
            }
        }
        if (flag) {
            break;
        }
        else {
            pp = &_nodes->objectAtIndex(i).point;
            i++;
        }
    }
    if (updating) {
        _pointIndex = i;
    }
    
    if (dir) {
        *dir = _nodes->count() ? _nodes->lastObject().dir : 0;
    }
    return *pp;
}

void Path::rewind() {
    _pointIndex = _nodes->count();
}

NavMesh::Edge::Edge(const FPoint& point) :
    point(point),
    next(NULL),
    prev(NULL),
    slab(NULL),
    open(true)
{

}

NavMesh::Bridge::Bridge(Edge* e0, Edge* e1, Slab* slab0, fixed collider) :
    edge0(e0),
    edge1(e1),
    slab0(slab0),
    slab1(NULL),
    collider(collider),
    half(CSMath::max(e0->point.distance(e1->point) / 2 - EpsilonCollider, fixed::Zero)),
    normal(FPoint::normalize(e1->point - e0->point))
{

}

FPoint NavMesh::Bridge::point0(fixed collider) const {
    return edge0->point + normal * CSMath::min(collider, half);
}

FPoint NavMesh::Bridge::point1(fixed collider) const {
    return edge1->point - normal * CSMath::min(collider, half);
}

bool NavMesh::Bridge::pass(fixed collider, const PathObject* target) const {
    return collider <= this->collider || (edge0->slab && edge0->slab->obstacle == target) || (edge1->slab && edge1->slab->obstacle == target);
}

NavMesh::Slab::Slab(uint id, const PathObject* obstacle) :
    id(id),
    group(0),
    obstacle(obstacle),
    edges(new CSArray<Edge*>(4)),
    bridges(new CSArray<Bridge*>()),
    marked(false)
{

}
NavMesh::Slab::~Slab() {
    edges->release();
    bridges->release();
}

NavMesh::NavMesh(uint width, uint height, const bool* fills) :
    _width(width),
    _height(height),
    _edges(new CSArray<Edge*>()),
    _lines(new CSArray<Edge*>()),
    _bridges(new CSArray<Bridge*>()),
    _slabs(new CSArray<Slab*>()),
    _slabId(0),
    _slabGroup(0)
{
    CSStopWatchStart();
    
    init(fills);
    
    CSStopWatchElapsed("init");
}

NavMesh::~NavMesh() {
    free(_grids);
    
    foreach(Slab*, s, _slabs) {
        delete s;
    }
    _slabs->release();
    foreach(Bridge*, b, _bridges) {
        delete b;
    }
    _bridges->release();

    foreach(Edge*, e, _edges) {
        delete e;
    }
    _edges->release();
    _lines->release();
}

struct InitialEdge {
    IPoint point;
    int dir;
    
    inline InitialEdge(const IPoint& point, int dir) : point(point), dir(dir) {
    
    }
};


static void shrinkLines(CSArray<IPoint>* lines, int from, int to, int step, CSArray<InitialEdge>* initEdges) {
    fixed dmax = fixed::Zero;
    int index = from;
    
    for (int i = from + 1; i < to; i++) {
        fixed d = ((FPoint)lines->objectAtIndex(i)).distanceSquaredToLine(lines->objectAtIndex(from), lines->objectAtIndex(to));
        
        if (d > dmax) {
            index = i;
            dmax = d;
        }
    }
    if (dmax >= fixed::One || step > 0) {
        int temp = lines->count();
        shrinkLines(lines, from, index, step - 1, initEdges);
        temp -= lines->count();
        index -= temp;
        to -= temp;
        shrinkLines(lines, index, to, step - 1, initEdges);
    }
    else {
        index = from + 1;
        for (int i = index; i < to; i++) {
            const IPoint& p = lines->objectAtIndex(index);
            for (int j = 0; j < initEdges->count(); j++) {
                if (initEdges->objectAtIndex(j).point == p) {
                    initEdges->removeObjectAtIndex(j);
                    break;
                }
            }
            lines->removeObjectAtIndex(index);
        }
    }
}

void NavMesh::init(const bool* fills) {
    IPoint p;
    
    _grids = (Grid*)fmalloc(_width * _height * sizeof(Grid));
    
    int i = 0;
    for (p.y = 0; p.y < _height; p.y++) {
        for (p.x = 0; p.x < _width; p.x++) {
            IPoint np;
            
            int nminx = CSMath::max(p.x - 2, 0);
            int nminy = CSMath::max(p.y - 2, 0);
            int nmaxx = CSMath::min(p.x + 2, (int)_width);
            int nmaxy = CSMath::min(p.y + 2, (int)_height);
            bool surrounded = true;
            bool intersected = false;
            
            for (np.y = nminy; np.y < nmaxy; np.y++) {
                for (np.x = nminx; np.x < nmaxx; np.x++) {
                    if (fills[np.y * _width + np.x]) {
                        intersected = true;
                    }
                    else {
                        surrounded = false;
                    }
                }
            }
            if (surrounded) {
                _grids[i] = GridClosed;
            }
            else if (intersected) {
                _grids[i] = GridIntersect;
            }
            else {
                _grids[i] = GridOpen;
            }
            i++;
        }
    }
    
    static const IPoint edgeOffsets[] = {
        IPoint(-1, -1),
        IPoint(0, -1),
        IPoint(-1, 0),
        IPoint(0, 0)
    };
    
    CSArray<InitialEdge>* initEdges = new CSArray<InitialEdge>();
    
    for (p.y = 0; p.y <= _height; p.y++) {
        for (p.x = 0; p.x <= _width; p.x++) {
            int open = 0;
            for (int i = 0; i < 4; i++) {
                IPoint np = p + edgeOffsets[i];
                
                if (np.x >= 0 && np.x < _width && np.y >= 0 && np.y < _height && !fills[np.y * _width + np.x]) {
                    open |= (1 << i);
                }
            }
            switch (open) {
                case 6:
                case 9:
                    CSAssert(false, "invalid data");
                    break;
                case 8:
                case 12:
                case 13:
                    initEdges->addObject(InitialEdge(p, 0));
                    break;
                case 4:
                case 5:
                case 7:
                    initEdges->addObject(InitialEdge(p, 1));
                    break;
                case 1:
                case 3:
                case 11:
                    initEdges->addObject(InitialEdge(p, 2));
                    break;
                case 2:
                case 10:
                case 14:
                    initEdges->addObject(InitialEdge(p, 3));
                    break;
            }
        }
    }
    static const IPoint lineOffsets[] = {
        IPoint(1, 0),
        IPoint(0, 1),
        IPoint(-1, 0),
        IPoint(0, -1)
    };
    CSArray<IPoint>* lines = CSArray<IPoint>::array();
    while (initEdges->count()) {
        lines->removeAllObjects();
        
        InitialEdge current = initEdges->objectAtIndex(0);
        lines->addObject(current.point);
        initEdges->removeObjectAtIndex(0);
        
        for (; ; ) {
            IPoint np = current.point + lineOffsets[current.dir];
            
            if (np == lines->objectAtIndex(0)) {
                lines->addObject(np);
                break;
            }
            else {
                bool flag = false;
                for (int i = 0; i < initEdges->count(); i++) {
                    const InitialEdge& other = initEdges->objectAtIndex(i);
                    
                    if (other.point == np) {
                        lines->addObject(np);
                        current = other;
                        initEdges->removeObjectAtIndex(i);
                        flag = true;
                        break;
                    }
                }
                CSAssert(flag, "invalid operation");
            }
        }
        CSAssert(lines->count() >= 4, "invalid operation");
        
        shrinkLines(lines, 0, lines->count() - 1, 2, initEdges);
        
        CSAssert(lines->count() >= 4, "invalid operation");
        
        Edge* firstEdge = new Edge(lines->objectAtIndex(0));
        _edges->addObject(firstEdge);
        
        Edge* currentEdge = firstEdge;
        for (int i = 1; i < lines->count() - 1; i++) {
            Edge* nextEdge = new Edge(lines->objectAtIndex(i));
            _edges->addObject(nextEdge);
            currentEdge->next = nextEdge;
            nextEdge->prev = currentEdge;
            currentEdge = nextEdge;
        }
        currentEdge->next = firstEdge;
        firstEdge->prev = currentEdge;
        
        _lines->addObject(firstEdge);
    }
    initEdges->release();
    
    resolveSlabs();
}

bool NavMesh::edgeIntersects(const Edge *e0, const Edge *e1) const {
    if (e0->next == e1 || e0->prev == e1) {
        return false;
    }
    if (!e1->point.between(e0->point, e0->prev->point, e0->next->point) || !e0->point.between(e1->point, e1->prev->point, e1->next->point)) {
        return true;
    }
    foreach(Edge*, first, _lines) {
        Edge* current = first;
        
        do {
            Edge* next = current->next;
            
            if (e0 != current && e1 != current && e0 != next && e1 != next && FPoint::intersectsLineToLine(e0->point, e1->point, current->point, next->point)) {
                return true;
            }
            current = next;
        } while (current != first);
    }
    foreach(Bridge*, bridge, _bridges) {
        bool flag0 = e0 == bridge->edge0 || e0 == bridge->edge1;
        bool flag1 = e1 == bridge->edge0 || e1 == bridge->edge1;
        
        if (flag0 && flag1) {
            return bridge->slab0 && bridge->slab1;
        }
        if (!flag0 && !flag1 && FPoint::intersectsLineToLine(e0->point, e1->point, bridge->edge0->point, bridge->edge1->point)) {
            return true;
        }
    }
    return false;
}
bool NavMesh::slabContains(const Slab* slab, const FPoint& p) const {
    if (p.x < slab->minx || p.x > slab->maxx || p.y < slab->miny || p.y > slab->maxy) {
        return false;
    }
    bool pos = false;
    bool neg = false;
    
    for (int i = 0; i < slab->edges->count(); i++) {
        const Edge* current = slab->edges->objectAtIndex(i);
        
        if (current->point == p) {
            return true;
        }
        const Edge* next = slab->edges->objectAtIndex((i + 1) % slab->edges->count());
        
        fixed d = (p.x - current->point.x) * (next->point.y - current->point.y) - (p.y - current->point.y) * (next->point.x - current->point.x);
        
        if (d > fixed::Zero) {
            pos = true;
        }
        else if (d < fixed::Zero) {
            neg = true;
        }
        if (pos && neg) {
            return false;
        }
    }
    p.slabId = slab->id;
    return true;
}

bool NavMesh::slabsContains(const Edge* e) const {
    foreach(Slab*, slab, _slabs) {
        if (!slab->edges->containsObjectIdenticalTo(const_cast<Edge*>(e)) && slabContains(slab, e->point)) {
            return true;
        }
    }
    return false;
}

void NavMesh::resolveSlab(Edge* e0, Edge* e1) {
    if (!e0->open || slabsContains(e0) || !e1->open || slabsContains(e1)) {
        return;
    }
    Edge* e2 = NULL;
    fixed mind2 = fixed::Max;
    foreach(Edge*, e, _edges) {
        if (e != e0 && e != e1 && e->open && !edgeIntersects(e0, e) && !edgeIntersects(e1, e)) {
            bool flag = true;
            foreach(Slab*, otherSlab, _slabs) {
                if (otherSlab->edges->containsObjectIdenticalTo(e0) && otherSlab->edges->containsObjectIdenticalTo(e1) && otherSlab->edges->containsObjectIdenticalTo(e)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                fixed d2 = e->point.distanceSquaredToLine(e0->point, e1->point);
                
                if (d2 < mind2) {
                    e2 = e;
                    mind2 = d2;
                }
            }
        }
    }
    if (e2) {
        Slab* slab = new Slab(++_slabId, NULL);
        slab->edges->addObject(e0);
        slab->edges->addObject(e1);
        slab->edges->addObject(e2);
        slab->minx = fixed::Max;
        slab->maxx = fixed::Zero;
        slab->miny = fixed::Max;
        slab->maxy = fixed::Zero;
        
        for (int i = 0; i < slab->edges->count(); i++) {
            Edge* se0 = slab->edges->objectAtIndex(i);
            
            if (se0->point.x < slab->minx) slab->minx = se0->point.x;
            if (se0->point.x > slab->maxx) slab->maxx = se0->point.x;
            if (se0->point.y < slab->miny) slab->miny = se0->point.y;
            if (se0->point.y > slab->maxy) slab->maxy = se0->point.y;
            
            Edge* se1 = slab->edges->objectAtIndex((i + 1) % slab->edges->count());
            
            if (!se0->slab && se0->next == se1) {
                se0->slab = slab;
            }
            else if (!se1->slab && se1->next == se0) {
                se1->slab = slab;
            }
            else {
                bool flag = true;
                
                foreach(Bridge*, bridge, _bridges) {
                    if ((se0 == bridge->edge0 && se1 == bridge->edge1) || (se0 == bridge->edge1 && se1 == bridge->edge0)) {
                        CSAssert(!bridge->slab0 || !bridge->slab1, "invalid operation");
                        if (!bridge->slab0) bridge->slab0 = slab;
                        else if (!bridge->slab1) bridge->slab1 = slab;
                        slab->bridges->addObject(bridge);
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    fixed collider;
                    if (se0->slab && se0->slab->obstacle) {
                        if (se1->slab && se1->slab->obstacle) {
                            CSAssert(se0->slab->obstacle != se1->slab->obstacle, "invalid operation");
                            collider = (se0->slab->obstacle->point().distance(se1->slab->obstacle->point()) - se0->slab->obstacle->collider() - se1->slab->obstacle->collider()) / 2;
                        }
                        else {
                            fixed d0 = se0->slab->obstacle->point().distanceSquaredToLine(se1->point, se1->prev->point);
                            fixed d1 = se0->slab->obstacle->point().distanceSquaredToLine(se1->point, se1->next->point);
                            
                            collider = CSMath::sqrt(CSMath::min(d0, d1)) - se0->slab->obstacle->collider();
                        }
                    }
                    else {
                        if (se1->slab && se1->slab->obstacle) {
                            fixed d0 = se1->slab->obstacle->point().distanceSquaredToLine(se0->point, se0->prev->point);
                            fixed d1 = se1->slab->obstacle->point().distanceSquaredToLine(se0->point, se0->next->point);
                            
                            collider = CSMath::sqrt(CSMath::min(d0, d1)) - se1->slab->obstacle->collider();
                        }
                        else {
                            collider = fixed::Max;
                        }
                    }
                    Bridge* bridge = new Bridge(se0, se1, slab, collider);
                    _bridges->addObject(bridge);
                    slab->bridges->addObject(bridge);
                }
            }
        }
        _slabs->addObject(slab);
    }
    else {
        CSErrorLog("nav mesh resolve slab fail:%d (%.2f, %.2f), %d (%.2f, %.2f)",
                   _edges->indexOfObjectIdenticalTo(e0), (float)e0->point.x, (float)e0->point.y,
                   _edges->indexOfObjectIdenticalTo(e1), (float)e1->point.x, (float)e1->point.y);
    }
}

void NavMesh::groupSlabs(Slab* first, CSArray<Slab*>* slabs) {
    uint group = first->group;
    
    for (; ; ) {
        foreach(Bridge*, bridge, first->bridges) {
            if (bridge->slab0 && !bridge->slab0->group) {
                slabs->addObject(bridge->slab0);
            }
            else if (bridge->slab1 && !bridge->slab1->group) {
                slabs->addObject(bridge->slab1);
            }
        }
        if (slabs->count()) {
            first = slabs->objectAtIndex(0);
            first->group = group;
            slabs->removeObjectAtIndex(0);
        }
        else {
            break;
        }
    }
}

void NavMesh::resolveSlabs() {
    foreach(Edge*, first, _lines) {
        Edge* e0 = first;
        
        do {
            Edge* e1 = e0->next;
            
            if (!e0->slab) {
                resolveSlab(e0, e1);
            }
            e0 = e1;
        } while (e0 != first);
    }
    foreach(Bridge*, bridge, _bridges) {
        if (!bridge->slab0 || !bridge->slab1) {
            resolveSlab(bridge->edge0, bridge->edge1);
        }
    }
    foreach(Edge*, edge, _edges) {
        if (edge->slab) {
            edge->open = false;
        }
    }
    CSArray<Slab*>* otherSlabs = new CSArray<Slab*>();
    foreach(Slab*, slab, _slabs) {
        if (slab->group) {
            groupSlabs(slab, otherSlabs);
        }
    }
    foreach(Slab*, slab, _slabs) {
        if (!slab->group) {
            slab->group = ++_slabGroup;
            
            groupSlabs(slab, otherSlabs);
        }
    }
    otherSlabs->release();
    
    CSLog("slabs:%d    bridges:%d   edges:%d", _slabs->count(), _bridges->count(), _edges->count());
}

void NavMesh::removeSlab(Slab *slab) {
    foreach(Edge*, e, slab->edges) {
        if (e->slab == slab) {
            e->slab = NULL;
        }
        e->open = true;
    }
    foreach(Bridge*, bridge, slab->bridges) {
        if (bridge->slab0 == slab) {
            bridge->slab0 = NULL;
        }
        else if (bridge->slab1 == slab) {
            bridge->slab1 = NULL;
        }
        if (!bridge->slab0 && !bridge->slab1) {
            _bridges->removeObjectIdenticalTo(bridge);
            delete bridge;
        }
    }
    _slabs->removeObjectIdenticalTo(slab);
    delete slab;
}

typename NavMesh::Slab* NavMesh::findSlab(uint slabId) const {
    int s = 0;
    int e = _slabs->count();
    
    for (; ; ) {
        if (s >= e) {
            return NULL;
        }
        int index = (s + e) / 2;
        
        Slab* slab = _slabs->objectAtIndex(index);
        
        if (slab->id == slabId) {
            return slab;
        }
        else if (slab->id < slabId) {
            s = index + 1;
        }
        else {
            e = index;
        }
    }
}

typename NavMesh::Slab* NavMesh::findSlab(const FPoint& p) const {
    if (p.slabId) {
        Slab* slab = findSlab(p.slabId);
        
        if (slab && slabContains(slab, p)) {
            return slab;
        }
    }
    foreach(Slab*, slab, _slabs) {
        if (slabContains(slab, p)) {
            return slab;
        }
    }
    p.slabId = 0;
    return NULL;
}

bool NavMesh::locatable(const FPoint &p) const {
    switch (grid(p)) {
        case GridClosed:
            return false;
        case GridOpen:
            return true;
        default:
            return findSlab(p) != NULL;
    }
}

void NavMesh::locateObstacle(const PathObject* obstacle) {
    static const fixed OnePerSquareTwo(fixed::Raw, 46341);
    
    const FPoint& p = obstacle->point();
    fixed range = obstacle->collider() * OnePerSquareTwo;
    
    CSLog("locate obstacle:%f, %f", (float)p.x, (float)p.y);
    
    FPoint nps[] = {
        FPoint(p.x - range, p.y - range),
        FPoint(p.x - range, p.y + range),
        FPoint(p.x + range, p.y + range),
        FPoint(p.x + range, p.y - range)
    };
    
    Edge* edges[4];
    
    CSArray<Slab*>* removingSlabs = new CSArray<Slab*>();
    for (int i = 0; i < 4; i++) {
        const FPoint& np0 = nps[i];
        const FPoint& np1 = nps[(i + 1) % 4];
        
        removingSlabs->removeAllObjects();
        foreach(Slab*, slab, _slabs) {
            if (!slab->obstacle && slabContains(slab, np0)) {
                removingSlabs->addObject(slab);
            }
        }
        fixed mind2 = fixed::Max;
        Edge* edge = NULL;
        foreach(Edge*, e, _edges) {
            fixed d2 = e->point.distanceSquaredToLine(np0, np1);
            if (d2 < mind2) {
                mind2 = d2;
                edge = e;
            }
        }
        if (edge) {
            foreach(Slab*, slab, _slabs) {
                if (!slab->obstacle && !removingSlabs->containsObjectIdenticalTo(slab) && slabContains(slab, edge->point)) {
                    removingSlabs->addObject(slab);
                }
            }
        }
        foreach(Bridge*, bridge, _bridges) {
            if (FPoint::intersectsLineToLine(np0, np1, bridge->edge0->point, bridge->edge1->point)) {
                if (bridge->slab0 && !bridge->slab0->obstacle && !removingSlabs->containsObjectIdenticalTo(bridge->slab0)) {
                    removingSlabs->addObject(bridge->slab0);
                }
                if (bridge->slab1 && !bridge->slab1->obstacle && !removingSlabs->containsObjectIdenticalTo(bridge->slab1)) {
                    removingSlabs->addObject(bridge->slab1);
                }
            }
        }
        foreach(Slab*, slab, removingSlabs) {
            removeSlab(slab);
        }
        edges[i] = new Edge(nps[i]);
    }
    removingSlabs->release();
    
    Slab* slab = new Slab(++_slabId, obstacle);
    
    slab->minx = p.x - range;
    slab->maxx = p.x + range;
    slab->miny = p.y - range;
    slab->maxy = p.y + range;
    
    _lines->addObject(edges[0]);
    
    for (int i = 0; i < 4; i++) {
        Edge* e0 = edges[i];
        Edge* e1 = edges[(i + 1) % 4];
        
        e0->slab = slab;
        e0->next = e1;
        e1->prev = e0;
        
        _edges->addObject(e0);
        slab->edges->addObject(e0);
        
        Bridge* bridge = new Bridge(e0, e1, slab, fixed::Max);
        _bridges->addObject(bridge);
        slab->bridges->addObject(bridge);
    }
    _slabs->addObject(slab);
    
    resolveSlabs();
}

void NavMesh::unlocateObstacle(const PathObject* obstacle) {
    for (int i = 0; i < _slabs->count(); i++) {
        Slab* slab = _slabs->objectAtIndex(i);
        
        if (slab->obstacle == obstacle) {
            _lines->removeObjectIdenticalTo(slab->edges->objectAtIndex(0));
            _slabs->removeObjectAtIndex(i);

            foreach(Edge*, edge, slab->edges) {
                int j = 0;
                while (j < _slabs->count()) {
                    Slab* otherSlab = _slabs->objectAtIndex(j);
                    
                    if (!otherSlab->obstacle && otherSlab->edges->containsObjectIdenticalTo(edge)) {
                        removeSlab(otherSlab);
                    }
                    else {
                        j++;
                    }
                }
                _edges->removeObjectIdenticalTo(edge);
                delete edge;
            }
            foreach(Bridge*, bridge, slab->bridges) {
                _bridges->removeObjectIdenticalTo(bridge);
                delete bridge;
            }
            
            delete slab;
            
            break;
        }
    }
    resolveSlabs();
}

bool NavMesh::hasPath(const FPoint& p0, const FPoint& p1) const {
    const Slab* s0 = findSlab(p0);
    if (!s0) return false;
    
    const Slab* s1 = findSlab(p1);
    if (!s1) return false;
    
    return s0->group == s1->group;
}

fixed NavMesh::distance(const FPoint& p0, const FPoint& p1) const {
    struct WayNode {
        Slab* nextSlab;
        Bridge* bridge;
        FPoint point;
        fixed distance;
        fixed remaining;
        fixed heuristic;
        WayNode* next;
    };
    Slab* s0 = findSlab(p0);
    if (!s0) return fixed::Max;
    
    Slab* s1 = findSlab(p1);
    if (!s1) return fixed::Max;
    
    if (s0->group != s1->group) return fixed::Max;
    
    if (s0 == s1) return p0.distance(p1);

    s0->marked = true;
    
    CSArray<WayNode*>* wayNodes = new CSArray<WayNode*>();
    
    WayNode* first = NULL;
    
    foreach(Bridge*, bridge, s0->bridges) {
        Slab* nextSlab = s0 == bridge->slab0 ? bridge->slab1 : bridge->slab0;
        
        if (nextSlab && (nextSlab == s1 || !nextSlab->obstacle)) {
            WayNode* node = new WayNode();
            wayNodes->addObject(node);
            node->nextSlab = nextSlab;
            node->bridge = bridge;
            node->distance = p0.distanceToLine(bridge->edge0->point, bridge->edge1->point, &node->point);
            node->remaining = node->point.distance(p1);
            node->heuristic = node->distance + node->remaining;
            
            WayNode** current = &first;
            while(*current && node->heuristic > (*current)->heuristic) {
                current = &(*current)->next;
            }
            node->next = *current;
            *current = node;
        }
    }
    while (first) {
        first->nextSlab->marked = true;
        
        if (first->nextSlab == s1) {
            break;
        }
        foreach(Bridge*, bridge, first->nextSlab->bridges) {
            Slab* nextSlab = first->nextSlab == bridge->slab0 ? bridge->slab1 : bridge->slab0;
            
            if (nextSlab && !nextSlab->marked && (nextSlab == s1 || !nextSlab->obstacle)) {
                WayNode* node = new WayNode();
                wayNodes->addObject(node);
                node->nextSlab = nextSlab;
                node->bridge = bridge;
                node->distance = first->distance + first->point.distanceToLine(bridge->edge0->point, bridge->edge1->point, &node->point);
                node->remaining = node->point.distance(p1);
                node->heuristic = node->distance + node->remaining;
                
                WayNode** current = &first->next;
                while(*current && node->heuristic > (*current)->heuristic) {
                    current = &(*current)->next;
                }
                node->next = *current;
                *current = node;
            }
        }
        first = first->next;
    }
    
    fixed rtn = first ? first->heuristic : fixed::Max;
    
    s0->marked = false;
    foreach(WayNode*, node, wayNodes) {
        node->nextSlab->marked = false;
        delete node;
    }
    wayNodes->release();
    
    return rtn;
}

typename NavMesh::Area NavMesh::findArea(const FPoint& p, const PathObject* target, uint group) const {
	Area area;
	area.slab = NULL;
	area.nearSlab = NULL;
    
    Slab* slab = findSlab(p);
    
    if (slab && (!group || slab->group == group)) {
		area.slab = slab;
        if (slab->obstacle && slab->obstacle != target) {
            Bridge* nearBridge = NULL;
            FPoint minnp;
            fixed mind = fixed::Max;
            
            foreach(Bridge*, bridge, slab->bridges) {
                FPoint np;
                fixed d = p.distanceSquaredToLine(bridge->edge0->point, bridge->edge1->point, &np);

                if (d < mind) {
                    nearBridge = bridge;
                    minnp = np;
                    mind = d;
                }
            }
            if (nearBridge) {
				area.nearSlab = nearBridge->slab0 == slab ? nearBridge->slab1 : nearBridge->slab0;
				area.nearPoint = minnp;
				area.nearPoint.slabId = area.nearSlab->id;
				area.nearBridgePoint0 = nearBridge->edge0->point;
				area.nearBridgePoint1 = nearBridge->edge1->point;
            }
        }
    }
    else {
        Edge* nearEdge = NULL;
        FPoint minnp;
        fixed mind = fixed::Max;
        foreach(Edge*, first, _lines) {
            Edge* current = first;
            
            do {
                Edge* next = current->next;
                
                if (current->slab && (!group || current->slab->group == group)) {
                    FPoint np;
                    fixed d = p.distanceSquaredToLine(current->point, next->point, &np);

                    if (d < mind) {
                        nearEdge = current;
                        minnp = np;
                        mind = d;
                    }
                }
                current = next;
            } while (current != first);
        }
        if (nearEdge) {
			area.nearSlab = nearEdge->slab;
			area.nearPoint = minnp;
			area.nearPoint.slabId = area.nearSlab->id;
			area.nearBridgePoint0 = nearEdge->point;
			area.nearBridgePoint1 = nearEdge->next->point;
        }
    }
    return area;
}

ProgressPathReturn NavMesh::progressPath(const FPoint& p, CSArray<Path::Node>* nodes, int i) const {
    const Path::Node& node = nodes->objectAtIndex(i);
    const Slab* slab = findSlab(node.slabId);
    
    if (slab) {
        if (slabContains(slab, p)) {
            i++;
            while (i < nodes->count()) {
                nodes->removeLastObject();
            }
            return ProgressPathSuccess;
        }
        else {
            return ProgressPathOut;
        }
    }
    else {
        return ProgressPathFail;
    }
}

ProgressPathReturn NavMesh::progressPath(const PathObject* obj, const PathObject* target, Path* path) const {
    Area firstArea = findArea(obj->point(), target, 0);
    if (!firstArea.slab) {
        return firstArea.nearSlab ? ProgressPathOut : ProgressPathFail;
    }
	Area lastArea = findArea(target->point(), target, firstArea.slab->group);
    if (!lastArea.slab && !lastArea.nearSlab) {
        return ProgressPathFail;
    }
    
	path->_lastPoint = lastArea.slab ? target->point() : lastArea.nearPoint;

    Slab* lastSlab = lastArea.slab ? lastArea.slab : lastArea.nearSlab;
    
    if (firstArea.slab == lastSlab && (!path->_nodes->count() || !firstArea.nearSlab)) {
        path->_lastSlabId = lastSlab->id;
        path->_nodes->removeAllObjects();
        return ProgressPathSuccess;
    }
    
    if (!path->_nodes->count()) return ProgressPathFail;
    
    const FPoint& p = obj->point();
    
    ProgressPathReturn rtn = ProgressPathOut;
    if (path->_nodes->count() >= 2 && (rtn = progressPath(p, path->_nodes, path->_nodes->count() - 2)) != ProgressPathOut) goto exit;
    if ((rtn = progressPath(p, path->_nodes, path->_nodes->count() - 1)) != ProgressPathOut) goto exit;
    for (int i = (int)path->_nodes->count() - 3; i >= 0; i--) {
        if ((rtn = progressPath(p, path->_nodes, i)) != ProgressPathOut) goto exit;
    }
exit:
    if (rtn == ProgressPathSuccess && path->_lastSlabId != lastSlab->id) rtn = ProgressPathOut;

    return rtn;
}

Path* NavMesh::findPath(const PathObject* obj, const PathObject* target) const {
    static const fixed BridgeCollisionHeuristicFactor(fixed::Raw, 0xFFFFFFFLL);
    struct WayNode {
        Slab* prevSlab;
        Slab* nextSlab;
        Bridge* bridge;
        FPoint point;
        FPoint bridgePoint0;
        FPoint bridgePoint1;
        uint depth;
        uint collision;
        fixed distance;
        fixed remaining;
        fixed heuristic;
        WayNode* next;
        WayNode* prev;
    };
    
    Area firstArea = findArea(obj->point(), target, 0);
	Slab* firstNearSlab = firstArea.nearSlab ? firstArea.nearSlab : firstArea.slab;
	if (!firstNearSlab) {
        return NULL;
    }
    Area lastArea = findArea(target->point(), target, firstNearSlab->group);
	Slab* lastNearSlab = lastArea.nearSlab ? lastArea.nearSlab : lastArea.slab;
	if (!lastNearSlab) {
        return NULL;
    }
    bool complete = lastArea.slab != NULL;
	
	const FPoint& lastPoint = lastArea.slab ? target->point() : lastArea.nearPoint;

    uint lastSlabId = (lastArea.slab ? lastArea.slab : lastArea.nearSlab)->id;
    
    if (firstNearSlab == lastNearSlab) {
        Path* path = Path::path(2, lastPoint, lastSlabId, complete);
        if (lastArea.nearSlab) {
            new (&path->_nodes->addObject()) Path::Node(lastArea.nearPoint, lastArea.nearBridgePoint0, lastArea.nearBridgePoint1, lastArea.nearSlab->id, lastArea.nearSlab->obstacle != NULL);
        }
        if (firstArea.nearSlab) {
            new (&path->_nodes->addObject()) Path::Node(firstArea.nearPoint, firstArea.nearBridgePoint0, firstArea.nearBridgePoint1, firstArea.slab ? firstArea.slab->id : 0, firstArea.slab && firstArea.slab->obstacle != NULL);
        }
        return path;
    }

	firstNearSlab->marked = true;
    
    CSArray<WayNode*>* wayNodes = new CSArray<WayNode*>();
    
    WayNode* first = NULL;
    
    const FPoint& p0 = obj->point();
    const FPoint& p1 = target->point();
    fixed collider = obj->collider();
    
    foreach(Bridge*, bridge, firstNearSlab->bridges) {
        Slab* nextSlab = firstNearSlab == bridge->slab0 ? bridge->slab1 : bridge->slab0;
        
        if (nextSlab && (nextSlab == lastNearSlab || !nextSlab->obstacle)) {
            WayNode* node = new WayNode();
            wayNodes->addObject(node);
            node->prevSlab = firstNearSlab;
            node->nextSlab = nextSlab;
            node->depth = 1;
            node->bridge = bridge;
            if (firstNearSlab->obstacle || nextSlab->obstacle) {
                node->bridgePoint0 = bridge->edge0->point;
                node->bridgePoint1 = bridge->edge1->point;
            }
            else {
                node->bridgePoint0 = bridge->point0(collider);
                node->bridgePoint1 = bridge->point1(collider);
            }
            node->distance = p0.distanceToLine(node->bridgePoint0, node->bridgePoint1, &node->point);
            node->remaining = node->point.distance(p1);
            node->heuristic = node->distance + node->remaining;
            if (bridge->pass(collider, target)) {
                node->collision = 0;
            }
            else {
                node->collision = 1;
                node->heuristic += BridgeCollisionHeuristicFactor;
            }
            node->prev = NULL;
            
            WayNode** current = &first;
            while(*current && node->heuristic > (*current)->heuristic) {
                current = &(*current)->next;
            }
            node->next = *current;
            *current = node;
        }
    }
    
    WayNode* beside = first;
    
    while (first) {
        first->nextSlab->marked = true;
        
        if (first->nextSlab == lastNearSlab) {
            goto make;
        }
        if (first->remaining < beside->remaining) {
			beside = first;
        }
        foreach(Bridge*, bridge, first->nextSlab->bridges) {
            Slab* nextSlab = first->nextSlab == bridge->slab0 ? bridge->slab1 : bridge->slab0;
            
            if (nextSlab && !nextSlab->marked && (nextSlab == lastNearSlab || !nextSlab->obstacle)) {
                WayNode* node = new WayNode();
                wayNodes->addObject(node);
                node->prevSlab = first->nextSlab;
                node->nextSlab = nextSlab;
                node->depth = first->depth + 1;
                node->bridge = bridge;
                if (first->nextSlab->obstacle || nextSlab->obstacle) {
                    node->bridgePoint0 = bridge->edge0->point;
                    node->bridgePoint1 = bridge->edge1->point;
                }
                else {
                    node->bridgePoint0 = bridge->point0(collider);
                    node->bridgePoint1 = bridge->point1(collider);
                }
                node->distance = first->distance + first->point.distanceToLine(node->bridgePoint0, node->bridgePoint1, &node->point);
                node->remaining = node->point.distance(p1);
                node->heuristic = node->distance + node->remaining;
                node->collision = first->collision;
                if (!bridge->pass(collider, target)) {
                    node->collision++;
                }
                if (node->collision) {
                    node->heuristic += BridgeCollisionHeuristicFactor * node->collision;
                }
                node->prev = first;
                
                WayNode** current = &first->next;
                while(*current && node->heuristic > (*current)->heuristic) {
                    current = &(*current)->next;
                }
                node->next = *current;
                *current = node;
            }
        }
        first = first->next;
    }
    
    complete = false;
    first = beside && beside->remaining < p0.distance(p1) ? beside : NULL;
    
make:

    Path* path;
    
    if (first) {
        path = Path::path(first->depth + 2, lastPoint, lastSlabId, complete);
        
        if (lastArea.nearSlab) {
            new (&path->_nodes->addObject()) Path::Node(lastArea.nearPoint, lastArea.nearBridgePoint0, lastArea.nearBridgePoint1, lastNearSlab->id, lastNearSlab->obstacle != NULL);
        }
        WayNode* current = first;
        
        FPoint pp = p1;
        
        do {
            new (&path->_nodes->addObject()) Path::Node(current->point, current->bridgePoint0, current->bridgePoint1, current->prevSlab->id, current->prevSlab->obstacle != NULL);
            
            current = current->prev;
        } while (current);
        
        if (firstArea.nearSlab) {
			new (&path->_nodes->addObject()) Path::Node(firstArea.nearPoint, firstArea.nearBridgePoint0, firstArea.nearBridgePoint1, firstArea.slab ? firstArea.slab->id : 0, firstArea.slab && firstArea.slab->obstacle != NULL);
        }
        
        fixed prevMoved = fixed::Max;
        
        for (; ; ) {
            FPoint pp0 = p1;
            
            fixed currentMoved = fixed::Zero;
            
            for (int j = 0; j < path->_nodes->count(); j++) {
                const FPoint& pp1 = j < path->_nodes->count() - 1 ? path->_nodes->objectAtIndex(j + 1).point : p0;
                
                Path::Node& node = path->_nodes->objectAtIndex(j);
                
                FPoint np;
                if (!FPoint::intersectsLineToLine(pp0, pp1, node.bridgePoint0, node.bridgePoint1, &np)) {
                    fixed d0 = node.bridgePoint0.distanceSquaredToLine(pp0, pp1);
                    fixed d1 = node.bridgePoint1.distanceSquaredToLine(pp0, pp1);
                    if (d0 <= d1) {
                        np = node.bridgePoint0;
                    }
                    else {
                        np = node.bridgePoint1;
                    }
                    if (node.obstacle) {
                        node.dir = 0;
                    }
                    else {
                        FPoint pv0 = pp0 - np;
                        FPoint pv1 = np - pp1;
                        node.dir = CSMath::sign(pv0.x * pv1.y - pv0.y * pv1.x);
                    }
                }
                else {
                    node.dir = 0;
                }
                currentMoved += np.distance(node.point);
                
                pp0 = node.point = np;
            }
            if (currentMoved < prevMoved) {
                prevMoved = currentMoved;
            }
            else {
                break;
            }
        }
    }
    else {
        path = Path::path(2, lastPoint, lastSlabId, complete);
		if (lastArea.nearSlab) {
			new (&path->_nodes->addObject()) Path::Node(lastArea.nearPoint, lastArea.nearBridgePoint0, lastArea.nearBridgePoint1, lastNearSlab->id, lastNearSlab->obstacle != NULL);
		}
		if (firstArea.nearSlab) {
			new (&path->_nodes->addObject()) Path::Node(firstArea.nearPoint, firstArea.nearBridgePoint0, firstArea.nearBridgePoint1, firstArea.slab ? firstArea.slab->id : 0, firstArea.slab && firstArea.slab->obstacle != NULL);
		}
    }
    firstNearSlab->marked = false;
    foreach(WayNode*, node, wayNodes) {
        node->nextSlab->marked = false;
        delete node;
    }
    wayNodes->release();

    return path;
}

void NavMesh::draw(CSGraphics* graphics, const Terrain* terrain) {
    graphics->push();
    
    graphics->begin();
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            CSColor color;
            switch (_grids[y * _width + x]) {
                case GridOpen:
                    continue;
                case GridIntersect:
                    color = CSColor(255, 255, 0, 128);
                    break;
                case GridClosed:
                    color = CSColor(255, 0, 0, 128);
                    break;
            }
            CSVertex vertices[4];
            vertices[0].position = CSVector3(x * TileSize, y * TileSize, (float)terrain->altitude(IPoint(x, y)) * TileSize);
            vertices[1].position = CSVector3((x + 1) * TileSize, y * TileSize, (float)terrain->altitude(IPoint(x + 1, y)) * TileSize);
            vertices[2].position = CSVector3(x * TileSize, (y + 1) * TileSize, (float)terrain->altitude(IPoint(x, y + 1)) * TileSize);
            vertices[3].position = CSVector3((x + 1) * TileSize, (y + 1) * TileSize, (float)terrain->altitude(IPoint(x + 1, y + 1)) * TileSize);
            
            vertices[0].color = color;
            vertices[1].color = color;
            vertices[2].color = color;
            vertices[3].color = color;
            
            graphics->addIndex(graphics->vertexCount() + 0);
            graphics->addIndex(graphics->vertexCount() + 1);
            graphics->addIndex(graphics->vertexCount() + 2);
            graphics->addIndex(graphics->vertexCount() + 1);
            graphics->addIndex(graphics->vertexCount() + 3);
            graphics->addIndex(graphics->vertexCount() + 2);
            
            graphics->addVertex(vertices[0]);
            graphics->addVertex(vertices[1]);
            graphics->addVertex(vertices[2]);
            graphics->addVertex(vertices[3]);
            
            if (graphics->indexCount() + 6 > 65536) {
                graphics->end(NULL, GL_TRIANGLES);
                graphics->begin();
            }
        }
    }
    graphics->end(NULL, GL_TRIANGLES);
    
    foreach(Slab*, slab, _slabs) {
        graphics->setColor(slab->obstacle ? CSColor(0, 0, 255, 64) : CSColor(0, 255, 0, 64));
        graphics->begin();
        for (int j = 0; j < slab->edges->count(); j++) {
            Edge* e0 = slab->edges->objectAtIndex(j);
            CSVector3 p((float)e0->point.x * TileSize, (float)e0->point.y * TileSize, (float)terrain->altitude(e0->point) * TileSize);
            graphics->addVertex(CSVertex(p));
            graphics->addIndex(j);
        }
        graphics->end(NULL, GL_TRIANGLE_FAN);
    }
    foreach(Edge*, first, _lines) {
        Edge* e0 = first;
        do {
            Edge* e1 = e0->next;
            
            CSVector3 p0((float)e0->point.x * TileSize, (float)e0->point.y * TileSize, (float)terrain->altitude(e0->point) * TileSize);
            CSVector3 p1((float)e1->point.x * TileSize, (float)e1->point.y * TileSize, (float)terrain->altitude(e1->point) * TileSize);
            graphics->setColor(e0->slab ? CSColor::Magenta : CSColor::Red);
            graphics->drawLine(p0, p1);
            graphics->drawDigit(_edges->indexOfObjectIdenticalTo(e0), false , CSVector3(p0.x, p0.y - 5, p0.z), CSAlignCenterBottom);
            e0 = e1;
        } while(e0 != first);
    }
    foreach(Bridge*, bridge, _bridges) {
        CSVector3 p0((float)bridge->edge0->point.x * TileSize, (float)bridge->edge0->point.y * TileSize, (float)terrain->altitude(bridge->edge0->point) * TileSize);
        CSVector3 p1((float)bridge->edge1->point.x * TileSize, (float)bridge->edge1->point.y * TileSize, (float)terrain->altitude(bridge->edge1->point) * TileSize);
        graphics->setColor(bridge->slab0 && bridge->slab1 ? CSColor::Blue : CSColor::Yellow);
        graphics->drawLine(p0, p1);
        /*
        if (bridge->inCollider != fixed::Max) {
            graphics->setColor(CSColor::White);
            graphics->drawString(CSString::cstringWithFormat("%.2f", (float)bridge->inCollider), (p0 + p1) * 0.5f, CSAlignCenterMiddle);
        }
         */
    }
    graphics->pop();
}

void NavMesh::draw(CSGraphics* graphics, float scale) {
    graphics->push();
    
    graphics->begin();
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            CSColor color;
            switch (_grids[y * _width + x]) {
                case GridOpen:
                    continue;
                case GridIntersect:
                    color = CSColor(255, 255, 0, 128);
                    break;
                case GridClosed:
                    color = CSColor(255, 0, 0, 128);
                    break;
            }
            CSVertex vertices[4];
            vertices[0].position = CSVector3(x * scale, y * scale, 0);
            vertices[1].position = CSVector3((x + 1) * scale, y * scale, 0);
            vertices[2].position = CSVector3(x * scale, (y + 1) * scale, 0);
            vertices[3].position = CSVector3((x + 1) * scale, (y + 1) * scale, 0);
            
            vertices[0].color = color;
            vertices[1].color = color;
            vertices[2].color = color;
            vertices[3].color = color;
            
            graphics->addIndex(graphics->vertexCount() + 0);
            graphics->addIndex(graphics->vertexCount() + 1);
            graphics->addIndex(graphics->vertexCount() + 2);
            graphics->addIndex(graphics->vertexCount() + 1);
            graphics->addIndex(graphics->vertexCount() + 3);
            graphics->addIndex(graphics->vertexCount() + 2);
            
            graphics->addVertex(vertices[0]);
            graphics->addVertex(vertices[1]);
            graphics->addVertex(vertices[2]);
            graphics->addVertex(vertices[3]);
        }
    }
    graphics->end(NULL, GL_TRIANGLES);
    
    foreach(Slab*, slab, _slabs) {
        graphics->setColor(slab->obstacle ? CSColor(0, 0, 255, 128) : CSColor(0, 255, 0, 128));
        graphics->begin();
        for (int j = 0; j < slab->edges->count(); j++) {
            Edge* e0 = slab->edges->objectAtIndex(j);
            graphics->addVertex(CSVertex((CSVector2)e0->point * scale));
            graphics->addIndex(j);
        }
        graphics->end(NULL, GL_TRIANGLE_FAN);
    }
    
    foreach(Edge*, first, _lines) {
        Edge* e0 = first;
        do {
            Edge* e1 = e0->next;
            
            CSVector2 p0((CSVector2)e0->point * scale);
            CSVector2 p1((CSVector2)e1->point * scale);
            graphics->setColor(e0->open ? (e0->slab ? CSColor::Magenta : CSColor::Red) : CSColor::DarkRed);
            graphics->drawLine(p0, p1);
            graphics->drawDigit(_edges->indexOfObjectIdenticalTo(e0), false, CSVector2(p0.x, p0.y - 5), CSAlignCenterBottom);
            e0 = e1;
        } while(e0 != first);
    }
    foreach(Bridge*, bridge, _bridges) {
        CSVector3 p0((CSVector2)bridge->edge0->point * scale);
        CSVector3 p1((CSVector2)bridge->edge1->point * scale);
        graphics->setColor(bridge->slab0 && bridge->slab1 ? CSColor::Blue : CSColor::Yellow);
        graphics->drawLine(p0, p1);
        /*
        if (bridge->inCollider != fixed::Max) {
            graphics->setColor(CSColor::White);
            graphics->drawString(CSString::cstringWithFormat("%.2f", (float)bridge->inCollider), (p0 + p1) * 0.5f, CSAlignCenterMiddle);
        }
         */
    }
    graphics->pop();
}
