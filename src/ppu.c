#include <SDL2/SDL.h>
#include "ceebee/ppu.h"
#include "ceebee/gpu.h"
#include "ceebee/common.h"

#include <stdlib.h>

PPU init_ppu(CPU *cpu) {
  PPU ppu;
   
  // Initialize LCD IO registers 
  ppu.LCDCONT = (uint8_t *) cpu->mmu->ram + 0xFF40;
  ppu.LCDSTAT = (uint8_t *) cpu->mmu->ram + 0xFF41;
  ppu.SCROLLY = (uint8_t *) cpu->mmu->ram + 0xFF42;
  ppu.SCROLLX = (uint8_t *) cpu->mmu->ram + 0xFF43;
  ppu.CURLINE = (uint8_t *) cpu->mmu->ram + 0xFF44;
  ppu.CMPLINE = (uint8_t *) cpu->mmu->ram + 0xFF45;
  ppu.DMACONT = (uint8_t *) cpu->mmu->ram + 0xFF46;
  ppu.BGRDPAL = (uint8_t *) cpu->mmu->ram + 0xFF47;
  ppu.OBJ0PAL = (uint8_t *) cpu->mmu->ram + 0xFF48;
  ppu.OBJ1PAL = (uint8_t *) cpu->mmu->ram + 0xFF49;
  ppu.WNDPOSY = (uint8_t *) cpu->mmu->ram + 0xFF4A;
  ppu.WNDPOSX = (uint8_t *) cpu->mmu->ram + 0xFF4B;
 
  return ppu;
}

void cycle_ppu(CPU *cpu, GPU *gpu) {
  int window_size = gpu->window_height * gpu->window_width;
  for (int i = 0; i < window_size; i++) {
    gpu->pixels[i] = rand();
  } 
  update_window(*gpu);
}
