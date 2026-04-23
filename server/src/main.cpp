#include "net/msg.hpp"
#include "src/lobby.hpp"
#include <App.h>
#include <exception>
#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

std::unordered_map<int, std::shared_ptr<Lobby>> lobbies;
std::mutex lobbiesMutex;
std::unordered_map<uWS::WebSocket<false, true, std::string> *, int>
    playerToLobby;
int nextPlayerId = 1;

int main() {
  std::atomic<bool> running = true;

  std::thread gameThread([&]() {
    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();

    while (running) {
      auto now = clock::now();
      float dt = std::chrono::duration<float>(now - last).count();
      last = now;

      {
        std::lock_guard<std::mutex> lock(lobbiesMutex);

        for (auto &[id, lobby] : lobbies) {
          lobby->update(dt);
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
  });

  uWS::App()
      .ws<std::string>(
          "/*", {.open = [](auto *ws) { std::cout << "client connected\n"; },

                 .message =
                     [](auto *ws, std::string_view msg, uWS::OpCode op) {
                       try {
                         Message m = decode_message(msg.data(), msg.size());

                         std::visit(
                             [&](auto &&data) {
                               using T = std::decay_t<decltype(data)>;

                               if constexpr (std::is_same_v<T, ClientInput>) {
                                 std::lock_guard<std::mutex> lock(lobbiesMutex);

                                 auto it = playerToLobby.find(ws);
                                 if (it == playerToLobby.end()) {
                                   std::cout << "player not in lobby\n";
                                   return;
                                 }

                                 int lobbyId = it->second;
                                 auto &lobby = lobbies[lobbyId];

                                 for (auto &[id, p] : lobby->players) {
                                   if (p->ws == ws) {
                                     auto &car = lobby->cars[id];
                                     car.throttle = data.throttle;
                                     car.steering = data.steering;
                                     break;
                                   }
                                 }
                               }

                               if constexpr (std::is_same_v<T, ClientJoin>) {
                                 std::lock_guard<std::mutex> lock(lobbiesMutex);

                                 if (!lobbies.contains(data.lobbyId)) {
                                   std::cout << "creating lobby "
                                             << data.lobbyId << std::endl;
                                   lobbies[data.lobbyId] =
                                       std::make_shared<Lobby>();
                                 }

                                 auto player = std::make_shared<Player>();
                                 player->ws = ws;
                                 player->id = nextPlayerId++;

                                 auto &lobby = lobbies[data.lobbyId];
                                 lobby->players[player->id] = player;
                                 lobby->cars[player->id] = Car{0, 0, 0, 0, 0};

                                 playerToLobby[ws] = data.lobbyId;
                               }
                             },
                             m);
                       } catch (const std::exception &e) {
                         std::cout << "decode error: " << e.what() << std::endl;
                       }
                     },

                 .close =
                     [&](auto *ws, int, std::string_view) {
                       std::lock_guard<std::mutex> lock(lobbiesMutex);

                       auto it = playerToLobby.find(ws);
                       if (it == playerToLobby.end())
                         return;

                       int lobbyId = it->second;
                       auto &lobby = lobbies[lobbyId];

                       for (auto p = lobby->players.begin();
                            p != lobby->players.end();) {
                         if (p->second->ws == ws) {
                           lobby->cars.erase(p->first);
                           p = lobby->players.erase(p);
                         } else {
                           ++p;
                         }
                       }

                       if (lobby->players.empty()) {
                         std::cout << "removing lobby " << lobbyId << std::endl;
                         lobbies.erase(lobbyId);
                       }

                       playerToLobby.erase(it);
                     }})
      .listen(9001,
              [](auto *token) {
                if (token) {
                  std::cout << "listening on 9001\n";
                }
              })
      .run();

  running = false;
  gameThread.join();
}
