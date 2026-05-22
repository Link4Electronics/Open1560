#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>

// Basic Windows types
using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using usize = std::size_t;
using isize = std::ptrdiff_t;

using b8 = bool;
using b16 = bool;
using b32 = bool;

using f32 = float;
using f64 = double;

using ilong = long;
using ulong = unsigned long;

using byte = u8;
using word = u16;
using dword = u32;
using qword = u64;

using BOOL = int;
using BYTE = u8;
using WORD = u16;
using DWORD = u32;
using DWORDLONG = unsigned long long;
using QWORD = u64;
using INT = int;
using UINT = u32;
using LONG = int32_t;
using ULONG = u32;
using PLONG = LONG*;
using LPDWORD = DWORD*;
using SHORT = int16_t;
using USHORT = u16;

using CHAR = char;
using WCHAR = wchar_t;
using PSTR = char*;
using LPCH = char*;
using LPSTR = char*;
using LPCSTR = const char*;
using LPWSTR = wchar_t*;
using LPCWSTR = const wchar_t*;

using VOID = void;
using LPVOID = void*;
using LPCVOID = const void*;
using PVOID = void*;

using HANDLE = void*;
using HMODULE = void*;
using HINSTANCE = void*;
struct HWND__ {};
using HWND = HWND__*;
using HDC = void*;
using HGDIOBJ = void*;
using HFONT = void*;
using HPEN = void*;
using HBRUSH = void*;
using HBITMAP = void*;
using HLOCAL = void*;
using HMENU = void*;
using HCURSOR = void*;
using HICON = void*;
using HKEY = void*;
using HACCEL = void*;
using HRGN = void*;

using HGLOBAL = HLOCAL;
using HRESULT = int32_t;
using LRESULT = intptr_t;
using LPARAM = intptr_t;
using WPARAM = uintptr_t;
using UINT_PTR = uintptr_t;
using LONG_PTR = intptr_t;
using ULONG_PTR = uintptr_t;
using DWORD_PTR = uintptr_t;

// Function pointer types
using FARPROC = intptr_t (*)();
using errno_t = int;

// Error codes
constexpr int ERROR_SUCCESS = 0;
constexpr int ERROR_FILE_NOT_FOUND = 2;
constexpr int ERROR_ACCESS_DENIED = 5;
constexpr int ERROR_INVALID_HANDLE = 6;
constexpr int ERROR_NOT_ENOUGH_MEMORY = 8;
constexpr int ERROR_BAD_FORMAT = 11;
constexpr int ERROR_NO_MORE_FILES = 18;
constexpr int ERROR_HANDLE_EOF = 38;
constexpr DWORD INVALID_FILE_ATTRIBUTES = 0xFFFFFFFF;
constexpr DWORD FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;
constexpr DWORD LANG_SYSTEM_DEFAULT = 0x0800;
constexpr DWORD STD_INPUT_HANDLE = (DWORD)-10;
constexpr DWORD MB_OKCANCEL = 1;
constexpr DWORD MB_YESNO = 4;
constexpr DWORD IDOK = 1;
constexpr DWORD IDCANCEL = 2;
constexpr DWORD IDYES = 6;
constexpr DWORD IDNO = 7;
constexpr DWORD FILE_MAP_COPY = 0x0001;
constexpr DWORD DUPLICATE_SAME_ACCESS = 2;

constexpr int WAIT_OBJECT_0 = 0;
constexpr int WAIT_TIMEOUT = 0x102;
constexpr int WAIT_FAILED = 0xFFFFFFFF;
constexpr int INFINITE = 0xFFFFFFFF;
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
constexpr int FALSE = 0;
constexpr int TRUE = 1;

constexpr DWORD FILE_ATTRIBUTE_DIRECTORY = 0x10;
constexpr DWORD FILE_ATTRIBUTE_NORMAL = 0x80;
constexpr DWORD FILE_CURRENT = 1;
constexpr DWORD FILE_END = 2;
constexpr DWORD GENERIC_READ = 0x80000000;
constexpr DWORD GENERIC_WRITE = 0x40000000;
constexpr DWORD FILE_SHARE_READ = 1;
constexpr DWORD FILE_SHARE_WRITE = 2;
constexpr DWORD OPEN_EXISTING = 3;
constexpr DWORD OPEN_ALWAYS = 4;
constexpr DWORD CREATE_ALWAYS = 2;
constexpr DWORD CREATE_NEW = 1;
constexpr DWORD TRUNCATE_EXISTING = 5;
constexpr DWORD FILE_ATTRIBUTE_NORMAL_W = 0x80;

