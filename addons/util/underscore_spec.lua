local __ = require 'util.underscore'

describe('underscore', function()
  describe('map', function()
    it('should map correctly', function()
      local e = {1, 2, 3}
      local i = {0, 1, 2}
      assert.are.same(
        __.map(i, function(v) return v + 1; end),
        e
      )
    end)
  end)

  describe('filter', function()
    it('should filter correctly', function()
      local e = {5, 7}
      local i = {2, 4, 6, 8, 10, 12, 14, 5, 7}
      assert.are.same(
        __.filter(i, function(v) return v % 2 ~= 0; end),
        e
      )
    end)
  end)

  describe('foldl', function()
    it('should fold correctly without initial value', function()
      local e = 3
      local i = {1, 1, 1}
      assert.are.same(
        __.foldl(i, function(a, v) return a+v; end),
        e
      )
    end)

    it('should fold correctly with initial value', function()
      local e = 4
      local i = {1, 1, 1}
      assert.are.same(
        __.foldl(i, function(a, v) return a+v; end, 1),
        e
      )
    end)
  end)

  describe('concat', function()
    it('should concat correctly', function()
      local e = {1, 2}
      assert.are.same(
        __.concat({1}, {2}),
        e
      )
    end)
  end)

  describe('head', function()
    it('should get the first element', function()
      local e = 4
      assert.are.same(
        __.head({4, 1, 2, 3}),
        e
      )
    end)
  end)


  describe('tail', function()
    it('should get all but the first element', function()
      local e = {2, 3, 4}
      assert.are.same(
        __.tail({1, 2, 3, 4}),
        e
      )
    end)
  end)

  describe('compose', function()
    it('should compose two functions', function()
      local f1 = function(a) return a..'hello'; end
      local f2 = function(a) return a..'hi'; end
      assert.are.same(
        __.compose(f1, f2)(''),
        'hihello'
      )
    end)
  end)

  describe('negate', function()
    it('should produce a negative predicate', function()
      local f = function() return true; end
      assert.are.same(
        __.negate(f)(),
        not f()
      )
    end)
  end)

  describe('const', function()
    it('should produce a const function returning the exact value', function()
      local t = {1, 2, 3}
      assert.are.equal(
        __.const(t)(),
        t
      )
    end)
  end)

  describe('identity', function()
    it('should be the identity function', function()
      local t = {1, 2}
      assert.are.equal(
        __.identity(t),
        t
      )
    end)
  end)

  describe('forEach', function()
    it('should run the function for each item', function()
      local t = {}
      local f = function() t[#t+1] = 1; end
      __.forEach({1, 2, 3}, f)
      assert.are.same(
        t,
        {1, 1, 1}
      )
    end)
  end)

  describe('toPairs', function()
    it('should map the table to a list of pairs', function()
      local e = {{'b', 2}}
      local v = {b = 2}
      assert.are.same(
        __.toPairs(v),
        e
      )
    end)
  end)

  describe('fromPairs', function()
    it('should map the list of pairs to a table', function()
      local v = {{'a', 1}, {'b', 2}}
      local e = {a = 1, b = 2}
      assert.are.same(
        __.fromPairs(v),
        e
      )
    end)
  end)

  describe('clear', function()
    it('should clear the same table', function()
      local t = {1, 2, 3, a = 4}
      __.clear(t)
      assert.is.equal(
        #t,
        0
      )
      assert.is.falsy(t.a)
    end)
  end)
end)
