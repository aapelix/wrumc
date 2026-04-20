#pragma once
#include "isocket.hpp"
#include <memory>

struct Socket {
  static std::unique_ptr<ISocket> create();
};
