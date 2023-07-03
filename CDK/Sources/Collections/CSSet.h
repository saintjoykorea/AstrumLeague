//
//  CSSet.h
//  CDK
//
//  Created by 김찬 on 14. 8. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSSet__
#define __CDK__CSSet__

#include "CSArray.h"

template <typename V>
class CSSet : public CSObject {
public:
    typedef typename CSEntryType<V, false>::Type Type;
    typedef typename CSEntryType<V, true>::ConstPointerParamType PointerParamType;
    typedef typename CSEntryType<V, true>::ConstValueParamType ValueParamType;
    typedef typename CSEntryType<V, true>::ConstValueReturnType ValueReturnType;
    typedef typename CSEntryType<V, false>::ConstTemplateType ConstTemplateType;
    
    class ReadonlyIterator {
    protected:
        CSSet* _set;
        uint _index;
        int* _cursor;
        bool _removed;
        
        ReadonlyIterator(CSSet* set);
    public:
        virtual ~ReadonlyIterator();
        
        bool isValid();
        ValueReturnType object();
        bool next();
        friend class CSSet;
    };
    
    class Iterator : public ReadonlyIterator {
    private:
        Iterator(CSSet* set);
    public:
        void remove();
        friend class CSSet;
    };
    
    friend class ReadonlyIterator;
    friend class Iterator;
private:
    struct Bucket {
        Type object;
        int next;
        bool exists;
    };
    
    Bucket* _buckets;
    int* _bucketIndices;
    uint _count;
    uint _expansion;
    uint _capacity;
#ifdef CS_ASSERT_DEBUG
    bool _fence;
#endif
private:
    void init();
public:
    CSSet();
    CSSet(uint capacity);
    CSSet(ValueParamType obj);
    CSSet(PointerParamType const* objects, uint count);
    template <typename otherV>
    CSSet(const CSSet<otherV>* otherSet);
    
protected:
    virtual ~CSSet();
public:
    static CSSet<V>* set();
    static CSSet<V>* setWithCapacity(uint capacity);
    static CSSet<V>* setWithObject(ValueParamType obj);
    static CSSet<V>* setWithObjects(PointerParamType const* objects, uint count);
    template <typename otherV>
    static CSSet<V>* setWithSet(const CSSet<otherV>* otherSet);
    
    inline uint count() const {
        return _count;
    }
    
    inline uint capacity() const {
        return _capacity;
    }
    
private:
    int expand();
public:
    CSArray<ConstTemplateType, 1, false>* allObjects() const;
    bool containsObject(ValueParamType object) const;
    void addObject(ValueParamType object);
    void removeObject(ValueParamType object);
    void removeAllObjects();
    
    Iterator iterator();
    ReadonlyIterator iterator() const;
    
    CSObject* copy() const override;
    
    template <typename friendV>
    friend class CSSet;
};

template <typename V>
void CSSet<V>::init() {
    _bucketIndices = (int*)fmalloc(_capacity * sizeof(int));
    _buckets = (Bucket*)fcalloc(_capacity, sizeof(Bucket));
    memset(_bucketIndices, -1, _capacity * sizeof(int));
}

template <typename V>
CSSet<V>::CSSet() : _capacity(12) {
    init();
}

template <typename V>
CSSet<V>::CSSet(uint capacity) : _capacity(capacity ? capacity : 1) {
    init();
}

template <typename V>
CSSet<V>::CSSet(ValueParamType object) : _capacity(1) {
    init();
    
    addObject(object);
}

template <typename V>
CSSet<V>::CSSet(PointerParamType const* objects, uint count) : _capacity(count ? count : 1) {
    init();
    
    for (uint i = 0; i < count; i++) {
        addObject(objects[i]);
    }
}

template <typename V> template <typename otherV>
CSSet<V>::CSSet(const CSSet<otherV>* otherSet) : _capacity(otherSet->_capacity) {
    init();
    
    _count = otherSet->_count;
    
    for (uint i = 0; i < _capacity; i++) {
        int srcIndex = otherSet->_bucketIndices[i];
        int* destIndex = &_bucketIndices[i];
        while (srcIndex != -1) {
            typename CSSet<otherV>::Bucket& src = otherSet->_buckets[srcIndex];
            
            *destIndex = _expansion++;
            
            Bucket& dest = _buckets[*destIndex];
            new (&dest.object) Type(CSEntryImpl<V>::copy(src.object));
            dest.next = -1;
            dest.exists = true;
            
            destIndex = &dest.next;
            srcIndex = src.next;
        }
    }
}

