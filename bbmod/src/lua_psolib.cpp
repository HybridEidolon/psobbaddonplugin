#include "lua_psolib.h"

#include <Windows.h>
#include "sol.hpp"

#include "lua_hooks.h"
#include "luastate.h"
#include "log.h"
#include "wrap_imgui_impl.h"

static int wrap_exceptions(lua_State *L, lua_CFunction f);
static int psolua_print(lua_State *L);
static void _load_addons(lua_State* L);
static std::string psolualib_read_cstr(int memory_address, int len = 2048);
static std::string psolualib_read_wstr(int memory_address, int len = 1024);
static sol::table psolualib_read_mem(sol::table t, int memory_address, int len);

bool psolua_initialize_on_next_frame = false;

// Catch C++ exceptions and convert them to Lua error messages.
// Customize as needed for your own exception classes.
static int wrap_exceptions(lua_State *L, lua_CFunction f) {
	try {
		return f(L);  // Call wrapped function and return result.
	}
	catch (const char *s) {  // Catch and convert exceptions.
		lua_pushstring(L, s);
	}
	catch (std::exception& e) {
		lua_pushstring(L, e.what());
	}
	catch (...) {
		lua_pushliteral(L, "caught (...)");
	}
	return lua_error(L);  // Rethrow as a Lua error.
}

std::string psolualib_error_handler(std::string msg) {
	sol::state_view lua(g_LuaState);

	try {
		g_log << "uncaught error: " << msg << std::endl;
		std::string traceback = lua["debug"]["traceback"]();
		g_log << traceback << std::endl;
	}
	catch (...) {
		// do nothing
	}
	return msg;
}

static int psolua_print(lua_State *L) {
	sol::state_view lua(L);
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; i++) {
		sol::object o = sol::stack::get<sol::object>(lua, i);
		std::string out = lua["tostring"](o);
		g_log << out << std::endl;
	}

	return 0;
}

static std::vector<std::string> _get_addon_directory_names(void) {
	HANDLE hFind;
	WIN32_FIND_DATA find;

	std::vector<std::string> ret;

	hFind = FindFirstFileA("addons/*", &find);
	do {
		std::string filename(find.cFileName);
		if (filename == "..") continue;
		if (filename == ".") continue;
		ret.push_back(filename);
	} while (FindNextFileA(hFind, &find));

	FindClose(hFind);

	return ret;
}

// Load addons by require-ing the directory names.
static void _load_addons(lua_State* L) {
	sol::state_view lua(L);

	HANDLE hFind;
	WIN32_FIND_DATA find;

	psolua_addons().clear();

	hFind = FindFirstFileA("addons/*", &find);
	do {
		std::string filename(find.cFileName);
		if (filename == "..") continue;
		if (filename == ".") continue;
		if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			sol::protected_function require = lua["require"];
			require.error_handler = lua["pso"]["error_handler"];
			if (!require(std::string(filename)).valid()) {
				g_log << "Error loading addon " << filename << "." << std::endl;
				psolua_add_failed_addon(filename);
			}
			else {
				g_log << "Addon module " << filename << " loaded." << std::endl;
			}
		}
	} while (FindNextFileA(hFind, &find));

	FindClose(hFind);
}

template <typename T> std::function<T(int)> read_t(void) {
	auto pid = GetCurrentProcess();
	return [=](int addr) {
		char buf[sizeof(T)];
		SIZE_T read;
		if (!ReadProcessMemory(pid, (LPCVOID)addr, (LPVOID)buf, sizeof(T), &read)) {
			throw "ReadProcessMemory error";
		}
		return *(T*)buf;
	};
}

void psolua_load_library(lua_State * L) {
	sol::state_view lua(L);

	lua["print"] = psolua_print;
	sol::table psoTable = lua.create_named_table("pso");
	psoTable["on_present"] = psolualib_on_present;
	psoTable["on_init"] = psolualib_on_init;
	psoTable["on_key_pressed"] = psolualib_on_key_pressed;
	psoTable["on_key_released"] = psolualib_on_key_released;

	psoTable["error_handler"] = psolualib_error_handler;
	psoTable["reload"] = []() { psolua_initialize_on_next_frame = true; };

	psoTable["read_i8"] = read_t<int8_t>();
	psoTable["read_i16"] = read_t<int16_t>();
	psoTable["read_i32"] = read_t<int32_t>();
	psoTable["read_i64"] = read_t<int64_t>();
	psoTable["read_u8"] = read_t<uint8_t>();
	psoTable["read_u16"] = read_t<uint16_t>();
	psoTable["read_u32"] = read_t<uint32_t>();
	psoTable["read_u64"] = read_t<uint64_t>();
	psoTable["read_f32"] = read_t<float>();
	psoTable["read_f64"] = read_t<double>();
	psoTable["read_cstr"] = psolualib_read_cstr;
	psoTable["read_wstr"] = psolualib_read_wstr;
	psoTable["read_mem"] = psolualib_read_mem;

	lua["package"]["path"] = "./addons/?.lua;./addons/?/init.lua;./?.lua;./?/init.lua";

	// Exception handling
	lua_pushlightuserdata(L, (void*)wrap_exceptions);
	luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
	lua_pop(L, 1);

	// ImGui library
	luaopen_imgui(L);

	psolua_install_hooks(L);

	_load_addons(L);
}

void psolua_initialize_state(void) {
	if (g_LuaState != nullptr) {
		lua_close(g_LuaState);
		g_LuaState = nullptr;
	}
	g_LuaState = luaL_newstate();

	if (!g_LuaState) {
		MessageBoxA(nullptr, "LuaJit new state failed.", "Lua error", 0);
		exit(1);
	}

	g_lualog.Clear();
	g_log << "Initializing Lua state" << std::endl;

	sol::state_view lua(g_LuaState);

	luaL_openlibs(g_LuaState);
	psolua_load_library(g_LuaState);

	psoluah_Init();

	psolua_initialize_on_next_frame = false;
}

static std::string psolualib_read_cstr(int memory_address, int len) {
	char buf[8192];
	memset(buf, 0, len);
	SIZE_T read;
	auto pid = GetCurrentProcess();
	if (!ReadProcessMemory(pid, (LPCVOID)memory_address, buf, len, &read)) {
		throw "ReadProcessMemory error";
	}
	return buf;
}

static std::string psolualib_read_wstr(int memory_address, int len) {
	char buf[8192];
	char buf2[8192];
	memset(buf, 0, len * 2);
	memset(buf2, 0, len * 2);
	SIZE_T read;
	auto pid = GetCurrentProcess();
	if (!ReadProcessMemory(pid, (LPCVOID)memory_address, buf, len * 2, &read)) {
		throw "ReadProcessMemory error";
	}
	if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, (LPCWCH)buf, len, buf2, 8192, nullptr, nullptr)) {
		throw "invalid utf-16 string";
	}
	return buf2;
}

static sol::table psolualib_read_mem(sol::table t, int memory_address, int len) {
	sol::state_view lua(g_LuaState);
	unsigned char buf[8192];
	memset(buf, 0, len);
	SIZE_T read;
	auto pid = GetCurrentProcess();
	if (len < 0) {
		throw "length must be greater than 0";
	}
	if (!ReadProcessMemory(pid, (LPCVOID)memory_address, buf, len, &read)) {
		throw "ReadProcessMemory error";
	}
	for (int i = 0; i < len; i++) {
		t.add((int)buf[i]);
	}
	return t;
}
