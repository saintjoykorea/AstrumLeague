//
//  CSFixed.h
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSFixed__
#define __CDK__CSFixed__

#include "CSTypes.h"

#include "CSLog.h"

#define frac_func(func)    \
    func(float)     \
    func(double)

#define integer_func(func)    \
    func(int64)     \
    func(uint64)    \
    func(int)       \
    func(uint)      \
    func(short)     \
    func(ushort)    \
    func(sbyte)     \
    func(byte)

class fixed {
public:
    int64 raw;
    
    enum FixedRaw { Raw };
    inline fixed(FixedRaw, int64 raw) : raw(raw) {}
    
    static const int64 OneRaw;
    static const int64 FracRaw;
    static const fixed One;
    static const fixed Half;
    static const fixed Quarter;
    static const fixed Zero;
    static const fixed Max;
    static const fixed Min;
    static const fixed Pi;
    static const fixed TwoPi;
    static const fixed PiOverTwo;
    static const fixed PiOverFour;
    static const fixed Epsilon;
    
    inline fixed() {}
    inline fixed(const fixed& a) : raw(a.raw) {}
    inline explicit fixed(float a) : raw(int64((double)a * OneRaw)) {}
    inline explicit fixed(double a) : raw(int64(a * OneRaw)) {}
    
#define fixed_integer_const(T) inline explicit fixed(T a) : raw((int64)a << 16) {}           //나눗셈을 위해 상위 16비트를 비워둠
    integer_func(fixed_integer_const)
    
    inline fixed& operator =(const fixed& a) {
        raw = a.raw;
        return *this;
    }
    
#define fixed_asg(T) inline fixed& operator =(T a) { raw = fixed(a).raw; return *this; }
    frac_func(fixed_asg)
    integer_func(fixed_asg)
    
    inline operator float() const {
        return raw / (float)OneRaw;
    }
    inline operator double() const {
        return raw / (double)OneRaw;
    }
    inline operator bool() const {
        return raw != 0;
    }
    
#define fixed_integer_conv(T) inline operator T() const { return raw >> 16; }
    integer_func(fixed_integer_conv)
    
    inline fixed operator +() const {
        return fixed(Raw, raw);
    }
    inline fixed operator -() const {
        return fixed(Raw, -raw);
    }
    inline bool operator !() const {
        return raw == 0;
    }
    
    inline fixed operator +(const fixed& a) const {
        return fixed(Raw, raw + a.raw);
    }
    inline fixed operator -(const fixed& a) const {
        return fixed(Raw, raw - a.raw);
    }
    inline fixed operator *(const fixed& a) const {
        return fixed(Raw, (raw * a.raw) >> 16);
    }
    inline fixed operator /(const fixed& a) const {
        return fixed(Raw, (raw << 16) / a.raw);
    }
    
#define fixed_frac_add(T) inline T operator +(T a) const { return (T)*this + a; }
    frac_func(fixed_frac_add)
#define fixed_integer_add(T) inline fixed operator +(T a) const { return *this + fixed(a); }
    integer_func(fixed_integer_add)
    
#define fixed_frac_sub(T) inline T operator -(T a) const { return (T)*this - a; }
    frac_func(fixed_frac_sub)
#define fixed_integer_sub(T) inline fixed operator -(T a) const { return *this - fixed(a); }
    integer_func(fixed_integer_sub)
    
#define fixed_frac_mul(T) inline T operator *(T a) const { return (T)*this * a; }
    frac_func(fixed_frac_mul)
#define fixed_integer_mul(T) inline fixed operator *(T a) const { return fixed(fixed::Raw, raw * a); }
    integer_func(fixed_integer_mul)
    
#define fixed_frac_div(T) inline T operator /(T a) const { return (T)*this / a; }
    frac_func(fixed_frac_div)
#define fixed_integer_div(T) inline fixed operator /(T a) const { return fixed(fixed::Raw, raw / a); }
    integer_func(fixed_integer_div)
    
#define fixed_sl(T) inline fixed operator <<(T a) const { return fixed(fixed::Raw, raw << a); }
#define fixed_sr(T) inline fixed operator >>(T a) const { return fixed(fixed::Raw, raw >> a); }
    integer_func(fixed_sl)
    integer_func(fixed_sr)
    
