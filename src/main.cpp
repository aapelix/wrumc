#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <cstddef>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

bool running = true;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* canvas;
SDL_Event event;
SDL_Texture* img;
float rotation = 0;

Uint64 now, last = 0;
double dt = 0;

static void loop() {
    if (!running) {
        SDL_DestroyTexture(img);
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

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
    }

    now = SDL_GetPerformanceCounter();
    dt = (double)(now - last) / SDL_GetPerformanceFrequency();
    last = now;

    rotation += 90 * dt;

    SDL_SetRenderTarget(renderer, canvas);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_FRect rect = { 100, 100, 200, 150 };
    SDL_RenderFillRect(renderer, &rect);

    float img_w, img_h;
    SDL_GetTextureSize(img, &img_w, &img_h);
    SDL_FRect rect2 = { 150, 150, img_w, img_h };
    SDL_RenderTextureRotated(renderer, img, nullptr, &rect2, rotation, nullptr, SDL_FLIP_NONE);

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderClear(renderer);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    SDL_FRect dst = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
    SDL_RenderTexture(renderer, canvas, nullptr, &dst);
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("wrum", 1280, 720, 0);
    if (!window) {
        std::cout << "sdl window error " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "sdl renderer error " << SDL_GetError() << std::endl;
        return 1;
    }

    canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
    if (!canvas) {
        std::cout << "sdl canvas error " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_NONE);

    img = IMG_LoadTexture(renderer, "assets/cars/img_1.png");
    SDL_SetTextureScaleMode(img, SDL_SCALEMODE_NEAREST);

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, true);
    #else
    while (1) { loop(); }
    #endif

    return 0;
}
