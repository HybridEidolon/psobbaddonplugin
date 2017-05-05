local addons = {}

local present_hooks = {}
local key_pressed_hooks = {}
local key_released_hooks = {}

local log_items = {}

-- Note: this does not check toggleable field of addon metadata.
-- Use this with care.
local function set_addon_enabled(path, enabled)
  addons[path].meta.enabled = enabled
  if (present_hooks[path]) then
    present_hooks[path].enabled = enabled
  end
  if (key_pressed_hooks[path]) then
    key_pressed_hooks[path].enabled = enabled
  end
  if (key_released_hooks[path]) then
    key_released_hooks[path].enabled = enabled
  end
end

local function make_hook(fn)
  return {
    enabled = true,
    fn = fn
  }
end

local function on_init()
  local dirs = pso.list_addon_directories()
  local loaded_addons = {}

  -- require each module
  for _, v in ipairs(dirs) do
    local status, module = pcall(function() return require(v); end)
    if (status and module.__addon ~= nil and type(module.__addon.init) == 'function') then
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
          name = val.name or addon.path,
          version = val.version or '?',
          author = val.author or '?',
          description = val.description,
          toggleable = val.toggleable or false,
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
          toggleable = false,
          loaded = false,
          enabled = false
        }
      end
    end
  end

  addons = loaded_addons
end

local function on_present()
  for a, v in pairs(present_hooks) do
    if (v.enabled) then
      local status, ret = pcall(v.fn)
      if (not status) then
        print('Addon ' .. a .. ' present handler errored; disabling addon')
        pso.error_handler(ret)
        set_addon_enabled(a, false)
      end
    end
  end
end

local function on_key_pressed(key)
  for a, v in pairs(key_pressed_hooks) do
    if (v.enabled) then
      local status, ret = pcall(function() v.fn(key); end)
      if (not status) then
        print('Addon ' .. a .. ' key_pressed handler errored; disabling addon')
        pso.error_handler(ret)
        set_addon_enabled(a, false)
      end
    end
  end
end

local function on_key_released(key)
  for a, v in pairs(key_released_hooks) do
    if (v.enabled) then
      local status, ret = pcall(function() v.fn(key); end)
      if (not status) then
        print('Addon ' .. a .. ' key_released handler errored; disabling addon')
        pso.error_handler(ret)
        set_addon_enabled(a, false)
      end
    end
  end
end

local function on_log(text)
  -- Addons aren't allowed to hook log.
  -- However, they can observe log_items.
  log_items[#log_items+1] = {'PRINT', text}
end

local function on_unhandled_error(msg)
  log_items[#log_items+1] = {'FATAL', 'unhandled error: ' .. msg .. '\n' .. debug.traceback()}
end

local function get_addons()
  return addons
end


return {
  on_init = on_init,
  on_present = on_present,
  on_key_pressed = on_key_pressed,
  on_key_released = on_key_released,
  on_log = on_log,
  on_unhandled_error = on_unhandled_error,
  get_addons = get_addons,
  set_addon_enabled = set_addon_enabled,
  log_items = log_items,
}
