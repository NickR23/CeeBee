#include <SDL2/SDL.h>
#include "ceebee/gpu.h"
#include "ceebee/common.h"

#define SDL_MAIN_HANDLED

GPU init_gpu() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    panic(NULL, "Could not open sdl :/ Is it installed?\n");
  }

  GPU gpu;

  gpu.window_height = PIXELS_H * 4;
  gpu.window_width = PIXELS_W * 4;

  gpu.window = SDL_CreateWindow("CeeBee",
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED,
    gpu.window_height,
    gpu.window_width,
    SDL_WINDOW_SHOWN);

  gpu.rend = SDL_CreateRenderer(gpu.window, -1, SDL_RENDERER_ACCELERATED);
  
  gpu.framebuffer = SDL_CreateTexture(
    gpu.rend,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    gpu.window_width,
    gpu.window_height);
  
  // These are the actual pixels of the framebuffer
  int window_size = gpu.window_height * gpu.window_width;
  gpu.pixels = (uint32_t*) malloc(window_size * (sizeof(uint32_t)));
  
  return gpu;
}

void update_window(const GPU gpu) {
  SDL_UpdateTexture(gpu.framebuffer, NULL, gpu.pixels, gpu.window_width * sizeof(uint32_t));
  SDL_RenderClear(gpu.rend);
  SDL_RenderCopy(gpu.rend, gpu.framebuffer, NULL, NULL);
  SDL_RenderPresent(gpu.rend); 
}

void destroy_gpu(GPU *gpu) {
  SDL_DestroyWindow(gpu->window);
  free(gpu->pixels);
}
