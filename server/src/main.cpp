#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
  try {
    net::io_context ioc;

    tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), 8000));

    std::cout << "server listening on 8000" << std::endl;

    for (;;) {
      tcp::socket socket(ioc);
      acceptor.accept(socket);

      websocket::stream<tcp::socket> ws(std::move(socket));
      ws.read_message_max(64 * 1024 * 1024);

      beast::flat_buffer buffer;

      ws.accept();

      for (;;) {
        ws.read(buffer);

        std::string msg = beast::buffers_to_string(buffer.data());
        std::cout << "recv: " << msg << "\n";

        ws.text(ws.got_text());
        ws.write(net::buffer("echo: " + msg));
      }
    }
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
