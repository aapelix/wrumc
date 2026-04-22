uwebsockets = subproject('uWebSockets')
uwebsockets_dep = uwebsockets.get_variable('uwebsockets_dep')

executable(
    'wrumc_server',
    files('src/main.cpp'),
    dependencies: [wrumc_shared_dep, uwebsockets_dep],
)
