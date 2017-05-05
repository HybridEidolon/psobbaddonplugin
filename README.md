# BB UI addons plugin

**The API is completely unstable for now, expect it to change.**

ImGui is exposed via the `imgui` module, which is in the global environment as `imgui`.

PSO specific functions are in the `pso` global table.

### API Docs

* Lua 5.1: https://www.lua.org/manual/5.1/
* LuaJit Extensions: http://bitop.luajit.org/api.html
* ImGui: https://github.com/ocornut/imgui
* ImGui API (C++ Header): https://github.com/ocornut/imgui/blob/master/imgui.h

`pso` global table:

 * read_u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, -- read mem at address as type (little endian)
 * read_cstr(addr, len) -- read c_str at address with len bytes, or null terminated (0 len)
 * read_wstr(addr, len) -- read utf16 str to utf8 at address with len characters, or double null terminated (0 len)
 * read_mem(table, addr, len) -- read len bytes from addr into table. the table should be initialized as empty, i.e. `local table = {}; pso.read_mem(table, 0x00400000, 0x7fffffff-0x00400000)` (don't read the entire address space, that's silly and will probably kill the process)
 * reload() -- at the end of present, re-initialize the lua state. all addons and modules will be reloaded, no state will be preserved.
