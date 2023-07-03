//
//  CSBuffer+array.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 19..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSBuffer_array__
#define __CDK__CSBuffer_array__

#include <type_traits>

template <typename V, int dimension, bool retain = derived<CSObject, V>::value>
class readArrayImpl {
public:
    static CSArray<V, dimension>* read(CSBuffer* buffer, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V, dimension> * array = len || !nullIfEmpty ? CSArray<V, dimension>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            array->addObject(readArrayImpl < V, dimension - 1, retain >::read(buffer, nullIfEmpty));
        }
        return array;
    }
    template <typename P>
    static CSArray<V, dimension>* readWithParam(CSBuffer* buffer, P param, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V, dimension> * array = len || !nullIfEmpty ? CSArray<V, dimension>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            array->addObject(readArrayImpl < V, dimension - 1, retain >::readWithParam(buffer, param, nullIfEmpty));
        }
        return array;
    }
    static CSArray<V, dimension>* read(CSBuffer* buffer, V * (*func)(CSBuffer*), bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V, dimension> * array = len || !nullIfEmpty ? CSArray<V, dimension>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            array->addObject(readArrayImpl < V, dimension - 1, retain >::read(buffer, func, nullIfEmpty));
        }
        return array;
    }
    template <typename P>
    static CSArray<V, dimension>* readWithParam(CSBuffer* buffer, V * (*func)(CSBuffer*, P), P param, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V, dimension> * array = len || !nullIfEmpty ? CSArray<V, dimension>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            array->addObject(readArrayImpl < V, dimension - 1, retain >::readWithParam(buffer, func, param, nullIfEmpty));
        }
        return array;
    }
};
template <typename V>
class readArrayImpl<V, 0, true> {
public:
    static inline V* read(CSBuffer* buffer, bool nullIfEmpty) {
        return CSObject::autorelease(new V(buffer));
    }
    template <typename P>
    static inline V* readWithParam(CSBuffer* buffer, P param, bool nullIfEmpty) {
        return CSObject::autorelease(new V(buffer, param));
    }
    static inline V* read(CSBuffer* buffer, V * (*func)(CSBuffer*), bool nullIfEmpty) {
        return func(buffer);
    }
    template <typename P>
    static inline V* readWithParam(CSBuffer* buffer, V * (*func)(CSBuffer*, P), P param, bool nullIfEmpty) {
        return func(buffer, param);
    }
};

template <typename V, bool constructor = std::is_constructible<V, CSBuffer*>::value>
class readNArrayImpl {
public:
    static CSArray<V>* read(CSBuffer* buffer, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V> * array = len || !nullIfEmpty ? CSArray<V>::arrayWithCapacity(len) : NULL;
        if (len) {
            array->addObjectsFromPointer(buffer->bytes(), len);
            buffer->skip(len * sizeof(V));
        }
        return array;
    }
};
template <typename V>
class readNArrayImpl<V, true> {
public:
    static CSArray<V>* read(CSBuffer* buffer, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V> * array = len || !nullIfEmpty ? CSArray<V>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            new (&array->addObject()) V(buffer);
        }
        return array;
    }
};
template <typename V>
class readArrayImpl<V, 1, false> {
public:
    static inline CSArray<V>* read(CSBuffer* buffer, bool nullIfEmpty) {
        return readNArrayImpl<V>::read(buffer, nullIfEmpty);
    }
    template <typename P>
    static inline CSArray<V, 1>* readWithParam(CSBuffer* buffer, P param, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V> * array = len || !nullIfEmpty ? CSArray<V>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            new (&array->addObject()) V(buffer, param);
        }
        return array;
    }
    static inline CSArray<V, 1>* read(CSBuffer* buffer, void (*func)(CSBuffer*, V&), bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V> * array = len || !nullIfEmpty ? CSArray<V>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            (*func)(buffer, array->addObject());
        }
        return array;
    }
    template <typename P>
    static inline CSArray<V, 1>* readWithParam(CSBuffer* buffer, void (*func)(CSBuffer*, P, V&), P param, bool nullIfEmpty) {
        uint len = buffer->readLength();
        
        CSArray<V> * array = len || !nullIfEmpty ? CSArray<V>::arrayWithCapacity(len) : NULL;
        for (uint i = 0; i < len; i++) {
            (*func)(buffer, param, array->addObject());
        }
        return array;
    }
};

template <typename V, int dimension>
CSArray<V, dimension> * CSBuffer::readArray(bool nullIfEmpty) {
    return readArrayImpl<V, dimension>::read(this, nullIfEmpty);
}

template <typename V, int dimension, typename P>
CSArray<V, dimension> * CSBuffer::readArrayWithParam(P param, bool nullIfEmpty) {
    return readArrayImpl<V, dimension>::readWithParam(this, param, nullIfEmpty);
}

