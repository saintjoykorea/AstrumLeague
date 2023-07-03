//
//  CSMath.cpp
//  CDK
//
//  Created by chan on 13. 4. 10..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMath.h"

#include "CSLog.h"

#include <limits>

static const fixed LN2 = fixed(0.69314718055994530942);
static const fixed LN2_INV = fixed(1.4426950408889634074);
static const fixed EXP_P[5] = {
    fixed(1.66666666666666019037e-01),
    fixed(-2.77777777770155933842e-03),
    fixed(6.61375632143793436117e-05),
    fixed(-1.65339022054652515390e-06),
    fixed(4.13813679705723846039e-08),
};
static const fixed LG[7] = {
    fixed(6.666666666666735130e-01),
    fixed(3.999999999940941908e-01),
    fixed(2.857142874366239149e-01),
    fixed(2.222219843214978396e-01),
    fixed(1.818357216161805012e-01),
    fixed(1.531383769920937332e-01),
    fixed(1.479819860511658591e-01)
};

fixed CSMath::exp(fixed x) {
    if (x == fixed::Zero) return fixed::One;
    
    fixed k = abs(x) * LN2_INV;
    k += fixed::Half;
    k.raw &= ~fixed::FracRaw;
    if (x < fixed::Zero) k = -k;
    x -= k * LN2;
    
    fixed z = x * x;
    
    fixed r = fixed(2) + (z * (EXP_P[0] + (z * (EXP_P[1] + (z * (EXP_P[2] + (z * (EXP_P[3] + (z * EXP_P[4])))))))));
    
    fixed xp = fixed::One + ((x * 2) / (r - x));
    
    if (k < 0)
        k = fixed::One >> (-k.raw >> 16);
    else
        k = fixed::One << (k.raw >> 16);
    return (k * xp);
}

fixed CSMath::log(fixed x) {
    if (x < fixed::Zero) return fixed::Zero;
    if (x == fixed::Zero) return -fixed::One;
    int log2 = 0;
    fixed xi = x;
    while (xi > fixed(2)) {
        xi >>= 1;
        log2++;
    }
    fixed f = xi - fixed::One;
    fixed s = f / (fixed(2) + f);
    fixed z = s * s;
    fixed w = z * z;
    
    fixed r = (w * (LG[1] + (w * (LG[3] + (w * LG[5]))))) + (z * (LG[0] + (w * (LG[2] + (w * LG[4] + (w * LG[6]))))));
    
    return (LN2 * log2) + f - (s * (f - r));
}

unsigned int CSMath::sqrt(unsigned int val) {
    unsigned int temp, g = 0;
    
    if (val >= 0x40000000) {
        g = 0x8000;
        val -= 0x40000000;
    }
#define INNER_SQRT(s)	\
	temp = (g << (s)) + (1 << (((s) << 1) - 2));  \
	if (temp <= val)	   \
	{	\
		g += 1 << ((s) - 1);  \
		val -= temp;	  \
	}
    
    INNER_SQRT(15)
    INNER_SQRT(14)
    INNER_SQRT(13)
    INNER_SQRT(12)
    INNER_SQRT(11)
    INNER_SQRT(10)
    INNER_SQRT(9)
    INNER_SQRT(8)
    INNER_SQRT(7)
    INNER_SQRT(6)
    INNER_SQRT(5)
    INNER_SQRT(4)
    INNER_SQRT(3)
    INNER_SQRT(2)
    
#undef INNER_SQRT
    
    temp = g + g + 1;
    if (temp <= val) {
        g++;
    }
    return g;
}

fixed CSMath::sqrt(fixed x) {
    if (x <= fixed::Zero) {
        return fixed::Zero;
    }
    uint64 num = x.raw << 16;
    uint64 res = 0;
    uint64 bit = (uint64)1 << 62;
    
    while (bit > num) {
        bit >>= 2;
    }
    while (bit) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return fixed(fixed::Raw, res);
}

#if defined(CS_FIXED_SIN_LUT)
#include "CSFixedSinLut.h"
#elif defined(CS_FIXED_CACHE)
static fixed _fixed_sin_cache_index[4096]  = { fixed::Zero };
static fixed _fixed_sin_cache_value[4096]  = { fixed::Zero };
#endif

