//
//  CSValue.h
//  CDK
//
//  Created by 김찬 on 12. 8. 1..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSValue__
#define __CDK__CSValue__

#include "CSString.h"

template <typename T>
class CSValue : public CSObject {
private:
    T _value;
public:
    inline CSValue() {
    }
    
    inline CSValue(const T& value) : _value(value) {
    }
    
private:
    inline ~CSValue() {
    }
    
public:
    static inline CSValue<T>* value(const T& value) {
        return autorelease(new CSValue<T>(value));
    }
    
    inline operator T() const {
        return _value;
    }
    
    inline void setValue(const T& value) {
        _value = value;
    }
    
    inline const T& value() const {
        return _value;
    }
    
    inline uint hash() const override {
        return (uint)_value;
    }
    
    inline CSObject* copy() const override {
        return new CSValue<T>(_value);
    }
    
    inline bool isEqualToValue(const CSValue<T>* value) const {
        return _value == value->_value;
    }
    
    inline bool isEqual(const CSObject* object) const override {
        const CSValue<T>* value = dynamic_cast<const CSValue<T>*>(object);
        
        return value && isEqualToValue(value);
    }
};
typedef CSValue<sbyte> CSSByte;
typedef CSValue<byte> CSByte;
typedef CSValue<short> CSShort;
typedef CSValue<ushort> CSUShort;
typedef CSValue<int> CSInteger;
typedef CSValue<uint> CSUInteger;
typedef CSValue<int64> CSLong;
typedef CSValue<uint64> CSULong;
typedef CSValue<float> CSFloat;
typedef CSValue<double> CSDouble;
typedef CSValue<bool> CSBool;

#endif
