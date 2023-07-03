//
//  CSArray.h
//  CDK
//
//  Created by 김찬 on 14. 8. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSArray__
#define __CDK__CSArray__

#include "CSEntry.h"

#include <functional>

template <typename V, int dimension, bool readonly>
class CSArray;

template <typename V, int dimension = 1, bool readonly = true>
class CSArray : public CSArray<CSArray<V, dimension - 1,readonly>, 1, readonly> {
public:
    typedef CSArray<V, dimension - 1, readonly> IV;
    
    CSArray();
    CSArray(uint capacity);
    CSArray(IV* const * objects, uint count);
    
    template<typename otherV, bool otherReadonly>
    CSArray(const CSArray<otherV, dimension, otherReadonly>* otherArray, bool deepCopy);
    
    static CSArray<V, dimension, readonly>* array();
    static CSArray<V, dimension, readonly>* arrayWithCapacity(uint capacity);
    static CSArray<V, dimension, readonly>* arrayWithObjects(IV* const* objects, uint count);
    
    template<typename otherV, bool otherReadonly>
    static CSArray<V, dimension, readonly>* arrayWithArray(const CSArray<otherV, dimension, otherReadonly>* otherArray, bool deepCopy);
    
    template <typename otherV, bool otherReadonly>
    void addObjectsFromArray(CSArray<otherV, dimension, otherReadonly>* otherArray);
    template <typename otherV, bool otherReadonly>
    void addObjectsFromArray(const CSArray<otherV, dimension, otherReadonly>* otherArray);
    template <typename otherV, bool otherReadonly>
    void insertObjectsFromArray(uint index, CSArray<otherV, dimension, otherReadonly>* otherArray);
    template <typename otherV, bool otherReadonly>
    void insertObjectsFromArray(uint index, const CSArray<otherV, dimension, otherReadonly>* otherArray);
    
    virtual CSObject* copy() const override;
};

template <typename V, bool readonly>
class CSArray<V, 1, readonly> : public CSObject {
public:
    typedef typename CSEntryType<V, readonly>::Type Type;
    typedef typename CSEntryType<V, readonly>::PointerParamType PointerParamType;
    typedef typename CSEntryType<V, readonly>::ValueParamType ValueParamType;
    typedef typename CSEntryType<V, readonly>::PointerReturnType PointerReturnType;
    typedef typename CSEntryType<V, readonly>::ValueReturnType ValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstPointerParamType ConstPointerParamType;
    typedef typename CSEntryType<V, readonly>::ConstValueParamType ConstValueParamType;
    typedef typename CSEntryType<V, readonly>::ConstPointerReturnType ConstPointerReturnType;
    typedef typename CSEntryType<V, readonly>::ConstValueReturnType ConstValueReturnType;
    typedef typename CSEntryType<V, readonly>::ConstTemplateType ConstTemplateType;
    
    class ReadonlyIterator {
    protected:
        CSArray* _array;
        uint _index;
        
        ReadonlyIterator(CSArray* array);
    public:
        virtual ~ReadonlyIterator();
        
        bool isValid();
        ConstValueReturnType object();
        bool next();
        friend class CSArray;
    };
    
    class Iterator : public ReadonlyIterator {
    private:
        Iterator(CSArray* array);
    public:
        void remove();
        void insert(ValueParamType obj);
        ValueReturnType insert();
        ValueReturnType object();
        friend class CSArray;
    };
    
    friend class ReadonlyIterator;
    friend class Iterator;
private:
    Type* _objects;
    uint _count;
    uint _capacity;
#ifdef CS_ASSERT_DEBUG
    bool _fence;
#endif
public:
    CSArray();
    CSArray(uint capacity);
    CSArray(PointerParamType const* objects, uint count);
    template<typename otherV, bool otherReadonly>
    CSArray(const CSArray<otherV, 1, otherReadonly>* otherArray, bool deepCopy);
private:
    void init();
protected:
    virtual ~CSArray();
public:

    static CSArray<V, 1, readonly>* array();
    static CSArray<V, 1, readonly>* arrayWithCapacity(uint capacity);
    static CSArray<V, 1, readonly>* arrayWithObjects(PointerParamType const* objects, uint count);
    
