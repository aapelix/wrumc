#ifdef __EMSCRIPTEN__
#include "socket_emscripten.hpp"
#include <cstdio>
#include <cstring>
#include <emscripten/websocket.h>

EmscriptenSocket::EmscriptenSocket() = default;

EmscriptenSocket::~EmscriptenSocket() {
  if (m_ws > 0) {
    emscripten_websocket_close(m_ws, 1000, "destructor");
    emscripten_websocket_delete(m_ws);
    m_ws = 0;
  }
}

void EmscriptenSocket::connect(const std::string &url,
                               const std::string &protocols) {
  if (!emscripten_websocket_is_supported()) {
    printf("[Socket] WebSockets not supported!\n");
    return;
  }

  EmscriptenWebSocketCreateAttributes attrs;
  emscripten_websocket_init_create_attributes(&attrs);

  attrs.url = url.c_str();
  attrs.protocols = protocols.empty() ? nullptr : protocols.c_str();
  attrs.createOnMainThread = true;

  m_ws = emscripten_websocket_new(&attrs);
  if (m_ws <= 0) {
    printf("[Socket] Failed to create WebSocket: %d\n", m_ws);
    return;
  }

  emscripten_websocket_set_onopen_callback(m_ws, this, onOpen);
  emscripten_websocket_set_onmessage_callback(m_ws, this, onMessage);
  emscripten_websocket_set_onerror_callback(m_ws, this, onError);
  emscripten_websocket_set_onclose_callback(m_ws, this, onClose);
}

void EmscriptenSocket::send(const std::string &text) {
  if (m_ws > 0)
    emscripten_websocket_send_utf8_text(m_ws, text.c_str());
}

void EmscriptenSocket::send(const Message &msg) {
  if (m_ws > 0) {
    std::string encoded = encode_message(msg);
    emscripten_websocket_send_binary(m_ws, encoded.data(), encoded.size());
  }
}

void EmscriptenSocket::close(unsigned short code, const std::string &reason) {
  if (m_ws > 0) {
    emscripten_websocket_close(m_ws, code, reason.c_str());
    emscripten_websocket_delete(m_ws);
    m_ws = 0;
  }
}

bool EmscriptenSocket::isOpen() const {
  if (m_ws <= 0)
    return false;
  unsigned short state = 0;
  emscripten_websocket_get_ready_state(m_ws, &state);
  return state == 1;
}

bool EmscriptenSocket::onOpen(int, const EmscriptenWebSocketOpenEvent *,
                              void *self) {
  auto *s = static_cast<ISocket *>(self);
  if (s->onOpen)
    s->onOpen();
  return true;
}

bool EmscriptenSocket::onMessage(int, const EmscriptenWebSocketMessageEvent *e,
                                 void *self) {
  auto *s = static_cast<ISocket *>(self);
  if (e->isText) {
    if (s->onMessage)
      s->onMessage(
          std::string(reinterpret_cast<const char *>(e->data), e->numBytes));
  } else {
    if (s->onBinary)
      s->onBinary(e->data, e->numBytes);
  }
  return true;
}

bool EmscriptenSocket::onError(int, const EmscriptenWebSocketErrorEvent *,
                               void *self) {
  printf("[Socket] Error on socket\n");
  return true;
}

bool EmscriptenSocket::onClose(int, const EmscriptenWebSocketCloseEvent *e,
                               void *self) {
  auto *s = static_cast<ISocket *>(self);
  if (s->onClose)
    s->onClose(e->code, std::string(e->reason));
  return true;
}
#endif
