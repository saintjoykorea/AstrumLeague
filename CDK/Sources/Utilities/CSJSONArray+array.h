//
//  CSJSONArray+array.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 19..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
template <typename V, int dimension, bool retain = derived<CSObject, V>::value>
class readArrayJSONImpl {
public:
    static CSArray<V, dimension>* read(const CSJSONArray* json) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::read(json->jsonArrayAtIndex(i)));
        }
        return array;
    }
    static CSArray<V, dimension>* readNumeric(const CSJSONArray* json) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::readNumeric(json->jsonArrayAtIndex(i)));
        }
        return array;
    }
    static CSArray<V, dimension>* readPoint(const CSJSONArray* json) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::readPoint(json->jsonArrayAtIndex(i)));
        }
        return array;
    }
    static CSArray<V, dimension>* readString(const CSJSONArray* json) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::readString(json->jsonArrayAtIndex(i)));
        }
        return array;
    }
    static CSArray<V, dimension>* readFunc(const CSJSONArray* json, V* (*func)(const CSJSONObject*)) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::readFunc(json->jsonArrayAtIndex(i), func));
        }
        return array;
    }
    template <typename P>
    static CSArray<V, dimension>* readFuncWithParam(const CSJSONArray* json, V* (*func)(const CSJSONObject*, non_deduced_t<P>), P param) {
        CSArray<V, dimension>* array = CSArray<V, dimension>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(readArrayJSONImpl<V, dimension - 1, retain>::readFuncWithParam(json->jsonArrayAtIndex(i), func, param));
        }
        return array;
    }
};
template <typename V>
class readArrayJSONImpl<V, 1, true> {
public:
    static CSArray<V>* read(const CSJSONArray* json) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            V* v = new V(json->jsonObjectAtIndex(i));
            array->addObject(v);
            v->release();
        }
        return array;
    }
    static CSArray<V>* readString(const CSJSONArray* json) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(json->stringAtIndex(i));
        }
        return array;
    }
    static CSArray<V>* readFunc(const CSJSONArray* json, V* (*func)(const CSJSONObject*)) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(func(json->jsonObjectAtIndex(i)));
        }
        return array;
    }
    template <typename P>
    static CSArray<V>* readFuncWithParam(const CSJSONArray* json, V* (*func)(const CSJSONObject*, non_deduced_t<P>), P param) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            array->addObject(func(json->jsonObjectAtIndex(i), param));
        }
        return array;
    }
};

template <typename V>
class readArrayJSONImpl<V, 1, false> {
public:
    static CSArray<V>* read(const CSJSONArray* json) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            new (&array->addObject()) V(json->jsonObjectAtIndex(i));
        }
        return array;
    }
    static CSArray<V>* readNumeric(const CSJSONArray* json) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            new (&array->addObject()) V(json->numberAtIndex(i));
        }
        return array;
    }
    static CSArray<V>* readPoint(const CSJSONArray* json) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            new (&array->addObject()) V(json->doubleAtIndex(i));
        }
        return array;
    }
    static CSArray<V>* readFunc(const CSJSONArray* json, void (*func)(const CSJSONObject*, V&)) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            func(json->jsonObjectAtIndex(i), array->addObject());
        }
        return array;
    }
    template <typename P>
    static CSArray<V>* readFuncWithParam(const CSJSONArray* json, void (*func)(const CSJSONObject*, non_deduced_t<P>, V&), P param) {
        CSArray<V>* array = CSArray<V>::arrayWithCapacity(json->count());
        for (uint i = 0; i < json->count(); i++) {
            func(json->jsonObjectAtIndex(i), param, array->addObject());
        }
        return array;
    }
};

template <typename V, int dimension>
CSArray<V, dimension>* CSJSONArray::readArray() const {
    return readArrayJSONImpl<V, dimension>::read(this);
}

template <typename V, int dimension>
CSArray<V, dimension>* CSJSONArray::readArray(typename readArrayEntry<V>::func func) const {
    return readArrayJSONImpl<V, dimension>::readFunc(this, func);
}

template <typename V, int dimension, typename P>
CSArray<V, dimension>* CSJSONArray::readArray(typename readArrayEntryWithParam<V, P>::func func, P param) const {
    return readArrayJSONImpl<V, dimension>::readFuncWithParam(this, func, param);
}


template <typename V, int dimension>
CSArray<V, dimension>* CSJSONArray::readNumericArray() const {
    return readArrayJSONImpl<V, dimension>::readNumeric(this);
}

template <typename V, int dimension>
CSArray<V, dimension>* CSJSONArray::readPointArray() const {
    return readArrayJSONImpl<V, dimension>::readPoint(this);
}

template <int dimension>
CSArray<CSString, dimension>* CSJSONArray::readStringArray() const {
    return readArrayJSONImpl<CSString, dimension>::readString(this);
}