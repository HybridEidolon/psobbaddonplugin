local addons = {}

local present_hooks = {}
local key_pressed_hooks = {}
local key_released_hooks = {}

local function make_hook(fn)
  return {
    enabled = true,
    fn = fn
  }
end

local function on_init()
  local dirs = io.popen('dir addons /b /ad'):lines()
  local loaded_addons = {}

  -- require each module
  for _, v in ipairs(dirs) do
    local status, module = pcall(function() return require(v); end)
    if (status and module.__addon ~= nil and type(addon.module.__addon.init) == 'function') then
      loaded_addons[v] = {
        path = v,
        module = module
      }
    end
  end

  -- call init for each addon
  for _, addon in pairs(loaded_addons) do
    if (addon.module.__addon ~= nil and type(addon.module.__addon.init) == 'function') then
      local success, val = pcall(addon.module.__addon.init)

      if (success) then
        addon.meta = {
          path = addon.path,
          name = val.name,
          version = val.version,
          author = val.author,
          loaded = true,
          enabled = true
        }

        if (type(val.present) == 'function') then
          present_hooks[addon.path] = make_hook(val.present)
        end
        if (type(val.key_pressed) == 'function') then
          key_pressed_hooks[addon.path] = make_hook(val.key_pressed)
        end
        if (type(val.key_released) == 'function') then
          key_released_hooks[addon.path] = make_hook(val.key_released)
        end
      else
        addon.meta = {
          path = addon.path,
          name = addon.path,
          version = 'error',
          author = 'see log',
          loaded = false,
          enabled = false
        }
      end
    end
  end

  addons = loaded_addons
end

local function on_present()
  for _, v in pairs(present_hooks) do
    if (v.enabled) then
      v.fn()
    end
  end
end

local function on_key_pressed(key)
  for _, v in pairs(key_pressed_hooks) do
    if (v.enabled) then
      v.fn(key)
    end
  end
end

local function on_key_released(key)
  for _, v in pairs(key_released_hooks) do
    if (v.enabled) then
      v.fn(key)
    end
  end
end

local function get_addons()
  return addons
end

local function set_addon_enabled(path, enabled)
  addons[path].meta.enabled = enabled
  present_hooks[path].enabled = enabled
  key_pressed_hooks[path].enabled = enabled
  key_released_hooks[path].enabled = enabled
end

return {
  on_init = on_init,
  on_present = on_present,
  on_key_pressed = on_key_pressed,
  on_key_released = on_key_released,
  get_addons = get_addons
}
