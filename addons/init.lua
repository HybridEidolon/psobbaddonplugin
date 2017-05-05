-- Addon environment initialization
-- Do not change unless you know what you're doing!

package.path = './addons/?/init.lua;./addons/?.lua'

local psointernal = require('psointernal')

-- Globally defined functions
-- The plugin calls directly into these functions by name

function pso_on_init()
  psointernal.on_init()
end

function pso_on_present()
  psointernal.on_present()
end

function pso_on_key_pressed(key)
  psointernal.on_key_pressed(key)
end

function pso_on_key_released(key)
  psointernal.on_key_released(key)
end

