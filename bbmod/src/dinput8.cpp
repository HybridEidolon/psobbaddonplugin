#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//#include "physfs.h"
#include "luajit/lua.hpp"
#include "d3d8.h"
#include "minhook.h"
#include "imgui_impl_d3d8.h"
#include "imgui_d3d8_dev.h"
#include "imgui_dinput8_dev.h"
#include "log.h"
#include "luastate.h"
#include "lua_hooks.h"
#include "lua_psolib.h"
#include "sol.hpp"

#define PSOBB_WINDOW_TITLE_ADDR 0x00ACBE70 - 0x00400000 + g_PSOBaseAddress
#define PSOBB_MAIN (void*)(0x0082D050 - 0x00400000 + g_PSOBaseAddress)
#define PSOBB_INITD3D (tPSOBB_InitD3D)(0x00838CA0 - 0x00400000 + g_PSOBaseAddress)
#define PSOBB_DIRECT3D8_PTR (IDirect3D8**)(0x00ACD4A0 - 0x00400000 + g_PSOBaseAddress)
#define PSOBB_DIRECT3DDEVICE8_PTR (IDirect3DDevice8**)(0x00ACD528 - 0x00400000 + g_PSOBaseAddress)
#define PSOBB_DIRECTINPUT8_PTR (IDirectInput8A**)(0x00ACBEDC - 0x00400000 + g_PSOBaseAddress)
#define PSOBB_HWND_PTR (HWND*)(0x00ACBED8 - 0x00400000 + g_PSOBaseAddress)

#define DO_HOOKS true

typedef HRESULT(WINAPI *tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create = nullptr;
typedef IDirect3D8*(WINAPI *tDirect3DCreate8)(UINT SDKVersion);
tDirect3DCreate8 oDirect3DCreate8 = nullptr;

static char* window_title_addr = nullptr;

typedef void(*tPSOBB_Main)(void);
static tPSOBB_Main oPSOBB_Main = nullptr;
typedef void(__thiscall *tPSOBB_InitD3D)(void*);
static tPSOBB_InitD3D oPSOBB_InitD3D = nullptr;

static IDirect3D8* d3dDevicePtr = nullptr;

BYTE *codeBase, *codeEnd, *dataBase, *dataEnd;

static IDirectInput8A* dinput8AModule = nullptr;
static IDirect3D8* d3d8Module = nullptr;

// hack to work around __thiscall not being usable
void __fastcall PSOBB_InitD3D(void* self, void* notused) {
    if (oPSOBB_InitD3D != nullptr) {
        oPSOBB_InitD3D(self);
        // bad d3d. evil. do not.
        // too bad, we have to deal with this every time we invoke Lua code
        // because otherwise Recons will occasionally just _break_
        // #SEGAProgramming
        //__asm finit;
    }

    ImGui_ImplD3D8_Init(*PSOBB_HWND_PTR, PSOBB_DIRECT3DDEVICE8_PTR);

    ImGui_ImplD3D8_NewFrame();
}

void Initialize() {
    g_PSOBaseAddress = (DWORD)GetModuleHandle(nullptr);
    auto idh = (PIMAGE_DOS_HEADER)g_PSOBaseAddress;
    auto inh = (PIMAGE_NT_HEADERS)(g_PSOBaseAddress + idh->e_lfanew);
    auto ioh = &inh->OptionalHeader;
    codeBase = (BYTE*)(g_PSOBaseAddress + ioh->BaseOfCode);
    codeEnd = codeBase + ioh->SizeOfCode;
    dataBase = (BYTE*)(g_PSOBaseAddress + ioh->BaseOfData);
    dataEnd = dataBase + ioh->SizeOfInitializedData;

    MH_Initialize();

    if (DO_HOOKS) {
        MH_CreateHook((void*)PSOBB_INITD3D, PSOBB_InitD3D, (void**)(&oPSOBB_InitD3D));
        MH_EnableHook((void*)PSOBB_INITD3D);
    }

    if (!oDirectInput8Create) {
        CHAR syspath[MAX_PATH];
        GetSystemDirectory(syspath, MAX_PATH);
        strcat_s(syspath, "\\dinput8.dll");

        CHAR dllpath[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, dllpath);
        strcat_s(dllpath, "\\customdlls\\dinput8.dll");

        DWORD dwAttrs;
        dwAttrs = GetFileAttributes(dllpath);

        if (dwAttrs != INVALID_FILE_ATTRIBUTES) {
            HMODULE hMod = LoadLibrary(dllpath);
            oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
        }
        else {
            HMODULE hMod = LoadLibrary(syspath);
            oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
        }
    }

    if (!oDirect3DCreate8) {
        CHAR syspath[MAX_PATH];
        GetSystemDirectory(syspath, MAX_PATH);
        strcat_s(syspath, "\\d3d8.dll");

        CHAR dllpath[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, dllpath);
        strcat_s(dllpath, "\\customdlls\\d3d8.dll");

        DWORD dwAttrs;
        dwAttrs = GetFileAttributes(dllpath);

        if (dwAttrs != INVALID_FILE_ATTRIBUTES) {
            HMODULE hMod = LoadLibrary(dllpath);
            oDirect3DCreate8 = (tDirect3DCreate8)GetProcAddress(hMod, "Direct3DCreate8");
        }
        else {
            HMODULE hMod = LoadLibrary(syspath);
            oDirect3DCreate8 = (tDirect3DCreate8)GetProcAddress(hMod, "Direct3DCreate8");
        }
    }

    FPUSTATE fpustate;
    psolua_store_fpu_state(fpustate);
    psolua_initialize_state();
    psolua_restore_fpu_state(fpustate);
}

void Uninitialize() {
    MH_Uninitialize();
    FPUSTATE fpustate;
    psolua_store_fpu_state(fpustate);
    lua_close(g_LuaState);
    psolua_restore_fpu_state(fpustate);
    g_LuaState = nullptr;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        Initialize();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        Uninitialize();
    }
    return TRUE;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk) {
    if (r_iid == IID_IDirectInput8A) {
        if (!dinput8AModule) {
            auto ret = oDirectInput8Create(inst_handle, version, r_iid, out_wrapper, p_unk);
            if (ret) {
                return ret;
            }
            dinput8AModule = *((IDirectInput8A**)out_wrapper);
            dinput8AModule = (IDirectInput8A*) new ImguiDInput(dinput8AModule);

        }
        *out_wrapper = dinput8AModule;
        return 0;
    }
    else {
        g_log << "UNICODE DInput8 is unsupported" << std::endl;
        MessageBoxA(NULL, "Unicode dinput8 was created, cannot continue", "bbmod: DirectInput8Create", 0);
        exit(1);
    }
}
