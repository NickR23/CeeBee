#include <stdio.h>
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

void NOP() {
  printf("NOP\n");
}

// Load the next 16 bits into sp
void LD_SP_d16(unsigned char const* cart, void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cart, cpu_ptr->pc + 1);
  printf("nn: %04x\n", nn);
  cpu_ptr->sp = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Lets u kno that this opcode is not implemented yet
void NOT_IMPL(unsigned char const* cart, void *cpu, Op_info *info) {
  printf("This instruction is not yet implemented :)\n");
}

void init_jmp (func_ptr jumptable[0xF][0xF]) {
  // Init table to NOT_IMPL
  for (int i = 0; i < 0xF; i++) {
    for (int j = 0; j < 0xF; j++) {
     jumptable[i][j] = NOT_IMPL;
    }
  }
 
  jumptable[0x00][0x00] = NOP;
  jumptable[0x03][0x01] = LD_SP_d16;
}
