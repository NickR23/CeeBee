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


void NOP(void *cpu, Op_info *info) {
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

// Load the next 16 bits into sp
void LD_SP_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->sp = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load the next 16 bits into bc
void LD_BC_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cpu_ptr, cpu_ptr->pc + 1);
  unsigned short* bc = getRegister16(cpu_ptr, B);
  *bc = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load a into addr in BC
void LDINDR_BC_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short* bc = getRegister16(cpu_ptr, B);
  unsigned char *a = getRegister(cpu_ptr, A);
  cpu_ptr->mmu[*bc] = *a;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Increment BC
void INC_BC(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short *bc = getRegister16(cpu_ptr, B); 
 *bc = *bc + 1;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment B
void INC_B(void *cpu, Op_info *info) {
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
void DEC_B(void *cpu, Op_info *info) {
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
void LD_B_d8(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *b = getRegister(cpu_ptr, B); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *b = getByte(cpu_ptr, cpu_ptr->pc + 1);
}
 
// Rotate A left (with wrapping) and save into cf
void RLCA(void *cpu, Op_info *info) {
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
void LD_a16_SP(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 
 unsigned short *sp = getRegister16(cpu_ptr, SP);
 unsigned short addr = getNN(cpu_ptr, cpu_ptr->pc + 1);

 *((short*)(cpu_ptr->mmu + addr)) = *sp;
 // Provide the info for the instruction
 info->cycles = 20;
 info->size = 3;
}

// Add BC to HL
void ADD_HL_BC(void *cpu, Op_info *info) {
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

// Load byte BC points to into A
void LD_A_INDIR_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  unsigned short *bc = getRegister16(cpu_ptr, B);
  unsigned char *a = getRegister(cpu_ptr, A);
  
  *a = cpu_ptr->mmu[*bc];

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Decrements BC
void DEC_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  cpu_ptr->bc--;

  info->cycles = 8;
  info->size = 1;
} 

// Decrement C
void DEC_C(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *c = getRegister(cpu_ptr, C); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*c - 1) & 0xff) == 0);
 setNF(cpu_ptr, true);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*c & 0xf) - (1 & 0xf)) < 0x00 );

 // Carry out the operation
 *c = *c - 1;
} 

// Increment C
void INC_C(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *c = getRegister(cpu_ptr, C); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*c + 1) & 0xff) == 0x00);
 setNF(cpu_ptr, false);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*c & 0xf) + (1 & 0xf)) & 0x10 );

 // Carry out the operation
 *c = *c + 1;
} 

// Load immediate 8 bits into C
void LD_C_d8(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *c = getRegister(cpu_ptr, C); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *c = getByte(cpu_ptr, cpu_ptr->pc + 1);
}

// Rotate A right (with wrapping) and save into cf
void RRCA(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
  
 unsigned char *a = getRegister(cpu_ptr, A);
 unsigned char cf = (*a & 0x01) << 7;
 *a = (*a >> 1) | cf;
 
 setCF(cpu_ptr, cf >> 7);
 
 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;
} 

// Load the next 16 bits into DE
void LD_DE_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->de = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load the next 16 bits into HL
void LD_HL_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->hl = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load a into addr in DE
void LDINDR_DE_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short *de = getRegister16(cpu_ptr, B);
  unsigned char *a = getRegister(cpu_ptr, A);
  cpu_ptr->mmu[*de] = *a;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Load a into addr in HL and INC
void LDINC_HL_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short hl = cpu_ptr->hl;
  unsigned char *a = getRegister(cpu_ptr, A);
  cpu_ptr->mmu[hl] = *a;
  cpu_ptr->hl++;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Load a into addr in HL and INC
void LDDEC_HL_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  unsigned short hl = cpu_ptr->hl;
  unsigned char *a = getRegister(cpu_ptr, A);
  cpu_ptr->mmu[hl] = *a;
  cpu_ptr->hl--;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Increment DE
void INC_DE(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;

 cpu_ptr->de++;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment HL
void INC_HL(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;

 cpu_ptr->hl++;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment SP
void INC_SP(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;

 cpu_ptr->sp++;
 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 1;
}

// Increment D
void INC_D(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *d = getRegister(cpu_ptr, D); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*d + 1) & 0xff) == 0x00);
 setNF(cpu_ptr, false);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*d & 0xf) + (1 & 0xf)) & 0x10 );

 // Carry out the operation
 *d = *d + 1;
} 

