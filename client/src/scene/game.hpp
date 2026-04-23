#pragma once
#include "scene.hpp"
#include "src/entity/car.hpp"
#include "src/net/isocket.hpp"
#include <vector>

class GameScene : public Scene {
public:
  GameScene(SDL_Renderer *renderer);
  ~GameScene() override;

  std::unique_ptr<Scene> update(float dt, ISocket *sock) override;
  void handleMessage(SDL_Renderer *renderer, const Message &msg) override;
  void draw(SDL_Renderer *renderer) override;

private:
  std::vector<Car> cars;
};
