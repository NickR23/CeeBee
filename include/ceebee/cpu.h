#ifndef CPU_H
#define CPU_H

// Register definitions
#define A 0
#define F 1
#define B 2
#define C 3
#define D 4
#define E 5
#define H 6
#define L 7
#define SP 8
#define PC 9

typedef struct Op_info {
  // The number of cycles for the instruction
  int cycles;
  // The size of the instruction
  int size;
} Op_info;

typedef void (*func_ptr)(unsigned char const* cart, void *, Op_info *);

#include "jumptable.h"
typedef struct CPU {
  //Regular registers
  unsigned short af;
  unsigned short bc;
  unsigned short de;
  unsigned short hl;
  //Program counter
  unsigned short pc;
  //Stack pointer
  unsigned short sp;
  //Memory
  unsigned char* ram;
  // Opcode jumptable
  func_ptr jumptable[0xF][0xF];
} CPU;

CPU initCPU();
void freeCPU(CPU *cpu);

unsigned short* getRegister16(CPU *cpu, int index);
unsigned char* getRegister(CPU *cpu, int index);

unsigned char getByte(unsigned char const *cart, unsigned short addr);
unsigned short getNN(unsigned char const *cart, unsigned short addr);

void printCpu(CPU cpu);
void run_cycle(CPU *cpu, unsigned char const *cart);
unsigned char* loadCart(char const *cartPath, unsigned int* cartSize);
#endif
