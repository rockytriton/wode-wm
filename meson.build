project('wode-wm', ['c', 'cpp'],
  version: '0.1.0',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false'
  ]
)

wlroots          = dependency('wlroots-0.19')
wayland_server   = dependency('wayland-server')
wayland_protocols   = dependency('wayland-protocols')
xkbcommon        = dependency('xkbcommon')

add_project_arguments('-Wno-unused-parameter', language: ['c', 'cpp'])
add_project_arguments('-DWLR_USE_UNSTABLE', language: 'cpp')

wode_inc = include_directories('include')
proto_inc = include_directories('.')

#wl_mod = import('wayland')

wayland_protos = dependency('wayland-protocols')
#wl_scanner     = dependency('wayland-scanner', native: true)
wl_scanner = find_program('wayland-scanner', native: true)

proto_dir = wayland_protos.get_variable(pkgconfig: 'pkgdatadir')

xdg_shell_xml = proto_dir / 'stable/xdg-shell/xdg-shell.xml'


xdg_shell_server_h = custom_target(
  'xdg-shell-server-protocol-h',
  output: 'xdg-shell-protocol.h', # Forced filename
  input: xdg_shell_xml,
  command: [wl_scanner, 'server-header', '@INPUT@', '@OUTPUT@']
)

xdg_shell_code = custom_target(
  'xdg-shell-protocol-c',
  output: 'xdg-shell-protocol.c',
  input: xdg_shell_xml,
  command: [wl_scanner, 'private-code', '@INPUT@', '@OUTPUT@']
)

xdg_shell_files = [xdg_shell_server_h, xdg_shell_code]

subdir('wm')
