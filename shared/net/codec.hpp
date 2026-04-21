#pragma once

#include "msg.hpp"
#include <msgpack.hpp>
#include <string>

class ServerMessageCodec {
public:
  static std::string encode(const ServerMessage &msg) {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, msg);
    return std::string(buffer.data(), buffer.size());
  }

  static ServerMessage decode(const std::string &data) {
    msgpack::object_handle oh = msgpack::unpack(data.data(), data.size());

    msgpack::object obj = oh.get();

    ServerMessage msg;
    obj.convert(msg);
    return msg;
  }
};

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(v1) {
  namespace adaptor {

  template <> struct pack<ServerMessage> {
    template <typename Stream>
    packer<Stream> &operator()(msgpack::packer<Stream> &o,
                               const ServerMessage &v) const {
      std::visit(
          [&](auto &&msg) {
            o.pack_array(2);
            o.pack(static_cast<int>(v.index())); // type id
            o.pack(msg);
          },
          v);
      return o;
    }
  };

  template <> struct convert<ServerMessage> {
    msgpack::object const &operator()(msgpack::object const &o,
                                      ServerMessage &v) const {
      if (o.type != msgpack::type::ARRAY || o.via.array.size != 2)
        throw msgpack::type_error();

      int type = o.via.array.ptr[0].as<int>();
      msgpack::object data = o.via.array.ptr[1];

      switch (type) {
      case 0: {
        ClientInput msg;
        data.convert(msg);
        v = msg;
        break;
      }
      default:
        throw msgpack::type_error();
      }

      return o;
    }
  };

  } // namespace adaptor
} // namespace MSGPACK_API_VERSION_NAMESPACE
