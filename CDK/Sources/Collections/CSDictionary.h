//
//  CSDictionary.h
//  CDK
//
//  Created by 김찬 on 14. 8. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSDictionary__
#define __CDK__CSDictionary__

#include "CSArray.h"

template <typename K, typename V, bool readonly = true>
class CSDictionary : public CSObject {
public:
    typedef typename CSEntryType<K, false>::Type KeyType;
    typedef typename CSEntryType<K, true>::ConstPointerParamType KeyPointerParamType;
    typedef typename CSEntryType<K, true>::ConstValueParamType KeyValueParamType;
    typedef typename CSEntryType<K, true>::ConstValueReturnType KeyValueReturnType;
    typedef typename CSEntryType<K, false>::ConstTemplateType KeyConstTemplateType;
    
    typedef typename CSEntryType<V, readonly>::Type ObjectType;
    typedef typename CSEntryType<V, readonly>::PointerParamType ObjectPointerParamType;
    typedef typename CSEntryType<V, readonly>::ValueParamType ObjectValueParamType;
    typedef typename CSEntryType<V, readonly>::ValueReturnType ObjectValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstValueReturnType ObjectConstValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstTemplateType ObjectConstTemplateType;
    
    class ReadonlyIterator {
    protected:
        CSDictionary* _dictionary;
        uint _index;
        int* _cursor;
        bool _removed;
        
        ReadonlyIterator(CSDictionary* dictionary);
    public:
        virtual ~ReadonlyIterator();
        
        bool isValid();
        KeyValueReturnType key();
        ObjectConstValueReturnType object();
        bool next();
        friend class CSDictionary;
    };
    
    class Iterator : public ReadonlyIterator {
    private:
        Iterator(CSDictionary* dictionary);
    public:
        void remove();
        ObjectValueReturnType object();
        friend class CSDictionary;
    };
    
    friend class ReadonlyIterator;
    friend class Iterator;
private:
    struct Bucket {
        KeyType key;
        ObjectType object;
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
public:
    CSDictionary();
    CSDictionary(uint capacity);
    CSDictionary(KeyValueParamType key, ObjectValueParamType object);
    CSDictionary(KeyPointerParamType const* keys, ObjectPointerParamType const* objects, uint count);
    
    template <typename otherK, typename otherV, bool otherReadonly>
    CSDictionary(const CSDictionary<otherK, otherV, otherReadonly>* otherDictionary, bool deepCopy);
    
private:
    void init();
protected:
    virtual ~CSDictionary();
public:
    static CSDictionary<K, V, readonly>* dictionary();
    static CSDictionary<K, V, readonly>* dictionaryWithCapacity(uint capacity);
    static CSDictionary<K, V, readonly>* dictionaryWithObject(KeyValueParamType key, ObjectValueParamType object);
    static CSDictionary<K, V, readonly>* dictionaryWithObjects(KeyPointerParamType const* keys, ObjectPointerParamType const* objects, uint count);
    template <typename otherK, typename otherV, bool otherReadonly>
    static CSDictionary<K, V, readonly>* dictionaryWithDictionary(const CSDictionary<otherK, otherV, otherReadonly>* otherDictionary, bool deepCopy);
    
    inline uint count() const {
        return _count;
    }
    inline uint capacity() const {
        return _capacity;
    }
private:
    int expand();
public:
    CSArray<KeyConstTemplateType>* allKeys() const;
    CSArray<V, 1, readonly>* allObjects();
    CSArray<ObjectConstTemplateType>* allObjects() const;
    bool containsKey(KeyValueParamType key) const;
    ObjectValueReturnType objectForKey(KeyValueParamType key);
    inline ObjectConstValueReturnType objectForKey(KeyValueParamType key) const {
        return const_cast<CSDictionary<K, V, readonly>*>(this)->objectForKey(key);
    }
    ObjectType* tryGetObjectForKey(KeyValueParamType key);
    inline const ObjectType* tryGetObjectForKey(KeyValueParamType key) const {
        return const_cast<CSDictionary<K, V, readonly>*>(this)->tryGetObjectForKey(key);
    }
    
    ObjectValueReturnType setObject(KeyValueParamType key);
    void setObject(KeyValueParamType key, ObjectValueParamType object);
    void removeObject(KeyValueParamType key);
    void removeAllObjects();
    
    Iterator iterator();
    ReadonlyIterator iterator() const;
    
    inline CSDictionary<K, V, true>* asReadOnly() {
        return reinterpret_cast<CSDictionary<K, V, true>*>(this);
    }
    
    CSObject* copy() const override;
    