    template<typename otherV, bool otherReadonly>
    static CSArray<V, 1, readonly>* arrayWithArray(const CSArray<otherV, 1, otherReadonly>* otherArray, bool deepCopy);
    
    inline ConstPointerReturnType const* pointer() const {
        return _objects;
    }
    
    inline PointerReturnType const* pointer() {
        return _objects;
    }
    
    inline uint count() const {
        return _count;
    }
    
    inline uint capacity() const {
        return _capacity;
    }
    
    int indexOfObject(ConstValueParamType object) const;
    int indexOfObjectIdenticalTo(ConstValueParamType object) const;
    ValueReturnType objectAtIndex(uint index);
    ConstValueReturnType objectAtIndex(uint index) const;
    ValueReturnType lastObject();
    ConstValueReturnType lastObject() const;
    bool containsObject(ConstValueParamType object) const;
    bool containsObjectIdenticalTo(ConstValueParamType object) const;
    
    CSArray<V, 1, readonly>* subarrayWithRange(uint offset, uint count);
    CSArray<ConstTemplateType, 1, false>* subarrayWithRange(uint offset, uint count) const;
    
    ValueReturnType addObject();
    void addObject(ValueParamType object);
    template <typename otherV, bool otherReadonly>
    void addObjectsFromArray(CSArray<otherV, 1, otherReadonly>* otherArray);
    template <typename otherV, bool otherReadonly>
    void addObjectsFromArray(const CSArray<otherV, 1, otherReadonly>* otherArray);
    void addObjectsFromPointer(const void* objects, uint count);
    ValueReturnType insertObject(uint index);
    void insertObject(uint index, ValueParamType object);
    template <typename otherV, bool otherReadonly>
    void insertObjectsFromArray(uint index, CSArray<otherV, 1, otherReadonly>* otherArray);
    template <typename otherV, bool otherReadonly>
    void insertObjectsFromArray(uint index, const CSArray<otherV, 1, otherReadonly>* otherArray);
    void insertObjectsFromPointer(uint index, const void* objects, uint count);
    void removeLastObject();
    bool removeObject(ConstValueParamType object);
    bool removeObjectIdenticalTo(ConstValueParamType object);
    void removeObjectAtIndex(uint index);
    void removeAllObjects();
    void sort();
	void sort(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo);
    template <class Object, typename Method>
    void sort(Object* obj, Method compareTo);
    
    CSArray<V, 1, readonly>* sortedArray();
    CSArray<ConstTemplateType, 1, false>* sortedArray() const;
    
    CSArray<V, 1, readonly>* sortedArray(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo);
    CSArray<ConstTemplateType, 1, false>* sortedArray(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo) const;
    
	template <class Object, typename Method>
    CSArray<V, 1, readonly>* sortedArray(Object* obj, Method compareTo);
	template <class Object, typename Method>
    CSArray<ConstTemplateType, 1, false>* sortedArray(Object* obj, Method compareTo) const;
    
    inline CSArray<V, 1, true>* asReadOnly() {
        return reinterpret_cast<CSArray<V, 1, true>*>(this);
    }
    
    Iterator iterator();
    ReadonlyIterator iterator() const;
    
    virtual CSObject* copy() const override;
private:
    void expand(uint count);
    
    template <typename friendV, int friendDimension, bool friendReadonly>
    friend class CSArray;
};
//============================================================================================================================================
template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>::CSArray() : CSArray<IV, 1, readonly>() {}
template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>::CSArray(uint capacity) : CSArray<IV, 1, readonly>(capacity) {}
template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>::CSArray(IV* const * objects, uint count) : CSArray<IV, 1, readonly>(objects, count) {}
template<typename V, int dimension, bool readonly> template<typename otherV, bool otherReadonly>
CSArray<V, dimension, readonly>::CSArray(const CSArray<otherV, dimension, otherReadonly>* otherArray, bool deepCopy) :
    CSArray<IV, 1, readonly>(static_cast < const CSArray < CSArray < otherV, dimension - 1, otherReadonly >, 1, otherReadonly > * > (otherArray), deepCopy)
{
}

