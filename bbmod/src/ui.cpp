#include <Windows.h>

#include <cstdio>
#include "imgui/imgui.h"
#include "imgui_memory_editor.h"
#include "luajit/lua.hpp"
#include "luastate.h"
#include "log.h"
#include <sstream>
#include "sol.hpp"
#include "lua_hooks.h"
#include "lua_psolib.h"

static bool allHidden = true;

static bool testWindowOpen = false;
static bool debugWindowOpen = false;
static bool hostsWindowOpen = false;

static char* loginHost1 = (char*) 0x96EB0C;
static char* loginHost2 = (char*) 0x96EB24;
static char* patchHost1 = (char*) 0x96EB50;
static char* patchHost2 = (char*) 0x96EB50;
static char* patchHost3 = (char*) 0x96EB50;
static char* patchHost4 = (char*) 0x96EB50;
static char* gameVersionAscii = (char*) 0x96E220;
static wchar_t* gameVersionUtf16 = (wchar_t*) 0x96E230;

static void DebugWindow(void) {
	static char utf8VersionStr[40] = "";
	if (utf8VersionStr[0] == 0) {
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, gameVersionUtf16, -1, utf8VersionStr, 40, nullptr, false);
	}

	if (debugWindowOpen) {
		ImGui::Begin("Custom Debug", &debugWindowOpen, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Frames: %d", ImGui::GetFrameCount());
		ImGui::Text("Seconds Elapsed: %f", ImGui::GetTime());
		ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
		ImGui::Text("dear imgui version: %s", ImGui::GetVersion());
		ImGui::Text("Client Version: %s", gameVersionAscii);
		ImGui::Text("Client Version (UTF16): %s", utf8VersionStr);
		
		ImGui::End();
	}
}

