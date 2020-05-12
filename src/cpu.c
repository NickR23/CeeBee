#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ceebee/cpu.h"
#include "ceebee/mmu.h"
#include "ceebee/common.h"
#include "ceebee/termColors.h"
#include "ceebee/jumptable.h"

CPU initCPU() {
  CPU cpu;

  /* The gameboy can address 65,536 positions in memory */
  /* http://gameboy.mongenel.com/dmg/asmmemmap.html */
  cpu.mmu = (MMU*) malloc(sizeof(MMU));
  
  mmu_load_boot_rom(cpu.mmu);
  
  init_jmp(cpu.jumptable, cpu.cb_jumptable);
  // Initialize the counter
  cpu.cycle_count = (uint16_t*) cpu.mmu->ram + 0xFF05;
  *cpu.cycle_count = 0;
  // Initialize into boot mode
  cpu.mmu->finishedBIOS = (uint8_t*) cpu.mmu->ram + 0xFF50;
  *cpu.mmu->finishedBIOS = 0;
 
  cpu.t = 0;
  return cpu;
}

void freeCPU(CPU *cpu)	{
  free(cpu->mmu);
}

/* Returns value of register as 16 bits*/
uint16_t read_r16(CPU *cpu, int index) {
  uint16_t res;
  switch(index){
    case AF:
      res = (cpu->a << 8) | (cpu->f);
      return res;
    case BC:
      res = (cpu->b << 8) | (cpu->c);
      return res;
    case DE:
      res = (cpu->d << 8) | (cpu->e);
      return res;
    case HL:
      res = (cpu->h << 8) | (cpu->l);
      return res;
    case SP:
      return cpu->sp;
    case PC:
      return cpu->pc;
    default:
      return 0x0000;
    }
}

/* Write value to register as 16 bits*/
void write_r16(CPU *cpu, int index, uint16_t val) {
  uint8_t *hi_reg;
  uint8_t *low_reg;
  switch(index){
    case AF:
      hi_reg = &cpu->a;
      low_reg = &cpu->f;
      break;
    case BC:
      hi_reg = &cpu->b;
      low_reg = &cpu->c;
      break;
    case DE:
      hi_reg = &cpu->d;
      low_reg = &cpu->e;
      break;
    case HL:
      hi_reg = &cpu->h;
      low_reg = &cpu->l;
      break;
    case SP:
      cpu->sp = val;
      return;
    case PC:
      cpu->pc = val;
      return;
    default:
      return;
    }
    *hi_reg = (val >> 8) & 0xFF;
    *low_reg = val & 0xFF; 
}

uint8_t* getRegister(CPU *cpu, int index) {
  switch(index){
    case 0:
      return (uint8_t*) &cpu->a;
    case 1: 
      return (uint8_t*) &cpu->f;
    case 2:
      return (uint8_t*) &cpu->b;
    case 3:
      return (uint8_t*) &cpu->c;
    case 4:
      return (uint8_t*) &cpu->d;
    case 5:
      return (uint8_t*) &cpu->e;
    case 6:
      return (uint8_t*) &cpu->h;
    case 7:
      return (uint8_t*) &cpu->l;
    default:
      return NULL;
    }
}

void printCpu(CPU cpu) {
  uint8_t b = *getRegister(&cpu, B);
  uint8_t c = *getRegister(&cpu, C);
  uint8_t d = *getRegister(&cpu, D);
  uint8_t e = *getRegister(&cpu, E);
  uint8_t a = *getRegister(&cpu, A);
  uint8_t f = *getRegister(&cpu, F);
  uint8_t h = *getRegister(&cpu, H);
  uint8_t l = *getRegister(&cpu, L);
  uint16_t pc = read_r16(&cpu, PC);
  uint16_t sp = read_r16(&cpu, SP);
  
  printf("~~~~~~~~~~~~~REGISTERS~~~~~~~~~~~~~~~~~\n");
  printf("\t\tb: 0x%02x\n", b);
  printf("\t\tc: 0x%02x\n", c);
  printf("\t\td: 0x%02x\n", d);
  printf("\t\te: 0x%02x\n", e);
  printf("\t\ta: 0x%02x\n", a);
  printf("\t\tf: 0x%02x\n", f);
  printf("\t\th: 0x%02x\n", h);
  printf("\t\tl: 0x%02x\n", l);
  printf("\t\tpc: 0x%04x\n", pc);
  printf("\t\tsp: 0x%04x\n", sp);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void printCart(int start, uint8_t const *cart) {
  for (int i = 0; i < 20; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}

void print_code_info(Op_info info) {
  printf(MAG "\tCycles: %d\n\tSize: %d\n" RESET, info.cycles, info.size);
}

void cycle_cpu(CPU *cpu) {
  // Skip boot copyright check
  if (cpu->pc >= 0x00E0 && cpu->pc <= 0x00FA) {
    printf("Skipping boot checksum\n");
    cpu->pc = 0x00FC;
  } 

  uint8_t code = readN(cpu, cpu->pc);
  struct Op_info info;

  // Initialize the info struct
  info.size = 0;
  info.cycles = 0; 
  #ifdef DEBUG
    printf("PC: 0x%04hx\tCode: 0x%02x CurrentLine: %02x\n", cpu->pc, code, cpu->mmu->ram[0xff44]);
    printCpu(*cpu);
  #endif

  // Parse the code
  int hi = code >> 4;
  int lo = code & (0x0F);

  // Handle CB prefix
  if (code == 0xCB) {
    /* Update the program counter */
    cpu->pc += 1;
    /* Get the new code */
    code = readN(cpu, cpu->pc);
    #ifdef DEBUG
      printf("***********CB PREFIX********\n");
      printf("PC: 0x%04hx\tCode: 0x%02x\n", cpu->pc, code);
      printCpu(*cpu);
    #endif

    hi = code >> 4;
    lo = code & (0x0F);
    cpu->cb_jumptable[hi][lo](cpu, &info);
  }
  else {
    // Run the opcode for the instruction
    cpu->jumptable[hi][lo](cpu, &info);
  }

  // Offset the pc register
  cpu->pc += info.size;
  cpu->t = info.cycles;
}