template <typename V>
CSSet<V>::~CSSet() {
    fence(_fence);

    for (uint i = 0; i < _expansion; i++) {
        if (_buckets[i].exists) {
            CSEntryImpl<V>::release(_buckets[i].object);
        }
    }
    free(_buckets);
    free(_bucketIndices);
}

template <typename V>
CSSet<V>* CSSet<V>::set() {
    return autorelease(new CSSet());
}

template <typename V>
CSSet<V>* CSSet<V>::setWithCapacity(uint capacity) {
    return autorelase(new CSSet(capacity));
}

template <typename V>
CSSet<V>* CSSet<V>::setWithObject(ValueParamType object) {
    return autorelease(new CSSet(object));
}

template <typename V>
CSSet<V>* CSSet<V>::setWithObjects(PointerParamType const* objects, uint count) {
    return autorelease(new CSSet(objects, count));
}

template <typename V> template <typename otherV>
CSSet<V>* CSSet<V>::setWithSet(const CSSet<otherV>* otherSet) {
    return autorelease(new CSSet(otherSet));
}

template <typename V>
int CSSet<V>::expand() {
    if (_expansion == _capacity) {
        for (uint i = 0; i < _expansion; i++) {
            if (!_buckets[i].exists) {
                return i;
            }
        }
        uint nextCapacity = _capacity << 1;
        _buckets = (Bucket*)frealloc(_buckets, nextCapacity * sizeof(Bucket));
        memset(&_buckets[_capacity], 0, (nextCapacity - _capacity) * sizeof(Bucket));
        _capacity = nextCapacity;
        
        int* newBucketIndices = (int*)fmalloc(_capacity * sizeof(int));
        memset(newBucketIndices, -1, _capacity * sizeof(int));
        for (uint i = 0; i < _expansion; i++) {
            _buckets[i].next = -1;
        }
        for (uint i = 0; i < _expansion; i++) {
            uint index = CSEntryImpl<V>::hash(_buckets[i].object) % _capacity;
            int* current = &newBucketIndices[index];
            while (*current != -1) {
                current = &_buckets[*current].next;
            }
            *current = i;
        }
        free(_bucketIndices);
        _bucketIndices = newBucketIndices;
    }
    return _expansion++;
}

template <typename V>
CSArray<typename CSSet<V>::ConstTemplateType, 1, false>* CSSet<V>::allObjects() const {
    if (_count == 0) return NULL;
    
    CSArray<ConstTemplateType, 1, false>* objects = CSArray<ConstTemplateType, 1, false>::arrayWithCapacity(_count);
    
    for (uint i = 0; i < _expansion; i++) {
        if (_buckets[i].exists) {
            Bucket& src = _buckets[i];
            
            objects->addObject(src.object);
        }
    }
    return objects;
}

template <typename V>
bool CSSet<V>::containsObject(ValueParamType object) const {
    if (_count) {
        uint index = CSEntryImpl<V>::hash(object) % _capacity;
        
        int current = _bucketIndices[index];
        
        while (current != -1) {
            Bucket& bucket = _buckets[current];
            if (CSEntryImpl<V>::isEqual(object, bucket.object)) {
                return true;
            }
            current = bucket.next;
        }
    }
    return false;
}

template <typename V>
void CSSet<V>::addObject(ValueParamType object) {
    fence(_fence);
    
    uint hash = CSEntryImpl<V>::hash(object);
    
    int current = _bucketIndices[hash % _capacity];
    
    while (current != -1) {
        Bucket& bucket = _buckets[current];
        if (CSEntryImpl<V>::isEqual(object, bucket.object)) {
            if (bucket.object != object) {
                CSEntryImpl<V>::release(bucket.object);
                new (&bucket.object) Type(CSEntryImpl<V>::copy(object));
            }
            return;
        }
        current = bucket.next;
    }
    
    current = expand();
    
    _count++;
    
    {
        uint index = hash % _capacity;
        Bucket& bucket = _buckets[current];
        new (&bucket.object) Type(CSEntryImpl<V>::copy(object));
        bucket.next = _bucketIndices[index];
        bucket.exists = true;
        _bucketIndices[index] = current;
    }
}

