cmake = import('cmake')

if emscripten
  emscripten_args = [
    '--preload-file', meson.project_source_root() / 'assets@assets',
  ]

  cc = meson.get_compiler('cpp')

  sdl3_inc = include_directories('../prefix/include')

  prefix = meson.project_source_root() / 'prefix'

  sdl3_dep = cc.find_library('SDL3', dirs: prefix / 'lib')
  sdl3_image_dep = cc.find_library('SDL3_image', dirs: prefix / 'lib')
else
  sdl3_dep = dependency('sdl3')
  sdl3_image_dep = dependency('sdl3_image')
endif

client_sources = files(
  'src/main.cpp',
  'src/stack.cpp',
  'src/utils/assets.cpp',
)

executable('wrumc_client',
  client_sources,
  include_directories: emscripten ? sdl3_inc : [],
  dependencies: [
    wrumc_shared_dep,
    sdl3_dep,
    sdl3_image_dep
  ],
  link_args: emscripten ? emscripten_args : []
)
