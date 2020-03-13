#include <stdio.h>
#include <stdbool.h>
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

// Sets the carry flag
void setCF(CPU *cpu, bool state) {
  if (state) 
    cpu->f |= 0x10;
  else 
    cpu->f &= 0xEF;
}

// Sets the half carry flag
void setHF(CPU *cpu, bool state) {
  if (state) 
    cpu->f |= 0x20;
  else
    cpu->f &= 0xDF;
}

// Sets the sets the subraction flag
void setNF(CPU *cpu, bool state) {
  if (state)
    cpu->f |= 0x40;
  else
    cpu->f &= 0xBF;
}

// Sets the sets the subraction flag
void setZF(CPU *cpu, bool state) {
  if (state)
    cpu->f |= 0x80;
  else 
    cpu->f &= 0x7F;
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

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*b + 1) & 0xff) == 0x00);
 setNF(cpu_ptr, false);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*b & 0xf) + (1 & 0xf)) & 0x10 );

 // Carry out the operation
 *b = *b + 1;
} 

// Decrement B
void DEC_B(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *b = getRegister(cpu_ptr, 0); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*b - 1) & 0xff) == 0);
 setNF(cpu_ptr, true);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*b & 0xf) - (1 & 0xf)) < 0x00 );

 // Carry out the operation
 *b = *b - 1;
} 

// Load immediate 8 bits into B
void LD_B_d8(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *b = getRegister(cpu_ptr, 0); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *b = getByte(cart, cpu_ptr->pc + 1);
}
 
// Rotate A left (with wrapping) and save into cf
void RLCA(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 
 unsigned char cf = (cpu_ptr->a & 0x80) >> 7;
 cpu_ptr->a = (cpu_ptr->a << 1) | cf;
 
 setCF(cpu_ptr, cf);
 
 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;
} 

// Load SP into 16 bit addr
void LD_a16_SP(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 
 unsigned short *sp = getRPRegister(cpu_ptr, 3);
 
 unsigned short addr = getNN(cart, cpu_ptr->pc + 1);
 printf(CYN "addr: 0x%04x\n" RESET, addr);
 cpu_ptr->ram[addr] = cpu_ptr->a;
 // Provide the info for the instruction
 info->cycles = 20;
 info->size = 3;
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
 
  // Set pointers for cpu operations
  jumptable[0x00][0x00] = NOP;
  jumptable[0x00][0x01] = LD_BC_d16;
  jumptable[0x00][0x02] = LDINDR_BC_A;
  jumptable[0x00][0x03] = INC_BC;
  jumptable[0x00][0x04] = INC_B;
  jumptable[0x00][0x05] = DEC_B;
  jumptable[0x00][0x06] = LD_B_d8;
  jumptable[0x00][0x07] = RLCA;
  jumptable[0x00][0x08] = LD_a16_SP;
  jumptable[0x03][0x01] = LD_SP_d16;
}
