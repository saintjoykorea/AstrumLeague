//
//  CSSortedSet.h
//  CDK
//
//  Created by 김찬 on 14. 8. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSSortedSet__
#define __CDK__CSSortedSet__

#include "CSArray.h"
#include "CSMath.h"

template <typename V>
class CSSortedSet : public CSObject {
public:
    typedef typename CSEntryType<V, false>::Type Type;
    typedef typename CSEntryType<V, true>::ConstPointerParamType PointerParamType;
    typedef typename CSEntryType<V, true>::ConstValueParamType ValueParamType;
    typedef typename CSEntryType<V, true>::ConstValueReturnType ValueReturnType;
    typedef typename CSEntryType<V, false>::ConstTemplateType ConstTemplateType;
    
    class ReadonlyIterator {
    private:
        CSSortedSet* _set;
        int* _cursor;
        bool _ascending;
        
        ReadonlyIterator(CSSortedSet* set, bool ascending);
    public:
        ~ReadonlyIterator();
        
        bool isValid();
        ValueReturnType object();
        bool next();
        
        friend class CSSortedSet;
    };
    
    friend class ReadonlyIterator;
private:
    struct Node {
        Type object;
        int prev;
        int left;
        int right;
        int height;
        bool exists;
    };
    Node* _nodes;
    uint _capacity;
    uint _count;
    uint _expansion;
    int _root;
#ifdef CS_ASSERT_DEBUG
    bool _fence;
#endif
public:
    CSSortedSet();
    CSSortedSet(uint capacity);
    CSSortedSet(ValueParamType obj);
    CSSortedSet(PointerParamType const* objects, uint count);
    template <typename otherV>
    CSSortedSet(const CSSortedSet<otherV>* otherSet);
protected:
    virtual ~CSSortedSet();
public:
    static CSSortedSet<V>* set();
    static CSSortedSet<V>* setWithCapacity(uint capacity);
    static CSSortedSet<V>* setWithObject(ValueParamType object);
    static CSSortedSet<V>* setWithObjects(PointerParamType const* objects, uint count);
    template <typename otherV>
    static CSSortedSet<V>* setWithSet(const CSSortedSet<otherV>* otherSet);
    
    inline uint count() const {
        return _count;
    }
    inline uint capacity() const {
        return _capacity;
    }
private:
    int expandReady();
    int expandCommit(int p);
    void insert(ValueParamType obj, int prev, int next, int& p);
    bool contains(ValueParamType, int p) const;
    void remove(ValueParamType, int& p);
    void remove(int& p);
    int height(int p);
    int srl(int p1);
    int srr(int p1);
    int drl(int p1);
    int drr(int p1);
    int rrr(int prev, int& p);
    void allObjects(int p, CSArray<ConstTemplateType>* values, bool ascending) const;
public:
    void addObject(ValueParamType obj);
    void removeObject(ValueParamType obj);
    void removeAllObjects();
    bool containsObject(ValueParamType obj) const;
    ReadonlyIterator iterator(bool ascending = true) const;
    CSArray<ConstTemplateType>* allObjects(bool ascending = true) const;
    
    virtual CSObject* copy() const override;

    template <typename friendV>
    friend class CSSortedSet;
};

