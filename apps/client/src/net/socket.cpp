#include "socket.hpp"

#ifdef __EMSCRIPTEN__
#include "socket_emscripten.hpp"
#else
#include "socket_native.hpp"
#endif

std::unique_ptr<ISocket> Socket::create() {
#ifdef __EMSCRIPTEN__
  return std::make_unique<EmscriptenSocket>();
#else
  return std::make_unique<NativeSocket>();
#endif
}
