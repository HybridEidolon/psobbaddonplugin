#pragma once

#include <Windows.h>
#include <string>

struct lua_State;

extern DWORD g_PSOBaseAddress;

std::string psolualib_error_handler(std::string msg);
void psolua_load_library(lua_State* L);
void psolua_initialize_state(void);

extern bool psolua_initialize_on_next_frame;

struct FPUSTATE {
    char state[256];
};

void psolua_store_fpu_state(struct FPUSTATE& fpustate);
void psolua_restore_fpu_state(struct FPUSTATE& fpustate);
