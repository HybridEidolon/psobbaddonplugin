# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## Unreleased

This release focuses on creating a resilient addon registration
interface that is compatible with the existing Lua module system. It
adds several new utilities for addon developers and a way for addons to
depend on other addons.

### Added

- `pso.base_address` is the base address of the psobb.exe module. Added
  for addon resilience when running on servers with modified psobb.exes.
  However, this does not mean the plugin works with compressed
  executables: decompressed executables are still required.
- `pso.list_addon_directories` returns a list of the directories in
  `addons/` as an array table. It is used in `psointernal` for loading
  addons.
- `pso_on_init`, `pso_on_present`, `pso_on_key_pressed`, and
  `pso_on_key_released` are all new low-level global functions that can
  be set to define the `addons/` behavior entirely. **You don't need
  this unless you want to completely change the plugin's behavior.**
- `util.underscore` package adds several utility functions,
  particularly for functional programming. You can use it like any other
  Lua module:

      local __ = require('util.underscore')
      local mapped = __.map({1, 2}, function(v) return v+4; end)
      -- mapped = {5, 6}

- New addon registration process. Your addon module's `init.lua` should
  export a table with a `__addon` key. See the example for details.
  Directories under `addons/` do _not_ have to export this key if they
  are simply regular Lua modules; they will not show up in the addon
  list and not be able to register hooks.

### Changed

- Addon hook API is overhauled. Most logic is done in pure Lua in place
  of C++. With this, also comes a change to the way hooks are
  registered. Please see the new `addon_examples/HelloWorld` path in
  the source.
- **All existing addons before this release are broken and must be
  updated to the new API.** After this release, the addon registration
  interface will be mostly stabilized and future releases will not
  break old addons (hopefully).

### Removed

- `on_` functions have all been removed from the `pso` global table.

## 0.2.1

No changes have been made to the plugin itself, this is just a release
test for AppVeyor.

### Added

- AppVeyor continuous integration builds.

## 0.2.0

This update "breaks" compatibility with 0.1.0 by ensuring read_mem
yields numbers of the range 0 to 255 rather than -127 to 127. This was
the intended behavior.

## 0.1.0

Minimum viable product.