template <typename V>
CSSortedSet<V>::CSSortedSet() : _root(-1), _capacity(12) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
}
template <typename V>
CSSortedSet<V>::CSSortedSet(uint capacity) : _root(-1), _capacity(capacity ? capacity : 1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
}
template <typename V>
CSSortedSet<V>::CSSortedSet(ValueParamType obj) : _root(-1), _capacity(1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    addObject(obj);
}
template <typename V>
CSSortedSet<V>::CSSortedSet(PointerParamType const* objects, uint count) : _root(-1), _capacity(count ? count : 1) {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    for (uint i = 0; i < count; i++) {
        addObject(objects[i]);
    }
}
template <typename V> template <typename otherV>
CSSortedSet<V>::CSSortedSet(const CSSortedSet<otherV>* otherSet) : _root(-1), _capacity(otherSet->_capacity)  {
    _nodes = (Node*)fcalloc(_capacity, sizeof(Node));
    
    for (uint i = 0; i < otherSet->_expansion; i++) {
        if (otherSet->_nodes[i].exists) {
            addObject(otherSet->_nodes[i].object);
        }
    }
}
template <typename V>
CSSortedSet<V>::~CSSortedSet() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        Node& node = _nodes[i];
        if (node.exists) {
            CSEntryImpl<V>::release(node.object);
        }
    }
    free(_nodes);
}
template <typename V>
CSSortedSet<V>* CSSortedSet<V>::set() {
    return autorelease(new CSSortedSet<V>());
}
template <typename V>
CSSortedSet<V>* CSSortedSet<V>::setWithCapacity(uint capacity) {
    return autorelease(new CSSortedSet<V>(capacity));
}
template <typename V>
CSSortedSet<V>* CSSortedSet<V>::setWithObject(ValueParamType object) {
    return autorelease(new CSSortedSet<V>(object));
}
template <typename V>
CSSortedSet<V>* CSSortedSet<V>::setWithObjects(PointerParamType const* objects, uint count) {
    return autorelease(new CSSortedSet<V>(objects, count));
}
template <typename V> template <typename otherV>
CSSortedSet<V>* CSSortedSet<V>::setWithSet(const CSSortedSet<otherV>* otherSet) {
    return autorelease(new CSSortedSet<V>(otherSet));
}
template <typename V>
int CSSortedSet<V>::expandReady() {
    if (_expansion == _capacity) {
        for (uint i = 0; i < _expansion; i++) {
            if (!_nodes[i].exists) {
                return i;
            }
        }
        uint nextCapacity = _capacity << 1;
        _nodes = (Node*)frealloc(_nodes, nextCapacity * sizeof(Node));
        memset(&_nodes[_capacity], 0, (nextCapacity - _capacity) * sizeof(Node));
        _capacity = nextCapacity;
    }
    return -1;
}

template <typename V>
int CSSortedSet<V>::expandCommit(int p) {
    return p == -1 ? _expansion++ : p;
}

template <typename V>
void CSSortedSet<V>::insert(ValueParamType obj, int prev, int next, int& p) {
    if (p == -1) {
        p = expandCommit(next);
        
        Node& node = _nodes[p];
        
        new (&node.object) Type(CSEntryImpl<V>::copy(obj));
        node.left = -1;
        node.right = -1;
        node.prev = prev;
        node.height = 0;
        node.exists = true;
        _count++;
    }
    else {
        Node& node = _nodes[p];
        
        int comp = CSEntryImpl<V>::compareTo(obj, node.object);
        
        if (comp == 0) {
            if (node.object != obj) {
                CSEntryImpl<V>::release(node.object);
                new (&node.object) Type(CSEntryImpl<V>::copy(obj));
            }
        }
        else if (comp < 0) {
            insert(obj, p, node.left);
            
            if ((height(node.left) - height(node.right)) == 2) {
                if (CSEntryImpl<V>::compareTo(obj, _nodes[node.left].object) < 0) {
                    p = srl(p);
                }
                else {
                    p = drl(p);
                }
            }
        }
        else {
            insert(obj, p, node.right);
            
            if ((height(node.right) - height(node.left)) == 2) {
                if (CSEntryImpl<V>::compareTo(obj, _nodes[node.right].object) > 0) {
                    p = srr(p);
                }
                else {
                    p = drr(p);
                }
            }
        }
        _nodes[p].height = CSMath::max(height(_nodes[p].left), height(_nodes[p].right)) + 1;
    }
}

template <typename V>
bool CSSortedSet<V>::contains(ValueParamType obj, int p) const {
    if (p == -1) {
        return false;
    }
    
    const Node& node = _nodes[p];
    
    int comp = CSEntryImpl<V>::compareTo(obj, node.object);
    
    if (comp < 0) {
        return contains(obj, node.left);
    }
    else if (comp > 0) {
        return contains(obj, node.right);
    }
    else {
        return CSEntryImpl<V>::isEqual(obj, node.object);
    }
}

