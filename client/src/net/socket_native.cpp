#include "net/msg.hpp"
#ifndef __EMSCRIPTEN__
#include "ixwebsocket/IXNetSystem.h"
#include "socket_native.hpp"

NativeSocket::NativeSocket() { ix::initNetSystem(); }

NativeSocket::~NativeSocket() { close(); }

void NativeSocket::connect(const std::string &url,
                           const std::string &protocols) {
  m_ws.setUrl(url);

  if (!protocols.empty()) {
    m_ws.addSubProtocol(protocols);
  }

  m_ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
    switch (msg->type) {
    case ix::WebSocketMessageType::Open:
      m_open = true;
      if (onOpen)
        onOpen();
      break;

    case ix::WebSocketMessageType::Message:
      if (msg->binary) {
        if (onBinary)
          onBinary(reinterpret_cast<const uint8_t *>(msg->str.data()),
                   (uint32_t)msg->str.size());
      } else {
        if (onMessage)
          onMessage(msg->str);
      }
      break;

    case ix::WebSocketMessageType::Close:
      m_open = false;
      if (onClose)
        onClose(msg->closeInfo.code, msg->closeInfo.reason);
      break;

    case ix::WebSocketMessageType::Error:
      if (onError)
        onError(msg->errorInfo.reason);
      break;

    default:
      break;
    }
  });

  m_ws.start();
}

void NativeSocket::send(const std::string &text) {
  if (m_open)
    m_ws.send(text);
}

void NativeSocket::send(const Message &msg) {
  if (m_open)
    m_ws.sendBinary(encode_message(msg));
}

void NativeSocket::close(uint16_t code, const std::string &reason) {
  if (m_open) {
    m_ws.stop(code, reason);
    m_open = false;
  }
}

bool NativeSocket::isOpen() const { return m_open; }
#endif