template <typename V>
void CSSet<V>::removeObject(ValueParamType object) {
    fence(_fence);
    
    uint index = CSEntryImpl<V>::hash(object) % _capacity;
    
    int* current = &_bucketIndices[index];
    
    while (*current != -1) {
        Bucket& bucket = _buckets[*current];
        if (CSEntryImpl<V>::isEqual(object, bucket.object)) {
            CSEntryImpl<V>::release(bucket.object);
            bucket.exists = false;
            *current = bucket.next;
            _count--;
            break;
        }
        current = &bucket.next;
    }
}

template <typename V>
void CSSet<V>::removeAllObjects() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        Bucket& bucket = _buckets[i];
        if (bucket.exists) {
            CSEntryImpl<V>::release(bucket.object);
            bucket.exists = false;
        }
    }
    memset(_bucketIndices, -1, _capacity * sizeof(int));
    _expansion = 0;
    _count = 0;
}

template <typename V>
CSObject* CSSet<V>::copy() const {
    return new CSSet<V>(this);
}

template <typename V>
CSSet<V>::ReadonlyIterator::ReadonlyIterator(CSSet<V>* set) : _set(set), _index(0), _cursor(NULL), _removed(false) {
#ifdef CS_ASSERT_DEBUG
    CSAssert(!_set->_fence, "concurrent modifying");
    _set->_fence = true;
#endif
    if (_set->_count) {
        while (_index < _set->_capacity) {
            if (_set->_bucketIndices[_index] != -1) {
                _cursor = &_set->_bucketIndices[_index];
                break;
            }
            _index++;
        }
    }
}

template <typename V>
CSSet<V>::ReadonlyIterator::~ReadonlyIterator() {
#ifdef CS_ASSERT_DEBUG
    _set->_fence = false;
#endif
}

template <typename V>
CSSet<V>::Iterator::Iterator(CSSet* set) : ReadonlyIterator(set) {

}

template <typename V>
bool CSSet<V>::ReadonlyIterator::isValid() {
    return _cursor != NULL;
}

template <typename V>
typename CSSet<V>::ValueReturnType CSSet<V>::ReadonlyIterator::object() {
    return _cursor ? _set->_buckets[*_cursor].object : CSEntryImpl<V>::nullValue();
}

template <typename V>
bool CSSet<V>::ReadonlyIterator::next() {
    if (_cursor) {
        if (_removed) {
            _removed = false;
            return true;
        }
        if (_set->_buckets[*_cursor].next != -1) {
            _cursor = &_set->_buckets[*_cursor].next;
            return true;
        }
        else {
            while (_index < _set->_capacity - 1) {
                _index++;
                _cursor = &_set->_bucketIndices[_index];
                if (*_cursor != -1) {
                    return true;
                }
            }
            _cursor = NULL;
        }
    }
    return false;
}

template <typename V>
void CSSet<V>::Iterator::remove() {
    CSAssert(ReadonlyIterator::_cursor, "invalid cursor");
    
    CSEntryImpl<V>::release(ReadonlyIterator::_set->_buckets[*ReadonlyIterator::_cursor].object);
    ReadonlyIterator::_set->_buckets[*ReadonlyIterator::_cursor].exists = false;
    ReadonlyIterator::_set->_count--;
    ReadonlyIterator::_removed = true;
    *ReadonlyIterator::_cursor = ReadonlyIterator::_set->_buckets[*ReadonlyIterator::_cursor].next;
    
    if (*ReadonlyIterator::_cursor == -1) {
        while (ReadonlyIterator::_index < ReadonlyIterator::_set->_capacity - 1) {
            ReadonlyIterator::_index++;
            ReadonlyIterator::_cursor = &ReadonlyIterator::_set->_bucketIndices[ReadonlyIterator::_index];
            if (*ReadonlyIterator::_cursor != -1) {
                return;
            }
        }
        ReadonlyIterator::_cursor = NULL;
    }
}

template <typename V>
typename CSSet<V>::Iterator CSSet<V>::iterator() {
    return Iterator(this);
}
template <typename V>
typename CSSet<V>::ReadonlyIterator CSSet<V>::iterator() const {
    return ReadonlyIterator(const_cast<CSSet<V>*>(this));
}

#endif
