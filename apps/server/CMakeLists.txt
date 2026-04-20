cmake_minimum_required(VERSION 3.20)
project(wrumc-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(wrumc-server
    src/main.cpp
)

target_include_directories(wrumc-server PRIVATE
    external/uWebSockets/src
)

target_include_directories(wrumc-server PRIVATE
    external/uWebSockets/uSockets/src
)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(wrumc-server PRIVATE
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)
