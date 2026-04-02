wode_sources = files(
  'main.cpp',
  'compositor.cpp',
  'output.cpp',
  'xdg-shell.cpp',
  'window.cpp',
  'toplevel-window.cpp',
  'layer-window.cpp',
  'popup-window.cpp',
  'input.cpp',
  'keyboard.cpp'
)

executable('wode-wm',
  wode_sources,
  xdg_shell_files,
  include_directories: [wode_inc, proto_inc],
  dependencies: [
    wlroots,
    wayland_server,
    xkbcommon,
    wayland_protocols
  ],
  install: true,
  install_dir : '/usr/bin'
)
