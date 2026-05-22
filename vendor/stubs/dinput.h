#pragma once

// Minimal stub for DirectInput types used by Open1560

#include <cstdint>
#include <cstddef>

#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0500

using REFGUID = const GUID&;
using REFIID = const GUID&;
using DWORD_PTR = uintptr_t;
using CHAR = char;
using LPBYTE = BYTE*;

// Forward declarations for structs and COM interfaces
struct DIDEVCAPS;
struct DIPROPHEADER;
struct DIDEVICEOBJECTDATA;
struct DIDEVICEINSTANCEA;
struct DIEFFECT;
struct DIENVELOPE;
struct DIDATAFORMAT;
struct IDirectInputEffect;
struct IDirectInputDeviceA;
struct IDirectInputDevice2A;
struct IDirectInputA;
struct IDirectInput2A;

// Pointer typedefs
using LPGUID = GUID*;
using LPCGUID = const GUID*;
using LPDWORD = DWORD*;
using LPUNKNOWN = void*;
using LPDIDEVCAPS = DIDEVCAPS*;
using LPDIPROPHEADER = DIPROPHEADER*;
using LPCDIPROPHEADER = const DIPROPHEADER*;
using LPDIDEVICEOBJECTDATA = DIDEVICEOBJECTDATA*;
using LPCDIDEVICEOBJECTDATA = const DIDEVICEOBJECTDATA*;
using LPCDIDATAFORMAT = const DIDATAFORMAT*;
using LPDIDEVICEINSTANCEA = DIDEVICEINSTANCEA*;
using LPCDIDEVICEINSTANCEA = const DIDEVICEINSTANCEA*;
using LPCDIEFFECT = const DIEFFECT*;
using LPDIRECTINPUTEFFECT = IDirectInputEffect*;
using LPDIRECTINPUTDEVICEA = IDirectInputDeviceA*;
using LPDIRECTINPUTDEVICE2A = IDirectInputDevice2A*;
using LPDIEFFECT = DIEFFECT*;
using LPDIDATAFORMAT = DIDATAFORMAT*;
using LPDIENVELOPE = DIENVELOPE*;
using LPCDIENVELOPE = const DIENVELOPE*;
using LPDIRECTINPUTA = IDirectInputA*;
using LPDIRECTINPUT2A = IDirectInput2A*;

// Constants
constexpr HRESULT DI_OK = 0;
constexpr HRESULT DIERR_OLDDIRECTINPUTVERSION = -2147023726;
constexpr HRESULT DIERR_BETADIRECTINPUTVERSION = -2147023725;
constexpr HRESULT DIERR_BADDRIVERVER = -2147023724;
constexpr HRESULT DIERR_DEVICENOTREG = -2147023723;
constexpr HRESULT DIERR_NOTFOUND = -2147023722;
constexpr HRESULT DIERR_OBJECTNOTFOUND = -2147023721;
constexpr HRESULT DIERR_INVALIDPARAM = -2147023720;
constexpr HRESULT DIERR_NOINTERFACE = -2147023719;
constexpr HRESULT DIERR_GENERIC = -2147023718;
constexpr HRESULT DIERR_OUTOFMEMORY = -2147023717;
constexpr HRESULT DIERR_UNSUPPORTED = -2147023716;
constexpr HRESULT DIERR_NOTINITIALIZED = -2147023715;
constexpr HRESULT DIERR_ALREADYINITIALIZED = -2147023714;
constexpr HRESULT DIERR_NOAGGREGATION = -2147023713;
constexpr HRESULT DIERR_OTHERAPPHASPRIO = -2147023712;
constexpr HRESULT DIERR_INPUTLOST = -2147023711;
constexpr HRESULT DIERR_ACQUIRED = -2147023710;
constexpr HRESULT DIERR_NOTACQUIRED = -2147023709;
constexpr HRESULT DIERR_NOTBUFFERED = -2147023708;
constexpr HRESULT DIERR_EFFECTPLAYING = -2147023707;
constexpr HRESULT DIERR_UNPLUGGED = -2147023706;
constexpr HRESULT E_NOTIMPL = 0x80004001;
constexpr HRESULT E_FAIL = 0x80004005;
constexpr HRESULT S_OK = 0;
constexpr HRESULT S_FALSE = 1;

