local map
local filter
local foldl
local forEach
local concat
local head
local tail
local compose
local negate
local const
local identity
local toPairs
local fromPairs
local clear

-- Map the elements of a list over a function
map = function(array, func)
  local ret = {}
  for _, v in ipairs(array) do
    ret[#ret+1] = func(v)
  end
  return ret
end

-- Filter a list by a predicate
filter = function(array, predicate)
  local ret = {}
  for _, v in ipairs(array) do
    if (predicate(v)) then
      ret[#ret+1] = v
    end
  end
  return ret
end

-- Reduce a list to a value
foldl = function(array, reducer, initial)
  local do_first = initial == nil
  local ret = initial

  if (do_first) then
    ret = array[1]
  end

  for i, v in ipairs(array) do
    if (i ~= 1 or i == 1 and not do_first) then
      ret = reducer(ret, v)
    end
  end

  return ret
end

forEach = function(array, func)
  for _, v in ipairs(array) do
    func(v)
  end
end

toPairs = function(t)
  local ret = {}
  for k, v in pairs(t) do
    ret[#ret+1] = {k, v}
  end
  return ret
end

fromPairs = function(a)
  local ret = {}
  for _, v in ipairs(a) do
    ret[v[1]] = v[2]
  end
  return ret
end

-- Concatenate two lists
concat = function(a, b)
  local ret = {}
  for _, v in ipairs(a) do
    ret[#ret+1] = v
  end
  for _, v in ipairs(b) do
    ret[#ret+1] = v
  end
  return ret
end

-- Get the first element of a list
head = function(array)
  return array[1]
end

-- Get all but the first element of a list
tail = function(array)
  local ret = {}
  for i, v in ipairs(array) do
    if (i ~= 1) then
      ret[#ret+1] = v
    end
  end
  return ret
end

-- Compose functions a . b as a(b(...))
compose = function(a, b)
  return function(...)
    return a(b(...))
  end
end

-- Create a negative predicate function
negate = function(predicate)
  return function(...)
    return not predicate(...)
  end
end

-- Create a function that returns the value given
const = function(v)
  return function()
    return v
  end
end

-- The identity function over any arguments
identity = function(...)
  return ...
end

-- Clear a table of all its keys
clear = function(t)
  for k, _ in pairs(t) do
    t[k] = nil
  end
end

return {
  map = map,
  filter = filter,
  foldl = foldl,
  forEach = forEach,
  concat = concat,
  head = head,
  tail = tail,
  compose = compose,
  negate = negate,
  const = const,
  identity = identity,
  toPairs = toPairs,
  fromPairs = fromPairs,
  clear = clear,
}
