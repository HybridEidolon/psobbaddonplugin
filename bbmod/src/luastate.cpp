#include "luastate.h"

#include <vector>

#include <Windows.h>

#include "imgui/imgui.h"
#include "log.h"
#include "wrap_imgui_impl.h"
#include "sol.hpp"
#include "lua_hooks.h"

struct lua_State* g_LuaState = nullptr;

struct KeyEvent {
    bool isPressed;
    int keyCode;
};

static std::vector<KeyEvent> key_events;
void psolua_push_key_pressed(int key_code) {
    KeyEvent e{};
    e.isPressed = true;
    e.keyCode = key_code;
    key_events.push_back(e);
}

void psolua_push_key_released(int key_code) {
    KeyEvent e{};
    e.isPressed = false;
    e.keyCode = key_code;
    key_events.push_back(e);
}

void psolua_process_key_events(void) {
    int evts = key_events.size();

    for (auto e : key_events) {
        if (e.isPressed) {
            psoluah_KeyPressed(e.keyCode);
        }
        else {
            psoluah_KeyReleased(e.keyCode);
        }
    }

    key_events.clear();
}
