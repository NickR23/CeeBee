#include <stdio.h>
#include <stdbool.h>
#ifndef JMPTBL_H
  #include "ceebee/jumptable.h"
#endif
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

// Sets the carry flag
void setCF(CPU *cpu, bool state) {
  unsigned char *f = getRegister(cpu, F); 
  if (state) 
    *f |= 0x10;
  else 
    *f &= 0xEF;
}

// Sets the half carry flag
void setHF(CPU *cpu, bool state) {
  unsigned char *f = getRegister(cpu, F); 
  if (state) 
    *f |= 0x20;
  else
    *f &= 0xDF;
}

// Sets the sets the subraction flag
void setNF(CPU *cpu, bool state) {
  unsigned char *f = getRegister(cpu, F); 
  if (state)
    *f |= 0x40;
  else
    *f &= 0xBF;
}

// Sets the sets the subraction flag
void setZF(CPU *cpu, bool state) {
  unsigned char *f = getRegister(cpu, F); 
  if (state)
    *f |= 0x80;
  else 
    *f &= 0x7F;
}


void NOP(unsigned char const* cart, void *cpu, Op_info *info) {
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
  unsigned short* bc = getRegister16(cpu_ptr, B);
  *bc = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load a into addr in BC
void LDINDR_BC_A(unsigned char const* cart, void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short* bc = getRegister16(cpu_ptr, B);
  unsigned char *a = getRegister(cpu_ptr, A);
  cpu_ptr->ram[*bc] = *a;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Increment BC
void INC_BC(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short *bc = getRegister16(cpu_ptr, B); 
 *bc = *bc + 1;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment B
void INC_B(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *b = getRegister(cpu_ptr, B); 

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
 unsigned char *b = getRegister(cpu_ptr, B); 

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
 unsigned char *b = getRegister(cpu_ptr, B); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *b = getByte(cart, cpu_ptr->pc + 1);
}
 
// Rotate A left (with wrapping) and save into cf
void RLCA(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
  
 unsigned char *a = getRegister(cpu_ptr, A);
 unsigned char cf = (*a & 0x80) >> 7;
 *a = (*a << 1) | cf;
 
 setCF(cpu_ptr, cf);
 
 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;
} 

// Load SP into 16 bit addr
void LD_a16_SP(unsigned char const* cart, void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 
 unsigned short *sp = getRegister16(cpu_ptr, SP);
 unsigned short addr = getNN(cart, cpu_ptr->pc + 1);

 *((short*)(cpu_ptr->ram + addr)) = *sp;
 // Provide the info for the instruction
 info->cycles = 20;
 info->size = 3;
}

// Add BC to HL
void ADD_HL_BC(unsigned char const* cart, void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  unsigned short *hl = getRegister16(cpu_ptr, H);
  unsigned short *bc = getRegister16(cpu_ptr, B);
  unsigned short res = *hl + *bc;
   
  setCF(cpu_ptr, res < *hl);
  setHF(cpu_ptr,  ((*hl & 0xf) + (*bc & 0xf)) & 0x10 );
  setNF(cpu_ptr, false);

  *hl = res;
  // Provide the info for the instruction
  info->cycles = 8;
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
 
  // Set pointers for cpu operations
  jumptable[0x0][0x0] = NOP;
  jumptable[0x0][0x1] = LD_BC_d16;
  jumptable[0x0][0x2] = LDINDR_BC_A;
  jumptable[0x0][0x3] = INC_BC;
  jumptable[0x0][0x4] = INC_B;
  jumptable[0x0][0x5] = DEC_B;
  jumptable[0x0][0x6] = LD_B_d8;
  jumptable[0x0][0x7] = RLCA;
  jumptable[0x0][0x8] = LD_a16_SP;
  jumptable[0x0][0x9] = ADD_HL_BC;
  jumptable[0x3][0x1] = LD_SP_d16;
}
