#pragma once

// Minimal MCI API stubs for Linux
// All types come from the including headers (Windows.h / minwin_linux.h)

using MCIERROR = DWORD;
using MCIDEVICEID = UINT;

struct MCI_OPEN_PARMSA {
    DWORD dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR lpstrDeviceType;
    LPCSTR lpstrElementName;
    LPCSTR lpstrAlias;
};

struct MCI_STATUS_PARMS {
    DWORD dwCallback;
    DWORD dwReturn;
    DWORD dwItem;
    DWORD dwTrack;
};

struct MCI_SET_PARMS {
    DWORD dwCallback;
    DWORD dwTimeFormat;
    DWORD dwAudio;
};

struct MCI_PLAY_PARMS {
    DWORD dwCallback;
    DWORD dwFrom;
    DWORD dwTo;
};

struct MCI_SEEK_PARMS {
    DWORD dwCallback;
    DWORD dwTo;
};

struct MCI_STOP_PARMS {
    DWORD dwCallback;
};

// MCI Error codes (ERROR_SUCCESS is from minwin_linux.h)

// MCI Messages
#define MCI_OPEN        0x0803
#define MCI_CLOSE       0x0804
#define MCI_PLAY        0x0806
#define MCI_SEEK        0x0807
#define MCI_STOP        0x0808
#define MCI_STATUS      0x080A
#define MCI_SET         0x080B

// Status items
#define MCI_STATUS_ITEM         0x00000100
#define MCI_STATUS_MEDIA_PRESENT 0x00001000
#define MCI_STATUS_LENGTH       0x00000001
#define MCI_STATUS_POSITION     0x00000002
#define MCI_STATUS_MODE         0x00000004
#define MCI_STATUS_READY        0x00000008
#define MCI_STATUS_NUMBER_OF_TRACKS 0x00000010

// MCI Mode returns
#define MCI_MODE_NOT_READY  0x00000400
#define MCI_MODE_STOP       0x00000401
#define MCI_MODE_PLAY       0x00000402
#define MCI_MODE_OPEN       0x00000403

// Flags
#define MCI_OPEN_SHAREABLE     0x00000100
#define MCI_OPEN_ELEMENT       0x00000200
#define MCI_OPEN_TYPE          0x00002000
#define MCI_NOTIFY             0x00000001
#define MCI_WAIT               0x00000002
#define MCI_FROM               0x00000004
#define MCI_TO                 0x00000008
#define MCI_SET_TIME_FORMAT    0x00000400
#define MCI_STATUS_TRACK       0x00000040

// Time formats
#define MCI_FORMAT_TMSF        0x00000008

// Device types
#define MCI_DEVTYPE_CD_AUDIO   0x00000002
#define MCI_OPEN_TYPE_ID       0x00001000
#define MCI_NOTIFY_SUCCESSFUL  0x0001
#define MAKE_MCIINT(a, b)      ((DWORD)(((a) << 8) | (b)))

// Notify message
#define MM_MCINOTIFY           0x03B9

#define MCI_MAKE_TMSF(t, m, s, f) ((DWORD)(((BYTE)(t) | ((WORD)(m) << 8) | ((DWORD)(s) << 16) | ((DWORD)(f) << 24))))
#define MCI_TMSF_TRACK(d) ((BYTE)(d))
#define MCI_TMSF_MINUTE(d) ((BYTE)(((WORD)(d)) >> 8))
#define MCI_TMSF_SECOND(d) ((BYTE)((d) >> 16))
#define MCI_TMSF_FRAME(d) ((BYTE)((d) >> 24))

inline MCIERROR mciSendCommandA(UINT, UINT, DWORD, DWORD_PTR) { return ERROR_SUCCESS; }
inline MCIERROR mciSendStringA(LPCSTR, LPSTR, UINT, HWND) { return ERROR_SUCCESS; }
