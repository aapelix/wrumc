#ifndef __EMSCRIPTEN__
#include "socket_native.hpp"
#include <libwebsockets.h>
#include <mutex>
#include <queue>
#include <vector>

struct NativeSocket::Impl {
  lws_context *context = nullptr;
  lws *wsi = nullptr;
  lws_context_creation_info ctxInfo = {};
  lws_client_connect_info connInfo = {};

  std::mutex sendMutex;
  std::queue<std::vector<uint8_t>> sendQueue;
  std::queue<std::string> sendTextQueue;

  std::mutex recvMutex;
  std::queue<std::function<void()>> recvQueue;

  std::string pendingUrl;
  std::string pendingProtocol;
  bool stopping = false;
};

static int lwsCallback(lws *wsi, lws_callback_reasons reason, void *user,
                       void *in, size_t len) {
  auto *self =
      static_cast<NativeSocket *>(lws_context_user(lws_get_context(wsi)));
  if (!self)
    return 0;
  auto &impl = *self->m_impl;

  switch (reason) {

  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    self->m_open = true;
    {
      std::lock_guard lock(impl.recvMutex);
      impl.recvQueue.push([self] {
        if (self->onOpen)
          self->onOpen();
      });
    }
    break;

  case LWS_CALLBACK_CLIENT_RECEIVE: {
    const bool isBinary = lws_frame_is_binary(wsi);
    if (isBinary) {
      std::vector<uint8_t> buf(static_cast<uint8_t *>(in),
                               static_cast<uint8_t *>(in) + len);
      std::lock_guard lock(impl.recvMutex);
      impl.recvQueue.push([self, buf = std::move(buf)] {
        if (self->onBinary)
          self->onBinary(buf.data(), static_cast<uint32_t>(buf.size()));
      });
    } else {
      std::string msg(static_cast<char *>(in), len);
      std::lock_guard lock(impl.recvMutex);
      impl.recvQueue.push([self, msg = std::move(msg)] {
        if (self->onMessage)
          self->onMessage(msg);
      });
    }
    break;
  }

  case LWS_CALLBACK_CLIENT_WRITEABLE: {
    {
      std::lock_guard lock(impl.sendMutex);
      if (!impl.sendTextQueue.empty()) {
        auto text = std::move(impl.sendTextQueue.front());
        impl.sendTextQueue.pop();

        std::vector<uint8_t> buf(LWS_PRE + text.size());
        std::memcpy(buf.data() + LWS_PRE, text.data(), text.size());
        lws_write(wsi, buf.data() + LWS_PRE, text.size(), LWS_WRITE_TEXT);

        if (!impl.sendTextQueue.empty() || !impl.sendQueue.empty())
          lws_callback_on_writable(wsi);
        break;
      }
      if (!impl.sendQueue.empty()) {
        auto frame = std::move(impl.sendQueue.front());
        impl.sendQueue.pop();

        std::vector<uint8_t> buf(LWS_PRE + frame.size());
        std::memcpy(buf.data() + LWS_PRE, frame.data(), frame.size());
        lws_write(wsi, buf.data() + LWS_PRE, frame.size(), LWS_WRITE_BINARY);

        if (!impl.sendQueue.empty())
          lws_callback_on_writable(wsi);
      }
    }
    break;
  }

  case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
    std::string reason_str =
        in ? std::string(static_cast<char *>(in), len) : "connection error";
    self->m_open = false;
    std::lock_guard lock(impl.recvMutex);
    impl.recvQueue.push([self, reason_str] {
      if (self->onError)
        self->onError(reason_str);
    });
    break;
  }

  case LWS_CALLBACK_CLIENT_CLOSED: {
    self->m_open = false;
    std::lock_guard lock(impl.recvMutex);
    impl.recvQueue.push([self] {
      if (self->onClose)
        self->onClose(1000, "closed");
    });
    break;
  }

  default:
    break;
  }

  return 0;
}

