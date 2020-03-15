#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ceebee/cpu.h"
#include "ceebee/common.h"
#include "ceebee/termColors.h"
#include "ceebee/jumptable.h"

/* BOOT ROM */
static unsigned char BIOS[0x100] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50 };

CPU initCPU() {
  CPU cpu;

  /* The gameboy can address 65,536 positions in memory */
  /* http://gameboy.mongenel.com/dmg/asmmemmap.html */
  cpu.mmu = (unsigned char*) malloc(sizeof(unsigned char) * (0x10000));
  
  mmu_load_boot_rom(cpu.mmu);
  
  init_jmp(cpu.jumptable);
  return cpu;
}

/* Load bootstrap in to memory */
/* https://blog.ryanlevick.com/DMG-01/public/book/memory_map.html#a0x0000---0x00ff-boot-rom */
/* The boot rom bootstraps into the main cart. Located at 0x0000 to 0x00FF */
void mmu_load_boot_rom(unsigned char *mmu) {
  memcpy(mmu, BIOS, sizeof(BIOS));
}


void freeCPU(CPU *cpu)	{
  free(cpu->mmu);
}

unsigned short* getRegister16(CPU *cpu, int index) {
  switch(index) {
    case 0:
      return &cpu->af;
    case 2:
      return &cpu->bc;
    case 4:
      return &cpu->de;
    case 6:
      return &cpu->hl;
    case 8:
      return &cpu->sp;
    case 9:
      return &cpu->pc;
    default: 
      return NULL;
    }
}

unsigned char* getRegister(CPU *cpu, int index) {
  switch(index){
    case 0:
      return (unsigned char*) &cpu->af;
    case 1: 
      return (unsigned char*) &cpu->af + 1;
    case 2:
      return (unsigned char*) &cpu->bc;
    case 3:
      return (unsigned char*) &cpu->bc + 1;
    case 4:
      return (unsigned char*) &cpu->de;
    case 5:
      return (unsigned char*) &cpu->de + 1;
    case 6:
      return (unsigned char*) &cpu->hl;
    case 7:
      return (unsigned char*) &cpu->hl + 1;
    default:
      return NULL;
    }
}

//Gets the next 16 bits in little endian from addr
//addr should be a pointer to the end of your op code.
unsigned short getNN(CPU *cpu, unsigned short addr) {
  short byte_2_addr = addr + 1;
  //Get first nibble
  char x2 = cpu->mmu[byte_2_addr] & 0x0f;
  //Get second nibble
  char x1 = cpu->mmu[byte_2_addr] >> 4;
  //Get third nibble
  char x4 = cpu->mmu[addr] & 0x0f;
  //Get fourth nibble
  char x3 = cpu->mmu[addr] >> 4;
  
  unsigned short nn = x1 << 12;
  nn = nn | (x2 << 8);
  nn = nn | (x3 << 4);
  nn = nn | x4;

  return nn;
} 

// Gets next 8 bits from addr
unsigned char getByte(CPU *cpu, unsigned short addr) {
  return cpu->mmu[addr];
}

void printCpu(CPU cpu) {
  unsigned char b = *getRegister(&cpu, B);
  unsigned char c = *getRegister(&cpu, C);
  unsigned char d = *getRegister(&cpu, D);
  unsigned char e = *getRegister(&cpu, E);
  unsigned char a = *getRegister(&cpu, A);
  unsigned char f = *getRegister(&cpu, F);
  unsigned char h = *getRegister(&cpu, H);
  unsigned char l = *getRegister(&cpu, L);
  unsigned short pc = *getRegister16(&cpu, PC);
  unsigned short sp = *getRegister16(&cpu, SP);
  
  printf(MAG "~~~~~~~~~~~~~REGISTERS~~~~~~~~~~~~~~~~~\n" RESET);
  printf(GRN "\t\tb: 0x%02x\n", b);
  printf(GRN "\t\tc: 0x%02x\n", c);
  printf(GRN "\t\td: 0x%02x\n", d);
  printf(GRN "\t\te: 0x%02x\n", e);
  printf(GRN "\t\ta: 0x%02x\n", a);
  printf(GRN "\t\tf: 0x%02x\n", f);
  printf(GRN "\t\th: 0x%02x\n", h);
  printf(GRN "\t\tl: 0x%02x\n", l);
  printf(GRN "\t\tpc: 0x%04x\n", pc);
  printf(GRN "\t\tsp: 0x%04x\n", sp);
  printf(MAG "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
}

unsigned char* loadCart(char const *cartPath, unsigned int* cartSize) {
  FILE *fp;
  unsigned char *buffer;
  long cartLength;
  
  fp = fopen(cartPath, "rb");
  if (fp == NULL) {
    panic(RED "Could not load cart" RESET);
  }
  fseek(fp, 0, SEEK_END);
  cartLength = ftell(fp);
  *cartSize = cartLength;
  rewind(fp);

  buffer = (unsigned char*)malloc(cartLength * sizeof(unsigned char));
  fread(buffer, cartLength, 1, fp);
  fclose(fp);
  return buffer;
}

void printCart(int start, unsigned char const *cart) {
  for (int i = 0; i < 20; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}

void print_code_info(Op_info info) {
  printf(MAG "\tCycles: %d\n\tSize: %d\n" RESET, info.cycles, info.size);
}

Op_info run_cycle(CPU *cpu) {
  unsigned char code = cpu->mmu[cpu->pc];
  struct Op_info info;

  // Initialize the info struct
  info.size = 0;
  info.cycles = 0; 

  #ifdef DEBUG
    printf(YEL "PC: 0x%04hx\tCode: 0x%02x\n" RESET, cpu->pc, code);
    printCpu(*cpu);
  #endif


  // Parse the code
  int hi = code >> 4;
  int lo = code & (0x0F);
  // Run the opcode for the instruction
  cpu->jumptable[hi][lo](cpu, &info);

  // Offset the pc register
  cpu->pc += info.size;
 
  return info;
}