constexpr DWORD MB_OK = 0;
constexpr DWORD MB_ICONERROR = 0x00000010;

constexpr DWORD STILL_ACTIVE = 0x00000103;

// Code page identifiers
constexpr DWORD CP_UTF8 = 65001;

// Virtual key codes
constexpr int VK_SCROLL = 0x91;

// MSG structure
struct MSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    int pt_x, pt_y;
};

// LARGE_INTEGER / ULARGE_INTEGER
union LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    struct { DWORD LowPart; LONG HighPart; } u;
    long long QuadPart;
};
union ULARGE_INTEGER {
    struct { DWORD LowPart; DWORD HighPart; };
    struct { DWORD LowPart; DWORD HighPart; } u;
    unsigned long long QuadPart;
};

constexpr DWORD PAGE_NOACCESS = 0x01;
constexpr DWORD PAGE_READONLY = 0x02;
constexpr DWORD PAGE_READWRITE = 0x04;
constexpr DWORD PAGE_WRITECOPY = 0x08;
constexpr DWORD PAGE_EXECUTE = 0x10;
constexpr DWORD PAGE_EXECUTE_READ = 0x20;
constexpr DWORD PAGE_EXECUTE_READWRITE = 0x40;
constexpr DWORD MEM_COMMIT = 0x1000;
constexpr DWORD MEM_RESERVE = 0x2000;
constexpr DWORD MEM_DECOMMIT = 0x4000;
constexpr DWORD MEM_RELEASE = 0x8000;
constexpr DWORD MEM_FREE = 0x10000;
constexpr DWORD MEM_PRIVATE = 0x20000;

// Memory mapping
#define FILE_MAP_READ 0x0004

constexpr DWORD DRIVE_CDROM = 5;
constexpr DWORD DRIVE_FIXED = 3;
constexpr DWORD DRIVE_REMOVABLE = 2;

constexpr DWORD IDLE_PRIORITY_CLASS = 0x00000040;
constexpr DWORD BELOW_NORMAL_PRIORITY_CLASS = 0x00004000;
constexpr DWORD NORMAL_PRIORITY_CLASS = 0x00000020;
constexpr DWORD ABOVE_NORMAL_PRIORITY_CLASS = 0x00008000;
constexpr DWORD HIGH_PRIORITY_CLASS = 0x00000080;
constexpr DWORD REALTIME_PRIORITY_CLASS = 0x00000100;

// Windows structure
struct OVERLAPPED {
    ULONG_PTR Internal = 0;
    ULONG_PTR InternalHigh = 0;
    DWORD Offset = 0;
    DWORD OffsetHigh = 0;
    HANDLE hEvent = nullptr;
};

struct FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

struct WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
};
using LPWIN32_FIND_DATAA = WIN32_FIND_DATAA*;

struct SYSTEM_INFO {
    DWORD dwOemId;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
};

struct MEMORYSTATUSEX {
    DWORD dwLength = sizeof(MEMORYSTATUSEX);
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
};

struct SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};

// COM-like types
#ifndef GUID_DEFINED
#define GUID_DEFINED
struct GUID {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];

    bool operator==(const GUID& other) const {
        return Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3 &&
               memcmp(Data4, other.Data4, sizeof(Data4)) == 0;
    }
    bool operator!=(const GUID& other) const {
        return !(*this == other);
    }
};
using REFGUID = const GUID&;
using REFIID = const GUID&;
using CLSID = struct GUID;
#endif  // GUID_DEFINED
using IID = struct GUID;
using _GUID = GUID;

struct IUnknown {
    virtual HRESULT QueryInterface(REFIID riid, LPVOID* ppvObject) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual ~IUnknown() = default;
};

// GUID definitions
inline constexpr GUID GUID_NULL {};
inline bool inlineIsEqualGUID(REFGUID rguid1, REFGUID rguid2) { return rguid1 == rguid2; }

// Calling convention macros
#define __cdecl
#define __stdcall
#define __fastcall
#define __declspec(x)
#define WINAPI
#define CALLBACK
#define PASCAL
#define STDMETHODCALLTYPE

