local core_mainmenu = require('core_mainmenu')
local psointernal = require('psointernal')
local __ = require('util.underscore')

local init
local present

local mm_button_func
local window_open

local last_log_items_length = 0

mm_button_func = function()
  window_open = not window_open
end

init = function()
  core_mainmenu.add_button('Log', mm_button_func)
  return {
    name = "Core - Log",
    author = "Eidolon",
    version = "0.3.2",
    description = "Provides a log window for all log items to the pso_on_log callback.",
    present = present,
    toggleable = false,
  }
end

local filter_value = ''

local function filterfunc(v)
  return string.find(tostring(v[2]), filter_value)
end

present = function()
  if (not window_open) then return; end

  local s
  imgui.SetNextWindowSize(500, 400, 'FirstUseEver')
  s, window_open = imgui.Begin('Log', window_open)

  -- Clear button
  if (imgui.Button('Clear')) then
    __.clear(psointernal.log_items)
  end
  imgui.SameLine()

  s, filter_value = imgui.InputText('Filter', filter_value, 200)

  imgui.Separator()

  imgui.BeginChild('scrolling', 0, 0, false, {'HorizontalScrollbar'})

  for _, v in ipairs(__.filter(psointernal.log_items, filterfunc)) do
    imgui.TextUnformatted('[' .. tostring(v[1]) .. '] ' .. tostring(v[2]))
  end

  if (#psointernal.log_items > last_log_items_length) then
    imgui.SetScrollHere(1)
  end
  imgui.EndChild()

  imgui.End()

  last_log_items_length = #psointernal.log_items
end

return {
  __addon = {
    init = init
  }
}
