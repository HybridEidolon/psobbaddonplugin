#pragma once

#include <vector>

#include "sol.hpp"

struct AddonDescriptor {
    std::string name = "";
    std::string version = "";
    std::string author = "";
    int status = 0;
};

std::vector<AddonDescriptor>& psolua_addons();
void psolua_add_failed_addon(std::string& dir_name);
void psolua_install_hooks(lua_State * L);

void psolualib_on_present(sol::function cb);
void psolualib_on_init(sol::function cb);
void psolualib_on_key_pressed(sol::function cb);
void psolualib_on_key_released(sol::function cb);

// Called before Direct3D8 Present function is called.
void psoluah_Present(void);
// Calls all the init callbacks and stores their return values into the addons list.
void psoluah_Init(void);

void psoluah_KeyPressed(int key_code);

void psoluah_KeyReleased(int key_code);
