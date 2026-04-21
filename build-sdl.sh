#!/usr/bin/env bash
set -e

PREFIX="$(pwd)/prefix"

# SDL3
cmake subprojects/SDL3 -B build-sdl3 \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DSDL_SHARED=ON \
  -DSDL_STATIC=ON \
  -DSDL_TESTS=OFF \
  -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

cmake --build build-sdl3 --target install

# SDL3_image
cmake subprojects/SDL3_image -B build-sdl3_image \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DSDL3_DIR=$PREFIX/lib/cmake/SDL3 \
  -DSDL3IMAGE_VENDORED=ON \
  -DSDL_SHARED=ON \
  -DSDL_STATIC=ON \
  -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

cmake --build build-sdl3_image --target install
