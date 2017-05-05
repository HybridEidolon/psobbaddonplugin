#include "lua_hooks.h"

#include <Windows.h>

#include "sol.hpp"
#include "luastate.h"
#include "log.h"
#include "lua_psolib.h"

void psoluah_Present(void) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_present"], lua["pso"]["error_handler"]);
    func();
}

void psoluah_Init(void) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_init"], lua["pso"]["error_handler"]);
    func();
}

void psoluah_KeyPressed(int key_code) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_key_pressed"], lua["pso"]["error_handler"]);
    func(key_code);
}

void psoluah_KeyReleased(int key_code) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_key_released"], lua["pso"]["error_handler"]);
    func(key_code);
}

void psoluah_Log(std::string text) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_log"], lua["pso"]["error_handler"]);
    func(text);
}

void psoluah_UnhandledError(std::string msg) {
    sol::state_view lua(g_LuaState);
    sol::protected_function func(lua["pso_on_unhandled_error"], lua["pso"]["error_handler"]);
    func(msg);
}
