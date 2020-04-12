#ifndef CPU_H
#define CPU_H
#include <stdint.h>

// Register definitions
#define A 0
#define F 1
#define B 2
#define C 3
#define D 4
#define E 5
#define H 6
#define L 7

// 16 bit registers
#define AF 0
#define BC 1
#define DE 2
#define HL 3

// Special registers
#define SP 8
#define PC 9

// Flag definitions
#define CF 4
#define HF 5
#define NF 6
#define ZF 7 

typedef struct Op_info {
  // The number of cycles for the instruction
  int cycles;
  // The size of the instruction
  int size;
} Op_info;

typedef struct MMU {
  uint8_t BIOS[0x0100]; 
  uint8_t ram[0xFFFF];

  /* This flag is initialized to 0xFF50 */
  /* When the BIOS is unmapped the flag is set */
  /* to true (1). The cartridge ram is now mapped*/
  /* from 0x0000 to 0x00FF. */
  uint8_t *finishedBIOS;
  
} MMU;

typedef void (*func_ptr)(void *, Op_info *);

#include "jumptable.h"
typedef struct CPU {
  //Regular registers
  uint8_t a;
  uint8_t f;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  //Program counter
  uint16_t pc;
  //Stack pointer
  uint16_t sp;
  
  // Special timer register
  uint16_t *cycle_count;
  //Memory
  MMU *mmu;
  // Opcode jumptable
  func_ptr jumptable[0xF][0xF];
  // Extended opcode jumptable (CB prefix)
  func_ptr cb_jumptable[0xF][0xF];
  /* How many more cycles the cpu has in it */
  /* it's current instruction. */
  uint8_t cycles_left;
} CPU;

CPU initCPU();
void freeCPU(CPU *cpu);

uint16_t read_r16(CPU *cpu, int index); 
void write_r16(CPU *cpu, int index, uint16_t val);

uint8_t* getRegister(CPU *cpu, int index);

void printCpu(CPU cpu);
void cycle_cpu(CPU *cpu);
#endif
