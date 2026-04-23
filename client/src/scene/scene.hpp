#pragma once
#include "net/msg.hpp"
#include "src/net/isocket.hpp"
#include <SDL3/SDL.h>
#include <memory>

class Scene {
public:
  virtual ~Scene() = default;
  virtual std::unique_ptr<Scene> update(float dt, ISocket *sock) = 0;
  virtual void handleMessage(SDL_Renderer *renderer, const Message &msg) {};
  virtual void draw(SDL_Renderer *renderer) = 0;
};
