cmake_minimum_required(VERSION 3.20)
project(wrumc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

add_executable(wrumc
    src/main.cpp
)

target_link_libraries(wrumc PRIVATE SDL3_image::SDL3_image SDL3::SDL3)

if(EMSCRIPTEN)
    target_link_options(wrumc PRIVATE
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@assets"
    )
endif()
