#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define SeLoadDriverPrivilege 10ull
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <TlHelp32.h>
#include <dwmapi.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <cstdlib>
#include <random>
#include <direct.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <d3d11.h>
#include <d3d9.h>
#include <winternl.h>

// driver
#include "../driver/driver.h"
#include "../driver/helpers.h"
inline kernel::driver driver;

// internal files
#include "../psef.h"

// misc
#include "../includes/misc/encrypter.h"
#include "../includes/misc/imports.hpp"
#include "../includes/settings.h"

// cheat
#include "../cheat/helpers/defs.h"
#include "../cheat/offsets.h"
#include "../cheat/helpers/aimbot/mouse.hpp"

// overlay
#include "../overlay/overlay.h"
#include "../overlay/imgui/imgui.h"
#include "../overlay/imgui/imgui_impl_dx11.h"
#include "../overlay/imgui/imgui_impl_win32.h"
#include "../cheat/helpers/structures.hpp"
#include "../overlay/menu/draw_menu.hpp"	

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d11.lib")

using namespace std;

inline int screen_width = GetSystemMetrics(SM_CXSCREEN);
inline int screen_height = GetSystemMetrics(SM_CYSCREEN);

#define u uintptr_t

inline DWORD FindProcessID(string name) {
    PROCESSENTRY32 ProcessEntry;
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &ProcessEntry) == (int)1) {
        while (Process32Next(snapshot, &ProcessEntry) == (int)1) {
            if (string(ProcessEntry.szExeFile) == name)
                return ProcessEntry.th32ProcessID;
        }
    }

    CloseHandle(snapshot);
    return NULL;
}

inline void debug_ptr(uintptr_t p, const char* s) {
    if (defines::debug)
        printf(_("%s %I64u \n"), s, p);
}
inline void debug_3d(vector3 three, const char* s) {
    if (defines::debug)
        printf(_("%s %f, %f, %f \n"), s, three.x, three.y, three.z);
}
inline void debug_2d(vector2 two, const char* s) {
    if (defines::debug)
        printf(_("%s %f, %f \n"), s, two.x, two.y);
}
inline void debug_1d(double one, const char* s) {
    if (defines::debug)
        printf(_("%s %f \n"), s, one);
}
inline void print_3d(vector3 three, const char* s) {
    if (defines::debug)
        printf(_("%s %f, %f, %f \n"), s, three.x, three.y, three.z);
}