constexpr DWORD DIENUM_CONTINUE = 0;
constexpr DWORD DIENUM_STOP = 1;
constexpr DWORD DIEDFL_ATTACHEDONLY = 1;
constexpr DWORD DIDEVTYPE_JOYSTICK = 0x00000010;
constexpr DWORD DIDEVTYPE_HID = 0x00000100;
constexpr DWORD DISCL_EXCLUSIVE = 1;
constexpr DWORD DISCL_NONEXCLUSIVE = 2;
constexpr DWORD DISCL_FOREGROUND = 4;
constexpr DWORD DISCL_BACKGROUND = 8;
constexpr DWORD DISCL_NOWINKEY = 16;

constexpr DWORD DIDFT_ABSOLUTEAXIS = 1;
constexpr DWORD DIDFT_BUTTON = 0x0000000C;
constexpr DWORD DIDFT_PSHBUTTON = 0x0000000E;
constexpr DWORD DIDFT_TGLBUTTON = 0x0000000C;
constexpr DWORD DIDFT_POV = 0x00000010;
constexpr DWORD DIDFT_ANYINSTANCE = 0x00FFFF00;

constexpr DWORD DIGDD_PEEK = 1;
constexpr DWORD DIEP_DURATION = 1;
constexpr DWORD DIEP_SAMPLEPERIOD = 2;
constexpr DWORD DIEP_GAIN = 4;
constexpr DWORD DIEP_TRIGGERBUTTON = 8;
constexpr DWORD DIEP_TRIGGERREPEATINTERVAL = 16;
constexpr DWORD DIEP_AXES = 32;
constexpr DWORD DIEP_DIRECTION = 64;
constexpr DWORD DIEP_ENVELOPE = 128;
constexpr DWORD DIEP_TYPESPECIFICPARAMS = 256;
constexpr DWORD DIEP_START = 512;
constexpr DWORD DIEP_NORESTART = 1024;
constexpr DWORD DIEP_NODOWNLOAD = 2048;
constexpr DWORD DIEP_ALLPARAMS = 4095;

constexpr DWORD DIEFF_CARTESIAN = 0x00000010;
constexpr DWORD DIEFF_POLAR = 0x00000020;
constexpr DWORD DIEFF_SPHERICAL = 0x00000040;
constexpr DWORD DIEFF_OBJECTIDS = 0x00000080;
constexpr DWORD DIEFF_OBJECTOFFSETS = 0x00000100;
constexpr DWORD DIEFF_OFFSET = 0x00000200;

constexpr DWORD DIDOI_FFACTUATOR = 1;
constexpr DWORD DIDOI_POLLED = 2;

constexpr DWORD DISFFC_RESET = 1;
constexpr DWORD DISFFC_STOPALL = 2;
constexpr DWORD DISFFC_PAUSE = 4;
constexpr DWORD DISFFC_CONTINUE = 8;
constexpr DWORD DISFFC_SETACTUATORSON = 16;
constexpr DWORD DISFFC_SETACTUATORSOFF = 32;

constexpr DWORD DIPROPRANGE_NOMAX = 1;
constexpr DWORD DIPROP_DEADZONE = 2;
constexpr DWORD DIPROP_SATURATION = 3;
constexpr DWORD DIPROP_RANGE = 4;
constexpr DWORD DIPROP_AXISMODE = 5;
constexpr DWORD DIPROP_BUFFERSIZE = 6;
constexpr DWORD DIPROP_FFGAIN = 7;
constexpr DWORD DIPROP_XX = 8;
constexpr DWORD DIPROP_CALIBRATIONMODE = 9;
constexpr LONG DIPROPRANGE_NOMIN = 0x80020000;

constexpr DWORD DIPH_DEVICE = 0;
constexpr DWORD DIPH_BYOFFSET = 1;
constexpr DWORD DIPH_BYID = 2;
constexpr DWORD DIPH_BYUSAGE = 3;

// Structures
struct DIPROPHEADER {
    DWORD dwSize;
    DWORD dwHeaderSize;
    DWORD dwObj;
    DWORD dwHow;
};

struct DIPROPDWORD {
    DIPROPHEADER diph;
    DWORD dwData;
};

