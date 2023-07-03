//
//  CSLog.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSLog__
#define __CDK__CSLog__

#include "CSConfig.h"

#include <assert.h>
#include <stdarg.h>

const char* __FILENAME(const char* path);

#define __FILENAME__    __FILENAME(__FILE__)

#ifdef CS_CONSOLE_DEBUG

void __CSLog(const char* tag, const char* format, ...);
void __CSWarnLog(const char* tag, const char* format, ...);
void __CSErrorLog(const char* tag, const char* format, ...);
void __CSLogv(const char* tag, const char* format, va_list args);
void __CSWarnLogv(const char* tag, const char* format, va_list args);
void __CSErrorLogv(const char* tag, const char* format, va_list args);

# define CSLog(...)                 __CSLog(__FILENAME__, __VA_ARGS__)
# define CSWarnLog(...)             __CSWarnLog(__FILENAME__, __VA_ARGS__)
# define CSErrorLog(...)            __CSErrorLog(__FILENAME__, __VA_ARGS__)
# define CSLogv(format, args)       __CSLogv(__FILENAME__, format, args)
# define CSWarnLogv(format, args)   __CSwarnLogv(__FILENAME__, format, args)
# define CSErrorLogv(format, args)  __CSErrorLogv(__FILENAME__, format, args)

#else

# define CSLog(...);
# define CSWarnLog(...);
# define CSErrorLog(...);
# define CSLogv(format, args);
# define CSWarnLogv(format, args);
# define CSErrorLogv(format, args);

#endif

#ifdef CS_ASSERT_DEBUG
# ifdef CDK_ANDROID
#  include <android/log.h>
#  define CSAssert(cond, ...)   if (!(cond)) __android_log_assert(#cond, __FILENAME__, __VA_ARGS__)
# else
#  ifdef CS_CONSOLE_DEBUG
#   define CSAssert(cond, ...)   do { if (!(cond)) CSErrorLog(__VA_ARGS__);assert(cond); } while (0)
#  else
#   define CSAssert(cond, ...)   assert(cond)
#  endif
# endif
#else
# define CSAssert(cond, ...);
#endif

#endif /* defined(__CDK__CSLog__) */
