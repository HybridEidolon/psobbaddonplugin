#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
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

#define PSOBB_WINDOW_TITLE_ADDR 0x00ACBE70
#define PSOBB_MAIN (void*)0x0082D050
#define PSOBB_INITD3D (tPSOBB_InitD3D)0x00838CA0
#define PSOBB_DIRECT3D8_PTR (IDirect3D8**)0x00ACD4A0
#define PSOBB_DIRECT3DDEVICE8_PTR (IDirect3DDevice8**)0x00ACD528
#define PSOBB_DIRECTINPUT8_PTR (IDirectInput8A**)0x00ACBEDC
#define PSOBB_HWND_PTR (HWND*)0x00ACBED8

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

void PSOBBMain() {
    //window_title_addr = (char*)0x00ACBE70;
    //strcpy_s(window_title_addr, strlen("PHANTASY STAR ONLINE Blue Burst"), "overbudgeted on candles");
    if (oPSOBB_Main != nullptr) {
        oPSOBB_Main();
    }
}

// hack to work around __thiscall not being usable
void __fastcall PSOBB_InitD3D(void* self, void* notused) {
    if (oPSOBB_InitD3D != nullptr) {
        // We have to hot-patch the device creation function so that it has FPU_PRESERVE on _EVERY POSSIBLE INVOCATION_
        // Otherwise we're in for a world of hurt in the LuaJIT runtime.
        /*
        BYTE* push_behavior_flags;
        push_behavior_flags = (BYTE*)0x00837C58;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x00837DC1;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x00837E23;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x00837FF4;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x00838167;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x008381D3;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x0083833C;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x0083839E;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x0083850A;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x0083856F;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;
        push_behavior_flags = (BYTE*)0x008386E2;
        push_behavior_flags[1] |= (BYTE)D3DCREATE_FPU_PRESERVE;*/

        oPSOBB_InitD3D(self);
        __asm finit;
    }

    ImGui_ImplD3D8_Init(*PSOBB_HWND_PTR, PSOBB_DIRECT3DDEVICE8_PTR);

    ImGui_ImplD3D8_NewFrame();
}

void Initialize() {
    DWORD base = (DWORD)GetModuleHandle(nullptr);
    auto idh = (PIMAGE_DOS_HEADER)base;
    auto inh = (PIMAGE_NT_HEADERS)(base + idh->e_lfanew);
    auto ioh = &inh->OptionalHeader;
    codeBase = (BYTE*)(base + ioh->BaseOfCode);
    codeEnd = codeBase + ioh->SizeOfCode;
    dataBase = (BYTE*)(base + ioh->BaseOfData);
    dataEnd = dataBase + ioh->SizeOfInitializedData;

    MH_Initialize();

    if (DO_HOOKS) {
        //MH_CreateHook(PSOBB_MAIN, PSOBBMain, (void**)(&oPSOBB_Main));
        //MH_EnableHook(PSOBB_MAIN);
        MH_CreateHook((void*)PSOBB_INITD3D, PSOBB_InitD3D, (void**)(&oPSOBB_InitD3D));
        MH_EnableHook((void*)PSOBB_INITD3D);
    }

    if (!oDirectInput8Create)
    {
        CHAR syspath[MAX_PATH];
        GetSystemDirectory(syspath, MAX_PATH);
        strcat_s(syspath, "\\dinput8.dll");
        HMODULE hMod = LoadLibrary(syspath);
        oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
    }
    if (!oDirect3DCreate8) {
        CHAR syspath[MAX_PATH];
        GetSystemDirectory(syspath, MAX_PATH);
        strcat_s(syspath, "\\d3d8.dll");
        HMODULE hMod = LoadLibrary(syspath);
        oDirect3DCreate8 = (tDirect3DCreate8)GetProcAddress(hMod, "Direct3DCreate8");
    }

    psolua_initialize_state();
}

void Uninitialize() {
    MH_Uninitialize();
    lua_close(g_LuaState);
    g_LuaState = nullptr;
    //PHYSFS_deinit();
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
    GUID aguid = {
        3212410928, 18490, 19874, { 170, 153, 93, 100, 237, 54, 151, 0 }
    };

    if (r_iid == aguid) {
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