static void HostsWindow(void) {
	if (hostsWindowOpen) {
		ImGui::Begin("Hosts", &hostsWindowOpen, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Login1: %s", loginHost1);
		ImGui::Text("Login2: %s", loginHost2);
		ImGui::Text("Patch1: %s", patchHost1);
		ImGui::Text("Patch2: %s", patchHost2);
		ImGui::Text("Patch3: %s", patchHost3);
		ImGui::Text("Patch4: %s", patchHost4);
		ImGui::End();
	}
}

static void TestWindow(void) {
	if (testWindowOpen) {
		ImGui::ShowTestWindow(&testWindowOpen);
	}
}

static bool luaReplOpen = false;

static void LuaRepl(void) {
	static char buf[1024];

	if (luaReplOpen) {
		ImGui::SetNextWindowSize(ImVec2(400, 80), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Lua REPL", &luaReplOpen);
		if (ImGui::InputText("Lua", buf, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
			
			luaL_dostring(g_LuaState, buf);
			lua_settop(g_LuaState, 0);
			memset(buf, 0, 1024);
		}
		ImGui::End();
	}
}

static bool memoryViewOpen = false;

static bool luaLogOpen = false;

static bool addonListOpen = false;

static void MemoryView(void) {
	static BYTE* mem = 0;
	static struct MemoryEditor memory_editor;

	memory_editor.AllowEdits = false;

	if (memoryViewOpen) {
		DWORD base = (DWORD)GetModuleHandle(nullptr);
		auto idh = (PIMAGE_DOS_HEADER)base;
		auto inh = (PIMAGE_NT_HEADERS)(base + idh->e_lfanew);
		auto ioh = &inh->OptionalHeader;
		auto codeBase = (BYTE*)(base + ioh->BaseOfCode);
		auto codeEnd = codeBase + ioh->SizeOfCode;
		auto dataBase = (BYTE*)(base + ioh->BaseOfData);
		auto dataEnd = dataBase + ioh->SizeOfInitializedData;
		if (mem < dataBase) {
			mem = dataBase;
		}

		memory_editor.Open = memoryViewOpen;
		memory_editor.Draw("Memory View", (BYTE*)0x00400000, (BYTE*)0x7fffffff- (BYTE*)0x00400000, (size_t)0x00400000);
		memoryViewOpen = memory_editor.Open;
	}
}

static bool aboutWindowOpen = false;

static void AboutWindow() {
	if (aboutWindowOpen) {
		ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin("About", &aboutWindowOpen)) {
			ImGui::End();
			return;
		}

		ImGui::TextWrapped(R"foo(BB Addon Plugin
By Eidolon (@HybridEidolon)

USE THIS MOD AT YOUR OWN RISK. All addons have full Lua standard library access and are not sandboxed. Only run addons you trust. The creators are not liable for any damages you incur by using the plugin or any addon made for it.
)foo");

		ImGui::End();
	}
}

static bool helpWindowOpen = false;

static void HelpWindow() {
	if (helpWindowOpen) {
		ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin("Help", &helpWindowOpen)) {
			ImGui::End();
			return;
		}
		ImGui::TextWrapped(R"foo(Help

Press ` to open and close the main window.

All keyboard and mouse input is intercepted whenever ImGui components are in focus. Click outside of the UI to un-focus it and restore control to the game. Gamepad input will work regardless of focus state.

Addons have read-only access to any section of memory; when an exception occurs, Lua will raise an error. An addon throwing a Lua error will currently crash the game.

Please read the readme for documentation on how to make addons.
)foo");

		ImGui::End();
	}
}

static void MainWindow() {
	if (allHidden) {
		if (!ImGui::Begin("ModUI", &allHidden)) {
			ImGui::End();
			return;
		}
		if (ImGui::Selectable("About", aboutWindowOpen)) {
			aboutWindowOpen = !aboutWindowOpen;
		}
		if (ImGui::Selectable("Help", helpWindowOpen)) {
			helpWindowOpen = !helpWindowOpen;
		}
		//if (ImGui::Selectable("Test Window", testWindowOpen)) {
		//	testWindowOpen = !testWindowOpen;
		//}
		//if (ImGui::Selectable("Debug Window", debugWindowOpen)) {
		//	debugWindowOpen = !debugWindowOpen;
		//}
		if (ImGui::Selectable("Memory View", memoryViewOpen)) {
			memoryViewOpen = !memoryViewOpen;
		}
		//if (ImGui::Selectable("Hosts", hostsWindowOpen)) {
		//	hostsWindowOpen = !hostsWindowOpen;
		//}
		if (ImGui::Selectable("Addons", addonListOpen)) {
			addonListOpen = !addonListOpen;
		}
		//if (ImGui::Selectable("REPL", luaReplOpen)) {
		//	luaReplOpen = !luaReplOpen;
		//}
		if (ImGui::Selectable("Log", luaLogOpen)) {
			luaLogOpen = !luaLogOpen;
		}
		if (ImGui::Button("Reload")) {
			psolua_initialize_on_next_frame = true;
		}
		if (ImGui::Button("Exit Game")) {
			exit(0);
		}

		ImGui::End();
	}
	
}

void ModUI_Main(void) {
	MainWindow();
	AboutWindow();
	HelpWindow();
	//TestWindow();
	//DebugWindow();
	MemoryView();
	//HostsWindow();
	//LuaRepl();
	if (luaLogOpen) g_lualog.Draw("Lua Log", &luaLogOpen);
	if (addonListOpen) {
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Addons", &addonListOpen);

		std::vector<AddonDescriptor>& addons = psolua_addons();
		ImGui::Columns(3);
		ImGui::Separator();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Version"); ImGui::NextColumn();
		ImGui::Text("Author"); ImGui::NextColumn();
		ImGui::Separator();
		for (auto i = addons.begin(); i != addons.end(); i++) {
			if (i->status != 0) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			}
			ImGui::Text(i->name.c_str()); ImGui::NextColumn();
			ImGui::Text(i->version.c_str()); ImGui::NextColumn();
			ImGui::Text(i->author.c_str()); ImGui::NextColumn();
			if (i->status != 0) {
				ImGui::PopStyleColor();
			}
			ImGui::Separator();
		}
		ImGui::Columns(1);

		ImGui::End();
	}

	if (ImGui::IsKeyPressed(192)) {
		allHidden = true;
	}
}