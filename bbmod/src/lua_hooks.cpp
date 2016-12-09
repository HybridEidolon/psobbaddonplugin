#include "lua_hooks.h"

#include <Windows.h>

#include "sol.hpp"
#include "luastate.h"
#include "log.h"
#include "lua_psolib.h"

struct Hook {
	int id;
};

static std::vector<Hook> present_hooks;
static std::vector<Hook> init_hooks;
static std::vector<Hook> key_pressed_hooks;
static std::vector<Hook> key_released_hooks;

static int present_hook_counter = 1;
static int init_hook_counter = 1;
static int key_pressed_hook_counter = 1;
static int key_released_hook_counter = 1;

static std::vector<AddonDescriptor> addons;

std::vector<AddonDescriptor>& psolua_addons() {
	return addons;
}

void psolua_add_failed_addon(std::string & dir_name) {
	AddonDescriptor addon;
	addon.name = dir_name;
	addon.version = "error";
	addon.author = "see log";
	addon.status = 1;
	addons.push_back(addon);
}

void psolualib_on_present(sol::function cb) {
	sol::state_view lua(cb.lua_state());
	Hook h;
	h.id = init_hook_counter++;
	sol::table present = lua["hooks"]["present"];
	present[h.id] = cb;
	present_hooks.push_back(h);
}

void psolualib_on_init(sol::function cb) {
	sol::state_view lua(cb.lua_state());
	Hook h;
	h.id = init_hook_counter++;
	sol::table init = lua["hooks"]["init"];
	init[h.id] = cb;
	init_hooks.push_back(h);
}

void psolualib_on_key_pressed(sol::function cb) {
	sol::state_view lua(cb.lua_state());
	Hook h;
	h.id = key_pressed_hook_counter++;
	sol::table key_pressed = lua["hooks"]["key_pressed"];
	key_pressed[h.id] = cb;
	key_pressed_hooks.push_back(h);
}

void psolualib_on_key_released(sol::function cb) {
	sol::state_view lua(cb.lua_state());
	Hook h;
	h.id = key_released_hook_counter++;
	sol::table key_released = lua["hooks"]["key_released"];
	key_released[h.id] = cb;
	key_released_hooks.push_back(h);
}

void psolua_install_hooks(lua_State * L) {
	sol::state_view lua(L);

	// Hook table
	sol::table hooks = lua.create_named_table("hooks");
	hooks["present"] = lua.create_table();
	hooks["init"] = lua.create_table();
	hooks["key_pressed"] = lua.create_table();
	hooks["key_released"] = lua.create_table();
	init_hooks.clear();
	present_hooks.clear();
	key_pressed_hooks.clear();
	key_released_hooks.clear();
}

void psoluah_Present(void) {
	sol::state_view lua(g_LuaState);

	sol::table hooks = lua["hooks"];
	sol::table present = hooks["present"];
	for (auto i = present_hooks.begin(); i != present_hooks.end(); i++) {
		sol::protected_function func(present[i->id], lua["pso"]["error_handler"]);
		func();
	}
}

void psoluah_Init(void) {
	sol::state_view lua(g_LuaState);

	sol::table hooks = lua["hooks"];
	sol::table init = hooks["init"];
	for (auto i = init_hooks.begin(); i != init_hooks.end(); i++) {
		sol::protected_function func(init[i->id], lua["pso"]["error_handler"]);
		sol::optional<sol::table> v = func();
		if (v) {
			AddonDescriptor desc;
			sol::table value = v.value();

			desc.name = value["name"];
			desc.version = value["version"];
			std::string author = value["author"];
			desc.author = author;
			desc.status = 0;
			addons.push_back(desc);
		}
	}
}

void psoluah_KeyPressed(int key_code) {
	sol::state_view lua(g_LuaState);

	sol::table hooks = lua["hooks"];
	sol::table key_pressed = hooks["key_pressed"];
	for (auto i : key_pressed_hooks) {
		sol::protected_function func(key_pressed[i.id], lua["pso"]["error_handler"]);
		func(key_code);
	}
}

void psoluah_KeyReleased(int key_code) {
	sol::state_view lua(g_LuaState);

	sol::table hooks = lua["hooks"];
	sol::table key_released = hooks["key_released"];
	for (auto i : key_released_hooks) {
		sol::protected_function func(key_released[i.id], lua["pso"]["error_handler"]);
		func(key_code);
	}
}