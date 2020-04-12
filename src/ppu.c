#include <SDL2/SDL.h>
#include <stdbool.h> 
#include "ceebee/ppu.h"
#include "ceebee/gpu.h"
#include "ceebee/common.h"

#include <stdlib.h>

/* PPU MODES */
#define OAM 0x00
#define VRAM 0x01
#define HBLANK 0x02
#define VBLANK 0x03
/* MODE CYCLE LENGTHS */
#define OAM_T 80
#define VRAM_T 172
#define HBLANK_T 204
/* TIME FOR LINE TO FINISH */
#define LINE_T OAM_T + VRAM_T + HBLANK_T
#define VBLANK_T 10 * LINE_T
/* TIME TO FINISH WHOLE FRAME */
#define FRAM_T PIXELS_H * LINE_T + VBLANK_T


PPU init_ppu(CPU *cpu) {
  PPU ppu;

  ppu.mode = 0x00;
  ppu.cycles_left = OAM_T;
   
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

bool lcd_enabled(PPU *ppu) {
  bool enabled = (*ppu->LCDCONT) >> 7;
  return enabled;
}

void render(CPU *cpu, GPU *gpu, PPU *ppu) {

  if (lcd_enabled(ppu)) {
    int window_size = gpu->window_height * gpu->window_width;
    for (int i = 0; i < window_size; i++) {
      gpu->pixels[i] = rand();
    } 
    update_window(*gpu);
  }
  

}

void cycle_ppu(CPU *cpu, GPU *gpu, PPU *ppu) {
  if (ppu->cycles_left != 0) {
    ppu->cycles_left--;
  }

  switch(ppu->mode) {
    case OAM:
      // Check if OAM had enough time to finish
      if (ppu->cycles_left == 0) {
        ppu->mode = VRAM;
        ppu->cycles_left = VRAM_T;
      } 
      break;
    
    case VRAM:
      if (ppu->cycles_left == 0) {
        ppu->mode = HBLANK;
        ppu->cycles_left = HBLANK_T;
        render(cpu, gpu, ppu);
      }
      break;
    
    case HBLANK:
      if (ppu->cycles_left == 0) {
        if (*ppu->CURLINE == PIXELS_H - 1) {
          ppu->mode = VBLANK;
          ppu->cycles_left = VBLANK_T;
        }
        else {
          ppu->mode = OAM;
          ppu->cycles_left = OAM_T;
        }
      }
      break;
  
    case VBLANK:
      if (ppu->cycles_left == 0) {
        ppu->mode = OAM;
        ppu->cycles_left = OAM_T;
      }
      break;
  }
}
