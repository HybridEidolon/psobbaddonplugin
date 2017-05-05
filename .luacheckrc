std = "luajit+busted"
globals = {
  "pso",
  "pso_on_init",
  "pso_on_key_pressed",
  "pso_on_key_released",
  "pso_on_present",
  "pso_on_log",
  "pso_on_unhandled_error",
  "imgui"
}
include_files = {
  "addons/**/*",
  "addon_examples/**/*",
  ".luacheckrc",
  ".busted"
}
