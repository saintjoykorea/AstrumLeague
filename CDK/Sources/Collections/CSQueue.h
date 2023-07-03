//
//  CSQueue.h
//  CDK
//
//  Created by 김찬 on 14. 8. 22..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSQueue__
#define __CDK__CSQueue__

#include "CSArray.h"

template <typename V, bool readonly = true>
class CSQueue : public CSObject {
public:
    typedef typename CSEntryType<V, readonly>::Type Type;
    typedef typename CSEntryType<V, readonly>::PointerParamType PointerParamType;
    typedef typename CSEntryType<V, readonly>::ValueParamType ValueParamType;
    typedef typename CSEntryType<V, readonly>::PointerReturnType PointerReturnType;
    typedef typename CSEntryType<V, readonly>::ValueReturnType ValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstPointerReturnType ConstPointerReturnType;
    typedef typename CSEntryType<V, readonly>::ConstValueReturnType ConstValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstTemplateType ConstTemplateType;
    
    class ReadonlyIterator {
    protected:
        CSQueue* _queue;
        int _cursor;
        bool _forward;
        
        ReadonlyIterator(CSQueue* queue, bool forward);
    public:
        virtual ~ReadonlyIterator();
        
        bool isValid();
        bool next();
        ConstValueReturnType object();
        friend class CSQueue;
    };
    
    class Iterator : public ReadonlyIterator {
    private:
        Iterator(CSQueue* queue, bool forward);
    public:
        void remove();
        ValueReturnType insert();
        void insert(ValueParamType obj);
        ValueReturnType object();
        friend class CSQueue;
    };
    
    friend class Iterator;
    friend class ReadonlyIterator;
    
private:
    struct Node {
        Type object;
        int prev;
        int next;
        bool exists;
    };
    Node* _nodes;
    uint _capacity;
    uint _count;
    uint _expansion;
    int _first;
    int _last;
#ifdef CS_ASSERT_DEBUG
    bool _fence;
#endif
public:
    CSQueue();
    CSQueue(uint capacity);
    CSQueue(ValueParamType object);
    CSQueue(PointerParamType const* objects, uint count);
    template <typename otherV, bool otherReadonly>
    CSQueue(const CSQueue<otherV, otherReadonly>* otherQueue, bool deepCopy);
protected:
    virtual ~CSQueue();
public:
    static CSQueue* queue();
    static CSQueue* queueWithCapacity(uint capacity);
    static CSQueue* queueWithObject(ValueParamType object);
    static CSQueue* queueWithObjects(PointerParamType const* objects, uint count);
    template <typename otherV, bool otherReadonly>
    static CSQueue* queueWithQueue(const CSQueue<otherV, otherReadonly>* otherQueue, bool deepCopy);
    
    inline uint count() const {
        return _count;
    }
    inline uint capacity() const {
        return _capacity;
    }
private:
    void expand(int& p);
    ValueReturnType insert(int p);
    void insert(ValueParamType obj, int p);
    void remove(int p);
    template <typename otherV>
    void allObjects(CSArray<otherV, 1, false>* values, bool forward) const;
public:
    ValueReturnType addObject(bool forward = true);
    void addObject(ValueParamType obj, bool forward = true);
    void removeObject(const ValueParamType obj, bool forward = true);
    bool containsObject(const ValueParamType obj, bool forward = true) const;
    Iterator iterator(bool forward = true);
    ReadonlyIterator iterator(bool forward = true) const;
    CSArray<V, 1, false>* allObjects(bool forward = true);
    CSArray<ConstTemplateType, 1, false>* allObjects(bool forward = true) const;
    
    virtual CSObject* copy() const override;
};

