//
//  CSMath.h
//  CDK
//
//  Created by chan on 13. 4. 10..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSMath__
#define __CDK__CSMath__

#include "CSFixed.h"

#include "CSLog.h"

#include <math.h>

class CSMath {
public:
    template <typename T>
    static inline T min(T a, T b) {
        return a < b ? a : b;
    }
    template <typename T>
    static inline T max(T a, T b) {
        return a > b ? a : b;
    }
    template <typename T>
    static inline T clamp(T a, T b, T c) {
        return min(max(a, b), c);
    }
    
    static inline float frac(float x) {
        return x -::floorf(x);
    }
    static inline double frac(double x) {
        return x -::floor(x);
    }
    static inline fixed frac(fixed x) {
        x.raw &= fixed::FracRaw;
        return x;
    }
    static inline float exp(float x) {
        return ::expf(x);
    }
    static inline double exp(double x) {
        return ::exp(x);
    }
    static fixed exp(fixed x);
    
    static inline float log(float x) {
        return ::logf(x);
    }
    static inline double log(double x) {
        return ::log(x);
    }
    static fixed log(fixed x);
    
    static inline int pow(int x, int y) {
        if (y < 0) return 0;
        if (y == 0) return 1;
        int rtn = x;
        while (--y) {
            rtn *= x;
        }
        return rtn;
    }
    static inline float pow(float x, float y) {
        return ::powf(x, y);
    }
    static inline double pow(double x, double y) {
        return ::pow(x, y);
    }
    static inline fixed pow(fixed x, fixed y) {
        if (y == fixed::Zero)
            return fixed::One;
        if (x < fixed::Zero)
            return fixed::Zero;
        return exp(log(x) * y);
    }
    static unsigned int sqrt(unsigned int x);
    static inline float sqrt(float x) {
        return ::sqrtf(x);
    }
    static inline double sqrt(double x) {
        return ::sqrt(x);
    }
    static fixed sqrt(fixed x);
    
    static inline int abs(int v) {
        return ::abs(v);
    }
    static inline float abs(float v) {
        return fabsf(v);
    }
    static inline double abs(double v) {
        return fabs(v);
    }
    static inline fixed abs(fixed x) {
        int64 mask = x.raw >> 63;
        x.raw = (x.raw + mask) ^ mask;
        return x;
    }
    
    static inline float mod(float x, float y) {
        return ::fmodf(x, y);
    }
    static inline double mod(double x, double y) {
        return ::fmod(x, y);
    }
    static inline fixed mod(fixed x, fixed y) {
        return fixed(fixed::Raw, x.raw % y.raw);
    }
    
    
    static inline float ceil(float x) {
        return ::ceilf(x);
    }
    static inline double ceil(double x) {
        return ::ceil(x);
    }
    static inline fixed ceil(fixed x) {
        if (x.raw & fixed::FracRaw) {
            x.raw += fixed::OneRaw;
            x.raw &= ~fixed::FracRaw;
        }
        return x;
    }
    
    static inline float round(float x) {
        return ::roundf(x);
    }
    static inline double round(double x) {
        return ::round(x);
    }
    static inline fixed round(fixed x) {
        x.raw = (x.raw + fixed::Half.raw) & ~(fixed::FracRaw);
        return x;
    }
    
    static inline float floor(float x) {
        return ::floorf(x);
    }
    static inline double floor(double x) {
        return ::floor(x);
    }
    static inline fixed floor(fixed x) {
        x.raw &= ~(fixed::FracRaw);
        return x;
    }
    
    static inline double degreesToRadians(double x) {
        return x * DoublePi / 180.0;
    }
    static inline double radiansToDegrees(double x) {
        return x * 180.0 / DoublePi;
    }
    static inline float degreesToRadians(float x) {
        return x * FloatPi / 180.0f;
    }
    static inline float radiansToDegrees(float x) {
        return x * 180.0f / FloatPi;
    }
    static inline fixed degreesToRadians(fixed x) {
        x *= fixed::Pi;
        x.raw /= 180;
        return x;
    }
    static inline fixed radiansToDegrees(fixed x) {
        x.raw *= 180;
        x /= fixed::Pi;
        return x;
    }
    
    static inline float sin(float x) {
        return ::sinf(x);
    }
    static inline double sin(double x) {
        return ::sin(x);
    }
    static fixed sin(fixed x);
    
    static inline float cos(float x) {
        return ::cosf(x);
    }
    static inline double cos(double x) {
        return ::cos(x);
    }
    static inline fixed cos(fixed x) {
        return sin(x + fixed::PiOverTwo);
    }
    
    static inline float tan(float x) {
        return ::tanf(x);
    }
    static inline double tan(double x) {
        return ::tan(x);
    }
    static inline fixed tan(fixed x) {
        fixed c = cos(x);
        CSAssert(c, "invalid operation");
        return sin(x) / c;
    }
    
    static inline float asin(float x) {
        return ::asinf(x);
    }
    static inline double asin(double x) {
        return ::asin(x);
    }
    static fixed asin(fixed x);
    
    static inline float acos(float x) {
        return ::acosf(x);
    }
    static inline double acos(double x) {
        return ::acos(x);
    }
    static inline fixed acos(fixed x) {
        return (fixed::PiOverTwo - asin(x));
    }
    
    static inline float atan(float x) {
        return ::atanf(x);
    }
    static inline double atan(double x) {
        return ::atan(x);
    }
    static inline fixed atan(fixed x) {
        return atan2(x, fixed::One);
    }
    
    static inline float atan2(float y, float x) {
        return ::atan2f(y, x);
    }
    static inline double atan2(double y, float x) {
        return ::atan2(y, x);
    }
    static fixed atan2(fixed y, fixed x);
    
    template <typename T>
    static inline T shift(T v, int s) {
        return (v >> s) | (v << (sizeof(T) * 8 - s));
    }
    
    static inline float smoothStep(float a) {
        return a <= 0 ? 0 : (a >= 1 ? 1 : a * a * (3 - (2 * a)));
    }
    static inline float lerp(float a, float b, float c) {
        return c <= 0 ? a : (c >= 1 ? b : a * (1 - c) + b * c);
    }
    static inline double lerp(double a, double b, double c) {
        return c <= 0 ? a : (c >= 1 ? b : a * (1 - c) + b * c);
    }
    static inline fixed lerp(fixed a, fixed b, fixed c) {
        return c <= fixed::Zero ? a : (c >= fixed::One ? b : a * (fixed::One - c) + b * c);
    }
    
    static inline int sign(float x) {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }
    
    static inline int sign(fixed x) {
        if (x > fixed::Zero) return 1;
        else if (x < fixed::Zero) return -1;
        return 0;
    }
    
    static inline int sign(int x) {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }
};

#endif /* defined(__CDK__CSMath__) */
