//
//  CSConfig.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSConfig__
#define __CDK__CSConfig__

#define CS_OES_DEBUG
#define CS_ASSERT_DEBUG
#define CS_CONSOLE_DEBUG
//#define CS_SECRET_THREAD_SAFE
//#define CS_DISPLAY_LAYER_FRAME
#define CS_DISPLAY_COUNTER
#define CS_DIAGNOSTICS
#define CS_LOCK_TIME

#define CS_FIXED_SIN_LUT
//#define CS_FIXED_FAST_SIN
#define CS_FIXED_CACHE

#define CS_IOS_CUSTOM_MEMORY_WARNING
//#define CS_IOS_OES_RELOAD

//================================================
#ifndef DEBUG

# undef CS_OES_DEBUG
# undef CS_ASSERT_DEBUG
# undef CS_CONSOLE_DEBUG
# undef CS_DISPLAY_LAYER_FRAME
# undef CS_DISPLAY_COUNTER
# undef CS_DIAGNOSTICS
# undef CS_LOCK_TIME

# undef CS_IOS_OES_RELOAD

#else

# ifdef CS_OES_DEBUG
#  ifndef CS_CONSOLE_DEBUG
#    pragma message("CS_OES_DEBUG runs with CS_CONSOLE_DEBUG.");
#  endif
#  ifndef CS_ASSERT_DEBUG
#    pragma message("CS_OES_DEBUG runs with CS_ASSERT_DEBUG.");
#  endif
# endif

#endif

//================================================

#endif