    template <typename friendK, class friendV, bool friendReadonly>
    friend class CSDictionary;
};
template <typename K, typename V, bool readonly>
void CSDictionary<K, V, readonly>::init() {
    _bucketIndices = (int*)fmalloc(_capacity * sizeof(int));
    _buckets = (Bucket*)fcalloc(_capacity, sizeof(Bucket));
    memset(_bucketIndices, -1, _capacity * sizeof(int));
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::CSDictionary() : _capacity(12) {
    init();
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::CSDictionary(uint capacity) : _capacity(capacity ? capacity : 1) {
    init();
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::CSDictionary(KeyValueParamType key, ObjectValueParamType object) : _capacity(1) {
    init();
    
    setObject(key, object);
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::CSDictionary(KeyPointerParamType const* keys, ObjectPointerParamType const* objects, uint count) : _capacity(count ? count : 1) {
    init();
    
    for (uint i = 0; i < count; i++) {
        setObject(keys[i], objects[i]);
    }
}

template <typename K, typename V, bool readonly> template <typename otherK, class otherV, bool otherReadonly>
CSDictionary<K, V, readonly>::CSDictionary(const CSDictionary<otherK, otherV, otherReadonly>* otherDictionary, bool deepCopy) : _capacity(otherDictionary->_capacity) {
    init();
    
    _count = otherDictionary->_count;
    
    for (uint i = 0; i < _capacity; i++) {
        int srcIndex = otherDictionary->_bucketIndices[i];
        int* destIndex = &_bucketIndices[i];
        while (srcIndex != -1) {
            typename CSDictionary<otherK, otherV, otherReadonly>::Bucket& src = otherDictionary->_buckets[srcIndex];
            
            *destIndex = _expansion++;
            
            Bucket& dest = _buckets[*destIndex];
            new (&dest.key) KeyType(CSEntryImpl<otherK>::copy(src.key));
            new (&dest.object) ObjectType(deepCopy ? CSEntryImpl<otherV>::copy(src.object) : CSEntryImpl<otherV>::retain(src.object));
            dest.next = -1;
            dest.exists = true;
            
            destIndex = &dest.next;
            srcIndex = src.next;
        }
    }
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::~CSDictionary() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        Bucket& bucket = _buckets[i];
        if (bucket.exists) {
            CSEntryImpl<K>::release(bucket.key);
            CSEntryImpl<V>::release(bucket.object);
        }
    }
    free(_buckets);
    free(_bucketIndices);
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>* CSDictionary<K, V, readonly>::dictionary() {
    return autorelease(new CSDictionary<K, V, readonly>());
}
template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>* CSDictionary<K, V, readonly>::dictionaryWithCapacity(uint capacity) {
    return autorelease(new CSDictionary<K, V, readonly>(capacity));
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>* CSDictionary<K, V, readonly>::dictionaryWithObject(KeyValueParamType key, ObjectValueParamType object) {
    return autorelease(new CSDictionary<K, V, readonly>(object, key));
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>* CSDictionary<K, V, readonly>::dictionaryWithObjects(KeyPointerParamType const* keys, ObjectPointerParamType const* objects, uint count) {
    return autorelease(new CSDictionary<K, V, readonly>(objects, keys, count));
}

template <typename K, typename V, bool readonly> template <typename otherK, typename otherV, bool otherReadonly>
CSDictionary<K, V, readonly>* CSDictionary<K, V, readonly>::dictionaryWithDictionary(const CSDictionary<otherK, otherV, otherReadonly>* otherDictionary, bool deepCopy) {
    return autorelease(new CSDictionary<K, V, readonly>(otherDictionary, deepCopy));
}

template <typename K, typename V, bool readonly>
int CSDictionary<K, V, readonly>::expand() {
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
            uint index = CSEntryImpl<K>::hash(_buckets[i].key) % _capacity;
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

template <typename K, typename V, bool readonly>
CSArray<typename CSDictionary<K, V, readonly>::KeyConstTemplateType>* CSDictionary<K, V, readonly>::allKeys() const {
    if (_count == 0) return NULL;
    
    CSArray<KeyConstTemplateType>* keys = CSArray<KeyConstTemplateType>::arrayWithCapacity(_count);
    
    for (uint i = 0; i < _expansion; i++) {
        if (_buckets[i].exists) {
            Bucket& src = _buckets[i];
            
            keys->addObject(src.key);
        }
    }
    
    return keys;
}

template <typename K, typename V, bool readonly>
CSArray<V, 1, readonly>* CSDictionary<K, V, readonly>::allObjects() {
    if (_count == 0) return NULL;
    
    CSArray<V, 1, readonly>* objects = CSArray<V, 1, readonly>::arrayWithCapacity(_count);
    
    for (uint i = 0; i < _expansion; i++) {
        if (_buckets[i].exists) {
            Bucket& src = _buckets[i];
            
            objects->addObject(src.object);
        }
    }
    
    return objects;
}

template <typename K, typename V, bool readonly>
CSArray<typename CSDictionary<K, V, readonly>::ObjectConstTemplateType>* CSDictionary<K, V, readonly>::allObjects() const {
    if (_count == 0) return NULL;
    
    CSArray<ObjectConstTemplateType>* objects = CSArray<ObjectConstTemplateType>::arrayWithCapacity(_count);
    
    for (uint i = 0; i < _expansion; i++) {
        if (_buckets[i].exists) {
            Bucket& src = _buckets[i];
            
            objects->addObject(src.object);
        }
    }
    
    return objects;
}

template <typename K, typename V, bool readonly>
bool CSDictionary<K, V, readonly>::containsKey(KeyValueParamType key) const {
    if (_count) {
        uint index = CSEntryImpl<K>::hash(key) % _capacity;
        
        int current = _bucketIndices[index];
        
        while (current != -1) {
            Bucket& bucket = _buckets[current];
            
            if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
                return true;
            }
            current = bucket.next;
        }
    }
    return false;
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ObjectValueReturnType CSDictionary<K, V, readonly>::objectForKey(KeyValueParamType key) {
    if (_count) {
        uint index = CSEntryImpl<K>::hash(key) % _capacity;
        
        int current = _bucketIndices[index];
        
        while (current != -1) {
            Bucket& bucket = _buckets[current];
            
            if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
                return bucket.object;
            }
            current = bucket.next;
        }
    }
    return CSEntryImpl<V>::nullValue();
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ObjectType* CSDictionary<K, V, readonly>::tryGetObjectForKey(KeyValueParamType key) {
    if (_count) {
        uint index = CSEntryImpl<K>::hash(key) % _capacity;
        
        int current = _bucketIndices[index];
        
        while (current != -1) {
            Bucket& bucket = _buckets[current];
            
            if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
                return &bucket.object;
            }
            current = bucket.next;
        }
    }
    return NULL;
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ObjectValueReturnType CSDictionary<K, V, readonly>::setObject(KeyValueParamType key) {
    fence(_fence);
    
    uint hash = CSEntryImpl<K>::hash(key);
    
    int current = _bucketIndices[hash % _capacity];
    
    while (current != -1) {
        Bucket& bucket = _buckets[current];
        
        if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
            return CSEntryImpl<V>::notNullValue(bucket.object);
        }
        current = bucket.next;
    }
    
    current = expand();
    
    _count++;
    
    {
        uint index = hash % _capacity;
        Bucket& bucket = _buckets[current];
        new (&bucket.key) KeyType(CSEntryImpl<K>::copy(key));
        bucket.next = _bucketIndices[index];
        bucket.exists = true;
        _bucketIndices[index] = current;
        return CSEntryImpl<V>::notNullValue(bucket.object);
    }
}

template <typename K, typename V, bool readonly>
void CSDictionary<K, V, readonly>::setObject(KeyValueParamType key, ObjectValueParamType object) {
    fence(_fence);
    
    uint hash = CSEntryImpl<K>::hash(key);
    
    int current = _bucketIndices[hash % _capacity];
    
    while (current != -1) {
        Bucket& bucket = _buckets[current];
        
        if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
            CSEntryImpl<V>::release(bucket.object);
            new (&bucket.object) ObjectType(CSEntryImpl<V>::retain(object));
            return;
        }
        current = bucket.next;
    }
    
    current = expand();
    
    _count++;
    
    {
        uint index = hash % _capacity;
        Bucket& bucket = _buckets[current];
        new (&bucket.key) KeyType(CSEntryImpl<K>::copy(key));
        new (&bucket.object) ObjectType(CSEntryImpl<V>::retain(object));
        bucket.next = _bucketIndices[index];
        bucket.exists = true;
        _bucketIndices[index] = current;
    }
}

template <typename K, typename V, bool readonly>
void CSDictionary<K, V, readonly>::removeObject(KeyValueParamType key) {
    fence(_fence);
    
    uint index = CSEntryImpl<K>::hash(key) % _capacity;
    
    int* current = &_bucketIndices[index];
    
    while (*current != -1) {
        Bucket& bucket = _buckets[*current];
        
        if (CSEntryImpl<K>::isEqual(key, bucket.key)) {
            *current = bucket.next;
            CSEntryImpl<K>::release(bucket.key);
            CSEntryImpl<V>::release(bucket.object);
            bucket.exists = false;
            _count--;
            break;
        }
        current = &bucket.next;
    }
}

template <typename K, typename V, bool readonly>
void CSDictionary<K, V, readonly>::removeAllObjects() {
    fence(_fence);
    
    for (uint i = 0; i < _expansion; i++) {
        Bucket& bucket = _buckets[i];
        if (bucket.exists) {
            CSEntryImpl<K>::release(bucket.key);
            CSEntryImpl<V>::release(bucket.object);
            bucket.exists = false;
        }
    }
    memset(_bucketIndices, -1, _capacity * sizeof(int));
    _expansion = 0;
    _count = 0;
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::ReadonlyIterator::ReadonlyIterator(CSDictionary* dictionary) : _dictionary(dictionary), _index(0), _cursor(NULL), _removed(false) {
#ifdef CS_ASSERT_DEBUG
    CSAssert(!_dictionary->_fence, "concurrent modifying");
    _dictionary->_fence = true;
#endif
    if (_dictionary->_count) {
        while (_index < _dictionary->_capacity) {
            if (_dictionary->_bucketIndices[_index] != -1) {
                _cursor = &_dictionary->_bucketIndices[_index];
                break;
            }
            _index++;
        }
    }
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::ReadonlyIterator::~ReadonlyIterator() {
#ifdef CS_ASSERT_DEBUG
    _dictionary->_fence = false;
#endif
}

template <typename K, typename V, bool readonly>
CSDictionary<K, V, readonly>::Iterator::Iterator(CSDictionary* dictionary) : ReadonlyIterator(dictionary) {
}

template <typename K, typename V, bool readonly>
bool CSDictionary<K, V, readonly>::ReadonlyIterator::isValid() {
    return _cursor != NULL;
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::KeyValueReturnType CSDictionary<K, V, readonly>::ReadonlyIterator::key() {
    return _cursor ? _dictionary->_buckets[*_cursor].key : CSEntryImpl<K>::nullValue();
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ObjectConstValueReturnType CSDictionary<K, V, readonly>::ReadonlyIterator::object() {
    return _cursor ? _dictionary->_buckets[*_cursor].object : CSEntryImpl<V>::nullValue();
}

template <typename K, typename V, bool readonly>
bool CSDictionary<K, V, readonly>::ReadonlyIterator::next() {
    if (_cursor) {
        if (_removed) {
            _removed = false;
            return true;
        }
        if (_dictionary->_buckets[*_cursor].next != -1) {
            _cursor = &_dictionary->_buckets[*_cursor].next;
            return true;
        }
        else {
            while (_index < _dictionary->_capacity - 1) {
                _index++;
                _cursor = &_dictionary->_bucketIndices[_index];
                if (*_cursor != -1) {
                    return true;
                }
            }
            _cursor = NULL;
        }
    }
    return false;
}

template <typename K, typename V, bool readonly>
void CSDictionary<K, V, readonly>::Iterator::remove() {
    CSAssert(ReadonlyIterator::_cursor, "invalid cursor");
    
    CSEntryImpl<K>::release(ReadonlyIterator::_dictionary->_buckets[*ReadonlyIterator::_cursor].key);
    CSEntryImpl<V>::release(ReadonlyIterator::_dictionary->_buckets[*ReadonlyIterator::_cursor].object);
    ReadonlyIterator::_dictionary->_buckets[*ReadonlyIterator::_cursor].exists = false;
    ReadonlyIterator::_dictionary->_count--;
    ReadonlyIterator::_removed = true;
    *ReadonlyIterator::_cursor = ReadonlyIterator::_dictionary->_buckets[*ReadonlyIterator::_cursor].next;
    
    if (*ReadonlyIterator::_cursor == -1) {
        while (ReadonlyIterator::_index < ReadonlyIterator::_dictionary->_capacity - 1) {
            ReadonlyIterator::_index++;
            ReadonlyIterator::_cursor = &ReadonlyIterator::_dictionary->_bucketIndices[ReadonlyIterator::_index];
            if (*ReadonlyIterator::_cursor != -1) {
                return;
            }
        }
        ReadonlyIterator::_cursor = NULL;
    }
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ObjectValueReturnType CSDictionary<K, V, readonly>::Iterator::object() {
    return ReadonlyIterator::_cursor ? ReadonlyIterator::_dictionary->_buckets[*ReadonlyIterator::_cursor].object : CSEntryImpl<V>::nullValue();
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::Iterator CSDictionary<K, V, readonly>::iterator() {
    return Iterator(this);
}

template <typename K, typename V, bool readonly>
typename CSDictionary<K, V, readonly>::ReadonlyIterator CSDictionary<K, V, readonly>::iterator() const {
    return ReadonlyIterator(const_cast<CSDictionary<K, V, readonly>*>(this));
}

template <typename K, typename V, bool readonly>
CSObject* CSDictionary<K, V, readonly>::copy() const {
    return new CSDictionary<K, V, readonly>(this, true);
}

#endif
