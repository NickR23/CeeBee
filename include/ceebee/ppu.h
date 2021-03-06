#ifndef PPU_H
   #define PPU_H
   #include "ceebee/cpu.h"
   #include "ceebee/gpu.h"
   typedef struct PPU {
      uint8_t mode;
      uint8_t *LCDCONT;
      uint8_t *LCDSTAT;
      uint8_t *SCROLLY;
      uint8_t *SCROLLX;
      uint8_t *CURLINE;
      uint8_t *CMPLINE;
      uint8_t *DMACONT;
      uint8_t *BGRDPAL;
      uint8_t *OBJ0PAL;
      uint8_t *OBJ1PAL;
      uint8_t *WNDPOSY;
      uint8_t *WNDPOSX;
      uint16_t modeclock;
   } PPU;
   PPU init_ppu(CPU *cpu);
   void cycle_ppu(CPU *cpu, GPU *gpu, PPU *ppu);
#endif