struct DIPROPRANGE {
    DIPROPHEADER diph;
    LONG lMin;
    LONG lMax;
};

struct DIDEVCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwDevType;
    DWORD dwAxes;
    DWORD dwButtons;
    DWORD dwPOVs;
    DWORD dwFFSamplePeriod;
    DWORD dwFFMinTimeResolution;
    DWORD dwFirmwareRevision;
    DWORD dwHardwareRevision;
    DWORD dwDriverVersion;
};

struct DIDEVICEINSTANCEA {
    DWORD dwSize;
    GUID guidInstance;
    GUID guidProduct;
    DWORD dwDevType;
    CHAR tszInstanceName[260];
    CHAR tszProductName[260];
    GUID guidFFDriver;
    WORD wUsagePage;
    WORD wUsage;
};

struct DIDEVICEOBJECTDATA {
    DWORD dwOfs;
    DWORD dwData;
    DWORD dwTimeStamp;
    DWORD dwSequence;
};

struct DIJOYSTATE {
    LONG lX;
    LONG lY;
    LONG lZ;
    LONG lRx;
    LONG lRy;
    LONG lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[32];
    BYTE rgbVendorSpecifc[1];
};

// Additional DirectInput types for force feedback
struct DIPERIODIC {
    DWORD dwMagnitude;
    LONG lOffset;
    DWORD dwPhase;
    DWORD dwPeriod;
};

struct DICONDITION {
    LONG lOffset;
    DWORD dwPositiveCoefficient;
    DWORD dwNegativeCoefficient;
    DWORD dwPositiveSaturation;
    DWORD dwNegativeSaturation;
    LONG lDeadBand;
};

struct DIEFFECTINFOA {
    DWORD dwSize;
    GUID guid;
    DWORD dwEffType;
    DWORD dwStaticParams;
    DWORD dwDynamicParams;
    CHAR tszName[260];
};

using LPCDIEFFECTINFOA = const DIEFFECTINFOA*;
using LPDIEFFECTINFOA = DIEFFECTINFOA*;
using LPDIENUMEFFECTSCALLBACKA = BOOL (*)(LPCDIEFFECTINFOA, LPVOID);
using LPDIENUMDEVICESCALLBACKA = BOOL (*)(LPCDIDEVICEINSTANCEA, LPVOID);

constexpr LONG DI_FFNOMINALMAX = 10000;
constexpr DWORD DI_SECONDS = 0x00000001;  // 1 microsecond unit
constexpr DWORD DI_DEGREES = 100;         // 100 = 1 degree
constexpr DWORD DIEB_NOTRIGGER = 0xFFFFFFFF;
constexpr DWORD DIJOFS_X = 0;
constexpr DWORD DIJOFS_Y = 4;
constexpr DWORD DIJOFS_Z = 8;

// Effect type flags (need to add the ones used by force feedback)
constexpr DWORD DIEFT_CONSTANTFORCE = 1;
constexpr DWORD DIEFT_RAMPFORCE = 2;
constexpr DWORD DIEFT_PERIODIC = 3;
constexpr DWORD DIEFT_CONDITION = 4;
constexpr DWORD DIEFT_CUSTOMFORCE = 5;
constexpr DWORD DIEFT_HARDWARE = 6;
constexpr DWORD DIEFT_FFATTACK = 0x00000200;
constexpr DWORD DIEFT_FFFADE = 0x00000400;
constexpr DWORD DIEFT_SATURATION = 0x00000800;
constexpr DWORD DIEFT_POSNEGCOEFFICIENTS = 0x00001000;
constexpr DWORD DIEFT_POSNEGSATURATION = 0x00002000;
constexpr DWORD DIEFT_DEADBAND = 0x00004000;
constexpr DWORD DIEFT_STARTDELAY = 0x00020000;

struct DIENVELOPE {
    DWORD dwSize;
    DWORD dwAttackLevel;
    DWORD dwAttackTime;
    DWORD dwFadeLevel;
    DWORD dwFadeTime;
};

struct DIEFFECT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwDuration;
    DWORD dwSamplePeriod;
    DWORD dwGain;
    DWORD dwTriggerButton;
    DWORD dwTriggerRepeatInterval;
    DWORD cAxes;
    LPDWORD rgdwAxes;
    LONG* rglDirection;
    DIENVELOPE* lpEnvelope;
    DWORD cbTypeSpecificParams;
    LPVOID lpvTypeSpecificParams;
    DWORD dwStartDelay;
};