template <typename V, int dimension>
CSArray<V, dimension> * CSBuffer::readArrayFunc(typename ReadArrayFunc<V>::func func, bool nullIfEmpty) {
    return readArrayImpl<V, dimension>::read(this, func, nullIfEmpty);
}

template <typename V, int dimension, typename P>
CSArray<V, dimension> * CSBuffer::readArrayFuncWithParam(typename ReadArrayFuncWithParam<V, P>::func func, P param, bool nullIfEmpty) {
    return readArrayImpl<V, dimension>::readWithParam(this, func, param, nullIfEmpty);
}

template <typename V, int dimension, bool retain = derived<CSObject, V>::value>
class writeArrayImpl {
public:
    static inline void write(CSBuffer* buffer, const CSArray<V, dimension>* array) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                writeArrayImpl < V, dimension - 1, retain >::write(buffer, array->objectAtIndex(i));
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    static inline void write(CSBuffer* buffer, const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*) const) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                writeArrayImpl < V, dimension - 1, retain >::write(buffer, array->objectAtIndex(i), func);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    template <typename P>
    static inline void write(CSBuffer* buffer, const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*, P) const, P param) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                writeArrayImpl < V, dimension - 1, retain >::write(buffer, array->objectAtIndex(i), func, param);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    static inline void write(CSBuffer* buffer, const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType)) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                writeArrayImpl < V, dimension - 1, retain >::write(buffer, array->objectAtIndex(i), func);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    template <typename P>
    static inline void write(CSBuffer* buffer, const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType, P), P param) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                writeArrayImpl < V, dimension - 1, retain >::write(buffer, array->objectAtIndex(i), func, param);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
};
template <typename V>
class writeArrayImpl<V, 0, true> {
public:
    static inline void write(CSBuffer* buffer, const V* v, void (V:: *func)(CSBuffer*) const) {
        (v->*func)(buffer);
    }
    template <typename P>
    static inline void write(CSBuffer* buffer, const V* v, void (V:: *func)(CSBuffer*, P) const, P param) {
        (v->*func)(buffer, param);
    }
    static inline void write(CSBuffer* buffer, const V* v, void (*func)(CSBuffer*, const V*)) {
        (*func)(buffer, v);
    }
    template <typename P>
    static inline void write(CSBuffer* buffer, const V* v, void (*func)(CSBuffer*, const V*, P), P param) {
        (*func)(buffer, v, param);
    }
};
template <typename V>
class writeArrayImpl<V, 1, false> {
public:
    static inline void write(CSBuffer* buffer, const CSArray<V, 1>* array) {
        if (array) {
            buffer->writeLength(array->count());
            buffer->write(array->pointer(), array->count()*sizeof(V));
        }
        else {
            buffer->writeLength(0);
        }
    }
    
    static inline void write(CSBuffer* buffer, const CSArray<V, 1>* array, void (V:: *func)(CSBuffer*) const) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                const V& v = array->objectAtIndex(i);
                
                (v.*func)(buffer);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    
    template <typename P>
    static inline void write(CSBuffer* buffer, const CSArray<V, 1>* array, void (V:: *func)(CSBuffer*, P) const, P param) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                const V& v = array->objectAtIndex(i);
                
                (v.*func)(buffer, param);
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    
    static inline void write(CSBuffer* buffer, const CSArray<V, 1>* array, void (*func)(CSBuffer*, const V&)) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                (*func)(buffer, array->objectAtIndex(i));
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
    
    template <typename P>
    static inline void write(CSBuffer* buffer, const CSArray<V, 1>* array, void (*func)(CSBuffer*, P, const V&), P param) {
        if (array) {
            buffer->writeLength(array->count());
            for (uint i = 0; i < array->count(); i++) {
                (*func)(buffer, param, array->objectAtIndex(i));
            }
        }
        else {
            buffer->writeLength(0);
        }
    }
};

template <typename V, int dimension>
void CSBuffer::writeArray(const CSArray<V, dimension>* array) {
    writeArrayImpl<V, dimension>::write(this, array);
}
template <typename V, int dimension>
void CSBuffer::writeArray(const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*) const) {
    writeArrayImpl<V, dimension>::write(this, array, func);
}
template <typename V, int dimension, typename P>
void CSBuffer::writeArray(const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*, P) const, P param) {
    writeArrayImpl<V, dimension>::write(this, array, func, param);
}
template <typename V, int dimension>
void CSBuffer::writeArray(const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType)) {
    writeArrayImpl<V, dimension>::write(this, array, func);
}
template <typename V, int dimension, typename P>
void CSBuffer::writeArray(const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType, P), P param) {
    writeArrayImpl<V, dimension>::write(this, array, func, param);
}

#endif /* __CDK__CSBuffer_array__ */
