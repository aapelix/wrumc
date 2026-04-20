#include "stack.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "utils/assets.hpp"
#include <string>
#include <vector>

Stack::Stack(SDL_Renderer *renderer, const char *path) {
  std::vector<std::string> files = list_pngs(path);

  for (const std::string &file : files) {
    SDL_Texture *texture = IMG_LoadTexture(renderer, file.c_str());
    if (texture) {
      SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
      textures.push_back(texture);
    }
  }
}

void Stack::draw(SDL_Renderer *renderer, float x, float y, float rotation) {
  if (textures.empty()) {
    return;
  }

  for (float i = 0; i < textures.size(); i++) {
    SDL_Texture *texture = textures[i];

    float img_w, img_h;
    SDL_GetTextureSize(texture, &img_w, &img_h);
    SDL_FRect rect = {x, y - i, img_w, img_h};
    SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, rotation,
                             nullptr, SDL_FLIP_NONE);
  }
}