// Additional types for SDL DirectInput wrapper
using LPDIPERIODIC = DIPERIODIC*;
using LPDICONDITION = DICONDITION*;

struct DIDEVICEOBJECTINSTANCEA {
    DWORD dwSize;
    GUID guidType;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
    CHAR tszName[260];
    DWORD dwFFMaxForce;
    DWORD dwFFForceResolution;
    WORD wCollectionNumber;
    WORD wDesignatorIndex;
    WORD wUsagePage;
    WORD wUsage;
    DWORD dwDimension;
    WORD wExponent;
    WORD wReportId;
};

using LPDIDEVICEOBJECTINSTANCEA = DIDEVICEOBJECTINSTANCEA*;
using LPDIENUMDEVICEOBJECTSCALLBACKA = BOOL (*)(LPDIDEVICEOBJECTINSTANCEA, LPVOID);

struct DIEFFESCAPE {
    DWORD dwSize;
    DWORD dwCommand;
    LPVOID lpvInBuffer;
    DWORD cbInBuffer;
    LPVOID lpvOutBuffer;
    DWORD cbOutBuffer;
};

using LPDIEFFESCAPE = DIEFFESCAPE*;
using LPDIENUMCREATEDEFFECTOBJECTSCALLBACK = BOOL (*)(LPDIRECTINPUTEFFECT, LPVOID);

constexpr HRESULT E_NOINTERFACE = 0x80004002;
constexpr DWORD DIDEVTYPEJOYSTICK_GAMEPAD = 0x00000013;
constexpr DWORD DIDC_FORCEFEEDBACK = 0x00000100;
#define MAKEWORD(a, b) ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))

// DirectInput GUIDs (simplified)
inline constexpr GUID GUID_Sine = {0x13541C21, 0x8E33, 0x11D0, {0x9A, 0xD0, 0x00, 0xA0, 0xC9, 0xA0, 0x6E, 0x35}};
inline constexpr GUID GUID_Joystick = {0x6F1D2B60, 0xD5A0, 0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
inline constexpr GUID IID_IDirectInputDevice2A = {0x5944E681, 0xC92E, 0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};

// COM interface stubs
struct IDirectInputEffect {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT Initialize(HINSTANCE, DWORD, REFGUID) = 0;
    virtual HRESULT GetEffectGuid(LPGUID) = 0;
    virtual HRESULT GetParameters(LPDIEFFECT, DWORD) = 0;
    virtual HRESULT SetParameters(LPCDIEFFECT, DWORD) = 0;
    virtual HRESULT Start(DWORD, DWORD) = 0;
    virtual HRESULT Stop() = 0;
    virtual HRESULT GetEffectStatus(LPDWORD) = 0;
    virtual HRESULT Download() = 0;
    virtual HRESULT Unload() = 0;
    virtual HRESULT Escape(LPDIEFFESCAPE) = 0;
    virtual ~IDirectInputEffect() = default;
};

struct IDirectInputDeviceA {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT GetCapabilities(LPDIDEVCAPS) = 0;
    virtual HRESULT EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD) = 0;
    virtual HRESULT GetProperty(REFGUID, LPDIPROPHEADER) = 0;
    virtual HRESULT SetProperty(REFGUID, LPCDIPROPHEADER) = 0;
    virtual HRESULT Acquire() = 0;
    virtual HRESULT Unacquire() = 0;
    virtual HRESULT GetDeviceState(DWORD, LPVOID) = 0;
    virtual HRESULT GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) = 0;
    virtual HRESULT SetDataFormat(LPCDIDATAFORMAT) = 0;
    virtual HRESULT SetEventNotification(HANDLE) = 0;
    virtual HRESULT SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD) = 0;
    virtual HRESULT GetDeviceInfo(LPDIDEVICEINSTANCEA) = 0;
    virtual HRESULT RunControlPanel(HWND, DWORD) = 0;
    virtual HRESULT Initialize(HINSTANCE, DWORD, REFGUID) = 0;
    virtual ~IDirectInputDeviceA() = default;
};