    inline fixed& operator +=(const fixed& a) {
        return *this = *this + a;
    }
    inline fixed& operator -=(const fixed& a) {
        return *this = *this - a;
    }
    inline fixed& operator *=(const fixed& a) {
        return *this = *this * a;
    }
    inline fixed& operator /=(const fixed& a) {
        return *this = *this / a;
    }
    
#define fixed_add_asg(T) inline fixed& operator +=(T a) { return *this = *this + a; }
#define fixed_sub_asg(T) inline fixed& operator -=(T a) { return *this = *this - a; }
#define fixed_mul_asg(T) inline fixed& operator *=(T a) { return *this = *this * a; }
#define fixed_div_asg(T) inline fixed& operator /=(T a) { return *this = *this / a; }
    frac_func(fixed_add_asg)
    frac_func(fixed_sub_asg)
    frac_func(fixed_mul_asg)
    frac_func(fixed_div_asg)
    integer_func(fixed_add_asg)
    integer_func(fixed_sub_asg)
    integer_func(fixed_mul_asg)
    integer_func(fixed_div_asg)
    
#define fixed_sl_asg(T) inline fixed& operator <<=(T a) { return *this = *this << a; }
#define fixed_rl_asg(T) inline fixed& operator >>=(T a) { return *this = *this >> a; }
    integer_func(fixed_sl_asg)
    integer_func(fixed_rl_asg)
    
    inline bool operator ==(const fixed& a) const {
        return raw == a.raw;
    }
    inline bool operator !=(const fixed& a) const {
        return raw != a.raw;
    }
    inline bool operator <=(const fixed& a) const {
        return raw <= a.raw;
    }
    inline bool operator >=(const fixed& a) const {
        return raw >= a.raw;
    }
    inline bool operator <(const fixed& a) const {
        return raw  < a.raw;
    }
    inline bool operator >(const fixed& a) const {
        return raw  > a.raw;
    }
    
#define fixed_eq(T) inline bool operator ==(T a) const { return operator ==(fixed(a)); }
#define fixed_neq(T) inline bool operator !=(T a) const { return operator !=(fixed(a)); }
#define fixed_lseq(T) inline bool operator <=(T a) const { return operator <=(fixed(a)); }
#define fixed_gteq(T) inline bool operator >=(T a) const { return operator >=(fixed(a)); }
#define fixed_ls(T) inline bool operator <(T a) const { return operator <(fixed(a)); }
#define fixed_gt(T) inline bool operator >(T a) const { return operator >(fixed(a)); }
    
    frac_func(fixed_eq)
    integer_func(fixed_eq)
    frac_func(fixed_neq)
    integer_func(fixed_neq)
    frac_func(fixed_lseq)
    integer_func(fixed_lseq)
    frac_func(fixed_gteq)
    integer_func(fixed_gteq)
    frac_func(fixed_ls)
    integer_func(fixed_ls)
    frac_func(fixed_gt)
    integer_func(fixed_gt)
};

#define frac_fixed_add(T)   inline T operator +(T a, const fixed& b) { return a + (T)b; }
#define frac_fixed_sub(T)   inline T operator -(T a, const fixed& b) { return a - (T)b; }
#define frac_fixed_mul(T)   inline T operator *(T a, const fixed& b) { return a * (T)b; }
#define frac_fixed_div(T)   inline T operator /(T a, const fixed& b) { return a / (T)b; }
frac_func(frac_fixed_add)
frac_func(frac_fixed_sub)
frac_func(frac_fixed_mul)
frac_func(frac_fixed_div)

#define integer_fixed_add(T) inline fixed operator +(T a, const fixed& b) { return fixed(a) + b; }
#define integer_fixed_sub(T) inline fixed operator -(T a, const fixed& b) { return fixed(a) - b; }
#define integer_fixed_mul(T) inline fixed operator *(T a, const fixed& b) { return fixed(a) * b; }
#define integer_fixed_div(T) inline fixed operator /(T a, const fixed& b) { return fixed(a) / b; }
integer_func(integer_fixed_add)
integer_func(integer_fixed_sub)
integer_func(integer_fixed_mul)
integer_func(integer_fixed_div)

#define frac_fixed_add_asg(T) inline T& operator +=(T& a, const fixed& b) { return a = a + (T)b; }
#define frac_fixed_sub_asg(T) inline T& operator -=(T& a, const fixed& b) { return a = a - (T)b; }
#define frac_fixed_mul_asg(T) inline T& operator *=(T& a, const fixed& b) { return a = a * (T)b; }
#define frac_fixed_div_asg(T) inline T& operator /=(T& a, const fixed& b) { return a = a / (T)b; }
frac_func(frac_fixed_add_asg)
frac_func(frac_fixed_sub_asg)
frac_func(frac_fixed_mul_asg)
frac_func(frac_fixed_div_asg)

#define integer_fixed_add_asg(T) inline T& operator +=(T& a, const fixed& b) { return a = (T)(fixed(a) + b); }
#define integer_fixed_sub_asg(T) inline T& operator -=(T& a, const fixed& b) { return a = (T)(fixed(a) - b); }
#define integer_fixed_mul_asg(T) inline T& operator *=(T& a, const fixed& b) { return a = (T)(fixed(a) * b); }
#define integer_fixed_div_asg(T) inline T& operator /=(T& a, const fixed& b) { return a = (T)(fixed(a) / b); }
integer_func(integer_fixed_add_asg)
integer_func(integer_fixed_sub_asg)
integer_func(integer_fixed_mul_asg)
integer_func(integer_fixed_div_asg)

#endif
