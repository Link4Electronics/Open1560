#pragma once

// Minimal stub for guiddef.h
// GUID is defined in minwin_linux.h - only define if not already present

#ifndef GUID_DEFINED
#define GUID_DEFINED

struct GUID {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];

    bool operator==(const GUID& other) const {
        return Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3 &&
               memcmp(Data4, other.Data4, sizeof(Data4)) == 0;
    }
};

using REFGUID = const GUID&;
using REFIID = const GUID&;

inline int IsEqualGUID(REFGUID lhs, REFGUID rhs) {
    return lhs == rhs;
}

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    constexpr GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#endif