template <typename V>
void CSSortedSet<V>::remove(ValueParamType obj, int& p) {
    if (p != -1) {
        Node& node = _nodes[p];
        
        int comp = CSEntryImpl<V>::compareTo(obj, node.object);
        
        if (comp < 0) {
            remove(obj, node.left);
        }
        else if (comp > 0) {
            remove(obj, node.right);
        }
        else if (CSEntryImpl<V>::isEqual(obj, node.object)) {
            remove(p);
        }
    }
}

template <typename V>
void CSSortedSet<V>::remove(int& p) {
    Node& node = _nodes[p];
    
    CSEntryImpl<V>::release(node.object);
    node.exists = false;
    _count--;
    
    if ((node.left == -1) && (node.right == -1)) {
        p = -1;
    }
    else if (node.left == -1) {
        p = node.right;
        _nodes[node.right].prev = node.prev;
    }
    else if (node.right == -1) {
        p = node.left;
        _nodes[node.left].prev = node.prev;
    }
    else {
        node.object = _nodes[rrr(p, node.right)].object;
    }
}

template <typename V>
int CSSortedSet<V>::height(int p) {
    return p == -1 ? -1 : _nodes[p].height;
}

template <typename V>
int CSSortedSet<V>::srl(int p1) {
    Node& n1 = _nodes[p1];
    int p2 = _nodes[p1].left;
    Node& n2 = _nodes[p2];
    n1.left = n2.right;
    if (n1.left != -1) {
        _nodes[n1.left].prev = p1;
    }
    n2.right = p1;
    n2.prev = n1.prev;
    n1.prev = p2;
    n1.height = CSMath::max(height(n1.left), height(n1.right)) + 1;
    n2.height = CSMath::max(height(n2.left), n1.height) + 1;
    return p2;
}

template <typename V>
int CSSortedSet<V>::srr(int p1) {
    Node& n1 = _nodes[p1];
    int p2 = _nodes[p1].right;
    Node& n2 = _nodes[p2];
    n1.right = n2.left;
    if (n1.right != -1) {
        _nodes[n1.right].prev = p1;
    }
    n2.left = p1;
    n2.prev = n1.prev;
    n1.prev = p2;
    n1.height = CSMath::max(height(n1.left), height(n1.right)) + 1;
    n2.height = CSMath::max(n1.height, height(n2.right)) + 1;
    return p2;
}
template <typename V>
int CSSortedSet<V>::drl(int p1) {
    _nodes[p1].left = srr(_nodes[p1].left);
    return srl(p1);
}
template <typename V>
int CSSortedSet<V>::drr(int p1) {
    _nodes[p1].right = srl(_nodes[p1].right);
    return srr(p1);
}
template <typename V>
int CSSortedSet<V>::rrr(int prev, int& p) {
    int rtn;
    
    Node& node = _nodes[p];
    if (node.left == -1) {
        rtn = p;
        p = node.right;
        if (p != -1) {
            _nodes[p].prev = prev;
        }
    }
    else
    {
        rtn = rrr(p, node.left);
    }
    return rtn;
}
template <typename V>
void CSSortedSet<V>::addObject(const ValueParamType obj) {
    fence(_fence);
    
    insert(obj, -1, expandReady(), _root);
}
template <typename V>
void CSSortedSet<V>::removeObject(const ValueParamType obj) {
    fence(_fence);
    
    remove(obj, _root);
}
template <typename V>
void CSSortedSet<V>::removeAllObjects() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        Node& node = _nodes[i];
        if (node.exists) {
            CSEntryImpl<V>::release(node.object);
            node.exists = false;
        }
    }
    _expansion = 0;
    _count = 0;
    _root = -1;
}
template <typename V>
bool CSSortedSet<V>::containsObject(ValueParamType obj) const {
    return contains(obj, _root);
}

