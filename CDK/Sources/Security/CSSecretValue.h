//
//  CSSecretValue.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 9. 8..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef CSSecretValue_h
#define CSSecretValue_h

#include "CSSecret.h"

template <typename T, int S = sizeof(T)>
class CSSecretValue;

template <typename T>
class CSSecretValue<T, 4> : public CSSecretValueBase {
private:
    uint _value;
    uint _check;
public:
    CSSecretValue();
    CSSecretValue(const T& other);
    ~CSSecretValue();
    
    void set(const T& other);
    T get() const;
    
    inline CSSecretValue& operator=(const T& other) {
        set(other);
        return *this;
    }
    inline operator T() const {
        return get();
    }
private:
    void reset(const uint64* table, uint64 valueMask, uint64 checkMask) override;
};

template <typename T>
class CSSecretValue<T, 8> : public CSSecretValueBase {
private:
    uint64 _value;
    uint64 _check;
public:
    CSSecretValue();
    CSSecretValue(const T& other);
    ~CSSecretValue();
    
    void set(const T& other);
    T get() const;
    
    inline CSSecretValue& operator=(const T& other) {
        set(other);
        return *this;
    }
    inline operator T() const {
        return get();
    }
private:
    void reset(const uint64* table, uint64 valueMask, uint64 checkMask) override;
};

template <typename T>
CSSecretValue<T, 4>::CSSecretValue() {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint c = (uint)t->table[0];
    _value = (uint)t->table[c % CSSecretCapacity] ^ (uint)t->valueMask;
    _check = c ^ (uint)t->checkMask;
    
    t->addValue(this);
    
    t->unlock();
}

template <typename T>
CSSecretValue<T, 4>::CSSecretValue(const T& other) {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint o = *(uint*)&other;
    uint c = (uint)t->table[o % CSSecretCapacity];
    _value = o ^ (uint)t->table[c % CSSecretCapacity] ^ (uint)t->valueMask;
    _check = c ^ (uint)t->checkMask;
    
    t->addValue(this);
    
    t->unlock();
}

template <typename T>
CSSecretValue<T, 4>::~CSSecretValue() {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    t->removeValue(this);
    t->unlock();
}

template <typename T>
void CSSecretValue<T, 4>::set(const T& other) {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint o = *(uint*)&other;
    uint c = (uint)t->table[o % CSSecretCapacity];
    _value = o ^ (uint)t->table[c % CSSecretCapacity] ^ (uint)t->valueMask;
    _check = c ^ (uint)t->checkMask;
    
    t->unlock();
}

template <typename T>
T CSSecretValue<T, 4>::get() const {
    CSSecret* t = CSSecret::sharedSecret();

    t->lock();
    
    uint c = _check ^ (uint)t->checkMask;
    uint o = _value ^ (uint)t->valueMask ^ (uint)t->table[c % CSSecretCapacity];
    bool success = c == (uint)t->table[o % CSSecretCapacity];

    t->unlock();

    if (success) {
        return *(T*)&o;
    }
    t->occurError();
    return 0;
}

template <typename T>
void CSSecretValue<T, 4>::reset(const uint64* table, uint64 valueMask, uint64 checkMask) {
    CSSecret* t = CSSecret::sharedSecret();
    if (table) {
        uint o = _value ^ (uint)t->valueMask ^ (uint)t->table[(_check ^ (uint)t->checkMask) % CSSecretCapacity];
        uint c = (uint)table[o % CSSecretCapacity];
        _value = o ^ (uint)table[c % CSSecretCapacity] ^ (uint)valueMask;
        _check = c ^ (uint)checkMask;
    }
    else {
        _value = _value ^ (uint)t->valueMask ^ (uint)valueMask;
        _check = _check ^ (uint)t->checkMask ^ (uint)checkMask;
    }
}

template <typename T>
CSSecretValue<T, 8>::CSSecretValue() {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint64 c = t->table[0];
    _value = t->table[c % CSSecretCapacity] ^ t->valueMask;
    _check = c ^ t->checkMask;
    
    t->addValue(this);
    
    t->unlock();
}

template <typename T>
CSSecretValue<T, 8>::CSSecretValue(const T& other) {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint64 o = *(uint64*)&other;
    uint64 c = t->table[o % CSSecretCapacity];
    _value = o ^ t->table[c % CSSecretCapacity] ^ t->valueMask;
    _check = c ^ t->checkMask;
    
    t->addValue(this);
    
    t->unlock();
}

template <typename T>
CSSecretValue<T, 8>::~CSSecretValue() {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    t->removeValue(this);
    t->unlock();
}

template <typename T>
void CSSecretValue<T, 8>::set(const T& other) {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint64 o = *(uint64*)&other;
    uint64 c = t->table[o % CSSecretCapacity];
    _value = o ^ t->table[c % CSSecretCapacity] ^ t->valueMask;
    _check = c ^ t->checkMask;
    
    t->unlock();
}

template <typename T>
T CSSecretValue<T, 8>::get() const {
    CSSecret* t = CSSecret::sharedSecret();
    
    t->lock();
    
    uint64 c = _check ^ t->checkMask;
    uint64 o = _value ^ t->valueMask ^ t->table[c % CSSecretCapacity];
    bool success = c == t->table[o % CSSecretCapacity];
    
    t->unlock();
    
    if (success) {
        return *(T*)&o;
    }
    t->occurError();
    return (T)0;
}

template <typename T>
void CSSecretValue<T, 8>::reset(const uint64* table, uint64 valueMask, uint64 checkMask) {
    CSSecret* t = CSSecret::sharedSecret();
    
    if (table) {
        uint64 o = _value ^ t->valueMask ^ t->table[(_check ^ t->checkMask) % CSSecretCapacity];
        uint64 c = table[o % CSSecretCapacity];
        _value = o ^ table[c % CSSecretCapacity] ^ valueMask;
        _check = c ^ checkMask;
    }
    else {
        _value = _value ^ t->valueMask ^ valueMask;
        _check = _check ^ t->checkMask ^ checkMask;
    }
}

typedef CSSecretValue<int> secint;
typedef CSSecretValue<uint> secuint;
typedef CSSecretValue<int64> secint64;
typedef CSSecretValue<uint64> secuint64;
typedef CSSecretValue<float> secfloat;
typedef CSSecretValue<double> secdouble;
typedef CSSecretValue<fixed> secfixed;

#endif /* CSSecretValue_h */
