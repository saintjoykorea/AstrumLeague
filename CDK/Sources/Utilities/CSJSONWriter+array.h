//
//  CSJSONWriter+array.h
//  CDK
//
//  Created by 김찬 on 2015. 1. 29..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

template <typename V, int dimension, bool retain = derived<CSObject, V>::value>
class writeArrayJSONImpl {
public:
    static void write(const char* name, CSJSONWriter* writer, const CSArray<V, dimension>* array, void (V:: *func)(CSJSONWriter*) const) {
        writer->startArray(name);
        if (array) {
            for (uint i = 0; i < array->count(); i++) {
                writeArrayJSONImpl < V, dimension - 1, retain >::write(writer, array->objectAtIndex(i), func);
            }
        }
        writer->endArray();
    }
    template <typename P>
    static void write(const char* name, CSJSONWriter* writer, const CSArray<V, dimension>* array, void (*func)(CSJSONWriter*, typename CSEntryType<V, true>::ConstValueParamType, P), P param) {
        writer->startArray(name);
        if (array) {
            for (uint i = 0; i < array->count(); i++) {
                writeArrayJSONImpl < V, dimension - 1, retain >::write(writer, array->objectAtIndex(i), func, param);
            }
        }
        writer->endArray();
    }
};
template <typename V>
class writeArrayJSONImpl<V, 0, true> {
public:
    static inline void write(CSJSONWriter* writer, const V* v, void (V:: *func)(CSJSONWriter*) const) {
        v->func(writer);
    }
    template <typename P>
    static inline void write(CSJSONWriter* writer, const V* v, void (*func)(CSJSONWriter*, const V*, P), P param) {
        func(writer, v, param);
    }
};
template <typename V>
class writeArrayJSONImpl<V, 0, false> {
public:
    static inline void write(CSJSONWriter* writer, const V& v, void (V:: *func)(CSJSONWriter*) const) {
        v.func(writer);
    }
    template <typename P>
    static inline void write(CSJSONWriter* writer, const V& v, void (*func)(CSJSONWriter*, const V&, P), P param) {
        func(writer, v, param);
    }
};

template <typename V, int dimension>
void CSJSONWriter::writeArray(const char* name, const CSArray<V, dimension>* array, void (V:: *func)(CSJSONWriter*) const) {
    writeArrayJSONImpl<V, dimension>::write(name, array, func);
}
template <typename V, typename P, int dimension>
void writeArray(const char* name, const CSArray<V, dimension>* array, void (*func)(CSJSONWriter*, typename CSEntryType<V, true>::ConstValueParamType, P), P param) {
    writeArrayJSONImpl<V, dimension>::write(name, array, func, param);
}

