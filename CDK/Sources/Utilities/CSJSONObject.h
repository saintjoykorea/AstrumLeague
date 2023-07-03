//
//  CSJSONObject.h
//  CDK
//
//  Created by 김찬 on 13. 6. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSJSONObject__
#define __CDK__CSJSONObject__

#include "CSDictionary.h"

#include "CSJSONArray.h"

class CSJSONObject : public CSDictionary<CSString, CSObject> {
public:
    inline CSJSONObject() {
    }
    
    inline CSJSONObject(uint capacity) : CSDictionary(capacity) {
    }
    
    inline CSJSONObject(const CSJSONObject* obj) : CSDictionary(obj, true) {
    }
    
private:
    inline ~CSJSONObject() {
    }
    
public:
    static inline CSJSONObject* object() {
        return autorelease(new CSJSONObject());
    }
    static inline CSJSONObject* objectWithCapacity(uint capacity) {
        return autorelease(new CSJSONObject(capacity));
    }
    
    bool containsKey(const char* key) const;
    bool boolForKey(const char* key, bool essential = true) const;
    int64 numberForKey(const char* key, bool essential = true) const;
    double doubleForKey(const char* key, bool essential = true) const;
    const CSString* stringForKey(const char* key, bool essential = true) const;
    const CSJSONArray* jsonArrayForKey(const char* key, bool essential = true) const;
    const CSJSONObject* jsonObjectForKey(const char* key, bool essential = true) const;

    template <typename V, int dimension = 1>
    CSArray<V, dimension>* arrayForKey(const char* key, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readArray<V, dimension>() : NULL;
    }
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* numericArrayForKey(const char* key, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readNumericArray<V, dimension>() : NULL;
    }
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* pointArrayForKey(const char* key, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readPointArray<V, dimension>() : NULL;
    }
    template <int dimension = 1>
    CSArray<CSString, dimension>* stringArrayForKey(const char* key, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readStringArray<dimension>() : NULL;
    }
    template <typename V, int dimension = 1>
    CSArray<V, dimension>* arrayForKey(const char* key, typename CSJSONArray::readArrayEntry<V>::func func, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readArray<V, dimension>(func) : NULL;
    }
    template <typename V, int dimension = 1, typename P>
    CSArray<V, dimension>* arrayForKey(const char* key, typename CSJSONArray::readArrayEntryWithParam<V, P>::func func, P param, bool essential = true) const {
        const CSJSONArray* jarr = jsonArrayForKey(key, essential);

        return jarr ? jarr->readArray<V, dimension, P>(func, param) : NULL;
    }
    
    CSString* stringForKey(const char* key, bool essential = true);
    CSJSONArray* jsonArrayForKey(const char* key, bool essential = true);
    CSJSONObject* jsonObjectForKey(const char* key, bool essential = true);
    
    void setBool(const char* key, bool value);
    void setNumber(const char* key, int64 value);
    void setDouble(const char* key, double value);
    void setString(const char* key, const char* value);
    void setJSONArray(const char* key, const CSJSONArray* value);
    void setJSONObject(const char* key, const CSJSONObject* value);
    
    inline CSObject* copy() const override {
        return new CSJSONObject(this);
    }
};

#endif /* defined(__CDK__CSJSONObject__) */
