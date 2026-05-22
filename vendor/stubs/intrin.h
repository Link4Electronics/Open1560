#pragma once

// Minimal intrin.h stub for GCC/Clang
// Provide MSVC-compatible __cpuid, __rdtsc, _mm_empty etc.

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
// GCC provides __rdtsc and _mm_empty via x86intrin.h
// Note: x86intrin.h may include cpuid.h which defines __cpuid as a 5-arg macro.
// We undefine it here and provide our own MSVC-compatible 2-arg version.
#    include <x86intrin.h>
#    ifdef __cpuid
#        undef __cpuid
#    endif
static inline void __cpuid(int cpuInfo[4], int function_id) {
    unsigned int a = function_id, b = 0, c = 0, d = 0;
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(a), "c"(c) :);
    cpuInfo[0] = (int)a;
    cpuInfo[1] = (int)b;
    cpuInfo[2] = (int)c;
    cpuInfo[3] = (int)d;
}
#elif defined(__GNUC__) && !defined(__x86_64__) && !defined(__i386__)
// Non-x86 GCC: provide stubs
static inline void __cpuid(int[4], int) {}
static inline unsigned long long __rdtsc() { return 0; }
#else
// MSVC/Clang-cl or unknown
static inline void __cpuid(int[4], int) {}
static inline unsigned long long __rdtsc() { return 0; }
#endif
