#include "game.hpp"
#include "net/msg.hpp"
#include "src/entity/car.hpp"
#include "src/net/isocket.hpp"
#include <iostream>

GameScene::GameScene(SDL_Renderer *renderer) {
  cars.emplace_back(renderer, "assets/cars", 200, 100, 45);
}

GameScene::~GameScene() { cars.clear(); }

std::unique_ptr<Scene> GameScene::update(float dt, ISocket *sock) {
  return nullptr;
}

void GameScene::handleMessage(SDL_Renderer *renderer, const Message &msg) {
  if (std::holds_alternative<LobbyState>(msg)) {
    std::cout << "received lobby state \n";
    const LobbyState &state = std::get<LobbyState>(msg);

    for (const ServerCar &sc : state.players) {
      auto it = std::find_if(cars.begin(), cars.end(),
                             [&](const Car &c) { return c.id == sc.id; });
      if (it != cars.end()) {
        it->x = sc.x;
        it->y = sc.y;
        it->rotation = sc.rotation;
      } else {
        cars.emplace_back(renderer, "assets/cars", sc.id, sc.x, sc.y,
                          sc.rotation);
      }
    }
  }
}

void GameScene::draw(SDL_Renderer *renderer) {
  for (Car &car : cars) {
    car.draw(renderer);
  }
}