#ifdef CS_FIXED_CACHE
static fixed _fixed_atan_cache_index[2][4096] = { { fixed::Zero }, { fixed::Zero } };
static fixed _fixed_atan_cache_value[4096] = { fixed::Zero };
#endif

fixed CSMath::sin(fixed x) {
    fixed a = mod(x, fixed::TwoPi);
    
    fixed out;
#ifdef CS_FIXED_SIN_LUT
    if (a < fixed::Zero)
        a += fixed::TwoPi;
        
    if (a >= fixed::Pi) {
        a -= fixed::Pi;
        if (a >= fixed::PiOverTwo)
            a = fixed::Pi - a;
        out.raw = -(a.raw >= _fixed_sin_lut_count ? fixed::OneRaw : _fixed_sin_lut[a.raw]);
    } else {
        if (a >= fixed::PiOverTwo)
            a = fixed::Pi - a;
        out.raw = (a.raw >= _fixed_sin_lut_count ? fixed::OneRaw : _fixed_sin_lut[a.raw]);
    }
#else
    if (a > fixed::Pi)
        a -= fixed::TwoPi;
    else if (a < -fixed::Pi)
        a += fixed::TwoPi;
    
#ifdef CS_FIXED_CACHE
    int cacheIndex = ((x.raw >> 5) & 0x00000FFF);
    if (_fixed_sin_cache_index[cacheIndex] == x.raw)
        return _fixed_sin_cache_value[cacheIndex];
#endif
    
    fixed a2 = a * a;
    
#ifndef CS_FIXED_FAST_SIN // Most accurate version, accurate to ~2.1%
    out = a;
    a *= a2;
    out.raw -= (a.raw / 6);
    a *= a2;
    out.raw += (a.raw / 120);
    a *= a2;
    out.raw -= (a.raw / 5040);
    a *= a2;
    out.raw += (a.raw / 362880);
    a *= a2;
    out.raw -= (a.raw / 39916800);
#else // Fast implementation, runs at 159% the speed of above 'accurate' version with an slightly lower accuracy of ~2.3%
    out.raw = (fixed(fixed::Raw, -13) * a2).raw + 546;
    out.raw = (out * a2).raw - 10923;
    out.raw = (out * a2).raw + 65536;
    out *= a;
#endif
    
#ifdef CS_FIXED_CACHE
    _fixed_sin_cache_index[cacheIndex] = x.raw;
    _fixed_sin_cache_value[cacheIndex] = out;
#endif
#endif
    return out;
}
fixed CSMath::asin(fixed x) {
    if (x >= fixed::One)
        return fixed::PiOverTwo;
    if (x <= -fixed::One)
        return -fixed::PiOverTwo;
        
    fixed out;
    out = (fixed::One - x * x);
    out = x / sqrt(out);
    out = atan(out);
    return out;
}
fixed CSMath::atan2(fixed y, fixed x) {
    if (!x && !y) {
        return fixed::Zero;
    }
#ifdef CS_FIXED_CACHE
    uint hash = (x.raw ^ y.raw);
    hash ^= hash >> 20;
    hash &= 0x0FFF;
    if ((_fixed_atan_cache_index[0][hash] == x.raw) && (_fixed_atan_cache_index[1][hash] == y.raw))
        return _fixed_atan_cache_value[hash];
#endif
        
    fixed angle;
    
    fixed absy = abs(y);
    
    if (x >= fixed::Zero) {
        fixed r = (x - absy) / (x + absy);
        angle = (fixed(fixed::Raw, 0x00003240) * r * r * r) - (fixed(fixed::Raw, 0x0000FB50) * r) + fixed::PiOverFour;
    }
    else {
        fixed r = (x + absy) / (absy - x);
        angle = (fixed(fixed::Raw, 0x00003240) * r * r * r) - (fixed(fixed::Raw, 0x0000FB50) * r) + fixed::PiOverFour * 3;
    }
    if (y < fixed::Zero) {
        angle = -angle;
    }
    
#ifdef CS_FIXED_CACHE
    _fixed_atan_cache_index[0][hash] = x;
    _fixed_atan_cache_index[1][hash] = y;
    _fixed_atan_cache_value[hash] = angle;
#endif
    
    return angle;
}