// Increment H
void INC_H(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *h = getRegister(cpu_ptr, H); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*h + 1) & 0xff) == 0x00);
 setNF(cpu_ptr, false);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*h & 0xf) + (1 & 0xf)) & 0x10 );

 // Carry out the operation
 *h = *h + 1;
} 

// Increment address in HL
void INCINDR_HL(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short hl = cpu_ptr->hl; 
 unsigned char val = cpu_ptr->mmu[hl];

 // Provide the info for the instruction
 info->cycles = 12;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((val + 1) & 0xff) == 0x00);
 setNF(cpu_ptr, false);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((val & 0xf) + (1 & 0xf)) & 0x10 );

 // Carry out the operation
 cpu_ptr->mmu[hl]++;
} 

// Decrement D
void DEC_D(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *d = getRegister(cpu_ptr, D); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*d - 1) & 0xff) == 0);
 setNF(cpu_ptr, true);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*d & 0xf) - (1 & 0xf)) < 0x00 );

 // Carry out the operation
 *d = *d - 1;
} 

// Decrement H
void DEC_H(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *h = getRegister(cpu_ptr, H); 

 // Provide the info for the instruction
 info->cycles = 4;
 info->size = 1;

 // Check flag states
 setZF(cpu_ptr, ((*h - 1) & 0xff) == 0);
 setNF(cpu_ptr, true);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((*h & 0xf) - (1 & 0xf)) < 0x00 );

 // Carry out the operation
 *h = *h - 1;
} 

// Decrement address at HL
void DECINDR_HL(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short hl = cpu_ptr->hl; 

 // Provide the info for the instruction
 info->cycles = 12;
 info->size = 1;
 
 unsigned char val = cpu_ptr->mmu[hl]; 

 // Check flag states
 setZF(cpu_ptr, ((val - 1) & 0xff) == 0);
 setNF(cpu_ptr, true);
 // [val] & 0xf gets rid of the upper nibble
 setHF(cpu_ptr,  ((val & 0xf) - (1 & 0xf)) < 0x00 );

 // Carry out the operation
 cpu_ptr->mmu[hl]--;
} 

// Load immediate 8 bits into D
void LD_D_d8(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *d = getRegister(cpu_ptr, d); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *d = getByte(cpu_ptr, cpu_ptr->pc + 1);
}

// Load immediate 8 bits into H
void LD_H_d8(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned char *h = getRegister(cpu_ptr, H); 

 // Provide the info for the instruction
 info->cycles = 8;
 info->size = 2;
  
 *h = getByte(cpu_ptr, cpu_ptr->pc + 1);
}

// Load immediate 8 bits into address in HL 
void LDINDR_HL_d8(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 unsigned short hl = cpu_ptr->hl;

 // Provide the info for the instruction
 info->cycles = 12;
 info->size = 2;
  
 cpu_ptr->mmu[hl] = getByte(cpu_ptr, cpu_ptr->pc + 1);
}

// Lets u kno that this opcode is not implemented yet
void NOT_IMPL(void *cpu, Op_info *info) {
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
  jumptable[0x0][0xA] = LD_A_INDIR_BC;
  jumptable[0x0][0xB] = DEC_BC;
  jumptable[0x0][0xC] = INC_C;
  jumptable[0x0][0xD] = DEC_C;
  jumptable[0x0][0xE] = LD_C_d8;
  jumptable[0x0][0xF] = RRCA;

  jumptable[0x1][0x1] = LD_DE_d16;
  jumptable[0x1][0x2] = LDINDR_DE_A;
  jumptable[0x1][0x3] = INC_DE;
  jumptable[0x1][0x4] = INC_D;
  jumptable[0x1][0x5] = DEC_D;
  jumptable[0x1][0x6] = LD_D_d8;

  jumptable[0x2][0x2] = LDINC_HL_A;
  jumptable[0x2][0x3] = INC_HL;
  jumptable[0x2][0x4] = INC_H;
  jumptable[0x2][0x5] = INC_D;
  jumptable[0x2][0x6] = LD_H_d8;

  jumptable[0x2][0x1] = LD_HL_d16;
  jumptable[0x3][0x2] = LDDEC_HL_A;
  jumptable[0x3][0x3] = INC_SP;
  jumptable[0x3][0x4] = INCINDR_HL;
  jumptable[0x3][0x5] = DECINDR_HL;
  jumptable[0x3][0x6] = LDINDR_HL_d8;

  /* SKIPPING STOP (0x10) FOR NOW */
  
  jumptable[0x3][0x1] = LD_SP_d16;
}
