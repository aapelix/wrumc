#pragma once

#include "msgpack.hpp"

struct ClientInput {
  float throttle;
  float steering;

  MSGPACK_DEFINE(throttle, steering)
};

struct ServerState {
  float x;
  float y;
  float rotation;

  MSGPACK_DEFINE(x, y, rotation)
};

using Message = std::variant<ClientInput, ServerState>;

template <std::size_t I = 0>
void decode_variant(int type, const msgpack::object &data, Message &out) {
  if constexpr (I < std::variant_size_v<Message>) {
    if (type == I) {
      using T = std::variant_alternative_t<I, Message>;
      T msg;
      data.convert(msg);
      out = msg;
    } else {
      decode_variant<I + 1>(type, data, out);
    }
  } else {
    throw msgpack::type_error();
  }
}

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(v1) {
  namespace adaptor {

  template <> struct pack<Message> {
    template <typename Stream>
    packer<Stream> &operator()(packer<Stream> &o, const Message &v) const {
      std::visit(
          [&](auto &&msg) {
            o.pack_array(2);
            o.pack(static_cast<int>(v.index())); // type id
            o.pack(msg);                         // payload
          },
          v);
      return o;
    }
  };

  template <> struct convert<Message> {
    msgpack::object const &operator()(msgpack::object const &o,
                                      Message &v) const {
      if (o.type != msgpack::type::ARRAY || o.via.array.size != 2)
        throw msgpack::type_error();

      int type = o.via.array.ptr[0].as<int>();
      msgpack::object data = o.via.array.ptr[1];

      decode_variant(type, data, v);
      return o;
    }
  };

  } // namespace adaptor
}
} // namespace msgpack

inline std::string encode_message(const Message &msg) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, msg);
  return std::string(buffer.data(), buffer.size());
}

inline Message decode_message(const char *data, size_t size) {
  msgpack::object_handle oh = msgpack::unpack(data, size);
  msgpack::object obj = oh.get();

  Message msg;
  obj.convert(msg);
  return msg;
}
