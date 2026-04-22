#pragma once

#ifdef __EMSCRIPTEN__
#include "isocket.hpp"
#include "net/msg.hpp"
#include <emscripten/websocket.h>
#include <string>

class EmscriptenSocket : public ISocket {
public:
  EmscriptenSocket();
  ~EmscriptenSocket() override;

  void connect(const std::string &url,
               const std::string &protocols = "") override;
  void send(const std::string &text) override;
  void send(const Message &msg) override;
  void close(unsigned short code = 1000,
             const std::string &reason = "") override;

  bool isOpen() const override;

private:
  int m_ws = 0; // EMSCRIPTEN_WEBSOCKET_T

  static bool onOpen(int, const EmscriptenWebSocketOpenEvent *, void *self);
  static bool onMessage(int, const EmscriptenWebSocketMessageEvent *,
                        void *self);
  static bool onError(int, const EmscriptenWebSocketErrorEvent *, void *self);
  static bool onClose(int, const EmscriptenWebSocketCloseEvent *, void *self);
};

#endif
