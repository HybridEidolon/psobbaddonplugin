local addons = {}

local present_hooks = {}
local key_pressed_hooks = {}
local key_released_hooks = {}

local log_items = {}

local balance_all_stacks

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

local function override_stack_functions()
  local push_pop_functions = {
    { push = "Begin",                     pop = "End" },
    { push = "Begin_2",                   pop = "End" },
    { push = "BeginChild",                pop = "EndChild" },
    { push = "BeginChild_2",              pop = "EndChild" },
    { push = "BeginChildFrame",           pop = "EndChildFrame" },
    { push = "BeginGroup",                pop = "EndGroup" },
    { push = "BeginMainMenuBar",          pop = "EndMainMenuBar" },
    { push = "BeginMenu",                 pop = "EndMenu" },
    { push = "BeginMenuBar",              pop = "EndMenuBar" },
    { push = "BeginPopup",                pop = "EndPopup" },
    { push = "BeginPopupContextMenuItem", pop = "EndPopup" },
    { push = "BeginPopupContextVoid",     pop = "EndPopup" },
    { push = "BeginPopupContextWindow",   pop = "EndPopup" },
    { push = "BeginPopupModal",           pop = "EndPopup" },
    { push = "BeginTooltip",              pop = "EndTooltip" },
    { push = "PushAllowKeyboardFocus",    pop = "PopAllowKeyboardFocus" },
    { push = "PushButtonRepeat",          pop = "PopButtonRepeat" },
    { push = "PushClipRect",              pop = "PopClipRect" },
    { push = "PushID",                    pop = "PopID" },
    { push = "PushID_2",                  pop = "PopID" },
    { push = "PushID_4",                  pop = "PopID" },
    { push = "PushItemWidth",             pop = "PopItemWidth" },
    { push = "PushStyleColor",            pop = "PopStyleColor" },
    { push = "PushStyleVar",              pop = "PopStyleVar" },
    { push = "PushStyleVar_2",            pop = "PopStyleVar" },
    { push = "PushTextWrapPos",           pop = "PopTextWrapPos" },
    { push = "TreePush",                  pop = "TreePop" },
    { push = "TreePush_2",                pop = "TreePop" },
    { push = "TreeNode",                  pop = "TreePop", cond = true },
    { push = "TreeNode_2",                pop = "TreePop", cond = true },
    { push = "TreeNodeEx",                pop = "TreePop", cond = true },
    { push = "TreeNodeEx_2",              pop = "TreePop", cond = true }
  }

  local pop_stack = {}

  local override_push_function = function(push, pop, cond)
    local push_function = imgui[push]
    local pop_function = imgui[pop]

    local override

    if not cond then
      override = function(...)
        table.insert(pop_stack, pop_function)
        return push_function(...)
      end
    else
      override = function(...)
        if push_function(...) then
          table.insert(pop_stack, pop_function)
          return true
        end

        return false
      end
    end

    imgui[push] = override
  end

  local override_pop_function = function(pop)
    local pop_function = imgui[pop]

    local override = function(count, ...)
      local _count = count or 1

      for i=1,_count do
        local pop_stack_function = table.remove(pop_stack)

        while pop_stack_function ~= nil and pop_stack_function ~= pop_function do
          pop_stack_function()
          pop_stack_function = table.remove(pop_stack)
        end

        if pop_stack_function ~= nil then
          pop_stack_function()
        else
          error(string.format("Push/Pop mismatch (imgui.%s())", pop))
        end
      end
    end

    imgui[pop] = override
  end

  for i,push_pop_function in ipairs(push_pop_functions) do
    local push = push_pop_function.push
    local pop = push_pop_function.pop
    local cond = push_pop_function.cond or false
    override_push_function(push, pop, cond)
  end

  local hash = {}
  for i,push_pop_function in ipairs(push_pop_functions) do
    local pop = push_pop_function.pop
    if not hash[pop] then
      override_pop_function(pop)
      hash[pop] = true
    end
  end

  return function()
    local pop_function = table.remove(pop_stack)
    while pop_function ~= nil do
      pop_function()
      pop_function = table.remove(pop_stack)
    end
  end
end

local function on_init()
  local dirs = pso.list_addon_directories()
  local loaded_addons = {}

  -- override functions to enable automatic balancing
  -- of imgui stacks to correct for addons that crash
  -- or those that do not properly balance their stacks
  balance_all_stacks = override_stack_functions()

  -- require each module
  for _, v in ipairs(dirs) do
    local status, module = xpcall(function() return require(v); end, debug.traceback)
    if (status and module.__addon ~= nil and type(module.__addon.init) == 'function') then
      loaded_addons[v] = {
        path = v,
        module = module
      }
    end
    if (not status) then
      print("Failed to load module from directory " .. v)
      pso.error_handler(module)
    end
  end

  -- call init for each addon
  for _, addon in pairs(loaded_addons) do
    if (addon.module.__addon ~= nil and type(addon.module.__addon.init) == 'function') then
      local success, val = xpcall(addon.module.__addon.init, debug.traceback)

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
        pso.error_handler(val)

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
      local status, ret = xpcall(v.fn, debug.traceback)
      if (not status) then
        print('Addon ' .. a .. ' present handler errored; disabling addon')
        pso.error_handler(ret)
        set_addon_enabled(a, false)
      end
      balance_all_stacks()
    end
  end
end

local function on_key_pressed(key)
  for a, v in pairs(key_pressed_hooks) do
    if (v.enabled) then
      local status, ret = xpcall(function() v.fn(key); end, debug.traceback)
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
      local status, ret = xpcall(function() v.fn(key); end, debug.traceback)
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
  log_items[#log_items+1] = {'FATAL', 'unhandled error: ' .. msg}
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