template <typename V>
CSSortedSet<V>::ReadonlyIterator::ReadonlyIterator(CSSortedSet* set, bool ascending) : _set(set), _ascending(ascending) {
#ifdef CS_ASSERT_DEBUG
    CSAssert(!_set->_fence, "concurrent modifying");
    _set->_fence = true;
#endif
    _cursor = &_set->_root;
    
    if (*_cursor != -1) {
        if (ascending) {
            while (_set->_nodes[*_cursor].left != -1) {
                _cursor = &_set->_nodes[*_cursor].left;
            }
        }
        else {
            while (_set->_nodes[*_cursor].right != -1) {
                _cursor = &_set->_nodes[*_cursor].right;
            }
        }
    }
    if (*_cursor == -1) {
        _cursor = NULL;
    }
}

template <typename V>
CSSortedSet<V>::ReadonlyIterator::~ReadonlyIterator() {
#ifdef CS_ASSERT_DEBUG
    _set->_fence = false;
#endif
}

template <typename V>
bool CSSortedSet<V>::ReadonlyIterator::isValid() {
    return _cursor != NULL;
}

template <typename V>
typename CSSortedSet<V>::ValueReturnType CSSortedSet<V>::ReadonlyIterator::object() {
    return _cursor ? _set->_nodes[*_cursor].object : CSEntryImpl<V>::nullValue();
}

template <typename V>
bool CSSortedSet<V>::ReadonlyIterator::next() {
    if (_cursor) {
        if (_ascending) {
            if (_set->_nodes[*_cursor].right != -1) {
                _cursor = &_set->_nodes[*_cursor].right;
                
                while (_set->_nodes[*_cursor].left != -1) {
                    _cursor = &_set->_nodes[*_cursor].left;
                }
            }
            else {
                for (; ; ) {
                    int& prev = _set->_nodes[*_cursor].prev;
                    
                    if (prev == -1) {
                        _cursor = NULL;
                        break;
                    }
                    else if (_set->_nodes[prev].left == *_cursor) {
                        _cursor = &prev;
                        break;
                    }
                    else {
                        _cursor = &prev;
                    }
                }
            }
        }
        else {
            if (_set->_nodes[*_cursor].left != -1) {
                _cursor = &_set->_nodes[*_cursor].left;
                
                while (_set->_nodes[*_cursor].right != -1) {
                    _cursor = &_set->_nodes[*_cursor].right;
                }
            }
            else {
                for (; ; ) {
                    int& prev = _set->_nodes[*_cursor].prev;
                    
                    if (prev == -1) {
                        _cursor = NULL;
                        break;
                    }
                    else if (_set->_nodes[prev].right == *_cursor) {
                        _cursor = &prev;
                        break;
                    }
                    else {
                        _cursor = &prev;
                    }
                }
            }
        }
    }
    return _cursor != NULL;
}

template <typename V>
typename CSSortedSet<V>::ReadonlyIterator CSSortedSet<V>::iterator(bool ascending) const {
    return ReadonlyIterator(const_cast<CSSortedSet<V>*>(this), ascending);
}

template <typename V>
void CSSortedSet<V>::allObjects(int p, CSArray<typename CSSortedSet<V>::ConstTemplateType>* objects, bool ascending) const {
    if (p != -1) {
        Node& node = _nodes[p];
        if (ascending) {
            allObjects(node.left, objects, ascending);
            objects->addObject(node.object);
            allObjects(node.right, objects, ascending);
        }
        else {
            allObjects(node.right, objects, ascending);
            objects->addObject(node.object);
            allObjects(node.left, objects, ascending);
        }
    }
}

template <typename V>
CSArray<typename CSSortedSet<V>::ConstTemplateType>* CSSortedSet<V>::allObjects(bool ascending) const {
    if (_count == 0) return NULL;
    CSArray<ConstTemplateType>* objects = CSArray<ConstTemplateType>::arrayWithCapacity(_count);
    allObjects(_root, objects, ascending);
    return objects;
}

template <typename V>
CSObject* CSSortedSet<V>::copy() const {
    return new CSSortedSet<V>(this);
}

#endif
