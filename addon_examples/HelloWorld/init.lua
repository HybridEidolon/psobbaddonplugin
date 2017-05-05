local function some_publicly_exported_function()
  imgui.Text('Hello, world!')
end

local function present()
  some_publicly_exported_function()
end

local function init()
  return {
    name = 'Hello world',
    version = '1.0',
    author = 'Eidolon',
    present = present,
    toggleable = true,
  }
end

return {
  __addon = {
    init = init,
  },
  some_publicly_exported_function = some_publicly_exported_function,
}
