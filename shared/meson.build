msgpack_proj = subproject('msgpackc-cxx')
msgpack_dep = msgpack_proj.get_variable('msgpack_cxx_dep')

shared_lib = static_library(
    'wrumc_shared',
    files('net/msg.cpp'),
    include_directories: include_directories('.'),
    dependencies: msgpack_dep,
)

wrumc_shared_dep = declare_dependency(
    link_with: shared_lib,
    include_directories: include_directories('.'),
    dependencies: msgpack_dep,
)
