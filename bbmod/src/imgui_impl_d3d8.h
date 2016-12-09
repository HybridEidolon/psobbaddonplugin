#pragma once

struct IDirect3DDevice8;

bool ImGui_ImplD3D8_Init(void* hwnd, IDirect3DDevice8** device);
void ImGui_ImplD3D8_Shutdown(void);
void ImGui_ImplD3D8_NewFrame(void);