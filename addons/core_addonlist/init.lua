-- Core Addon - Addon List

local psointernal = require('psointernal')
local core_mainmenu = require('core_mainmenu')

local init
local present
local button_func

local window_open = false

button_func = function()
  window_open = not window_open
end

init = function()
  core_mainmenu.add_button('Addons', button_func)
  return {
    name = "Core - Addon List",
    author = "Eidolon",
    version = "0.3.2",
    description = "Shows a list of detected addons to the user.",
    present = present,
    toggleable = false,
  }
end

present = function()
  if (window_open) then
    local status
    imgui.SetNextWindowSize(300, 200, 'FirstUseEver')
    status, window_open = imgui.Begin('Addons', window_open)

    imgui.Columns(3)
    imgui.Separator()
    imgui.Text('Name'); imgui.NextColumn()
    imgui.Text('Version'); imgui.NextColumn()
    imgui.Text('Author'); imgui.NextColumn()
    -- imgui.Text('Toggle'); imgui.NextColumn()
    imgui.Separator()

    for _, v in pairs(psointernal.get_addons()) do
      local do_desc_tooltip = function()
        if (imgui.IsItemHovered() and type(v.meta.description) == 'string') then
          imgui.BeginTooltip()
          imgui.TextUnformatted(v.meta.description)
          imgui.EndTooltip()
        end
      end

      if (not v.meta.loaded) then
        imgui.PushStyleColor('Text', 1, 0, 0, 1)
      end

      imgui.TextUnformatted(v.meta.name)
      do_desc_tooltip()
      imgui.NextColumn()

      imgui.TextUnformatted(v.meta.version)
      do_desc_tooltip()
      imgui.NextColumn()

      imgui.TextUnformatted(v.meta.author)
      do_desc_tooltip()
      imgui.NextColumn()

      if (not v.meta.loaded) then
        imgui.PopStyleColor()
      end

      -- if (v.meta.toggleable) then
      --   local ss
      --   local do_enabled = v.meta.enabled
      --   ss, do_enabled = imgui.Selectable('Enable', do_enabled)
      --   if (do_enabled ~= v.meta.enabled) then
      --     psointernal.set_addon_enabled(k, do_enabled)
      --   end
      --   imgui.NextColumn()
      -- else
      --   imgui.NextColumn()
      -- end

    end
    imgui.End()
  end
end

return {
  __addon = {
    init = init
  }
}
