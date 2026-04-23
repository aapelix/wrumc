#include "manager.hpp"

SceneManager::SceneManager() = default;

void SceneManager::update(float dt, ISocket *sock) {
  if (!currentScene)
    return;

  auto next = currentScene->update(dt, sock);
  if (next) {
    currentScene = std::move(next);
  }
}
