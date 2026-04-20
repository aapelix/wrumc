cmake_minimum_required(VERSION 3.20)
project(wrumc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

if(NOT EMSCRIPTEN)
    add_subdirectory(external/ixwebsocket EXCLUDE_FROM_ALL)
endif()

add_executable(wrumc
    src/main.cpp
    src/stack.cpp
    src/utils/assets.cpp
    src/net/socket.cpp
)

if(EMSCRIPTEN)
    target_sources(wrumc PRIVATE src/net/socket_emscripten.cpp)
else()
    target_sources(wrumc PRIVATE src/net/socket_native.cpp)
endif()

if(NOT EMSCRIPTEN)
    target_link_libraries(wrumc PRIVATE ixwebsocket::ixwebsocket)
endif()
target_link_libraries(wrumc PRIVATE SDL3_image::SDL3_image SDL3::SDL3)

if(EMSCRIPTEN)
    target_link_options(wrumc PRIVATE
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@assets"
        "-lwebsocket.js"
    )
endif()
