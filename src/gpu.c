#include <SDL2/SDL.h>
#include "ceebee/gpu.h"
#include "ceebee/common.h"

#define SDL_MAIN_HANDLED

const int window_height = PIXELS_H * 4;
const int window_width = PIXELS_W * 4;

GPU init_gpu() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    panic(NULL, "Could not open sdl :/ Is it installed?\n");
  }

  GPU gpu;

  gpu.window = SDL_CreateWindow("CeeBee",
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED,
    window_height,
    window_width,
    SDL_WINDOW_SHOWN);
 
  return gpu;
}

void destroy_gpu(GPU *gpu) {
  SDL_DestroyWindow(gpu->window);
}
