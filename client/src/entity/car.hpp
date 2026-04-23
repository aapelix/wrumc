#pragma once
#include "src/stack.hpp"
#include <SDL3/SDL.h>

class Car {
public:
  Car(SDL_Renderer *renderer, const char *path, int id, float x = 0,
      float y = 0, float rotation = 0)
      : id(id), x(x), y(y), rotation(rotation), stack(renderer, path) {}

  void draw(SDL_Renderer *renderer) { stack.draw(renderer, x, y, rotation); }

  int id;
  float x;
  float y;
  float rotation;

private:
  Stack stack;
};
