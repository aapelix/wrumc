#include <App.h>
#include <iostream>

int main() {
  uWS::App()
      .ws<std::string>(
          "/*", {.open = [](auto *ws) { std::cout << "Client connected\n"; },

                 .message = [](auto *ws, std::string_view msg,
                               uWS::OpCode op) { ws->send(msg, op); },

                 .close =
                     [](auto *ws, int code, std::string_view message) {
                       std::cout << "Client disconnected\n";
                     }})
      .listen(9001,
              [](auto *token) {
                if (token) {
                  std::cout << "Listening on 9001\n";
                }
              })
      .run();
}