template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>* CSArray<V, dimension, readonly>::array() {
    return CSObject::autorelease(new CSArray<V, dimension, readonly>());
}
template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>* CSArray<V, dimension, readonly>::arrayWithCapacity(uint capacity) {
    return CSObject::autorelease(new CSArray<V, dimension, readonly>(capacity));
}
template<typename V, int dimension, bool readonly>
CSArray<V, dimension, readonly>* CSArray<V, dimension, readonly>::arrayWithObjects(IV* const* objects, uint count) {
    return CSObject::autorelease(new CSArray<V, dimension, readonly>(objects, count));
}
template<typename V, int dimension, bool readonly> template<typename otherV, bool otherReadonly>
CSArray<V, dimension, readonly>* CSArray<V, dimension, readonly>::arrayWithArray(const CSArray<otherV, dimension, otherReadonly>* otherArray, bool deepCopy) {
    return CSObject::autorelease(new CSArray<V, dimension, readonly>(otherArray, deepCopy));
}

template<typename V, int dimension, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, dimension, readonly>::addObjectsFromArray(CSArray<otherV, dimension, otherReadonly>* otherArray) {
    CSArray<IV, 1, readonly>::addObjectsFromArray(static_cast < CSArray < CSArray < otherV, dimension - 1, otherReadonly >, 1, otherReadonly > * > (otherArray));
}
template<typename V, int dimension, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, dimension, readonly>::addObjectsFromArray(const CSArray<otherV, dimension, otherReadonly>* otherArray) {
    CSArray<IV, 1, readonly>::addObjectsFromArray(static_cast < const CSArray < CSArray < otherV, dimension - 1, otherReadonly >, 1, otherReadonly > * > (otherArray));
}
template<typename V, int dimension, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, dimension, readonly>::insertObjectsFromArray(uint index, CSArray<otherV, dimension, otherReadonly>* otherArray) {
    CSArray<IV, 1, readonly>::insertObjectsFromArray(index, static_cast < CSArray < CSArray < otherV, dimension - 1, otherReadonly >, 1, otherReadonly > * > (otherArray));
}
template<typename V, int dimension, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, dimension, readonly>::insertObjectsFromArray(uint index, const CSArray<otherV, dimension, otherReadonly>* otherArray) {
    CSArray<IV, 1, readonly>::insertObjectsFromArray(index, static_cast < const CSArray < CSArray < otherV, dimension - 1, otherReadonly >, 1, otherReadonly > * > (otherArray));
}

template<typename V, int dimension, bool readonly>
CSObject* CSArray<V, dimension, readonly>::copy() const {
    return new CSArray<V, dimension, readonly>(this, true);
}

