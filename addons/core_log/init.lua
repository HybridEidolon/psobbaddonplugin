local core_mainmenu = require('core_mainmenu')

local init
local present

local mm_button_func
local window_open

local log_items = {}

local add_log_item

local old_print = print

local pack_table = function(...)
  return { n = select('#', ...), ... }
end

local my_print = function(...)
  local s = ''
  local t = pack_table(...)
  for i=1, t.n do
    if (i > 1) then
      s = s .. '\t'
    end
    s = s .. tostring(t[i])
  end
  add_log_item(s)
  old_print(unpack(t))
end

add_log_item = function(s)
  log_items[#log_items+1] = tostring(s)
end

print = my_print

mm_button_func = function()
  window_open = not window_open
end

init = function()
  core_mainmenu.add_button('Log', mm_button_func)
  return {
    name = "Core - Log",
    author = "Eidolon",
    version = "0.3.0",
    description = "Wraps print and provides a viewable log.\nNote, can't log anything until after it is loaded.",
    present = present
  }
end

present = function()
  if (not window_open) then return; end

  local s
  imgui.SetNextWindowSize(400, 300, 'FirstUseEver')
  s, window_open = imgui.Begin('Log', window_open)

  imgui.Text('log')
  for _, v in ipairs(log_items) do
    imgui.TextUnformatted(v)
  end

  imgui.End()
end

return {
  __addon = {
    init = init
  }
}
