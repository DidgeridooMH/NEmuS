project('NEmuS', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

qt = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Core5Compat'])

fmt = subproject('fmt')
fmt_dep = fmt.get_variable('fmt_dep')

quazip = subproject('quazip')
quazip_dep = quazip.get_variable('quazip_dep')

src = [
  'src/main.cpp',
  'src/Core/NES.cpp',
  'src/Core/CPU.cpp',
  'src/Core/Memory.cpp',
  'src/Debug/Logger.cpp',
  'src/Core/PPU.cpp',
  'src/Core/Mappers/NROM.cpp',
  'src/Core/Mappers/MMC1.cpp',
  'src/Core/Input.cpp',
  'src/UI/Settings.cpp',
  'src/UI/Screen.cpp'
]

src += qt.compile_moc(
  headers: [
    'src/UI/Settings.h',
    'src/UI/Screen.h'])

executable('NEmuS',
           src,
           dependencies : [qt6_dep, quazip_dep, fmt_dep],
           install : true)
