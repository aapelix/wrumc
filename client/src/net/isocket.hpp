#pragma once
#include "net/msg.hpp"
#include <cstdint>
#include <functional>
#include <string>

class ISocket {
public:
  using OnOpen = std::function<void()>;
  using OnMessage = std::function<void(const std::string &)>;
  using OnBinary = std::function<void(const uint8_t *, uint32_t)>;
  using OnClose = std::function<void(uint16_t code, const std::string &reason)>;
  using OnError = std::function<void(const std::string &what)>;

  virtual ~ISocket() = default;

  virtual void connect(const std::string &url,
                       const std::string &protocols = "") = 0;
  virtual void send(const std::string &text) = 0;
  virtual void send(const Message &msg) = 0;
  virtual void close(uint16_t code = 1000, const std::string &reason = "") = 0;
  virtual bool isOpen() const = 0;

  virtual void poll() {}

  OnOpen onOpen;
  OnMessage onMessage;
  OnBinary onBinary;
  OnClose onClose;
  OnError onError;
};
