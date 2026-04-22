project('wrumc', 'cpp', default_options: ['cpp_std=c++20'])

emscripten = host_machine.system() == 'emscripten'

subdir('shared')
subdir('client')

if not emscripten
    subdir('server')
endif
