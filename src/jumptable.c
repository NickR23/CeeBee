#include <stdio.h>
#include <stdbool.h>
#ifndef JMPTBL_H
  #include "ceebee/jumptable.h"
#endif
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

/* SUPER RAD COMMON FUNCTIONS FOR ALL OPS */
  
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

// 8bit load from 8 bits after pc
void load_n_to_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  unsigned char *dest = getRegister(cpu, reg); 
 
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 2;
   
  *dest = getByte(cpu, cpu->pc + 1);
}

// 16bit load from 16 bits after pc
void load_nn_to_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  // Get the next 16 bits from just after pc
  unsigned int nn = getNN(cpu, cpu->pc + 1);
  // Get the register
  unsigned short *dest = getRegister16(cpu, reg);
  *dest = nn;
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load 8 bits to address in src
void loadindr_n_from_reg(CPU *cpu, Op_info *info, unsigned short dest_reg, unsigned short src_reg) {
  unsigned short *dest = getRegister16(cpu, dest_reg);
  unsigned char *src = getRegister(cpu, src_reg);
  cpu->mmu[*dest] = *src;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void inc_16_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  unsigned short *dest = getRegister16(cpu, reg); 
  *dest = *dest + 1;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void dec_16_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  unsigned short *dest = getRegister16(cpu, reg); 
  
  *dest = *dest - 1;

  info->cycles = 8;
  info->size = 1;
}

void indir_n_load(CPU *cpu, Op_info *info, unsigned short dest_reg, unsigned short src_reg) {
  unsigned short *src = getRegister16(cpu, src_reg);
  unsigned char *dest = getRegister(cpu, dest_reg);
  
  *dest = cpu->mmu[*src];

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void add_16_reg(CPU *cpu, Op_info *info, unsigned short dest_reg, unsigned short src_reg) {
  unsigned short *dest = getRegister16(cpu, dest_reg);
  unsigned short *src = getRegister16(cpu, src_reg);
  unsigned short res = *dest + *src;
   
  setCF(cpu, res < *dest);
  setHF(cpu,  ((*dest & 0xf) + (*src & 0xf)) & 0x10 );
  setNF(cpu, false);

  *dest = res;
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}


void inc_8_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  unsigned char *dest = getRegister(cpu, reg); 
 
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
 
  // Check flag states
  setZF(cpu, ((*dest + 1) & 0xff) == 0x00);
  setNF(cpu, false);
  // [val] & 0xf gets rid of the upper nibble
  setHF(cpu, ((*dest & 0xf) + (1 & 0xf)) & 0x10 );
 
  // Carry out the operation
  *dest = *dest + 1;
}

void dec_8_reg(CPU *cpu, Op_info *info, unsigned short reg) {
  unsigned char *dest = getRegister(cpu, reg); 
 
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
 
  // Check flag states
  setZF(cpu, ((*dest - 1) & 0xff) == 0);
  setNF(cpu, true);
  // [val] & 0xf gets rid of the upper nibble
  setHF(cpu, ((*dest & 0xf) - (1 & 0xf)) < 0x00 );

  // Carry out the operation
  *dest = *dest - 1;
}

/* JUMPTABLE FUNCTIONS */

void NOP(void *cpu, Op_info *info) {
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

// Load the next 16 bits into sp
void LD_SP_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  load_nn_to_reg(cpu_ptr, info, SP);
}

// Load the next 16 bits into bc
void LD_BC_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  load_nn_to_reg(cpu_ptr, info, B);
}

// Load a into addr in BC
void LDINDR_BC_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  loadindr_n_from_reg(cpu_ptr, info, B, A);
}

// Increment BC
void INC_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, B); 
}

// Increment B
void INC_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, B);
} 

// Decrement B
void DEC_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_8_reg(cpu_ptr, info, B);
} 

// Load immediate 8 bits into B
void LD_B_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, B);
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

// Rotate A left and save into cf
void RLA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
   
  unsigned char *a = getRegister(cpu_ptr, A);
  unsigned char *f = getRegister(cpu_ptr, F);
  unsigned char cf = (*a & 0x80) >> 7;
  *a = (*a << 1) | ((*f & 0x10) >> 4);
  
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
  add_16_reg(cpu_ptr, info, H, B);
}

// Add DE to HL
void ADD_HL_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, H, D);
}

// Add HL to HL
void ADD_HL_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, H, H);
}

// Add SP to HL
void ADD_HL_SP(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, H, SP);
}

// Load byte BC points to into A
void LD_A_INDIR_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, B);
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
  dec_8_reg(cpu_ptr, info, C);
} 

// Increment C
void INC_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, C);
} 

// Load immediate 8 bits into C
void LD_C_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, C);
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
  load_nn_to_reg(cpu_ptr, info, D);
}

// Load the next 16 bits into HL
void LD_HL_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  load_nn_to_reg(cpu_ptr, info, H);
}

// Load a into addr in DE
void LDINDR_DE_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  loadindr_n_from_reg(cpu_ptr, info, D, A);
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
  inc_16_reg(cpu_ptr, info, D);
}

// Increment HL
void INC_HL(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, H);
}

// Increment SP
void INC_SP(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, SP);
}

// Increment D
void INC_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, D);
} 

// Increment H
void INC_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, H);
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
  dec_8_reg(cpu_ptr, info, D);
} 

// Decrement H
void DEC_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_8_reg(cpu_ptr, info, H);
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
  load_n_to_reg(cpu_ptr, info, D);
}