// COM macros
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#define PURE = 0

// Inline functions
inline HANDLE GetModuleHandleA(LPCSTR) { return nullptr; }
inline HANDLE GetModuleHandleW(LPCWSTR) { return nullptr; }
inline HANDLE GetModuleHandle(LPCSTR lpModuleName) { return GetModuleHandleA(lpModuleName); }
inline DWORD GetModuleFileNameA(HANDLE, LPSTR, DWORD) { return 0; }

inline HANDLE GetCurrentProcess() { return (HANDLE)(intptr_t)-1; }

inline HANDLE CreateMutexA(SECURITY_ATTRIBUTES*, BOOL, LPCSTR) { return (HANDLE)(intptr_t)1; }
inline BOOL ReleaseMutex(HANDLE) { return TRUE; }
inline DWORD WaitForSingleObject(HANDLE, DWORD) { return WAIT_OBJECT_0; }
inline DWORD WaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD) { return WAIT_OBJECT_0; }
inline BOOL CloseHandle(HANDLE h)
{
    if (h && h != INVALID_HANDLE_VALUE)
    {
        int fd = (int)(intptr_t)h;
        if (fd > 2) ::close(fd);
    }
    return TRUE;
}
inline BOOL DuplicateHandle(HANDLE, HANDLE, HANDLE, HANDLE*, DWORD, BOOL, DWORD) { return FALSE; }

inline BOOL SetEvent(HANDLE) { return TRUE; }
inline HANDLE CreateEventA(SECURITY_ATTRIBUTES*, BOOL, BOOL, LPCSTR) { return (HANDLE)(intptr_t)2; }

inline BOOL GetModuleHandleExA(DWORD, LPCSTR, HMODULE*) { return FALSE; }
inline DWORD GetLastError() { return 0; }
inline void SetLastError(DWORD) {}

inline DWORD GetFileSize(HANDLE hFile, DWORD* lpFileSizeHigh)
{
    int fd = (int)(intptr_t)hFile;
    off_t saved = ::lseek(fd, 0, SEEK_CUR);
    off_t size = ::lseek(fd, 0, SEEK_END);
    ::lseek(fd, saved, SEEK_SET);
    if (lpFileSizeHigh) *lpFileSizeHigh = (DWORD)((u64)size >> 32);
    return (DWORD)(u64)size;
}

inline DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, LONG* lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    int fd = (int)(intptr_t)hFile;
    off_t offset;
    if (lpDistanceToMoveHigh)
        offset = (off_t)(((u64)(u32)*lpDistanceToMoveHigh << 32) | (u64)(u32)lDistanceToMove);
    else
        offset = lDistanceToMove;
    static const int whence[] = {SEEK_SET, SEEK_CUR, SEEK_END};
    off_t result = ::lseek(fd, offset, whence[dwMoveMethod]);
    if (lpDistanceToMoveHigh)
        *lpDistanceToMoveHigh = (LONG)((u64)result >> 32);
    return (DWORD)(u64)result;
}
inline BOOL SetEndOfFile(HANDLE) { return TRUE; }
inline BOOL GetFileSizeEx(HANDLE hFile, LARGE_INTEGER* lpFileSize)
{
    int fd = (int)(intptr_t)hFile;
    off_t saved = ::lseek(fd, 0, SEEK_CUR);
    off_t size = ::lseek(fd, 0, SEEK_END);
    ::lseek(fd, saved, SEEK_SET);
    lpFileSize->QuadPart = size;
    return TRUE;
}

inline HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    SECURITY_ATTRIBUTES* lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    (void)lpSecurityAttributes;
    (void)dwShareMode;
    (void)dwFlagsAndAttributes;
    (void)hTemplateFile;

    int flags = 0;
    if ((dwDesiredAccess & GENERIC_READ) && (dwDesiredAccess & GENERIC_WRITE))
        flags = O_RDWR;
    else if (dwDesiredAccess & GENERIC_WRITE)
        flags = O_WRONLY;
    else
        flags = O_RDONLY;

    switch (dwCreationDisposition)
    {
        case CREATE_NEW:     flags |= O_CREAT | O_EXCL; break;
        case CREATE_ALWAYS:  flags |= O_CREAT | O_TRUNC; break;
        case OPEN_EXISTING:  /* default open */ break;
        case OPEN_ALWAYS:    flags |= O_CREAT; break;
        case TRUNCATE_EXISTING: flags |= O_TRUNC; break;
    }

    int fd = ::open(lpFileName, flags, 0644);
    if (fd < 0)
        return INVALID_HANDLE_VALUE;
    return (HANDLE)(intptr_t)fd;
}

