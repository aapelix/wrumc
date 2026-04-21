#pragma once

#include <msgpack.hpp>
#include <variant>

struct ClientInput {
  float throttle;
  float steering;

  MSGPACK_DEFINE(throttle, steering);
};

using ServerMessage = std::variant<ClientInput>;
