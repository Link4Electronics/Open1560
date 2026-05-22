#pragma once

// Minimal stub for DirectSound types used by Open1560

#include <cstdint>
#include <cstddef>

// Types DWORD, LONG, BOOL, BYTE, WORD, UINT, LPVOID, LPCVOID, LPCSTR, LPSTR,
// LPCWSTR, LPWSTR, HRESULT, REFGUID, REFIID, HWND, HANDLE, HMODULE, HINSTANCE,
// DWORD_PTR, LPDWORD, ULONG, GUID, IUnknown come from minwin_linux.h (via Windows.h)

using LPBYTE = BYTE*;
using LPLONG = LONG*;
using LPUNKNOWN = IUnknown*;
using LPCGUID = const GUID*;
using LPGUID = GUID*;

constexpr HRESULT DS_OK = 0;
constexpr HRESULT DSERR_ALLOCATED = 0x8878000A;
constexpr HRESULT DSERR_INVALIDPARAM = 0x88780008;
constexpr HRESULT DSERR_NOAGGREGATION = 0x88780007;
constexpr HRESULT DSERR_OUTOFMEMORY = 0x8878000E;
constexpr HRESULT DSERR_UNINITIALIZED = 0x88780003;
constexpr HRESULT DSERR_UNSUPPORTED = 0x88780004;
constexpr HRESULT DSERR_BUFFERLOST = 0x88780018;
constexpr HRESULT DSERR_OTHERAPPHASPRIO = 0x88780005;
constexpr HRESULT DSERR_CONTROLUNAVAIL = 0x8878001E;
constexpr HRESULT DSERR_PRIOLEVELNEEDED = 0x88780002;
constexpr HRESULT DSERR_BADFORMAT = 0x88780020;
constexpr HRESULT DSERR_NODRIVER = 0x88780032;

constexpr DWORD DSSCL_NORMAL = 1;
constexpr DWORD DSSCL_PRIORITY = 2;
constexpr DWORD DSSCL_EXCLUSIVE = 3;
constexpr DWORD DSSCL_WRITEPRIMARY = 4;

constexpr DWORD DSBCAPS_PRIMARYBUFFER = 1;
constexpr DWORD DSBCAPS_GLOBALFOCUS = 0x00008000;
constexpr DWORD DSBCAPS_CTRLVOLUME = 0x00000080;
constexpr DWORD DSBCAPS_CTRLPAN = 0x00000100;
constexpr DWORD DSBCAPS_CTRLFREQUENCY = 0x00000020;
constexpr DWORD DSBCAPS_CTRL3D = 0x00001000;
constexpr DWORD DSBCAPS_STICKYFOCUS = 0x00004000;
constexpr DWORD DSBCAPS_CTRLPOSITIONNOTIFY = 0x00000100;
constexpr DWORD DSBCAPS_CTRLFX = 0x00000200;
constexpr DWORD DSBCAPS_LOCSOFTWARE = 0x00000004;
constexpr DWORD DSBCAPS_LOCHARDWARE = 0x00000002;
constexpr DWORD DSBCAPS_MUTE3DATMAXDISTANCE = 0x00080000;
constexpr DWORD DSBCAPS_LOCDEFER = 0x00000008;

constexpr DWORD DSBPLAY_LOOPING = 1;
constexpr DWORD DSBPLAY_LOCHARDWARE = 2;
constexpr DWORD DSBPLAY_LOCSOFTWARE = 4;
constexpr DWORD DSBPLAY_TERMINATEDBY_TIME = 8;
constexpr DWORD DSBPLAY_TERMINATEDBY_DISTANCE = 16;

constexpr DWORD DSSTATUS_PLAYING = 1;
constexpr DWORD DSSTATUS_BUFFERLOST = 2;
constexpr DWORD DSSTATUS_LOOPING = 4;
constexpr DWORD DSSTATUS_LOCSOFTWARE = 8;
constexpr DWORD DSSTATUS_LOCHARDWARE = 16;

constexpr DWORD DSBLOCK_FROMWRITECURSOR = 1;
constexpr DWORD DSBLOCK_ENTIREBUFFER = 2;

constexpr DWORD DS3DMODE_NORMAL = 0;
constexpr DWORD DS3DMODE_HEADRELATIVE = 1;
constexpr DWORD DS3DMODE_DISABLE = 2;

// WAVEFORMATEX
struct WAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
};

using tWAVEFORMATEX = WAVEFORMATEX;
using LPWAVEFORMATEX = WAVEFORMATEX*;
using LPCWAVEFORMATEX = const WAVEFORMATEX*;

// DSBUFFERDESC
struct DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    LPWAVEFORMATEX lpwfxFormat;
    GUID guid3DAlgorithm;
};

using LPCDSBUFFERDESC = const DSBUFFERDESC*;

// DSBCAPS
struct DSBCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwUnlockTransferRate;
    DWORD dwPlayCpuOverhead;
};

// 3D buffer
struct DS3DBUFFER {
    DWORD dwSize;
    DWORD dwMode;
};

// Position
struct D3DVECTOR {
    float x;
    float y;
    float z;
};

// IDirectSound
struct IDirectSoundBuffer;
struct IDirectSound {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT CreateSoundBuffer(LPCDSBUFFERDESC, IDirectSoundBuffer**, LPUNKNOWN) = 0;
    virtual HRESULT GetCaps(LPVOID) = 0;
    virtual HRESULT DuplicateSoundBuffer(IDirectSoundBuffer*, IDirectSoundBuffer**) = 0;
    virtual HRESULT SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT Compact() = 0;
    virtual HRESULT GetSpeakerConfig(LPDWORD) = 0;
    virtual HRESULT SetSpeakerConfig(DWORD) = 0;
    virtual HRESULT Initialize(LPCGUID) = 0;
    virtual ~IDirectSound() = default;
};

