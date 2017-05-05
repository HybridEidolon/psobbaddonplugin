local core_mainmenu = require('core_mainmenu')
local psointernal = require('psointernal')

local init
local present

local mm_button_func
local window_open

mm_button_func = function()
  window_open = not window_open
end

init = function()
  core_mainmenu.add_button('Log', mm_button_func)
  return {
    name = "Core - Log",
    author = "Eidolon",
    version = "0.3.0",
    description = "Provides a log window for all log items to the pso_on_log callback.",
    present = present,
    toggleable = false,
  }
end

present = function()
  if (not window_open) then return; end

  local s
  imgui.SetNextWindowSize(400, 300, 'FirstUseEver')
  s, window_open = imgui.Begin('Log', window_open)

  imgui.Text('log')
  for _, v in ipairs(psointernal.log_items) do
    imgui.TextUnformatted('[' .. tostring(v[1]) .. '] ' .. tostring(v[2]))
  end

  imgui.End()
end

return {
  __addon = {
    init = init
  }
}
