#include <SDL2/SDL.h>
#include "ceebee/ppu.h"
#include "ceebee/gpu.h"
#include "ceebee/common.h"

#include <stdlib.h>

void cycle_ppu(GPU *gpu) {
  int window_size = gpu->window_height * gpu->window_width;
  for (int i = 0; i < window_size; i++) {
    gpu->pixels[i] = rand();
  } 
  update_window(*gpu);
}
