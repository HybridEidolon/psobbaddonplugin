#pragma once

#include <dinput.h>

class ImguiDInput : IDirectInput8A {
public:
    IDirectInput8A* inner;
    ImguiDInput() : ImguiDInput(nullptr) {}
    ImguiDInput(IDirectInput8A* inner) : inner(inner) {
        if (inner != nullptr) {
            inner->AddRef();
        }
    }

    virtual ~ImguiDInput() {
        if (inner != nullptr) {
            inner->Release();
        }
    }

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    /*** IDirectInput8A methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICE8A *, LPUNKNOWN);
    STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
    STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
    STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
    STDMETHOD(FindDevice)(THIS_ REFGUID, LPCSTR, LPGUID);
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR, LPDIACTIONFORMATA, LPDIENUMDEVICESBYSEMANTICSCBA, LPVOID, DWORD);
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMSA, DWORD, LPVOID);
};

class ImguiDInputDevice : IDirectInputDevice8A {
public:
    IDirectInputDevice8A* device;
    bool isKeyboard;
    int mouseX;
    int mouseY;
    int mouseZ;
    ImguiDInputDevice() : ImguiDInputDevice(nullptr, false) {}
    ImguiDInputDevice(IDirectInputDevice8A* device, bool isKeyboard) : device(device), isKeyboard(isKeyboard) {
        mouseX = 0;
        mouseY = 0;
        mouseZ = 0;
        if (device != nullptr) {
            device->AddRef();
        }

    }
    virtual ~ImguiDInputDevice() {
        if (device != nullptr) {
            device->Release();
        }
    }

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    /*** IDirectInputDevice8W methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS);
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
    STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER);
    STDMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER);
    STDMETHOD(Acquire)(THIS);
    STDMETHOD(Unacquire)(THIS);
    STDMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID);
    STDMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
    STDMETHOD(SetEventNotification)(THIS_ HANDLE);
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA);
    STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
    STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);
    STDMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD);
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA, REFGUID);
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
    STDMETHOD(Poll)(THIS);
    STDMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR, DWORD, LPDIFILEEFFECT, DWORD);
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATA, LPCSTR, DWORD);
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATA, LPCSTR, DWORD);
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERA);
};