inline HANDLE CreateFileMappingA(HANDLE, SECURITY_ATTRIBUTES*, DWORD, DWORD, DWORD, LPCSTR) { return nullptr; }
inline LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, usize) { return nullptr; }
inline BOOL UnmapViewOfFile(LPCVOID) { return TRUE; }

inline BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
    OVERLAPPED* lpOverlapped)
{
    (void)lpOverlapped;
    int fd = (int)(intptr_t)hFile;
    ssize_t result = ::read(fd, lpBuffer, nNumberOfBytesToRead);
    if (result < 0) return FALSE;
    if (lpNumberOfBytesRead) *lpNumberOfBytesRead = (DWORD)result;
    return TRUE;
}

inline BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
    OVERLAPPED* lpOverlapped)
{
    (void)lpOverlapped;
    int fd = (int)(intptr_t)hFile;
    ssize_t result = ::write(fd, lpBuffer, nNumberOfBytesToWrite);
    if (result < 0) return FALSE;
    if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = (DWORD)result;
    return TRUE;
}

inline BOOL GetFileInformationByHandle(HANDLE, void*) { return FALSE; }

inline DWORD GetFileAttributesA(LPCSTR path) {
    struct stat st;
    if (stat(path, &st) == 0) return FILE_ATTRIBUTE_NORMAL;
    return (DWORD)-1;
}

inline BOOL SetFileAttributesA(LPCSTR, DWORD) { return TRUE; }

inline BOOL GetDiskFreeSpaceExA(LPCSTR, ULARGE_INTEGER*, ULARGE_INTEGER*, ULARGE_INTEGER*) {
    // Return fake values to keep the game happy
    static ULARGE_INTEGER fake_free;
    fake_free.QuadPart = 1024LL << 30;
    return TRUE;
}

// Virtual key state (Always returns 0 on Linux)
inline SHORT GetAsyncKeyState(int vKey) { return 0; }

inline BOOL GlobalMemoryStatusEx(MEMORYSTATUSEX* buffer) {
    buffer->dwLength = sizeof(MEMORYSTATUSEX);
    buffer->ullAvailPhys = 512 << 20;
    buffer->ullTotalPhys = 1024 << 20;
    buffer->ullAvailPageFile = 1024 << 20;
    buffer->ullTotalPageFile = 2048 << 20;
    buffer->ullAvailVirtual = 2047 << 20;
    buffer->ullTotalVirtual = 2048 << 20;
    return TRUE;
}

inline DWORD GetCompressedFileSizeA(LPCSTR, DWORD*) { return 0; }
inline DWORD GetDriveTypeA(LPCSTR) { return DRIVE_FIXED; }

inline HANDLE FindFirstFileA(LPCSTR, LPWIN32_FIND_DATAA) {
    return INVALID_HANDLE_VALUE;
}

inline BOOL FindNextFileA(HANDLE, LPWIN32_FIND_DATAA) { return FALSE; }
inline BOOL FindClose(HANDLE) { return TRUE; }

inline BOOL DeleteFileA(LPCSTR path) { return unlink(path) == 0; }
inline BOOL MoveFileA(LPCSTR, LPCSTR) { return FALSE; }
inline BOOL CopyFileA(LPCSTR, LPCSTR, BOOL) { return FALSE; }

inline BOOL CreateDirectoryA(LPCSTR, SECURITY_ATTRIBUTES*) { return FALSE; }
inline BOOL RemoveDirectoryA(LPCSTR) { return FALSE; }

inline DWORD GetCurrentDirectoryA(DWORD, LPSTR) { return 0; }
inline BOOL SetCurrentDirectoryA(LPCSTR) { return TRUE; }

inline DWORD SearchPathA(LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPSTR*) { return 0; }
inline DWORD GetFullPathNameA(LPCSTR, DWORD, LPSTR, LPSTR*) { return 0; }

inline BOOL SetPriorityClass(HANDLE, DWORD) { return TRUE; }
inline DWORD GetPriorityClass(HANDLE) { return NORMAL_PRIORITY_CLASS; }