struct IDirectInputDevice2A : IDirectInputDeviceA {
    virtual HRESULT CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT*, LPUNKNOWN) = 0;
    virtual HRESULT EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD) = 0;
    virtual HRESULT GetEffectInfo(LPDIEFFECTINFOA, REFGUID) = 0;
    virtual HRESULT GetForceFeedbackState(LPDWORD) = 0;
    virtual HRESULT SendForceFeedbackCommand(DWORD) = 0;
    virtual HRESULT EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD) = 0;
    virtual HRESULT Escape(LPDIEFFESCAPE) = 0;
    virtual HRESULT Poll() = 0;
    virtual HRESULT SendDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) = 0;
};

struct IDirectInputA {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT Initialize(HINSTANCE, DWORD) = 0;
    virtual HRESULT EnumDevices(DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD) = 0;
    virtual HRESULT GetDeviceStatus(REFGUID) = 0;
    virtual HRESULT RunControlPanel(HWND, DWORD) = 0;
    virtual HRESULT CreateDevice(REFGUID, LPDIRECTINPUTDEVICEA*, LPUNKNOWN) = 0;
    virtual ~IDirectInputA() = default;
};

struct IDirectInput2A : IDirectInputA {
    virtual HRESULT FindDevice(REFGUID, LPCSTR, LPGUID) = 0;
};

// Object data format struct
struct DIOBJECTDATAFORMAT {
    const GUID* pguid;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
};

using LPDIOBJECTDATAFORMAT = DIOBJECTDATAFORMAT*;

// Data format stub
struct DIDATAFORMAT {
    DWORD dwSize;
    DWORD dwObjSize;
    DWORD dwFlags;
    DWORD dwDataSize;
    DWORD dwNumObjs;
    LPDIOBJECTDATAFORMAT rgodf;
};

// Keyboard device
extern const DIDATAFORMAT c_dfDIKeyboard;
extern const DIDATAFORMAT c_dfDIMouse;
extern const DIDATAFORMAT c_dfDIJoystick;

