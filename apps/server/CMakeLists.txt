cmake_minimum_required(VERSION 3.20)
project(wrumc-server LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE USOCKETS_SOURCES
    external/uWebSockets/uSockets/src/*.c
    external/uWebSockets/uSockets/src/*.cpp
)

add_library(uSockets STATIC ${USOCKETS_SOURCES})

target_include_directories(uSockets PRIVATE
    external/uWebSockets/uSockets/src
)

target_compile_definitions(uSockets PRIVATE
    LIBUS_USE_OPENSSL
    LIBUS_USE_KQUEUE
)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(uSockets PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_executable(wrumc-server
    src/main.cpp
)

target_include_directories(wrumc-server PRIVATE
    external/uWebSockets/src
    external/uWebSockets/uSockets/src
)

target_link_libraries(wrumc-server PRIVATE
    uSockets
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)