inline BOOL SetProcessAffinityMask(HANDLE, DWORD) { return TRUE; }
inline BOOL SetThreadAffinityMask(HANDLE, DWORD) { return TRUE; }

inline BOOL IsDebuggerPresent() { return FALSE; }
inline void OutputDebugStringA(LPCSTR) {}
inline void DebugBreak() {}
inline int __debugbreak() { raise(SIGTRAP); return 0; }
inline void __fastfail(unsigned int) { _exit(1); }

inline DWORD GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (DWORD)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

inline BOOL QueryPerformanceCounter(int64_t* counter) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *counter = (int64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
    return TRUE;
}

inline BOOL QueryPerformanceFrequency(int64_t* frequency) {
    *frequency = 1000000000;
    return TRUE;
}

inline void Sleep(DWORD ms) { usleep(ms * 1000); }

inline void GetSystemInfo(SYSTEM_INFO* info) {
    info->dwPageSize = 4096;
    info->dwNumberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    info->dwAllocationGranularity = 65536;
}

inline LPVOID VirtualAlloc(LPVOID address, usize size, DWORD type, DWORD protect) {
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (address) flags |= MAP_FIXED;
    LPVOID result = mmap(address, size, PROT_READ | PROT_WRITE, flags, -1, 0);
    if (result == MAP_FAILED) return nullptr;
    return result;
}

inline BOOL VirtualFree(LPVOID address, usize size, DWORD type) {
    return munmap(address, size) == 0;
}

inline BOOL VirtualProtect(LPVOID address, usize size, DWORD newProtect, DWORD* oldProtect) {
    int prot = PROT_READ;
    if (newProtect & PAGE_READWRITE) prot = PROT_READ | PROT_WRITE;
    if (newProtect & PAGE_EXECUTE_READWRITE) prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    if (newProtect & PAGE_EXECUTE_READ) prot = PROT_READ | PROT_EXEC;
    if (newProtect & PAGE_EXECUTE) prot = PROT_EXEC;
    if (newProtect & PAGE_NOACCESS) prot = PROT_NONE;
    return mprotect(address, size, prot) == 0;
}

inline HLOCAL LocalAlloc(DWORD flags, usize size) { return malloc(size); }
inline HLOCAL LocalFree(HLOCAL ptr) { free(ptr); return nullptr; }
inline HLOCAL GlobalAlloc(DWORD flags, usize size) { return malloc(size); }
inline HLOCAL GlobalFree(HLOCAL ptr) { free(ptr); return nullptr; }
inline LPVOID GlobalLock(HLOCAL ptr) { return ptr; }
inline BOOL GlobalUnlock(HLOCAL) { return TRUE; }

inline HMODULE LoadLibraryA(LPCSTR name) {
    return dlopen(name, RTLD_LAZY | RTLD_LOCAL);
}

inline BOOL FreeLibrary(HMODULE mod) {
    if (mod) dlclose(mod);
    return TRUE;
}

inline FARPROC GetProcAddress(HMODULE mod, LPCSTR name) {
    return (FARPROC)(uintptr_t)dlsym(mod, name);
}

inline DWORD FormatMessageA(DWORD, LPVOID, DWORD, DWORD, LPSTR, DWORD, va_list*) { return 0; }

inline int MultiByteToWideChar(DWORD, DWORD, LPCSTR, int, LPWSTR, int) { return 0; }
inline int WideCharToMultiByte(DWORD, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, BOOL*) { return 0; }

// wmemcpy is provided by <cwchar>

inline DWORD GetModuleBaseNameA(HANDLE, HMODULE, LPSTR, DWORD) { return 0; }

// Thread functions
using LPTHREAD_START_ROUTINE = DWORD (*)(LPVOID);

inline HANDLE CreateThread(SECURITY_ATTRIBUTES*, usize, LPTHREAD_START_ROUTINE func, LPVOID param, DWORD, DWORD*) {
    pthread_t thread;
    pthread_create(&thread, nullptr, (void* (*)(void*))func, param);
    pthread_detach(thread);
    return (HANDLE)(intptr_t)3;
}

inline DWORD GetExitCodeThread(HANDLE, LPDWORD) { return TRUE; }
inline BOOL TerminateThread(HANDLE, DWORD) { return FALSE; }