// Load immediate 8 bits into H
void LD_H_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, H);
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

/* THIS DAA CODE COMES FROM: https://forums.nesdev.com/viewtopic.php?t=15944 */
/* shout out to AWJ for the help :) */
// Decimal Adjust
void DAA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  unsigned char *a = getRegister(cpu_ptr, A);
  unsigned char *f = getRegister(cpu_ptr, F);
  unsigned short n_flag = (*f & 0x40) >> 6;
  unsigned short h_flag = (*f & 0x20) >> 5;
  unsigned short c_flag = (*f & 0x10) >> 4;

  // note: assumes a is a uint8_t and wraps from 0xff to 0
  if (!n_flag) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
    if (c_flag || *a > 0x99) { *a += 0x60; setCF(cpu_ptr, true); }
    if (h_flag || (*a & 0x0f) > 0x09) { *a += 0x6; }
  } 
  else {  // after a subtraction, only adjust if (half-)carry occurred
    if (c_flag) { *a -= 0x60; }
    if (h_flag) { *a -= 0x6; }
  }
  // these flags are always updated
  setZF(cpu_ptr, *a == 0); // the usual z flag
  setHF(cpu_ptr, false); // h flag is always cleared

  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;

}
 
// Set the carry flag
void SCF(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, false); 
  setCF(cpu_ptr, true); 

  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

// Set the jump to address
void JR_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  char offset = getByte(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->pc += offset;

  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 2;
}

void JR_Z_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  unsigned char *f = getRegister(cpu_ptr, F); 
  unsigned char zf = (*f & 0x80) >> 7;
  if (zf) {
    JR_r8(cpu_ptr, info);
  }
  else {
    // Provide the info for the instruction
    info->cycles = 8;
    info->size = 2;
  }
}

void JR_C_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  unsigned char *f = getRegister(cpu_ptr, F); 
  unsigned char cf = (*f & 0x10) >> 4;
  if (cf) {
    JR_r8(cpu_ptr, info);
  }
  else {
    // Provide the info for the instruction
    info->cycles = 8;
    info->size = 2;
  }
}

// Load byte DE points to into A
void LD_A_INDR_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  unsigned short *de = getRegister16(cpu_ptr, D);
  unsigned char *a = getRegister(cpu_ptr, A);
  
  *a = cpu_ptr->mmu[*de];

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Load byte HL points to into A
void LDINC_A_INDR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, H); 
  unsigned short *hl = getRegister16(cpu_ptr, H);
  *hl = *hl + 1;
}

// Load byte HL points to into A
void LDDEC_A_INDR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, H); 
  unsigned short *hl = getRegister16(cpu_ptr, H);
  *hl = *hl - 1;
}

// Decrements DE
void DEC_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_16_reg(cpu_ptr, info, D);
} 

// Decrements HL
void DEC_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_16_reg(cpu_ptr, info, H);
} 

// Decrements SP
void DEC_SP(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_16_reg(cpu_ptr, info, SP);
} 

// Increment E
void INC_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, E);
} 

// Increment L
void INC_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, L);
} 

// Increment A
void INC_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_8_reg(cpu_ptr, info, A);
} 

// Decrement E
void DEC_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_8_reg(cpu_ptr, info, E);
} 

// Decrement L
void DEC_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_8_reg(cpu_ptr, info, L);
} 

// Decrement A
void DEC_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_8_reg(cpu_ptr, info, A);
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
  jumptable[0x1][0x7] = RLA;
  jumptable[0x1][0x8] = JR_r8;
  jumptable[0x1][0x9] = ADD_HL_DE;
  jumptable[0x1][0xA] = LD_A_INDR_DE;
  jumptable[0x1][0xB] = DEC_DE;
  jumptable[0x1][0xC] = INC_E;
  jumptable[0x1][0xD] = DEC_E;

  jumptable[0x2][0x1] = LD_HL_d16;
  jumptable[0x2][0x2] = LDINC_HL_A;
  jumptable[0x2][0x3] = INC_HL;
  jumptable[0x2][0x4] = INC_H;
  jumptable[0x2][0x5] = INC_D;
  jumptable[0x2][0x6] = LD_H_d8;
  jumptable[0x2][0x7] = DAA;
  jumptable[0x2][0x8] = JR_Z_r8;
  jumptable[0x2][0x9] = ADD_HL_HL;
  jumptable[0x2][0xA] = LDINC_A_INDR_HL;
  jumptable[0x2][0xB] = DEC_HL;
  jumptable[0x2][0xC] = INC_L;
  jumptable[0x2][0xD] = DEC_L;

  jumptable[0x3][0x1] = LD_SP_d16;
  jumptable[0x3][0x2] = LDDEC_HL_A;
  jumptable[0x3][0x3] = INC_SP;
  jumptable[0x3][0x4] = INCINDR_HL;
  jumptable[0x3][0x5] = DECINDR_HL;
  jumptable[0x3][0x6] = LDINDR_HL_d8;
  jumptable[0x3][0x7] = SCF;
  jumptable[0x3][0x8] = JR_C_r8;
  jumptable[0x3][0x8] = ADD_HL_SP;
  jumptable[0x3][0xA] = LDDEC_A_INDR_HL;
  jumptable[0x3][0xB] = DEC_SP;
  jumptable[0x3][0xC] = INC_A;
  jumptable[0x3][0xD] = DEC_A;

}
