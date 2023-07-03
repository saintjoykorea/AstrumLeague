//
//  CSJSONArray.h
//  CDK
//
//  Created by 김찬 on 13. 6. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSJSONArray__
#define __CDK__CSJSONArray__

#include "CSArray.h"

#include "CSMacro.h"

class CSJSONObject;

class CSJSONArray : public CSArray<CSObject> {
public:
    inline CSJSONArray() {
    }
    
    inline CSJSONArray(uint capacity) : CSArray(capacity) {
    }
    
    inline CSJSONArray(const CSJSONArray* arr) : CSArray(arr, true) {
    }
    
private:
    inline ~CSJSONArray() {
    }
    
public:
    static inline CSJSONArray* array() {
        return autorelease(new CSJSONArray());
    }
    static inline CSJSONArray* arrayWithCapacity(uint capacity) {
        return autorelease(new CSJSONArray(capacity));
    }
    
    bool boolAtIndex(uint index) const;
    int64 numberAtIndex(uint index) const;
    double doubleAtIndex(uint index) const;
    const CSString* stringAtIndex(uint index) const;
    const CSJSONArray* jsonArrayAtIndex(uint index) const;
    const CSJSONObject* jsonObjectAtIndex(uint index) const;
    
    CSString* stringAtIndex(uint index);
    CSJSONArray* jsonArrayAtIndex(uint index);
    CSJSONObject* jsonObjectAtIndex(uint index);
    
    void addBool(bool value);
    void addNumber(int64 value);
    void addDouble(double value);
    void addString(const char* value);
    void addJSONArray(const CSJSONArray* value);
    void addJSONObject(const CSJSONObject* value);
    
    void insertBool(uint index, bool value);
    void insertNumber(uint index, int64 value);
    void insertDouble(uint index, double value);
    void insertString(uint index, const char* value);
    void insertJSONArray(uint index, const CSJSONArray* value);
    void insertJSONObject(uint index, const CSJSONObject* value);
    
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* readArray() const;
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* readNumericArray() const;
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* readPointArray() const;
    template <int dimension = 1>
    CSArray<CSString, dimension>* readStringArray() const;
    
    //===============================================================
    template <typename V, bool retain = derived<CSObject, V>::value>
    struct readArrayEntry {
        typedef V* (*func)(const CSJSONObject*);
    };
    template <typename V>
    struct readArrayEntry<V, false> {
        typedef void (*func)(const CSJSONObject*, V&);
    };
    template <typename V, typename P, bool retain = derived<CSObject, V>::value>
    struct readArrayEntryWithParam {
        typedef V* (*func)(const CSJSONObject*, P);
    };
    template <typename V, typename P>
    struct readArrayEntryWithParam<V, P, false> {
        typedef void (*func)(const CSJSONObject*, P, V&);
    };
    //===============================================================
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* readArray(typename readArrayEntry<V>::func func) const;
    template <typename V, int dimension = 1, typename P>
    CSArray<V, dimension>* readArray(typename readArrayEntryWithParam<V, P>::func func, P param) const;
    
    inline CSObject* copy() const override {
        return new CSJSONArray(this);
    }
};

#include "CSJSONArray+array.h"

#endif /* defined(__CDK__CSJSONArray__) */