//============================================================================================================================================
template <typename V, bool readonly>
void CSArray<V, 1, readonly>::init() {
	_objects = (Type*)fmalloc(_capacity * sizeof(Type));
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>::CSArray() : _capacity(12) {
    init();
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>::CSArray(uint capacity) : _capacity(capacity ? capacity : 1) {
    init();
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>::CSArray(PointerParamType const* objects, uint count) : _capacity(count ? count : 1) {
    init();
    
    for (uint i = 0; i < count; i++) {
		new (&_objects[_count++]) Type(CSEntryImpl<V>::retain(objects[i]));
    }
}

template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
CSArray<V, 1, readonly>::CSArray(const CSArray<otherV, 1, otherReadonly>* otherArray, bool deepCopy) : _capacity(otherArray->_capacity) {
    init();
    
    for (uint i = 0; i < otherArray->_count; i++) {
		new (&_objects[_count++]) Type(deepCopy ? CSEntryImpl<otherV>::copy(otherArray->_objects[i]) : CSEntryImpl<otherV>::retain(otherArray->_objects[i]));
    }
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>::~CSArray() {
    fence(_fence);
    
    CSEntryImpl<V>::release(_objects, _count);

    free(_objects);
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::array() {
    return autorelease(new CSArray<V, 1, readonly>());
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::arrayWithCapacity(uint capacity) {
    return autorelease(new CSArray<V, 1, readonly>(capacity));
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::arrayWithObjects(PointerParamType const* objects, uint count) {
    return autorelease(new CSArray<V, 1, readonly>(objects, count));
}

template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::arrayWithArray(const CSArray<otherV, 1, otherReadonly>* otherArray, bool deepCopy) {
    return autorelease(new CSArray<V, 1, readonly>(otherArray, deepCopy));
}

template <typename V, bool readonly>
int CSArray<V, 1, readonly>::indexOfObject(ConstValueParamType object) const {
    for (uint i = 0; i < _count; i++) {
        if (CSEntryImpl<V>::isEqual(_objects[i], object)) return i;
    }
    return CSNotFound;
}

template <typename V, bool readonly>
int CSArray<V, 1, readonly>::indexOfObjectIdenticalTo(ConstValueParamType object) const {
    for (uint i = 0; i < _count; i++) {
        if (_objects[i] == object) return i;
    }
    return CSNotFound;
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::objectAtIndex(uint index) {
    CSAssert(index < _count, "index out of range");
    return _objects[index];
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ConstValueReturnType CSArray<V, 1, readonly>::objectAtIndex(uint index) const {
    CSAssert(index < _count, "index out of range");
    return _objects[index];
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::lastObject() {
    return _count ? _objects[_count - 1] : CSEntryImpl<V>::nullValue();
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ConstValueReturnType CSArray<V, 1, readonly>::lastObject() const {
    return _count ? _objects[_count - 1] : CSEntryImpl<V>::nullValue();
}

template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::containsObject(ConstValueParamType object) const {
    return indexOfObject(object) != CSNotFound;
}

template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::containsObjectIdenticalTo(ConstValueParamType object) const {
    return indexOfObjectIdenticalTo(object) != CSNotFound;
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::subarrayWithRange(uint offset, uint count) {
    CSAssert(_count >= offset + count, "index out of range");
    
    CSArray<V, 1, readonly>* arr = CSArray<V, 1, readonly>::arrayWithCapacity(count);
    for (uint i = offset; i < offset + count; i++) {
		new (&arr->_objects[arr->_count++]) Type(CSEntryImpl<V>::retain(_objects[i]));
    }
    return arr;
}

template <typename V, bool readonly>
CSArray<typename CSArray<V, 1, readonly>::ConstTemplateType, 1, false>* CSArray<V, 1, readonly>::subarrayWithRange(uint offset, uint count) const {
    CSAssert(_count >= offset + count, "index out of range");
    
    CSArray<ConstTemplateType, 1, false>* arr = CSArray<ConstTemplateType, 1, false>::arrayWithCapacity(count);
    for (uint i = offset; i < offset + count; i++) {
		new (&arr->_objects[arr->_count++]) Type(CSEntryImpl<V>::retain(_objects[i]));
    }
    return arr;
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::addObject() {
    fence(_fence);
    
    expand(1);
    
    return CSEntryImpl<V>::notNullValue(_objects[_count++]);
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::addObject(ValueParamType object) {
    fence(_fence);
    
    expand(1);

    new (&_objects[_count++]) Type(CSEntryImpl<V>::retain(object));
}

template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, 1, readonly>::addObjectsFromArray(CSArray<otherV, 1, otherReadonly>* otherArray) {
    addObjectsFromPointer(otherArray->_objects, otherArray->_count);
}
template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, 1, readonly>::addObjectsFromArray(const CSArray<otherV, 1, otherReadonly>* otherArray) {
    addObjectsFromPointer(otherArray->_objects, otherArray->_count);
}
template <typename V, bool readonly>
void CSArray<V, 1, readonly>::addObjectsFromPointer(const void* objects, uint count) {
    fence(_fence);
    
    expand(count);
    memcpy(_objects + _count, objects, count * sizeof(Type));
    CSEntryImpl<V>::retain(_objects + _count, count);
    _count += count;
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::insertObject(uint index) {
    fence(_fence);
    
    CSAssert(index <= _count, "index out of range");
    
    expand(1);
    
    uint remaining = _count - index;
    if (remaining) {
        memmove(&_objects[index + 1], &_objects[index], remaining * sizeof(Type));
    }
    _count++;
    return CSEntryImpl<V>::notNullValue(_objects[index]);
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::insertObject(uint index, ValueParamType object) {
    fence(_fence);
    
    CSAssert(index <= _count, "index out of range");
    
    expand(1);
    
    uint remaining = _count - index;
    if (remaining) {
        memmove(&_objects[index + 1], &_objects[index], remaining * sizeof(Type));
    }
    _count++;
    new (&_objects[index]) Type(CSEntryImpl<V>::retain(object));
}

template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, 1, readonly>::insertObjectsFromArray(uint index, CSArray<otherV, 1, otherReadonly>* otherArray) {
    insertObjectsFromPointer(otherArray->_objects, otherArray->_count, index);
}
template <typename V, bool readonly> template <typename otherV, bool otherReadonly>
void CSArray<V, 1, readonly>::insertObjectsFromArray(uint index, const CSArray<otherV, 1, otherReadonly>* otherArray) {
    insertObjectsFromPointer(otherArray->_objects, otherArray->_count, index);
}
template <typename V, bool readonly>
void CSArray<V, 1, readonly>::insertObjectsFromPointer(uint index, const void* objects, uint count) {
    fence(_fence);
    
    CSAssert(index <= _count, "index out of range");
    
    expand(count);
    uint remaining = _count - index;
    if (remaining) {
        memmove(&_objects[index + count], &_objects[index], remaining * sizeof(Type));
    }
    memcpy(_objects + _count, objects, count * sizeof(Type));
    CSEntryImpl<V>::retain(_objects + _count, count);
    _count += count;
}
template <typename V, bool readonly>
void CSArray<V, 1, readonly>::removeLastObject() {
    fence(_fence);
    
    if (_count) {
        _count--;
        CSEntryImpl<V>::release(_objects[_count]);
    }
}

template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::removeObject(ConstValueParamType object) {
    uint index = indexOfObject(object);
    
    if (index == CSNotFound) {
        return false;
    }
    removeObjectAtIndex(index);
    return true;
}

template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::removeObjectIdenticalTo(ConstValueParamType object) {
    uint index = indexOfObjectIdenticalTo(object);
    
    if (index == CSNotFound) {
        return false;
    }
    removeObjectAtIndex(index);
    return true;
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::removeObjectAtIndex(uint index) {
    fence(_fence);
    
    CSAssert(index < _count, "index out of range");
    
    CSEntryImpl<V>::release(_objects[index]);

    _count--;
    
    uint remaining = _count - index;
    if (remaining) {
        memmove(&_objects[index], &_objects[index + 1], remaining * sizeof(Type));
    }
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::removeAllObjects() {
    fence(_fence);
    
    CSEntryImpl<V>::release(_objects, _count);

    _count = 0;
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::sort() {
    fence(_fence);
    
    if (_count) {
        bool flag;
        
        do {
            flag = false;
            
            for (uint i = 0; i < _count - 1; i++) {
                int rtn = CSEntryImpl<V>::compareTo(_objects[i], _objects[i + 1]);
                
                if (rtn > 0) {
                    Type temp = _objects[i];
                    _objects[i] = _objects[i + 1];
                    _objects[i + 1] = temp;
                    flag = true;
                }
            }
        } while(flag);
    }
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::sort(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo) {
    fence(_fence);
    
    if (_count) {
        bool flag;
        
        do {
            flag = false;
            
            for (uint i = 0; i < _count - 1; i++) {
                int rtn = compareTo(_objects[i], _objects[i + 1]);
                
                if (rtn > 0) {
                    Type temp = _objects[i];
                    _objects[i] = _objects[i + 1];
                    _objects[i + 1] = temp;
                    flag = true;
                }
            }
        } while(flag);
    }
}

template <typename V, bool readonly> template <class Object, typename Method>
void CSArray<V, 1, readonly>::sort(Object* obj, Method compareTo) {
    fence(_fence);
    
    if (_count) {
        bool flag;
        
        do {
            flag = false;
            
            for (uint i = 0; i < _count - 1; i++) {
                int rtn = (obj->*compareTo)(_objects[i], _objects[i + 1]);
                
                if (rtn > 0) {
                    Type temp = _objects[i];
                    _objects[i] = _objects[i + 1];
                    _objects[i + 1] = temp;
                    flag = true;
                }
            }
        } while(flag);
    }
}

template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::sortedArray() {
    CSArray<V, 1, readonly>* array = CSArray<V, 1, readonly>::arrayWithArray(this, false);
    array->sort();
    return array;
}

template <typename V, bool readonly>
CSArray<typename CSArray<V, 1, readonly>::ConstTemplateType, 1, false>* CSArray<V, 1, readonly>::sortedArray() const {
    CSArray<ConstTemplateType, 1, false>* array = CSArray<ConstTemplateType, 1, false>::arrayWithArray(this, false);
    array->sort();
    return array;
}
template <typename V, bool readonly>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::sortedArray(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo) {
    CSArray<V, 1, readonly>* array = CSArray<V, 1, readonly>::arrayWithArray(this, false);
    array->sort(compareTo);
    return array;
}

template <typename V, bool readonly>
CSArray<typename CSArray<V, 1, readonly>::ConstTemplateType, 1, false>* CSArray<V, 1, readonly>::sortedArray(std::function<int(typename CSEntryType<V, readonly>::ConstValueParamType, typename CSEntryType<V, readonly>::ConstValueParamType)> compareTo) const {
    CSArray<ConstTemplateType, 1, false>* array = CSArray<ConstTemplateType, 1, false>::arrayWithArray(this, false);
    array->sort(compareTo);
    return array;
}
template <typename V, bool readonly> template <class Object, typename Method>
CSArray<V, 1, readonly>* CSArray<V, 1, readonly>::sortedArray(Object* obj, Method compareTo) {
    CSArray<V, 1, readonly>* array = CSArray<V, 1, readonly>::arrayWithArray(this, false);
    array->sort(obj, compareTo);
    return array;
}
template <typename V, bool readonly> template <class Object, typename Method>
CSArray<typename CSArray<V, 1, readonly>::ConstTemplateType, 1, false>* CSArray<V, 1, readonly>::sortedArray(Object* obj, Method compareTo) const {
    CSArray<ConstTemplateType, 1, false>* array = CSArray<ConstTemplateType, 1, false>::arrayWithArray(this, false);
    array->sort(obj, compareTo);
    return array;
}

template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::Iterator CSArray<V, 1, readonly>::iterator() {
    return Iterator(this);
}
template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ReadonlyIterator CSArray<V, 1, readonly>::iterator() const {
    return Iterator(const_cast<CSArray<V, 1, readonly>*>(this));
}

template <typename V, bool readonly>
CSObject* CSArray<V, 1, readonly>::copy() const {
    return new CSArray<V, 1, readonly>(this, true);
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::expand(uint count) {
    uint nextCapacity = _capacity;
    while (nextCapacity < _count + count) {
        nextCapacity <<= 1;
    }
    if (nextCapacity > _capacity) {
        _objects = (Type*)frealloc(_objects, nextCapacity * sizeof(Type));
        _capacity = nextCapacity;
    }
}
template <typename V, bool readonly>
CSArray<V, 1, readonly>::ReadonlyIterator::ReadonlyIterator(CSArray<V, 1, readonly>* array) : _array(array), _index(0) {

}
template <typename V, bool readonly>
CSArray<V, 1, readonly>::ReadonlyIterator::~ReadonlyIterator() {

}
template <typename V, bool readonly>
CSArray<V, 1, readonly>::Iterator::Iterator(CSArray<V, 1, readonly>* array) : ReadonlyIterator(array) {
}
template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::ReadonlyIterator::isValid() {
    return _index < _array->_count;
}
template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ConstValueReturnType CSArray<V, 1, readonly>::ReadonlyIterator::object() {
    return _array->objectAtIndex(_index);
}
template <typename V, bool readonly>
bool CSArray<V, 1, readonly>::ReadonlyIterator::next() {
    return ++_index < _array->_count;
}

template <typename V, bool readonly>
void CSArray<V, 1, readonly>::Iterator::remove() {
    ReadonlyIterator::_array->removeObjectAtIndex(ReadonlyIterator::_index);
}
template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::Iterator::insert() {
    return ReadonlyIterator::_array->insertObject(ReadonlyIterator::_index);
}
template <typename V, bool readonly>
void CSArray<V, 1, readonly>::Iterator::insert(ValueParamType obj) {
    ReadonlyIterator::_array->insertObject(ReadonlyIterator::_index, obj);
}
template <typename V, bool readonly>
typename CSArray<V, 1, readonly>::ValueReturnType CSArray<V, 1, readonly>::Iterator::object() {
    return ReadonlyIterator::_array->objectAtIndex(ReadonlyIterator::_index);
}

#endif
