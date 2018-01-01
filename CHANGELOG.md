# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## Unreleased

### Added

- Custom imgui theme support (@Solybum #18). Theme is loaded from `addons/theme.ini`
- `get_cwd` returns a string of the cwd (@jtuu #17)
- `play_sound` invokes winmm PlaySound with given path, ignoring errors (@jtuu #17)
- `is_pso_focused` returns a boolean of whether or not the PSOBB HWND
  is the active window. (@jtuu #17)



## v0.3.4

### Fixed

- Fixed a crash where systems running Windows XP may be unable to
  parse wide strings from memory. (@StephenCWillis)
- Fixed an issue where the stack trace emitted from the Lua error
  handler would emit the stack from the error handler's context
  rather than the context of the traced call. (@StephenCWillis)
- Save and restore the FPU state when entering Lua code rather than
  initializing the FPU at launch. This fixes a bug where Recons would
  occasionally fail to leave their spawning state and be stuck
  untargetable in mid-air, and possibly other subtle arithmetic bugs.

## v0.3.3

### Added

- `pso.read_mem_str(addr, len)` - Read a block of memory into a Lua
  string. This is likely to be more performant, but you will have to
  use `string.byte` on the characters yourself and bit ops to
  compose larger integers, floats, etc.

## v0.3.2

### Added

- `pso.set_sleep_hack_enabled(enabled)`: `true` to patch the frame
  limiter to use `Sleep(1)` and reduce CPU usage dramatically. _This
  tweak will cause Ephinea to instantly close the game. Sorry! It's
  out of our control._ The game normally calls `Sleep(0)` to yield to
  other threads, but Windows does not guarantee that the process
  gives up its timeslice. Using 1 millisecond is enough to get the
  desired effect without skewing timing. Since this doesn't work on
  Ephinea, there is no built-in UI to enable it.

## v0.3.1

A minor release to clean up clutter in PSOBB installations.

### Changed

- The location of imgui.ini and imgui_log.txt have moved to the addons
  directory.

## v0.3.0

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
- `pso_on_init`, `pso_on_present`, `pso_on_key_pressed`,
  `pso_on_key_released`, `pso_on_log`, and `pso_on_unhandled_error`
  are all new low-level global functions that can be set to define the
  `addons/` behavior entirely. **You don't need this unless you want
  to completely change the plugin's behavior.**
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
- Basic unit testing for Underscore utility library.

### Changed

- Addon hook API is overhauled. Most logic is done in pure Lua in place
  of C++. With this, also comes a change to the way hooks are
  registered. Please see the new `addon_examples/HelloWorld` path in
  the source.
- **All existing addons before this release are broken and must be
  updated to the new API.** After this release, the addon registration
  interface will be mostly stabilized and future releases will not
  break old addons (hopefully).
- Core UI has been rewritten in pure Lua.

### Removed

- `on_` functions have all been removed from the `pso` global table.

## v0.2.1

No changes have been made to the plugin itself, this is just a release
test for AppVeyor.

### Added

- AppVeyor continuous integration builds.

## v0.2.0

This update "breaks" compatibility with 0.1.0 by ensuring read_mem
yields numbers of the range 0 to 255 rather than -127 to 127. This was
the intended behavior.

## v0.1.0

Minimum viable product.
