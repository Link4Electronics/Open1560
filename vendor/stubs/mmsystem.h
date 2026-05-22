#pragma once

#include <cstdint>

// Types UINT, DWORD, LONG, WORD, HWND come from minwin_linux.h (via Windows.h/core/minwin.h)

constexpr DWORD MMSYSERR_NOERROR = 0;
constexpr DWORD MMSYSERR_ERROR = 1;
constexpr DWORD MMSYSERR_BADDEVICEID = 2;
constexpr DWORD MMSYSERR_NOTENABLED = 3;
constexpr DWORD MMSYSERR_ALLOCATED = 4;
constexpr DWORD MMSYSERR_INVALHANDLE = 5;
constexpr DWORD MMSYSERR_NODRIVER = 6;
constexpr DWORD MMSYSERR_NOMEM = 7;
constexpr DWORD MMSYSERR_NOTSUPPORTED = 8;
constexpr DWORD MMSYSERR_BADERRNUM = 9;
constexpr DWORD MMSYSERR_INVALFLAG = 10;
constexpr DWORD MMSYSERR_INVALPARAM = 11;
constexpr DWORD MMSYSERR_HANDLEBUSY = 12;
constexpr DWORD MMSYSERR_INVALIDALIAS = 13;
constexpr DWORD MMSYSERR_BADDB = 14;
constexpr DWORD MMSYSERR_KEYNOTFOUND = 15;
constexpr DWORD MMSYSERR_READERROR = 16;
constexpr DWORD MMSYSERR_WRITEERROR = 17;
constexpr DWORD MMSYSERR_DELETEERROR = 18;
constexpr DWORD MMSYSERR_VALNOTFOUND = 19;
constexpr DWORD MMSYSERR_NODRIVERCB = 20;
constexpr DWORD MMSYSERR_MOREDATA = 21;

constexpr DWORD MIXERR_INVALLINE = 1024;
constexpr DWORD MIXERR_INVALCONTROL = 1025;
constexpr DWORD MIXERR_INVALVALUE = 1026;

constexpr UINT MM_WOM_OPEN = 0x3BB;
constexpr UINT MM_MCINOTIFY = 0x3B8;

struct MIXERCAPSA {
    WORD wMid;
    WORD wPid;
    UINT vDriverVersion;
    char szPname[32];
    DWORD fdwSupport;
    DWORD cDestinations;
};

inline UINT mixerGetNumDevs() { return 0; }
inline DWORD mixerGetDevCapsA(UINT, MIXERCAPSA*, DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerOpen(DWORD*, UINT, DWORD, DWORD, DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerClose(DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerGetLineInfoA(DWORD, void*, DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerGetLineControlsA(DWORD, void*, DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerGetControlDetailsA(DWORD, void*, DWORD) { return MMSYSERR_NODRIVER; }
inline DWORD mixerSetControlDetailsA(DWORD, void*, DWORD) { return MMSYSERR_NODRIVER; }
