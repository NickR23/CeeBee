#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ceebee/cpu.h"
#include "ceebee/common.h"
#include "ceebee/termColors.h"
#include "ceebee/jumptable.h"

CPU initCPU() {
  CPU cpu;

  /* The gameboy can address 65,536 positions in memory */
  /* http://gameboy.mongenel.com/dmg/asmmemmap.html */
  cpu.ram = (unsigned char*) malloc(sizeof(unsigned char) * (65536));

  init_jmp(cpu.jumptable);
  return cpu;
}

void freeCPU(CPU *cpu)	{
  free(cpu->ram);
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
      return (unsigned char*) &cpu->bc +1;
    default:
      return NULL;
    }
}

//Gets the next 16 bits in little endian from addr
//addr should be a pointer to the end of your op code.
unsigned int getNN(unsigned char const* cart, unsigned short addr) {
  int16_t byte_2_addr = addr + 1;
  //Get first nibble
  char x2 = cart[byte_2_addr] & 0x0f;
  //Get second nibble
  char x1 = cart[byte_2_addr] >> 4;
  //Get third nibble
  char x4 = cart[addr] & 0x0f;
  //Get fourth nibble
  char x3 = cart[addr] >> 4;
  
  int nn = x1 << 12;
  nn = nn | (x2 << 8);
  nn = nn | (x3 << 4);
  nn = nn | x4;

  return nn;
} 

// Gets next 8 bits from addr
unsigned char getByte(unsigned char const *cart, unsigned short addr) {
  return cart[addr];
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
  unsigned char const *ptr = &cart[start];
  for (int i = 0; i < 20; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}

void print_code_info(Op_info info) {
  printf(MAG "\tCycles: %d\n\tSize: %d\n" RESET, info.cycles, info.size);
}

void run_cycle(CPU *cpu, unsigned char const *cart) {
  //unsigned char code = cart[cpu->pc];
  unsigned char code = 0x03;
  struct Op_info info;

  #ifdef DEBUG
    printf(YEL "PC: 0x%04hx\tCode: 0x%02x\n" RESET, cpu->pc, code);
    printCpu(*cpu);
  #endif


  // Parse the code
  int hi = code >> 4;
  int lo = code & (0x0F);
  // Run the opcode for the instruction
  //cpu->jumptable[hi][lo](cart, cpu, &info);
  cpu->jumptable[0x0][0x8](cart, cpu, &info);

  // Offset the pc register
  cpu->pc += info.size;
   
  // Reset the info struct
  info.size = 0;
  info.cycles = 0; 
}
