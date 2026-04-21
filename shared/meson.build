shared_lib = static_library(
  'wrumc_shared',
  files(
    'net/msg.cpp'
  ),
  include_directories: include_directories('.')
)

wrumc_shared_dep = declare_dependency(
  link_with: shared_lib,
  include_directories: include_directories('.')
)