template <typename V, bool readonly>
CSQueue<V, readonly>::CSQueue() : _first(-1), _last(-1), _capacity(12) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
}
template <typename V, bool readonly>
CSQueue<V, readonly>::CSQueue(uint capacity) : _first(-1), _last(-1), _capacity(capacity ? capacity : 1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
}
template <typename V, bool readonly>
CSQueue<V, readonly>::CSQueue(ValueParamType obj) : _first(-1), _last(-1), _capacity(1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    addObject(obj);
}
template <typename V, bool readonly>
CSQueue<V, readonly>::CSQueue(PointerParamType const* objects, uint count) : _first(-1), _last(-1), _capacity(count ? count : 1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    for (uint i = 0; i < count; i++) {
        addObject(objects[i]);
    }
}
template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
CSQueue<V, readonly>::CSQueue(const CSQueue<otherV, otherReadonly>* otherQueue, bool deepCopy) : _first(-1), _last(-1), _capacity(otherQueue->_capacity) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    int srcIndex = otherQueue->_first;
    
    while (srcIndex != -1) {
        addObject(deepCopy ? CSEntryImpl<V>::copy(otherQueue->_nodes[srcIndex].object, true) : otherQueue->_nodes[srcIndex].object);
        srcIndex = otherQueue->_nodes[srcIndex].next;
    }
}
template <typename V, bool readonly>
CSQueue<V, readonly>::~CSQueue() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        if (_nodes[i].exists) {
            CSEntryImpl<V>::release(_nodes[i].object);
        }
    }
    free(_nodes);
}
template <typename V, bool readonly>
CSQueue<V, readonly>* CSQueue<V, readonly>::queue() {
    return autorelease(new CSQueue<V, readonly>());
}
template <typename V, bool readonly>
CSQueue<V, readonly>* CSQueue<V, readonly>::queueWithCapacity(uint capacity) {
    return autorelease(new CSQueue<V, readonly>(capacity));
}
template <typename V, bool readonly>
CSQueue<V, readonly>* CSQueue<V, readonly>::queueWithObject(ValueParamType object) {
    return autorelease(new CSQueue(object));
}
template <typename V, bool readonly>
CSQueue<V, readonly>* CSQueue<V, readonly>::queueWithObjects(PointerParamType const* objects, uint count) {
    return autorelease(new CSQueue(objects, count));
}
template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
CSQueue<V, readonly>* CSQueue<V, readonly>::queueWithQueue(const CSQueue<otherV, otherReadonly>* otherQueue, bool deepCopy) {
    return autorelease(new CSQueue(otherQueue, deepCopy));
}
template <typename V, bool readonly>
void CSQueue<V, readonly>::expand(int& p) {
    if (_expansion == _capacity) {
        for (uint i = 0; i < _expansion; i++) {
            if (!_nodes[i].exists) {
                p = i;
                return;
            }
        }
		uint nextCapacity = _capacity << 1;
		_nodes = (Node*)frealloc(_nodes, nextCapacity * sizeof(Node));
		memset(&_nodes[_capacity], 0, (nextCapacity - _capacity) * sizeof(Node));
		_capacity = nextCapacity;
    }
    p = _expansion++;
}

template <typename V, bool readonly>
typename CSQueue<V, readonly>::ValueReturnType CSQueue<V, readonly>::insert(int p) {
    int prev = p;
    int next = p != -1 ? _nodes[p].next : -1;
    
    expand(p);
    
    Node& node = _nodes[p];
    node.prev = prev;
    node.next = next;
    
    if (prev != -1) {
        _nodes[prev].next = p;
    }
    else {
        _first = p;
    }
    if (next != -1) {
        _nodes[next].prev = p;
    }
    else {
        _last = p;
    }
    return CSEntryImpl<V>::notNullValue(node.object);
}
template <typename V, bool readonly>
void CSQueue<V, readonly>::insert(ValueParamType obj, int p) {
    int prev = p;
    int next = p != -1 ? _nodes[p].next : -1;
    
    expand(p);
    
    Node& node = _nodes[p];
    new (&node.object) Type(CSEntryImpl<V>::retain(obj));
    node.prev = prev;
    node.next = next;
    
    if (prev != -1) {
        _nodes[prev].next = p;
    }
    else {
        _first = p;
    }
    if (next != -1) {
        _nodes[next].prev = p;
    }
    else {
        _last = p;
    }
}

template <typename V, bool readonly>
void CSQueue<V, readonly>::remove(int p) {
    Node& node = _nodes[p];
    
    CSEntryImpl<V>::release(node.object);
    node.exists = false;
    
    _nodes[node.prev].next = node.next;
    if (_first == p) {
        _first = node.next;
    }
    _nodes[node.next].prev = node.prev;
    if (_last == p) {
        _last = node.prev;
    }
}

template <typename V, bool readonly>
typename CSQueue<V, readonly>::ValueReturnType CSQueue<V, readonly>::addObject(bool forward) {
    fence(_fence);
    return insert(forward ? _first : _last);
}
template <typename V, bool readonly>
void CSQueue<V, readonly>::addObject(ValueParamType obj, bool forward) {
    fence(_fence);
    insert(obj, forward ? _first : _last);
}
template <typename V, bool readonly>
void CSQueue<V, readonly>::removeObject(ValueParamType obj, bool forward) {
    fence(_fence);
    
    if (forward) {
        int p = _first;
        
        while (p != -1) {
            Node& node = _nodes[p];
            
            if (CSEntryImpl<V>::isEqual(obj, node.object)) {
                remove(p);
                return;
            }
            p = node.next;
        }
    }
    else {
        int p = _last;
        
        while (p != -1) {
            Node& node = _nodes[p];
            
            if (CSEntryImpl<V>::isEqual(obj, node.object)) {
                remove(p);
                return;
            }
            p = node.prev;
        }
    }
}
template <typename V, bool readonly>
bool CSQueue<V, readonly>::containsObject(ValueParamType obj, bool forward) const {
    if (forward) {
        int p = _first;
        
        while (p != -1) {
            Node& node = _nodes[p];
            
            if (CSEntryImpl<V>::isEqual(obj, node.object)) {
                return true;
            }
            p = node.next;
        }
    }
    else {
        int p = _last;
        
        while (p != -1) {
            Node& node = _nodes[p];
            
            if (CSEntryImpl<V>::isEqual(obj, node.object)) {
                return true;
            }
            p = node.prev;
        }
    }
    return false;
}

