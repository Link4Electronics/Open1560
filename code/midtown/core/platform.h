#pragma once

#include "minwin.h"

#include <cstdlib>

#if defined(_MSC_VER)
#    define ArDebugBreak() __debugbreak()
#else
#    define ArDebugBreak() ::raise(SIGTRAP)
#endif

#if defined(_MSC_VER)
#    if 1
#        define ArAbort() __fastfail(FAST_FAIL_FATAL_APP_EXIT)
#    else
#        define ArAbort() ((IsDebuggerPresent() && (ArDebugBreak(), 0)), std::abort())
#    endif
#elif defined(__has_builtin)
#    if __has_builtin(__builtin_trap)
#        define ArAbort() __builtin_trap()
#    else
#        define ArAbort() (ArDebugBreak(), std::abort())
#    endif
#else
#    define ArAbort() (ArDebugBreak(), std::abort())
#endif
