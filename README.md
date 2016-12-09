# BB UI addons plugin

## UI integration

Press \` to open the ImGui-based UI mod. (Note: may be turned on by default in the future)

**The API is completely unstable for now, expect it to change.**

ImGui is exposed via the `imgui` module, which is in the global environment as `imgui`.

PSO specific functions are in the `pso` global table.

## Lua Addon layout

    +addons/MyAddon
    |--+init.lua
    |--+utils.lua

`init.lua` is evaluated on startup to register hooks and evaluate. Any other file can be loaded with `local utils = require("utils")`, for example if you want to be able to call `utils.myfunc` then

`utils.lua`:

```lua
local myfunc = function()
  print('my func')
  return 4
end

return {
  myfunc = myfunc
}
```

`init.lua`:

```lua
local utils = require("utils")

pso.on_present(utils.myfunc)
```

### Hooks

You can register a callback function on various hooks.

 * `pso.on_init(f)` - `f` is called on addon initialization. Return a table containing `name`, `author`, and `version` to show up in the addon list.
 * `pso.on_present(f)` - `f` is called every time d3d8's Present function is called, once per frame.
 * `pso.on_key_pressed(f)` - `f(key_code)` is called every time a key is pressed.
 * `pso.on_key_released(f)` - `f(key_code)` is called every time a key is released.

### Examples

Hello world

```lua
pso.on_present(function()
  print('Hello world!')
end)
```

ImGui hello world

```lua
local imgui_demo_window = true;

function imgui_demo()
  local status

  if imgui_demo_window then
    status, imgui_demo_window = imgui.Begin("ImGui Hello World", imgui_demo_window, {"AlwaysAutoResize"})
    imgui.Text("Hello, world");
    imgui.End()
  end
end

pso.on_present(imgui_demo)
```

Addon definition.

```lua
pso.on_init(function()
  return {
    name = "MyAddon",
    version = "1.0",
    author = "Eidolon"
  }
end)
```

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
 * on_present
 * on_init -- pass a function that returns a table with `name`, `version`, `author` to show up in the addon list
 * on_key_pressed
 * on_key_released
