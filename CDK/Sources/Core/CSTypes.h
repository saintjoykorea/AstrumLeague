//
//  CSTypes.h
//  CDK
//
//  Created by 김찬 on 12. 8. 1..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSTypes__
#define __CDK__CSTypes__

#include "CSConfig.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>

#ifndef byte
typedef unsigned char byte;
#endif
typedef signed char sbyte;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef int64_t int64;
typedef uint64_t uint64;
typedef UChar uchar;

#ifndef NULL
# define NULL    0
#endif

extern const float FloatPi;
extern const float FloatPiOverTwo;
extern const float FloatPiOverFour;
extern const float FloatTwoPi;
extern const float FloatEpsilon;
extern const float FloatMin;
extern const float FloatMax;

extern const double DoublePi;
extern const double DoublePiOverTwo;
extern const double DoublePiOverFour;
extern const double DoubleTwoPi;
extern const double DoubleEpsilon;
extern const double DoubleMin;
extern const double DoubleMax;

#endif