inline BOOL CreateProcessA(LPCSTR, LPSTR, SECURITY_ATTRIBUTES*, SECURITY_ATTRIBUTES*, BOOL, DWORD, LPVOID, LPCSTR, void*, void*) {
    return FALSE;
}

inline DWORD GetCurrentThreadId() { return (DWORD)pthread_self(); }
inline void ExitProcess(DWORD code) { _exit(code); }

// MessageBox
inline int MessageBoxA(HWND, LPCSTR text, LPCSTR caption, DWORD) {
    fprintf(stderr, "%s: %s\n", caption ? caption : "", text ? text : "");
    return 0;
}

inline int MessageBoxW(HWND, LPCWSTR, LPCWSTR, DWORD) { return 0; }

// Registry stubs
inline LONG RegOpenKeyExA(HKEY, LPCSTR, DWORD, DWORD, HKEY*) { return 1; }
inline LONG RegCloseKey(HKEY) { return 0; }
inline LONG RegQueryValueExA(HKEY, LPCSTR, DWORD*, DWORD*, BYTE*, DWORD*) { return 1; }
inline LONG RegSetValueExA(HKEY, LPCSTR, DWORD, DWORD, const BYTE*, DWORD) { return 0; }
inline LONG RegCreateKeyExA(HKEY, LPCSTR, DWORD, LPSTR, DWORD, DWORD, SECURITY_ATTRIBUTES*, HKEY*, DWORD*) { return 0; }
inline LONG RegDeleteKeyA(HKEY, LPCSTR) { return 0; }
inline LONG RegEnumKeyExA(HKEY, DWORD, LPSTR, DWORD*, DWORD*, LPSTR, DWORD*, FILETIME*) { return 1; }

// Shell
inline HINSTANCE ShellExecuteA(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT) { return nullptr; }

// Resource stubs
inline HANDLE FindResourceA(HMODULE, LPCSTR, LPCSTR) { return nullptr; }
inline HANDLE LoadResource(HMODULE, HANDLE) { return nullptr; }
inline LPVOID LockResource(HANDLE) { return nullptr; }
inline DWORD SizeofResource(HMODULE, HANDLE) { return 0; }

// String functions
using LPTCH = char*;
inline LPSTR lstrcpyA(LPSTR dest, LPCSTR src) { return strcpy(dest, src); }
inline int lstrlenA(LPCSTR str) { return (int)strlen(str); }

// Environment
inline DWORD GetEnvironmentVariableA(LPCSTR name, LPSTR buffer, DWORD size) {
    const char* val = getenv(name);
    if (!val) return 0;
    DWORD len = (DWORD)strlen(val);
    if (buffer && size > len) { strcpy(buffer, val); return len; }
    return len + 1;
}
inline BOOL SetEnvironmentVariableA(LPCSTR name, LPCSTR value) {
    return setenv(name, value ? value : "", 1) == 0;
}

// Performance
inline BOOL GetProcessTimes(HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME*) { return FALSE; }

// Bit scan intrinsics (MSVC -> GCC)
inline unsigned char _BitScanReverse(unsigned long* index, unsigned long mask) {
    if (mask == 0) { *index = 0; return 0; }
    *index = 31 - __builtin_clz(mask);
    return 1;
}
inline unsigned char _BitScanForward(unsigned long* index, unsigned long mask) {
    if (mask == 0) { *index = 0; return 0; }
    *index = __builtin_ctz(mask);
    return 1;
}

// COM macros
#ifndef SUCCEEDED
#    define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#endif
#ifndef FAILED
#    define FAILED(hr) ((HRESULT)(hr) < 0)
#endif

// COM types
using LPCLASSFACTORY = void*;
using LPFNDLLGETCLASSOBJECT = HRESULT (*)(GUID&, GUID&, void**);

// Misc
inline void GetStartupInfoA(void*) {}
inline DWORD GetActiveWindow() { return 0; }
inline HWND SetActiveWindow(HWND) { return nullptr; }
inline HWND SetFocus(HWND) { return nullptr; }
inline BOOL SetForegroundWindow(HWND) { return TRUE; }
inline BOOL ShowWindow(HWND, int) { return TRUE; }
inline HWND GetDesktopWindow() { return nullptr; }

