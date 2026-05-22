#pragma once

#if defined(_WIN32)
#    define VC_EXTRALEAN
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <Windows.h>
#    undef GetClassName
#else
// On Linux, our custom minwin_linux.h provides all needed types
// (included from vendor/stubs/Windows.h which replaces the system Windows.h)
#    include <Windows.h>
#endif
