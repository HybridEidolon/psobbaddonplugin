#pragma once

#include <vector>

struct lua_State;

extern struct lua_State* g_LuaState;

void psolua_push_key_pressed(int key_code);
void psolua_push_key_released(int key_code);
void psolua_process_key_events(void);
