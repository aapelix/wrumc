#pragma once

#include "net/msg.hpp"
#include "scene.hpp"
#include <SDL3/SDL.h>
#include <memory>

class SceneManager {
public:
  SceneManager();
  ~SceneManager() = default;

  void update(float dt, ISocket *sock);
  void draw(SDL_Renderer *renderer) {
    if (currentScene) {
      currentScene->draw(renderer);
    }
  }

  void handleMessage(SDL_Renderer *renderer, const Message &msg) {
    if (currentScene) {
      currentScene->handleMessage(renderer, msg);
    }
  }

  void setScene(std::unique_ptr<Scene> scene) {
    currentScene = std::move(scene);
  }

private:
  std::unique_ptr<Scene> currentScene;
};
