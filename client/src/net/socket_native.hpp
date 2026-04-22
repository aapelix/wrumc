#pragma once
#include "isocket.hpp"
#include "net/msg.hpp"
#include <ixwebsocket/IXWebSocket.h>

#ifndef __EMSCRIPTEN__
class NativeSocket : public ISocket {
public:
  NativeSocket();
  ~NativeSocket() override;

  void connect(const std::string &url,
               const std::string &protocols = "") override;

  void send(const std::string &text) override;
  void send(const Message &msg) override;

  void close(uint16_t code = 1000, const std::string &reason = "") override;

  bool isOpen() const override;

private:
  ix::WebSocket m_ws;
  bool m_open = false;
};
#endif
