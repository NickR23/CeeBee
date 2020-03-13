#include <stdio.h>
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

// Sets the carry flag
void setCF(CPU *cpu) {
  cpu->f |= 0x10;
}

// Sets the half carry flag
void setHF(CPU *cpu) {
  cpu->f |= 0x20;
}

// Sets the sets the subraction flag
void setNF(CPU *cpu) {
  cpu->f |= 0x40;
}

// Sets the sets the subraction flag
void setZF(CPU *cpu) {
  cpu->f |= 0x80;
}


void NOP(unsigned char const* cart, void *cpu, Op_info *info) {
  printf("NOP\n");
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

// Load the next 16 bits into sp
void LD_SP_d16(unsigned char const* cart, void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cart, cpu_ptr->pc + 1);
  cpu_ptr->sp = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load the next 16 bits into bc
void LD_BC_d16(unsigned char const* cart, void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cart, cpu_ptr->pc + 1);
  unsigned short* bc = getRPRegister(cpu_ptr, 0);
  *bc = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load a into addr in BC
void LDINDR_BC_A(unsigned char const* cart, void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short *bc = getRPRegister(cpu_ptr, 0); 
  cpu_ptr->ram[*bc] = cpu_ptr->a;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Increment BC
void INC_BC(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short *bc = getRPRegister(cpu_ptr, 0); 
 *bc = *bc + 1;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment B
void INC_B(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *b = getRegister(cpu_ptr, 0); 
 *b = *b + 1;
 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

} 

// Lets u kno that this opcode is not implemented yet
void NOT_IMPL(unsigned char const* cart, void *cpu, Op_info *info) {
  printf(WHT "This instruction is not yet implemented :)\n" RESET);
}

void init_jmp (func_ptr jumptable[0xF][0xF]) {
  // Init table to NOT_IMPL
  for (int i = 0; i < 0xF; i++) {
    for (int j = 0; j < 0xF; j++) {
     jumptable[i][j] = NOT_IMPL;
    }
  }
 
  jumptable[0x00][0x00] = NOP;
  jumptable[0x00][0x01] = LD_BC_d16;
  jumptable[0x00][0x02] = LDINDR_BC_A;
  jumptable[0x00][0x03] = INC_BC;
  jumptable[0x00][0x04] = INC_B;
  jumptable[0x03][0x01] = LD_SP_d16;
}