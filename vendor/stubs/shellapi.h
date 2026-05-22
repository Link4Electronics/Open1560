#pragma once

// Get common Windows types from the main stub
#include <Windows.h>

struct SHELLEXECUTEINFOA {
    DWORD cbSize;
    DWORD fMask;
    HWND hwnd;
    LPCSTR lpVerb;
    LPCSTR lpFile;
    LPCSTR lpParameters;
    LPCSTR lpDirectory;
    int nShow;
    HINSTANCE hInstApp;
    LPVOID lpIDList;
    LPCSTR lpClass;
    HKEY hkeyClass;
    DWORD dwHotKey;
    HANDLE hMonitor;
    HANDLE hProcess;
};

inline HINSTANCE ShellExecuteExA(SHELLEXECUTEINFOA*) { return nullptr; }

inline LPWSTR* CommandLineToArgvW(LPCWSTR, int*) { return nullptr; }
