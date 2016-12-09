#pragma once

#include <string>

struct lua_State;

std::string psolualib_error_handler(std::string msg);
void psolua_load_library(lua_State* L);
void psolua_initialize_state(void);

extern bool psolua_initialize_on_next_frame;
