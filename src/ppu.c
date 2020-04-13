#include <SDL2/SDL.h>
#include <stdbool.h> 
#include <unistd.h> 
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

bool background_enabled(PPU *ppu) {
  bool enabled = (*ppu->LCDCONT) & 0x01;
  return enabled;
}

uint16_t background_addr(PPU *ppu) {
  bool flag = ((*ppu->LCDCONT) >> 3) & 0x01;
  uint16_t addr = flag ? 0x9C00 : 0x9FFF;
  return addr;
}

uint16_t tilepattern_addr(PPU *ppu) {
  bool flag = ((*ppu->LCDCONT) >> 4) & 0x01;
  uint16_t addr = flag ? 0x8000 : 0x8800;
  return addr;
}

void renderScan(CPU *cpu, GPU *gpu, PPU *ppu) {

  if (lcd_enabled(ppu)) {
    if (background_enabled(ppu)) {
      int window_size = gpu->window_height * gpu->window_width;
      for (int i = 0; i < window_size; i++) {
        gpu->pixels[i] = rand();
      } 
        
    }
  } 

  update_window(gpu);
}

void cycle_ppu(CPU *cpu, GPU *gpu, PPU *ppu) {
  ppu->modeclock += cpu->t;
  /* printf("cpu_t: %04d modeclock: %04d mode: %02x\n", cpu->t, ppu->modeclock, ppu->mode); */
  switch(ppu->mode) {
    case OAM:
      // Check if OAM had enough time to finish
      if (ppu->modeclock >= OAM_T) {
        ppu->mode = VRAM;
        ppu->modeclock = 0;
      } 
      break;
    
    case VRAM:
      if (ppu->modeclock >= VRAM_T) {
        ppu->mode = HBLANK;
        ppu->modeclock = 0;
        renderScan(cpu, gpu, ppu);
      }
      break;
    
    case HBLANK:
      if (ppu->modeclock >= HBLANK_T) {
        *ppu->CURLINE += 1;
        ppu->modeclock = 0;
        if (*ppu->CURLINE == 143) {
          ppu->mode = VBLANK;
        }
        else {
          ppu->mode = OAM;
        }
      }
      break;
  
    case VBLANK:
      if (ppu->modeclock >= 456) {
        *ppu->CURLINE += 1;
        ppu->modeclock = 0;
        if (*ppu->CURLINE > 153) {
          printf("VBLANK!\n");
          ppu->mode = OAM;
          *ppu->CURLINE = 0;
        }
      }
      break;
  }
}
