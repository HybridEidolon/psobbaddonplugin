-- Core Addon - Main Menu

local present
local init
local key_pressed

local hooked_buttons = {}
local add_button
local remove_button

local window_open = true

init = function()
  return {
    name = "Core - Main Menu",
    author = "Eidolon",
    version = "0.3.0",
    description = "An addon main menu with hooks for adding buttons.",
    present = present,
    key_pressed = key_pressed
  }
end

key_pressed = function(key)
  if (key == 192) then
    window_open = not window_open
  end
end

present = function()
  if (not window_open) then
    return
  end
  local status
  status, window_open = imgui.Begin('Main', window_open)
  for name, func in pairs(hooked_buttons) do
    if (imgui.Button(name)) then
      func()
    end
  end
  if (imgui.Button('Reload')) then
    pso.reload()
  end
  imgui.End()
end

add_button = function(name, func)
  hooked_buttons[name] = func
end

remove_button = function(name, func)
  hooked_buttons[name] = nil
end

return {
  __addon = {
    init = init
  },
  add_button = add_button,
  remove_button = remove_button
}