static lws_protocols s_protocols[] = {
    {"default", lwsCallback, 0, 4096, 0, nullptr, 0}, LWS_PROTOCOL_LIST_TERM};

NativeSocket::NativeSocket() : m_impl(std::make_unique<Impl>()) {}

NativeSocket::~NativeSocket() {
  close(1000, "destructor");
  if (m_impl->context) {
    lws_context_destroy(m_impl->context);
    m_impl->context = nullptr;
  }
}

void NativeSocket::connect(const std::string &url,
                           const std::string &protocols) {
  bool tls = url.rfind("wss://", 0) == 0;
  std::string rest = url.substr(tls ? 6 : 5);

  std::string host, path;
  int port = tls ? 443 : 80;

  auto slashPos = rest.find('/');
  std::string hostPort =
      (slashPos == std::string::npos) ? rest : rest.substr(0, slashPos);
  path = (slashPos == std::string::npos) ? "/" : rest.substr(slashPos);

  auto colonPos = hostPort.find(':');
  if (colonPos != std::string::npos) {
    host = hostPort.substr(0, colonPos);
    port = std::stoi(hostPort.substr(colonPos + 1));
  } else {
    host = hostPort;
  }

  m_impl->pendingUrl = host;
  m_impl->pendingProtocol = protocols.empty() ? "default" : protocols;

  lws_context_creation_info &ci = m_impl->ctxInfo;
  std::memset(&ci, 0, sizeof(ci));
  ci.port = CONTEXT_PORT_NO_LISTEN;
  ci.protocols = s_protocols;
  ci.user = this;
  if (tls)
    ci.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

  m_impl->context = lws_create_context(&ci);
  if (!m_impl->context) {
    if (onError)
      onError("failed to create lws context");
    return;
  }

  lws_client_connect_info &cc = m_impl->connInfo;
  std::memset(&cc, 0, sizeof(cc));
  cc.context = m_impl->context;
  cc.address = m_impl->pendingUrl.c_str();
  cc.port = port;
  cc.path = path.c_str();
  cc.host = cc.address;
  cc.origin = cc.address;
  cc.protocol = m_impl->pendingProtocol.c_str();
  cc.ssl_connection = tls ? LCCSCF_USE_SSL : 0;

  m_impl->wsi = lws_client_connect_via_info(&cc);
  if (!m_impl->wsi) {
    if (onError)
      onError("lws_client_connect_via_info failed");
  }
}

void NativeSocket::send(const std::string &text) {
  if (!m_open)
    return;
  {
    std::lock_guard lock(m_impl->sendMutex);
    m_impl->sendTextQueue.push(text);
  }
  if (m_impl->wsi)
    lws_callback_on_writable(m_impl->wsi);
}

void NativeSocket::send(const void *data, uint32_t length) {
  if (!m_open)
    return;
  {
    std::lock_guard lock(m_impl->sendMutex);
    const auto *ptr = static_cast<const uint8_t *>(data);
    m_impl->sendQueue.push(std::vector<uint8_t>(ptr, ptr + length));
  }
  if (m_impl->wsi)
    lws_callback_on_writable(m_impl->wsi);
}

void NativeSocket::close(unsigned short code, const std::string &reason) {
  if (m_impl->wsi && m_open) {
    m_open = false;
    lws_close_reason(
        m_impl->wsi, static_cast<lws_close_status>(code),
        reinterpret_cast<unsigned char *>(const_cast<char *>(reason.c_str())),
        reason.size());
    m_impl->wsi = nullptr;
  }
}

bool NativeSocket::isOpen() const { return m_open; }

void NativeSocket::poll() {
  if (m_impl->context)
    lws_service(m_impl->context, 0);

  std::queue<std::function<void()>> local;
  {
    std::lock_guard lock(m_impl->recvMutex);
    std::swap(local, m_impl->recvQueue);
  }
  while (!local.empty()) {
    local.front()();
    local.pop();
  }
}

#endif // __EMSCRIPTEN__
