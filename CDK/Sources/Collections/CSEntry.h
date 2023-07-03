//
//  CSEntry.h
//  CDK
//
//  Created by 김찬 on 14. 8. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSEntry__
#define __CDK__CSEntry__

#include "CSMacro.h"

#include "CSString.h"

#include <type_traits>

#define CSNotFound    -1

//TODO:collection copy constructor, const check if deepCopy=false   (static_assert(std::is_assignable))

template <typename V, bool readonly, bool retaining = derived<CSObject, V>::value>
struct CSEntryType {

};

template <typename V>
class CSEntryType<V, true, false> {
public:
    typedef V Type;
    typedef const V PointerParamType;
    typedef const V& ValueParamType;
    typedef V PointerReturnType;
    typedef V& ValueReturnType;
    typedef const V ConstPointerParamType;
    typedef const V& ConstValueParamType;
    typedef const V ConstPointerReturnType;
    typedef const V& ConstValueReturnType;
    typedef V ConstTemplateType;
};

template <typename V>
class CSEntryType<V, false, false> {
public:
    typedef V Type;
    typedef const V PointerParamType;
    typedef const V& ValueParamType;
    typedef V PointerReturnType;
    typedef V& ValueReturnType;
    typedef const V ConstPointerParamType;
    typedef const V& ConstValueParamType;
    typedef V ConstPointerReturnType;
    typedef V& ConstValueReturnType;
    typedef V ConstTemplateType;
};

template <typename V>
class CSEntryType<V, true, true> {
public:
    typedef V* Type;
    typedef V* PointerParamType;
    typedef V* ValueParamType;
    typedef V* PointerReturnType;
    typedef V* ValueReturnType;
    typedef const V* ConstPointerParamType;
    typedef const V* ConstValueParamType;
    typedef const V* ConstPointerReturnType;
    typedef const V* ConstValueReturnType;
    typedef const V ConstTemplateType;
};

template <typename V>
class CSEntryType<V, false, true> {
public:
    typedef V* Type;
    typedef V* PointerParamType;
    typedef V* ValueParamType;
    typedef V* PointerReturnType;
    typedef V* ValueReturnType;
    typedef const V* ConstPointerParamType;
    typedef const V* ConstValueParamType;
    typedef V* ConstPointerReturnType;
    typedef V* ConstValueReturnType;
    typedef V ConstTemplateType;
};

template <typename V, bool retaining = derived<CSObject, V>::value>
class CSEntryImpl {
public:
    static inline uint hash(const V& a) {
        return (uint)a;
    }
    static int compareTo(const V& a, const V& b) {
        if (a < b) return -1;
        else if (a > b) return 1;
        else return 0;
    }
    static inline bool isEqual(const V& a, const V& b) {
        return a == b;
    }
    static inline const V& copy(const V& a, bool autorelease = false) {
        return a;
    }
    static inline const V& retain(const V& v) {
        return v;
    }
    static inline void retain(V* a, uint count) {
    
    }
    static inline void release(V& v) {
        v.~V();
    }
    static inline void release(V* a, uint count) {
        if (!std::is_trivially_destructible<V>::value) {
            for (uint i = 0; i < count; i++) {
                a[i].~V();
            }
        }
    }
    static inline V& nullValue() {
        CSErrorLog("invalid call");
        abort();
    }
    static inline V& notNullValue(V& a) {
        return a;
    }
    static inline void toString(const V& a, CSString* str) {
        for (uint i = 0; i < sizeof(V); i++) {
            str->appendFormat("%08X ", ((const byte*)&a)[i]);
        }
    }
};

template <typename V>
class CSEntryImpl<V, true> {
public:
    static inline uint hash(const V* a) {
        return a ? a->hash() : 0;
    }
    static inline bool isEqual(const V* a, const V* b) {
        return a ? b && a->isEqual(b) : b == NULL;
    }
    static inline int compareTo(const V* a, const V* b) {
        return a && b ? a->compareTo(b) : 0;
    }
    static inline V* copy(const V* a, bool autorelease = false) {
        V* c = CSObject::copy(a);
        if (c && autorelease) {
            c->autorelease();
        }
        return c;
    }
    static inline V* retain(V* a) {
        return CSObject::retain(a);
    }
    static inline void retain(V** a, uint count) {
        for (uint i = 0; i < count; i++) {
            CSObject::retain(a[i]);
        }
    }
    static inline void release(V*& a) {
        CSObject::release(a);
    }
    static inline void release(V** a, uint count) {
        for (uint i = 0; i < count; i++) {
            CSObject::release(a[i]);
        }
    }
    static inline V* nullValue() {
        return NULL;
    }
    static inline V*& notNullValue(V*& a) {
        CSErrorLog("invalid call");
        abort();
    }
    static inline void toString(const V* a, CSString* str) {
        str->append((const char*)*a);
    }
};

template <typename Type, typename Array>
class CSForeachBlock {
private:
    Array _arr;
    uint _i;
    bool _flag;
public:
    inline CSForeachBlock(Array arr) : _arr(arr), _i(0), _flag(true) {
    }
    
    inline void next1() {
        ++_i;
    }
    
    inline bool check1() {
        return _arr && _i < _arr->count() && _flag;
    }
    
    inline void next2() {
        _flag = true;
    }
    
    inline bool check2() {
        return !_flag;
    }
    
    inline Type object() {
        _flag = false;
        
        return static_cast<Type>(_arr->objectAtIndex(_i));
    }
};

#define foreach(type, value, array)	\
for (CSForeachBlock < type, decltype(array) > __fb_ ## value(array); __fb_ ## value.check1(); __fb_ ## value.next1()) for (type value = __fb_ ## value.object(); __fb_ ## value.check2(); __fb_ ## value.next2())

#ifdef CS_ASSERT_DEBUG
class CSFenceBlock {
private:
    bool& _fence;
public:
    inline CSFenceBlock(bool& fence) : _fence(fence) {
        CSAssert(!_fence, "fence error");
        _fence = true;
    }
    inline ~CSFenceBlock() {
        _fence = false;
    }
    inline operator bool() const {
        return _fence;
    }
	inline bool operator !() const {
		return !_fence;
	}
    inline void next() {
        _fence = false;
    }
};

#define fence_range(value)      for (CSFenceBlock __fb_ ## __LINE__(value); __fb_ ## __LINE__; __fb_ ## __LINE__.next())
#define fence(value)            CSFenceBlock __fb_ ## __LINE__(value)
#else

#define fence_range(value);
#define fence(value);

#endif

#endif
