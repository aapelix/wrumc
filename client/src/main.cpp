#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "net/msg.hpp"
#include "src/net/isocket.hpp"
#include "src/net/socket.hpp"
#include "src/scene/game.hpp"
#include "src/scene/manager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <exception>
#include <iostream>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static bool running = true;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *canvas;

static Uint64 now, last;
static double dt;

static SceneManager sceneManager;

static std::unique_ptr<ISocket> sock;

constexpr int CANVAS_WIDTH = 320;
constexpr int CANVAS_HEIGHT = 240;

static void loop() {
  if (!running) {
    SDL_DestroyTexture(canvas);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#else
    exit(0);
#endif
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running = false;
    }
  }

  now = SDL_GetPerformanceCounter();
  dt = (double)(now - last) / SDL_GetPerformanceFrequency();
  last = now;

  sock->poll();

  sceneManager.update((float)dt, sock.get());

  SDL_SetRenderTarget(renderer, canvas);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  sceneManager.draw(renderer);

  SDL_SetRenderTarget(renderer, nullptr);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  float scale = std::min((float)w / CANVAS_WIDTH, (float)h / CANVAS_HEIGHT);

  float scaled_w = CANVAS_WIDTH * scale;
  float scaled_h = CANVAS_HEIGHT * scale;

  float x = (w - scaled_w) * 0.5f;
  float y = (h - scaled_h) * 0.5f;

  SDL_FRect dst = {x, y, scaled_w, scaled_h};
  SDL_RenderTexture(renderer, canvas, nullptr, &dst);
  SDL_RenderPresent(renderer);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer("wrum", 1280, 720, SDL_WINDOW_RESIZABLE, &window,
                              &renderer);
  if (!window) {
    std::cout << "sdl window error " << SDL_GetError() << std::endl;
    return 1;
  }

  if (!renderer) {
    std::cout << "sdl renderer error " << SDL_GetError() << std::endl;
    return 1;
  }

  canvas =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, CANVAS_WIDTH, CANVAS_HEIGHT);
  if (!canvas) {
    std::cout << "sdl canvas error " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_NONE);

  last = SDL_GetPerformanceCounter();

  sock = Socket::create();
  sock->onOpen = [] {
    SDL_Log("connected");
    sock->send(ClientJoin{1});
  };
  sock->onMessage = [](const std::string &m) { SDL_Log("msg: %s", m.c_str()); };
  sock->onError = [](const std::string &e) { SDL_Log("err: %s", e.c_str()); };
  sock->onClose = [](uint16_t c, const std::string &r) {
    SDL_Log("closed %d %s", c, r.c_str());
  };
  sock->onBinary = [](const uint8_t *data, uint32_t size) {
    SDL_Log("binary msg: %d bytes", size);

    try {
      Message m = decode_message((const char *)data, size);
      sceneManager.handleMessage(renderer, m);
    } catch (const std::exception &e) {
      SDL_Log("decode error: %s", e.what());
    };
  };

  sock->connect("ws://127.0.0.1:9001");

  sceneManager.setScene(std::make_unique<GameScene>(renderer));

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, true);
#else
  while (1) {
    loop();
  }
#endif

  return 0;
}