// Console
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE ((DWORD)-12)
inline HANDLE GetStdHandle(DWORD nStdHandle) { return (HANDLE)(intptr_t)(nStdHandle == STD_ERROR_HANDLE ? 2 : 1); }
inline BOOL SetConsoleTextAttribute(HANDLE, WORD) { return TRUE; }
inline BOOL WriteConsoleA(HANDLE, LPCSTR, DWORD, LPDWORD, void*) { return FALSE; }
inline BOOL FlushFileBuffers(HANDLE) { return TRUE; }
inline HANDLE GetConsoleWindow() { return nullptr; }
inline BOOL AttachConsole(DWORD) { return FALSE; }
inline BOOL AllocConsole() { return FALSE; }
inline BOOL SetConsoleTitleA(LPCSTR) { return TRUE; }
inline BOOL FreeConsole() { return TRUE; }

#define ATTACH_PARENT_PROCESS ((DWORD)-1)

// Console colors
#define FOREGROUND_BLUE      0x0001
#define FOREGROUND_GREEN     0x0002
#define FOREGROUND_RED       0x0004
#define FOREGROUND_INTENSITY 0x0008

// File flags (FILE_BEGIN only; FILE_CURRENT and FILE_END are defined above)
#define FILE_BEGIN           0

// MessageBox flags
#define MB_SETFOREGROUND 0x00010000
#define MB_TOPMOST       0x00040000

// Thread priorities
#define THREAD_PRIORITY_TIME_CRITICAL 15

inline HANDLE GetCurrentThread() { return (HANDLE)(intptr_t)-2; }
inline BOOL SetThreadPriority(HANDLE, int) { return TRUE; }
inline int GetThreadPriority(HANDLE) { return 0; }
inline BOOL SwitchToThread() { return FALSE; }
#define YieldProcessor()



// localtime_s replacement
inline struct tm* localtime_s(struct tm* tmp, const time_t* timer) {
    return localtime_r(timer, tmp);
}

// freopen_s replacement
inline errno_t freopen_s(FILE** file, const char* path, const char* mode, FILE* stream) {
    *file = freopen(path, mode, stream);
    return *file ? 0 : errno;
}

inline int wsprintfA(LPSTR, LPCSTR, ...) { return 0; }

inline HDC GetDC(HWND) { return nullptr; }
inline int ReleaseDC(HWND, HDC) { return 0; }

inline HGDIOBJ SelectObject(HDC, HGDIOBJ) { return nullptr; }
inline BOOL DeleteDC(HDC) { return TRUE; }
inline BOOL DeleteObject(HGDIOBJ) { return TRUE; }
inline HGDIOBJ GetStockObject(int) { return nullptr; }
inline HDC CreateCompatibleDC(HDC) { return nullptr; }
inline HBITMAP CreateDIBSection(HDC, void*, DWORD, void**, HANDLE, DWORD) { return nullptr; }
inline HFONT CreateFontA(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR) { return nullptr; }

// Process query
inline BOOL GetExitCodeProcess(HANDLE, LPDWORD) { return FALSE; }

// Thread local storage
inline DWORD TlsAlloc() { static pthread_key_t key; pthread_key_create(&key, nullptr); return (DWORD)(intptr_t)&key; }
inline LPVOID TlsGetValue(DWORD) { return nullptr; }
inline BOOL TlsSetValue(DWORD, LPVOID) { return TRUE; }
inline BOOL TlsFree(DWORD) { return TRUE; }

// Interlocked
inline LONG InterlockedIncrement(LONG volatile* v) { return __sync_add_and_fetch(v, 1); }
inline LONG InterlockedDecrement(LONG volatile* v) { return __sync_sub_and_fetch(v, 1); }
inline LONG InterlockedExchange(LONG volatile* v, LONG newv) { return __sync_lock_test_and_set(v, newv); }
inline LONG InterlockedCompareExchange(LONG volatile* v, LONG newv, LONG cmp) { return __sync_val_compare_and_swap(v, cmp, newv); }

#define InterlockedIncrement InterlockedIncrement
#define InterlockedDecrement InterlockedDecrement
#define InterlockedExchange InterlockedExchange
#define InterlockedCompareExchange InterlockedCompareExchange

// Keyboard
inline int GetKeyNameTextA(LONG lParam, LPSTR lpString, int cchSize) {
    if (cchSize > 0) lpString[0] = '\0';
    return 0;
}

// Debug
inline void RtlCaptureContext(void*) {}


