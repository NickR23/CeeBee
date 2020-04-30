#include <SDL2/SDL.h>
#include <stdbool.h> 
#include <stdio.h> 
#include <unistd.h> 
#include "ceebee/ppu.h"
#include "ceebee/gpu.h"
#include "ceebee/mmu.h"
#include "ceebee/cpu.h"
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


enum COLOR {WHITE, LIGHT_GREY, DARK_GREY, BLACK};


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

bool window_enabled(PPU *ppu) {
  bool enabled = ((*ppu->LCDCONT) >> 5) * 0x01;
  return enabled;
}

bool background_enabled(PPU *ppu) {
  bool enabled = (*ppu->LCDCONT) & 0x01;
  return enabled;
}

uint16_t background_addr(PPU *ppu) {
  bool flag = ((*ppu->LCDCONT) >> 3) & 0x01;
  uint16_t addr = flag ? 0x9C00 : 0x9800;
  return addr;
}

uint16_t window_addr(PPU *ppu) {
  bool flag = ((*ppu->LCDCONT) >> 6) & 0x01;
  uint16_t addr = flag ? 0x9C00 : 0x9800;
  return addr;
}

uint16_t tilepattern_addr(PPU *ppu) {
  bool flag = ((*ppu->LCDCONT) >> 4) & 0x01;
  uint16_t addr = flag ? 0x8000 : 0x8800;
  return addr;
}

enum COLOR getColor(uint8_t colorNum, CPU *cpu, PPU *ppu) {
  enum COLOR res = WHITE;
  uint8_t palette = *ppu->BGRDPAL;
  int hi = 0;
  int lo = 0;
 
  switch(colorNum) {
    case 0: hi = 1; lo = 0; break;
    case 1: hi = 3; lo = 2; break;
    case 2: hi = 5; lo = 4; break;
    case 3: hi = 7; lo = 6; break;
  }
  
  int color = 0;
  color = ((palette >> hi) & 0x01) << 1;
  color |= ((palette >> lo) & 0x01);
  /* printf("Color: %02x\n", color); */
  
  switch(color) {
    case 0: res = WHITE; break;
    case 1: res = LIGHT_GREY; break;
    case 2: res = DARK_GREY; break;
    case 3: res = DARK_GREY; break;
  }
  return res;
}

void renderScan(CPU *cpu, GPU *gpu, PPU *ppu) {
  uint16_t tileData = 0;
  uint16_t backgroundMem = 0;
  bool unsig = true;
  
  uint8_t scrollX = *ppu->SCROLLX;
  uint8_t scrollY = *ppu->SCROLLY;
  uint8_t windowX = *ppu->WNDPOSX;
  uint8_t windowY = *ppu->WNDPOSY;
  
  bool usingWindow = false;

  bool stop = false;
  if (window_enabled(ppu)) {
    if (windowY <= (*ppu->CURLINE))
      usingWindow = true;
  }
  
  if (tilepattern_addr(ppu) == 0x8000) {
    tileData = 0x8000;
  } else {
    tileData = 0x8800;
    unsig = false;
  }

  if (usingWindow == false) {
    backgroundMem = background_addr(ppu);
  }
  else {
    backgroundMem = window_addr(ppu);
  }
  
  /* printf("Background Memory: 0x%04x\n", backgroundMem); */
  
  uint8_t yPos = 0;
  
  if (!usingWindow)
    yPos = scrollY + (*ppu->CURLINE);
  else
    yPos = (*ppu->CURLINE) - windowY;
 
  uint16_t tileRow = (((uint8_t) (yPos/8))*32);
  
  for (int pixel = 0; pixel < 160; pixel++) {
    uint8_t xPos = pixel + scrollX;
 
    if (usingWindow) {
      if (pixel >= windowX) {
        xPos = pixel - windowX;
      }
    }
    
    uint16_t tileCol = (xPos / 8);
    int16_t tileNum;

    uint16_t tileAddress = backgroundMem + tileRow + tileCol;
    if (unsig)
      tileNum = (uint8_t) readN(cpu, tileAddress);
    else
      tileNum = (int8_t) readN(cpu, tileAddress);
   
    uint16_t tileLocation = tileData;
   
    if (unsig)
      tileLocation += (tileNum * 16);
    else
      tileLocation += ((tileNum + 128) * 16);
  
    uint8_t line = yPos % 8;
    line *= 2;
    uint8_t data1 = readN(cpu, tileLocation + line);
    uint8_t data2 = readN(cpu, tileLocation + line + 1);
  
    int colourBit = xPos % 8;
    colourBit -= 7;
    colourBit *= -1;
    
    int colourNum = (data2 >> colourBit) & 0x01;
    colourNum = colourNum << 1;
    colourNum |= (data1 >>  colourBit) & 0x01;
    
    
    enum COLOR col = getColor(colourNum, cpu, ppu);
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    
    switch(col) {
      case WHITE:
        red = 255;
        green = 255;
        blue = 255;
        break;
      case LIGHT_GREY:
        red = 0xCC;
        green = 0xCC;
        blue = 0xCC;
        break;
      case DARK_GREY:
        red = 0x77;
        green = 0x77;
        blue = 0x77;
        break;
      case BLACK:
        red = 0x00;
        green = 0x00;
        blue = 0x00;
        break;
    }
    
    int finalY = *ppu->CURLINE;
    uint32_t final_color = 0x000000FF;
    final_color |= (red << 24); 
    final_color |= (green << 16); 
    final_color |= (blue << 8); 
    
    gpu->pixels[pixel + finalY * PIXELS_W] = final_color;
  }

  update_window(gpu);
  /* sleep(1); */
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
          ppu->mode = OAM;
          *ppu->CURLINE = 0;
        }
      }
      break;
  }
}