// IDirectSoundBuffer
struct IDirectSoundBuffer {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT GetCaps(LPVOID) = 0;
    virtual HRESULT GetCurrentPosition(LPDWORD, LPDWORD) = 0;
    virtual HRESULT GetFormat(LPWAVEFORMATEX, DWORD, LPDWORD) = 0;
    virtual HRESULT GetVolume(LPLONG) = 0;
    virtual HRESULT GetPan(LPLONG) = 0;
    virtual HRESULT GetFrequency(LPDWORD) = 0;
    virtual HRESULT GetStatus(LPDWORD) = 0;
    virtual HRESULT Initialize(HANDLE, LPCDSBUFFERDESC) = 0;
    virtual HRESULT Lock(DWORD, DWORD, LPVOID*, LPDWORD, LPVOID*, LPDWORD, DWORD) = 0;
    virtual HRESULT Play(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT SetCurrentPosition(DWORD) = 0;
    virtual HRESULT SetFormat(LPCWAVEFORMATEX) = 0;
    virtual HRESULT SetVolume(LONG) = 0;
    virtual HRESULT SetPan(LONG) = 0;
    virtual HRESULT SetFrequency(DWORD) = 0;
    virtual HRESULT Stop() = 0;
    virtual HRESULT Unlock(LPVOID, DWORD, LPVOID, DWORD) = 0;
    virtual HRESULT Restore() = 0;
    virtual ~IDirectSoundBuffer() = default;
};

// IDirectSound3DBuffer
struct IDirectSound3DBuffer {
    virtual HRESULT QueryInterface(REFIID, LPVOID*) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual HRESULT GetAllParameters(LPVOID) = 0;
    virtual HRESULT GetConeAngles(LPDWORD, LPDWORD) = 0;
    virtual HRESULT GetConeOrientation(LPVOID) = 0;
    virtual HRESULT GetConeOutsideVolume(LPLONG) = 0;
    virtual HRESULT GetMaxDistance(LPVOID) = 0;
    virtual HRESULT GetMinDistance(LPVOID) = 0;
    virtual HRESULT GetMode(LPDWORD) = 0;
    virtual HRESULT GetPosition(LPVOID) = 0;
    virtual HRESULT GetVelocity(LPVOID) = 0;
    virtual HRESULT SetAllParameters(LPCVOID, DWORD) = 0;
    virtual HRESULT SetConeAngles(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT SetConeOrientation(float, float, float, DWORD) = 0;
    virtual HRESULT SetConeOutsideVolume(LONG, DWORD) = 0;
    virtual HRESULT SetMaxDistance(float, DWORD) = 0;
    virtual HRESULT SetMinDistance(float, DWORD) = 0;
    virtual HRESULT SetMode(DWORD, DWORD) = 0;
    virtual HRESULT SetPosition(float, float, float, DWORD) = 0;
    virtual HRESULT SetVelocity(float, float, float, DWORD) = 0;
    virtual HRESULT CommitDeferredSettings() = 0;
    virtual ~IDirectSound3DBuffer() = default;
};

// Types
using LPDIRECTSOUND = IDirectSound*;
using LPDIRECTSOUNDBUFFER = IDirectSoundBuffer*;
using LPDIRECTSOUND3DBUFFER = IDirectSound3DBuffer*;
using LPCGUID = const GUID*;
using LPGUID = GUID*;

// Auxiliary data types
struct DS3DBUFFERDESC {
    DWORD dwSize;
    DWORD dwMode;
};

// DirectSoundCreate stub
inline HRESULT DirectSoundCreate(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN) { return DS_OK; }
inline HRESULT DirectSoundEnumerateA(LPVOID, LPVOID) { return DS_OK; }

// Property set interface
struct IKsPropertySet : IUnknown {
    virtual HRESULT Get(REFGUID, DWORD, LPVOID, DWORD, LPVOID, DWORD, DWORD*) = 0;
    virtual HRESULT Set(REFGUID, DWORD, LPVOID, DWORD, LPVOID, DWORD) = 0;
    virtual HRESULT QuerySupport(REFGUID, DWORD, DWORD*) = 0;
};

using LPKSPROPERTYSET = IKsPropertySet*;

// DirectSound private property sets
struct DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA {
    GUID DeviceId;
    DWORD DescriptionString;
    DWORD Module;
    DWORD Interface;
    DWORD WaveDeviceId;
    DWORD DataFlow;
};

using PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA = DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_DATA*;

constexpr DWORD DSPROPSETID_DirectSoundDevice_Data1 = 0x84624F82;
constexpr DWORD DSPROPSETID_DirectSoundDevice_Data2 = 0x44F8;
constexpr GUID DSPROPSETID_DirectSoundDevice = {
    DSPROPSETID_DirectSoundDevice_Data1, DSPROPSETID_DirectSoundDevice_Data2, 0x46E6, {0x88, 0x4B, 0x9D, 0xEA, 0xEA, 0x59, 0xBB, 0xF9}
};

constexpr DWORD DSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION = 1;
constexpr DWORD DIRECTSOUNDDEVICE_DATAFLOW_RENDER = 0;

// CLSID
inline GUID CLSID_DirectSoundPrivate;
inline GUID CLSID_DirectSound;
inline GUID IID_IClassFactory;
inline GUID IID_IKsPropertySet;
inline GUID DSDEVID_DefaultPlayback;
inline GUID DSDEVID_DefaultCapture;
inline GUID DSDEVID_DefaultVoiceCapture;
inline GUID DSDEVID_DefaultVoicePlayback;

// GUID_NULL is provided by minwin_linux.h