inline constexpr DWORD DIK_ESCAPE = 0x01;
inline constexpr DWORD DIK_1 = 0x02;
inline constexpr DWORD DIK_2 = 0x03;
inline constexpr DWORD DIK_3 = 0x04;
inline constexpr DWORD DIK_4 = 0x05;
inline constexpr DWORD DIK_5 = 0x06;
inline constexpr DWORD DIK_6 = 0x07;
inline constexpr DWORD DIK_7 = 0x08;
inline constexpr DWORD DIK_8 = 0x09;
inline constexpr DWORD DIK_9 = 0x0A;
inline constexpr DWORD DIK_0 = 0x0B;
inline constexpr DWORD DIK_A = 0x1E;
inline constexpr DWORD DIK_B = 0x30;
inline constexpr DWORD DIK_C = 0x2E;
inline constexpr DWORD DIK_D = 0x20;
inline constexpr DWORD DIK_E = 0x12;
inline constexpr DWORD DIK_F = 0x21;
inline constexpr DWORD DIK_G = 0x22;
inline constexpr DWORD DIK_H = 0x23;
inline constexpr DWORD DIK_I = 0x17;
inline constexpr DWORD DIK_J = 0x24;
inline constexpr DWORD DIK_K = 0x25;
inline constexpr DWORD DIK_L = 0x26;
inline constexpr DWORD DIK_M = 0x32;
inline constexpr DWORD DIK_N = 0x31;
inline constexpr DWORD DIK_O = 0x18;
inline constexpr DWORD DIK_P = 0x19;
inline constexpr DWORD DIK_Q = 0x10;
inline constexpr DWORD DIK_R = 0x13;
inline constexpr DWORD DIK_S = 0x1F;
inline constexpr DWORD DIK_T = 0x14;
inline constexpr DWORD DIK_U = 0x16;
inline constexpr DWORD DIK_V = 0x2F;
inline constexpr DWORD DIK_W = 0x11;
inline constexpr DWORD DIK_X = 0x2D;
inline constexpr DWORD DIK_Y = 0x15;
inline constexpr DWORD DIK_Z = 0x2C;
inline constexpr DWORD DIK_F1 = 0x3B;
inline constexpr DWORD DIK_F2 = 0x3C;
inline constexpr DWORD DIK_F3 = 0x3D;
inline constexpr DWORD DIK_F4 = 0x3E;
inline constexpr DWORD DIK_F5 = 0x3F;
inline constexpr DWORD DIK_F6 = 0x40;
inline constexpr DWORD DIK_F7 = 0x41;
inline constexpr DWORD DIK_F8 = 0x42;
inline constexpr DWORD DIK_F9 = 0x43;
inline constexpr DWORD DIK_F10 = 0x44;
inline constexpr DWORD DIK_F11 = 0x57;
inline constexpr DWORD DIK_F12 = 0x58;
inline constexpr DWORD DIK_UP = 0xC8;
inline constexpr DWORD DIK_DOWN = 0xD0;
inline constexpr DWORD DIK_LEFT = 0xCB;
inline constexpr DWORD DIK_RIGHT = 0xCD;
inline constexpr DWORD DIK_SPACE = 0x39;
inline constexpr DWORD DIK_RETURN = 0x1C;
inline constexpr DWORD DIK_LCONTROL = 0x1D;
inline constexpr DWORD DIK_RCONTROL = 0x9D;
inline constexpr DWORD DIK_LSHIFT = 0x2A;
inline constexpr DWORD DIK_RSHIFT = 0x36;
inline constexpr DWORD DIK_LMENU = 0x38;
inline constexpr DWORD DIK_RMENU = 0xB8;
inline constexpr DWORD DIK_TAB = 0x0F;
inline constexpr DWORD DIK_BACK = 0x0E;
inline constexpr DWORD DIK_DELETE = 0xD3;
inline constexpr DWORD DIK_INSERT = 0xD2;
inline constexpr DWORD DIK_HOME = 0xC7;
inline constexpr DWORD DIK_END = 0xCF;
inline constexpr DWORD DIK_PRIOR = 0xC9;
inline constexpr DWORD DIK_NEXT = 0xD1;
inline constexpr DWORD DIK_LWIN = 0xDB;
inline constexpr DWORD DIK_RWIN = 0xDC;
inline constexpr DWORD DIK_SCROLL = 0x46;
inline constexpr DWORD DIK_NUMLOCK = 0x45;
inline constexpr DWORD DIK_CAPITAL = 0x3A;
inline constexpr DWORD DIK_MINUS = 0x0C;
inline constexpr DWORD DIK_EQUALS = 0x0D;
inline constexpr DWORD DIK_LBRACKET = 0x1A;
inline constexpr DWORD DIK_RBRACKET = 0x1B;
inline constexpr DWORD DIK_SEMICOLON = 0x27;
inline constexpr DWORD DIK_APOSTROPHE = 0x28;
inline constexpr DWORD DIK_GRAVE = 0x29;
inline constexpr DWORD DIK_BACKSLASH = 0x2B;
inline constexpr DWORD DIK_COMMA = 0x33;
inline constexpr DWORD DIK_PERIOD = 0x34;
inline constexpr DWORD DIK_SLASH = 0x35;
inline constexpr DWORD DIK_MULTIPLY = 0x37;
inline constexpr DWORD DIK_ADD = 0x4E;
inline constexpr DWORD DIK_SUBTRACT = 0x4A;
inline constexpr DWORD DIK_DECIMAL = 0x53;
inline constexpr DWORD DIK_DIVIDE = 0xB5;
inline constexpr DWORD DIK_NUMPAD0 = 0x52;
inline constexpr DWORD DIK_NUMPAD1 = 0x4F;
inline constexpr DWORD DIK_NUMPAD2 = 0x50;
inline constexpr DWORD DIK_NUMPAD3 = 0x51;
inline constexpr DWORD DIK_NUMPAD4 = 0x4B;
inline constexpr DWORD DIK_NUMPAD5 = 0x4C;
inline constexpr DWORD DIK_NUMPAD6 = 0x4D;
inline constexpr DWORD DIK_NUMPAD7 = 0x47;
inline constexpr DWORD DIK_NUMPAD8 = 0x48;
inline constexpr DWORD DIK_NUMPAD9 = 0x49;
inline constexpr DWORD DIK_PAUSE = 0x45;
inline constexpr DWORD DIK_OEM_102 = 0x56;