template <typename V, bool readonly>
CSQueue<V, readonly>::ReadonlyIterator::ReadonlyIterator(CSQueue* queue, bool forward) : _queue(queue), _forward(forward) {
#ifdef CS_ASSERT_DEBUG
    CSAssert(!_queue->_fence, "concurrent modifying");
    _queue->_fence = true;
#endif
    _cursor = forward ? _queue->_first : _queue->_last;
}

template <typename V, bool readonly>
CSQueue<V, readonly>::ReadonlyIterator::~ReadonlyIterator() {
#ifdef CS_ASSERT_DEBUG
    _queue->_fence = false;
#endif
}

template <typename V, bool readonly>
bool CSQueue<V, readonly>::ReadonlyIterator::isValid() {
    return _cursor != -1;
}

template <typename V, bool readonly>
bool CSQueue<V, readonly>::ReadonlyIterator::next() {
    if (_cursor != -1) {
        Node& node = queue->_nodes[_cursor];
        _cursor = _forward ? node.next : node.prev;
    }
    return _cursor != -1;
}

template <typename V, bool readonly>
typename CSQueue<V, readonly>::ConstValueReturnType CSQueue<V, readonly>::ReadonlyIterator::object() {
    return _cursor != -1 ? queue->_nodes[_cursor].object : NULL;
}

template <typename V, bool readonly>
CSQueue<V, readonly>::Iterator::Iterator(CSQueue* queue, bool forward) : ReadonlyIterator(queue, forward) {

}

template <typename V, bool readonly>
void CSQueue<V, readonly>::Iterator::remove() {
    ReadonlyIterator::_queue->remove(ReadonlyIterator::_cursor);
}

template <typename V, bool readonly>
void CSQueue<V, readonly>::Iterator::insert(ValueParamType obj) {
    CSAssert(ReadonlyIterator::_cursor != -1, "invalid cursor");
    ReadonlyIterator::_queue->insert(obj, ReadonlyIterator::_cursor);
}

template <typename V, bool readonly>
typename CSQueue<V, readonly>::ValueReturnType CSQueue<V, readonly>::Iterator::object() {
    return ReadonlyIterator::_cursor != -1 ? ReadonlyIterator::_queue->_nodes[ReadonlyIterator::_cursor].object : NULL;
}

template <typename V, bool readonly>
typename CSQueue<V, readonly>::Iterator CSQueue<V, readonly>::iterator(bool forward) {
    return Iterator(this, forward);
}
template <typename V, bool readonly>
typename CSQueue<V, readonly>::ReadonlyIterator CSQueue<V, readonly>::iterator(bool forward) const {
    return ReadonlyIterator(const_cast<CSQueue<V, readonly>*>(this), forward);
}

template <typename V, bool readonly> template <typename otherV>
void CSQueue<V, readonly>::allObjects(CSArray<otherV, 1, false>* values, bool forward) const {
    if (forward) {
        int p = _first;
        while (p != -1) {
            Node& node = _nodes[p];
            values->addObject(node.obj);
            p = node.next;
        }
    }
    else {
        int p = _last;
        while (p != -1) {
            Node& node = _nodes[p];
            values->addObject(node.obj);
            p = node.prev;
        }
    }
}

template <typename V, bool readonly>
CSArray<V, 1, false>* CSQueue<V, readonly>::allObjects(bool forward) {
    CSArray<V, 1, false>* objects = CSArray<V, 1, false>::arrayWithCapacity(_count);
    allObjects(objects, forward);
    return objects;
}

template <typename V, bool readonly>
CSArray<typename CSQueue<V, readonly>::ConstTemplateType, 1, false>* CSQueue<V, readonly>::allObjects(bool forward) const {
    CSArray<ConstTemplateType, 1, false>* objects = CSArray<ConstTemplateType, 1, false>::arrayWithCapacity(_count);
    allObjects(objects, forward);
    return objects;
}

template <typename V, bool readonly>
CSObject* CSQueue<V, readonly>::copy() const {
    return new CSQueue<V, readonly>(this, true);
}

#endif
