#pragma once
#include "isocket.hpp"
#include <atomic>
#include <thread>
#ifndef __EMSCRIPTEN__

class NativeSocket : public ISocket {
public:
  NativeSocket();
  ~NativeSocket() override;

  void connect(const std::string &url,
               const std::string &protocols = "") override;
  void send(const std::string &text) override;
  void send(const void *data, uint32_t length) override;
  void close(unsigned short code = 1000,
             const std::string &reason = "") override;
  bool isOpen() const override;
  void poll() override;

  struct Impl;
  std::unique_ptr<Impl> m_impl;
  std::atomic<bool> m_open{false};

private:
  std::thread m_thread;
};

#endif
