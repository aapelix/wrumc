boost_dep = dependency('boost', modules: ['system'])

executable(
    'wrumc_server',
    files('src/main.cpp'),
    dependencies: [wrumc_shared_dep, boost_dep],
)
