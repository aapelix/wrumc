#pragma once

#include "SDL3/SDL_render.h"
#include <vector>

class Stack {
public:
  Stack(SDL_Renderer *renderer, const char *path);
  void draw(SDL_Renderer *renderer, float x, float y, float rotation);

  ~Stack() {
    for (SDL_Texture *texture : textures) {
      SDL_DestroyTexture(texture);
    }
    textures.clear();
  }

private:
  std::vector<SDL_Texture *> textures;
};
