#include <stdio.h>
#include <stdbool.h>
#include "ceebee/jumptable.h"
#include "ceebee/common.h"
#include "ceebee/mmu.h"
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

/* SUPER RAD COMMON FUNCTIONS FOR ALL OPS */


bool getCF(CPU *cpu) {
  uint8_t *f = getRegister(cpu, F); 
  
  bool cf = (*f >> 4) & 0x01;
  return cf;
} 
  
// Sets the carry flag
void setCF(CPU *cpu, bool state) {
  uint8_t *f = getRegister(cpu, F); 
  if (state) 
    *f |= 0x10;
  else 
    *f &= 0xEF;
}

// Sets the half carry flag
void setHF(CPU *cpu, bool state) {
  uint8_t *f = getRegister(cpu, F); 
  if (state) 
    *f |= 0x20;
  else
    *f &= 0xDF;
}

// Sets the sets the subraction flag
void setNF(CPU *cpu, bool state) {
  uint8_t *f = getRegister(cpu, F); 
  if (state)
    *f |= 0x40;
  else
    *f &= 0xBF;
}

// Sets the sets the subraction flag
void setZF(CPU *cpu, bool state) {
  uint8_t *f = getRegister(cpu, F); 
  if (state)
    *f |= 0x80;
  else 
    *f &= 0x7F;
}

// Set bit b in 16bit register r
void set(CPU *cpu, uint8_t b, uint16_t r) {
  uint16_t reg = read_r16(cpu, r);
  reg = reg | (0x01 << b);
  write_r16(cpu, r, reg);
}

// Returns flag status
bool check_flag(CPU *cpu, uint8_t flag) {
  uint8_t flag_status = *getRegister(cpu, F);
  flag_status = flag_status >> flag;
  flag_status = flag_status & 0x01;
  return (flag_status == 1);
}

// Compares accumulator with val
void comp(CPU *cpu, uint8_t val) {
  setZF(cpu, cpu->a == val);
  setNF(cpu, true);
  setHF(cpu, ((cpu->a & 0xf) - (val & 0xf)) < 0x00 );
  setCF(cpu, cpu->a < val);
}

/* Restarts */
/* Push present address onto stack. */
/* Jump to address $0000 + n. */
void restarts(CPU *cpu, Op_info *info, uint8_t n) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  cpu_ptr->sp -= 2;
  writeNN(cpu_ptr, cpu_ptr->sp, cpu_ptr->pc);
  
  cpu_ptr->pc = n;

  // Provide the info for the instruction
  info->cycles = 16;
  info->size = 1;
}

// Pushes 16 bit register onto the stack
void push_r16(CPU *cpu, Op_info *info, uint16_t dest_reg) {
  uint16_t dest = read_r16(cpu, dest_reg);
  cpu->sp -= 2;
  writeNN(cpu, cpu->sp, dest);

  // Provide the info for the instruction
  info->cycles = 16;
  info->size = 1;
}

// Pop 16 bit register off the stack
void pop_r16(CPU *cpu, Op_info *info, uint16_t dest_reg) {
  uint16_t src = readNN(cpu, cpu->sp);
  cpu->sp += 2;
  
  write_r16(cpu, dest_reg, src);
  
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 1;
}

// Moves 8bit register to 8bit register
void move(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint8_t *dest = getRegister(cpu, dest_reg);
  uint8_t *src = getRegister(cpu, src_reg);

  *dest = *src;

  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

void xor_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint8_t *dest = getRegister(cpu, dest_reg); 
  uint8_t *src = getRegister(cpu, src_reg); 
  uint8_t result = *src ^ *dest;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);
  
  *dest = result;  
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

void or_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint8_t *dest = getRegister(cpu, dest_reg); 
  uint8_t *src = getRegister(cpu, src_reg); 
  uint8_t result = *src | *dest;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);
  
  *dest = result;
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}

// 8bit load from 8 bits after pc
void load_n_to_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  uint8_t *dest = getRegister(cpu, reg); 
 
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 2;
   
  *dest = readN(cpu, cpu->pc + 1);
}

// 16bit load from 16 bits after pc
void load_nn_to_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  // Get the next 16 bits from just after pc
  uint16_t nn = readNN(cpu, cpu->pc + 1);

  write_r16(cpu, reg, nn);

  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 3;
}

// Load 8 bits to address in dest
void loadindr_n_from_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint16_t dest = read_r16(cpu, dest_reg);
  uint8_t *src = getRegister(cpu, src_reg);
  writeN(cpu, dest, *src);
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Conditional jump to addr
void cond_jmp_r8(CPU *cpu, Op_info *info, bool condition) {
  
  if (condition){
    // Get the next 8 bytes after pc
    signed char r8 = readN(cpu, cpu->pc + 1);
    cpu->pc += r8;
    info->cycles = 12;
  } 
  else {
    info->cycles = 8;
  }
  info->size = 2;
}

void inc_16_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  uint16_t dest_val = read_r16(cpu, reg); 
  dest_val++;
  write_r16(cpu, reg, dest_val);
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void dec_16_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  uint16_t dest_val = read_r16(cpu, reg); 
  dest_val--;
  write_r16(cpu, reg, dest_val);

  info->cycles = 8;
  info->size = 1;
}

void indir_n_load(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint16_t src_val = read_r16(cpu, src_reg);
  uint8_t *dest = getRegister(cpu, dest_reg);
  
  *dest = readN(cpu, src_val);

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void add_16_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint16_t dest = read_r16(cpu, dest_reg);
  uint16_t src = read_r16(cpu, src_reg);
  uint16_t val = dest + src;
   
  setCF(cpu, val < dest);
  setHF(cpu,  ((dest & 0xf) + (src & 0xf)) & 0x10 );
  setNF(cpu, false);

  write_r16(cpu, dest_reg, val);
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void add_8_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint8_t *dest = getRegister(cpu, dest_reg);
  uint8_t *src = getRegister(cpu, src_reg);
  setZF(cpu, ((*dest + *src) & 0xff) == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (*src & 0xf)) & 0x10);
  setCF(cpu, *dest + *src < *dest);

  *dest = *dest + *src;
  
  info->cycles = 4;
  info->size = 1;
}  

void adc_8_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint16_t src_reg) {
  uint8_t *dest = getRegister(cpu, dest_reg);
  uint8_t *src = getRegister(cpu, src_reg);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t newVal = *dest + *src + carry;
  setZF(cpu, newVal == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (*src & 0xf) + (carry & 0xf)) & 0x10);
  setCF(cpu, newVal < *dest);

  *dest = newVal;
  
  info->cycles = 4;
  info->size = 1;
}  


void inc_8_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  uint8_t *dest = getRegister(cpu, reg); 
 
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

void dec_8_reg(CPU *cpu, Op_info *info, uint16_t reg) {
  uint8_t *dest = getRegister(cpu, reg); 
 
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

// Subtracts n from A.
void sub_n(CPU *cpu, Op_info *info, uint8_t n) {
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
 
  uint8_t *a = getRegister(cpu, A); 
  uint8_t res = *a - n;

  setZF(cpu, res == 0);
  setNF(cpu, true);
  setHF(cpu, ((*a & 0xf) - (n & 0xf)) < 0x00 );
  setCF(cpu, *a < n);
  
  *a = res;
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
  load_nn_to_reg(cpu_ptr, info, BC);
}

// Load a into addr in BC
void LDINDR_BC_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  loadindr_n_from_reg(cpu_ptr, info, BC, A);
}

void SUB_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  sub_n(cpu_ptr, info, val);

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 2;
} 


void SUB_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *b = getRegister(cpu, B);
  sub_n(cpu_ptr, info, *b); 
}

// Increment BC
void INC_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, BC); 
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

void LD_B_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  cpu_ptr->b = readN(cpu_ptr, addr);

  info->cycles = 8;
  info->size = 1;
}

void LD_A_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  cpu_ptr->a = readN(cpu_ptr, addr);

  info->cycles = 8;
  info->size = 1;
}

// Load immediate 8 bits into E
void LD_E_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, E);
}

// Load immediate 8 bits into L
void LD_L_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, L);
}

// Load immediate 8 bits into A
void LD_A_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  load_n_to_reg(cpu_ptr, info, A);
}

// Load A into E
void LD_E_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, A);
}

void LD_A_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, B);
}

void LD_A_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, C);
}

void LD_A_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, D);
}

// Shift A right with carry. MSB set to 0
void SRL_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t cf = *a & 0x01;
  setCF(cpu_ptr, cf);
  
  *a = *a >> 1;
  
  if (*a == 0)
    setZF(cpu_ptr, true);

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 2;
}
 
// Rotate A left (with wrapping) and save into cf
void RLCA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
   
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t cf = (*a & 0x80) >> 7;
  *a = (*a << 1) | cf;
  
  setCF(cpu_ptr, cf);
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
} 

// Rotate A left and save into cf
void RLA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
   
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t *f = getRegister(cpu_ptr, F);
  uint8_t cf = (*a & 0x80) >> 7;
  *a = (*a << 1) | ((*f & 0x10) >> 4);
  
  setCF(cpu_ptr, cf);
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
} 

// Rotate A right and save into cf
void RRA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
   
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t *f = getRegister(cpu_ptr, F);
  uint8_t cf = (*a & 0x01) << 7;
  *a = (*a >> 1) | ((*f & 0x10) << 3);
  
  setCF(cpu_ptr, cf);
  
  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
} 

// Compliment the A register
void CPL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu_ptr, A);
  *a = ~*a;
  
  setNF(cpu_ptr, true);
  setHF(cpu_ptr, true);

  info->cycles = 4;
  info->size = 1;
}

// Load SP into 16 bit addr
void LD_a16_SP(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  uint16_t sp_val = read_r16(cpu_ptr, SP);
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->pc + 1);
  writeNN(cpu, addr, sp_val);

  // Provide the info for the instruction
  info->cycles = 20;
  info->size = 3;
}

// Add BC to HL
void ADD_HL_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, HL, BC);
}

// Add DE to HL
void ADD_HL_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, HL, DE);
}

// Add HL to HL
void ADD_HL_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, HL, HL);
}

// Add SP to HL
void ADD_HL_SP(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_16_reg(cpu_ptr, info, HL, SP);
}

// Load byte BC points to into A
void LD_A_INDIR_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, BC);
}

// Decrements BC
void DEC_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t bc_val = read_r16(cpu_ptr, BC);
  bc_val--;
  write_r16(cpu_ptr, BC, bc_val);

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
   
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t cf = (*a & 0x01) << 7;
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
  load_nn_to_reg(cpu_ptr, info, DE);
}

// Load the next 16 bits into HL
void LD_HL_d16(void *cpu, Op_info *info) {
  // Trick to pass cpu into instruction
  CPU *cpu_ptr = (CPU*) cpu; 
  load_nn_to_reg(cpu_ptr, info, HL);
}

// Load a into addr in DE
void LDINDR_DE_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  loadindr_n_from_reg(cpu_ptr, info, DE, A);
}

// Load a into addr in HL and INC
void LDINC_HL_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  uint16_t hl_val = read_r16(cpu_ptr, HL);
  uint8_t *a = getRegister(cpu_ptr, A);
  writeN(cpu_ptr, hl_val, *a);
  hl_val++;
  write_r16(cpu_ptr, HL, hl_val);
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Load a into addr in HL and INC
void LDDEC_HL_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu; 
  uint16_t hl_val = read_r16(cpu_ptr, HL);
  uint8_t *a = getRegister(cpu_ptr, A);
  writeN(cpu_ptr, hl_val, *a);
  hl_val--;
  write_r16(cpu_ptr, HL, hl_val);
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Increment DE
void INC_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, DE);
}

// Increment HL
void INC_HL(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
  inc_16_reg(cpu_ptr, info, HL);
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
  uint16_t hl_val = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, hl_val);

  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 1;

  // Check flag states
  setZF(cpu_ptr, ((val + 1) & 0xff) == 0x00);
  setNF(cpu_ptr, false);
  // [val] & 0xf gets rid of the upper nibble
  setHF(cpu_ptr,  ((val & 0xf) + (1 & 0xf)) & 0x10 );

  // Carry out the operation
  writeN(cpu_ptr, hl_val, val + 1);
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
  uint16_t hl_val = read_r16(cpu_ptr, HL); 
 
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 1;

  uint8_t val = readN(cpu_ptr, hl_val); 
 
  // Check flag states
  setZF(cpu_ptr, ((val - 1) & 0xff) == 0);
  setNF(cpu_ptr, true);
  // [val] & 0xf gets rid of the upper nibble
  setHF(cpu_ptr,  ((val & 0xf) - (1 & 0xf)) < 0x00 );
 
  // Carry out the operation
  writeN(cpu_ptr, hl_val, val - 1);
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
  uint16_t hl_val = read_r16(cpu_ptr, HL);
 
  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 2;
  uint8_t d8 = readN(cpu_ptr, cpu_ptr->pc + 1);
  writeN(cpu_ptr, hl_val, d8);
}

/* THIS DAA CODE COMES FROM: https://forums.nesdev.com/viewtopic.php?t=15944 */
/* shout out to AWJ for the help :) */
// Decimal Adjust
void DAA(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t *f = getRegister(cpu_ptr, F);
  uint16_t n_flag = (*f & 0x40) >> 6;
  uint16_t h_flag = (*f & 0x20) >> 5;
  uint16_t c_flag = (*f & 0x10) >> 4;

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

// Complement the carry flag
void CCF(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;

  setNF(cpu_ptr, false);
  setHF(cpu_ptr, false);
  
  if (getCF(cpu_ptr))
    setCF(cpu_ptr, false);
  else
    setCF(cpu_ptr, true);

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
  
  char offset = readN(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->pc += offset;

  // Provide the info for the instruction
  info->cycles = 12;
  info->size = 2;
}

void JP_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->pc + 1);
  cpu_ptr->pc = addr;

  // Provide the info for the instruction
  info->cycles = 16;
  info->size = 3;
}

void JP_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t hlptr = read_r16(cpu_ptr, HL);
  uint16_t addr = readNN(cpu_ptr, hlptr);
  cpu_ptr->pc = addr;

  // Provide the info for the instruction
  info->cycles = 4;
  info->size = 1;
}
  

void JR_Z_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *f = getRegister(cpu_ptr, F); 
  uint8_t zf = (*f & 0x80) >> 7;
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
  uint8_t *f = getRegister(cpu_ptr, F); 
  uint8_t cf = (*f & 0x10) >> 4;
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
  
  uint16_t de_val = read_r16(cpu_ptr, DE);
  uint8_t *a = getRegister(cpu_ptr, A);
  
  *a = readN(cpu_ptr, de_val);

  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

// Load byte HL points to into A
void LDINC_A_INDR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, HL); 
  uint16_t hl_val = read_r16(cpu_ptr, HL);
  hl_val++;
  write_r16(cpu_ptr, HL, hl_val);
}

// Load byte HL points to into A
void LDDEC_A_INDR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  indir_n_load(cpu_ptr, info, A, HL); 
  uint16_t hl_val = read_r16(cpu_ptr, HL);
  hl_val--;
  write_r16(cpu_ptr, HL, hl_val);
}

// Decrements DE
void DEC_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_16_reg(cpu_ptr, info, DE);
} 

// Decrements HL
void DEC_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  dec_16_reg(cpu_ptr, info, HL);
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

// Xor A with A
void XOR_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, A);
}

void OR_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, B);
}

void OR_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, C);
}

void JR_NZ_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  cond_jmp_r8(cpu_ptr, info, !check_flag(cpu_ptr, ZF));
}

/* This LD puts a into (c + 0xFF00) */
void LDINDR_C_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  writeN(cpu_ptr, cpu_ptr->c + 0xFF00, cpu_ptr->a);
  
  info->cycles = 8;
  info->size = 1;
}

void LDH_a8_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t n = readN(cpu_ptr, cpu_ptr->pc + 1);
  writeN(cpu_ptr, n + 0xFF00, cpu_ptr->a);
  info->cycles = 12;
  info->size = 2;
}

void LDH_A_a8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t addr = readN(cpu_ptr, cpu_ptr->pc + 1);
  uint8_t n = readN(cpu_ptr, addr + 0xFF00);
  cpu_ptr->a = n;
  info->cycles = 12;
  info->size = 2;
}
  
void LDINDR_HL_A(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint16_t hl_val = read_r16(cpu_ptr, HL); 
 writeN(cpu_ptr, hl_val, cpu_ptr->a);
 info->cycles = 8;
 info->size = 1;
}
  
/* This LD puts (c + 0xFF00) into a*/
void LD_A_INDR_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu_ptr, A);
  uint8_t *c = getRegister(cpu_ptr, C);
  *a = readN(cpu_ptr, *c + 0xFF00); 
  info->cycles = 8;
  info->size = 1;
}

void LD_A_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, H);
}

void LD_C_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, A);
}

void LD_A_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, E);
}

void LD_A_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, L);
}

void LD_H_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, A);
}

void LD_D_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, A);
}

void LDINDR_a16_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->pc + 1);
  writeN(cpu_ptr, addr, cpu_ptr->a);

  info->cycles = 16;
  info->size = 3;
}

void CP_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = readN(cpu_ptr, cpu_ptr->pc + 1);
  comp(cpu_ptr, d8);
  info->cycles = 8;
  info->size = 2;
}

void AND_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  bool res = cpu_ptr->a & cpu_ptr->b;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  
  info->cycles = 4;
  info->size = 1;
}

void AND_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  uint8_t d8 = readN(cpu_ptr, cpu_ptr->pc + 1);
  
  bool res = cpu_ptr->a & d8;

  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 

  info->cycles = 8;
  info->size = 2;
}

void RST_38H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x38);
}

void RST_28H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x28);
}

void RST_18H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x18);
}

void RST_08H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x08);
}

// Push next instruction onto stack and jump to nn
void CALL_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  cpu_ptr->sp -= 2;
  writeNN(cpu_ptr, cpu_ptr->sp, cpu_ptr->pc + 3);
  
  // Get addr to jump to
  cpu_ptr->pc = readNN(cpu_ptr, cpu_ptr->pc + 1);
  info->cycles = 24;
}

// Pop two bytes off of stack & jump to that address
void RET(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->sp);
  cpu_ptr->pc = addr;
  cpu_ptr->sp += 2;
  info->cycles = 8;
}

// Push BC on the stack
void PUSH_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  push_r16(cpu_ptr, info, BC);
} 

void POP_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  pop_r16(cpu_ptr, info, BC);
}  

void DI(void *cpu, Op_info *info) {
  printf("Interrupts are not implemented yet...\nContinuing...\n");
  info->cycles = 4;
  info->size = 1;
}

void ADD_A_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, B);
}

void ADD_A_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, C);
}

void ADD_A_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, D);
}

void ADD_A_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, E);
}

void ADD_A_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, H);
}

void ADD_A_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, L);
}

void ADD_A_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  add_8_reg(cpu_ptr, info, A, A);
}

void ADD_A_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t *dest = getRegister(cpu, A);

  setZF(cpu, ((*dest + val) & 0xff) == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (val & 0xf)) & 0x10);
  setCF(cpu, *dest + val < *dest);

  *dest = *dest + val;

  info->cycles = 8;
  info->size = 1;
}

void ADC_A_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, B);
}

void ADC_A_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, C);
}

void ADC_A_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, D);
}

void ADC_A_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, E);
}

void ADC_A_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, H);
}

void ADC_A_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, L);
}

void ADC_A_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  adc_8_reg(cpu_ptr, info, A, A);
}

void ADC_A_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t *dest = getRegister(cpu, A);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t newVal = *dest + val + carry;

  setZF(cpu, newVal == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (val & 0xf) + (carry & 0xff)) & 0x10);
  setCF(cpu, newVal < *dest);

  *dest = newVal;

  info->cycles = 8;
  info->size = 1;
}

/* CB PREFIX FUNCTIONS */

/* CB COMMON FUNCTIONS */
// Set zero flag if bit n of register reg is 0.
void bit_n(CPU *cpu, Op_info *info, uint8_t n, uint16_t reg) {
  uint8_t *dest = getRegister(cpu, reg); 
  uint8_t bit_n = (*dest >> n) & 0x01;
  setZF(cpu, bit_n == 0);
  setNF(cpu, false);
  setHF(cpu, true);
  info->cycles = 8;
  info->size = 1;
}

void RL(CPU *cpu, Op_info *info, uint16_t reg) {
  uint8_t *dest = getRegister(cpu, reg);
  uint8_t *f = getRegister(cpu, F);
  uint8_t cf = (*dest & 0x80) >> 7;
  *dest = (*dest << 1) | ((*f & 0x10) >> 4);
  
  setCF(cpu, cf);
  setNF(cpu,false);
  setHF(cpu,false);

  info->cycles = 8;
  info->size = 1;
}

/* CB FUNCTIONS */
void RL_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  RL(cpu_ptr, info, C);
}

void BIT_7_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  bit_n(cpu_ptr, info, 7, H);
}

void BIT_3_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  bit_n(cpu_ptr, info, 3, A);
}

void SET_7_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  set(cpu_ptr, 7, HL);

  info->cycles = 16;
  info->size = 2;
}


// Lets u kno that this opcode is not implemented yet
void NOT_IMPL(void *cpu, Op_info *info) {
  panic(cpu, GRN "This instruction is not yet implemented ... Exiting :)\n" RESET);
}

void init_jmp (func_ptr jumptable[0xF][0xF], func_ptr cb_jumptable[0xF][0xF]) {
  // Init tables to NOT_IMPL
  for (int i = 0; i < 0xF; i++) {
    for (int j = 0; j < 0xF; j++) {
     jumptable[i][j] = NOT_IMPL;
    }
  }
  for (int i = 0; i < 0xF; i++) {
    for (int j = 0; j < 0xF; j++) {
     cb_jumptable[i][j] = NOT_IMPL;
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
  jumptable[0x1][0xE] = LD_E_d8;
  jumptable[0x1][0xF] = RRA;

  jumptable[0x2][0x0] = JR_NZ_r8;
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
  jumptable[0x2][0xE] = LD_L_d8;
  jumptable[0x2][0xF] = CPL;

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
  jumptable[0x3][0xE] = LD_A_d8;
  jumptable[0x3][0xF] = CCF;

  jumptable[0x4][0x6] = LD_B_INDRHL;
  jumptable[0x4][0xF] = LD_C_A;

  jumptable[0x5][0x7] = LD_D_A;
  jumptable[0x5][0xF] = LD_E_A;

  jumptable[0x6][0x7] = LD_H_A;

  jumptable[0x7][0x7] = LDINDR_HL_A;
  jumptable[0x7][0x8] = LD_A_B;
  jumptable[0x7][0x9] = LD_A_C;
  jumptable[0x7][0xA] = LD_A_D;
  jumptable[0x7][0xB] = LD_A_E;
  jumptable[0x7][0xC] = LD_A_H;
  jumptable[0x7][0xD] = LD_A_L;
  jumptable[0x7][0xE] = LD_A_INDRHL;

  jumptable[0x8][0x0] = ADD_A_B;
  jumptable[0x8][0x1] = ADD_A_C;
  jumptable[0x8][0x2] = ADD_A_D;
  jumptable[0x8][0x3] = ADD_A_E;
  jumptable[0x8][0x4] = ADD_A_H;
  jumptable[0x8][0x5] = ADD_A_L;
  jumptable[0x8][0x6] = ADD_A_HL;
  jumptable[0x8][0x7] = ADD_A_A;

  jumptable[0x8][0x8] = ADC_A_B;
  jumptable[0x8][0x9] = ADC_A_C;
  jumptable[0x8][0xA] = ADC_A_D;
  jumptable[0x8][0xB] = ADC_A_E;
  jumptable[0x8][0xC] = ADC_A_H;
  jumptable[0x8][0xD] = ADC_A_L;
  jumptable[0x8][0xE] = ADC_A_HL;
  jumptable[0x8][0xF] = ADC_A_A;
  

  jumptable[0x9][0x0] = SUB_B;

  jumptable[0xA][0x0] = AND_B;
  jumptable[0xA][0xF] = XOR_A;

  jumptable[0xB][0x0] = OR_B;
  jumptable[0xB][0x1] = OR_C;
  
  jumptable[0xC][0x1] = POP_BC;
  jumptable[0xC][0x3] = JP_a16;
  jumptable[0xC][0x5] = PUSH_BC;
  jumptable[0xC][0x9] = RET;
  jumptable[0xC][0xD] = CALL_a16;
  jumptable[0xC][0xF] = RST_08H;

  jumptable[0xD][0x6] = SUB_d8;
  jumptable[0xD][0xF] = RST_18H;
  
  jumptable[0xE][0x0] = LDH_a8_A;
  jumptable[0xE][0x2] = LDINDR_C_A;
  jumptable[0xE][0x6] = AND_d8;
  jumptable[0xE][0x9] = JP_INDRHL;
  jumptable[0xE][0xA] = LDINDR_a16_A;
  jumptable[0xE][0xF] = RST_28H;

  jumptable[0xF][0x0] = LDH_A_a8;
  jumptable[0xF][0x2] = LD_A_INDR_C;
  jumptable[0xF][0x3] = DI;
  jumptable[0xF][0xE] = CP_d8;
  jumptable[0xF][0xF] = RST_38H;
 
  /* CB JUMPTABLE */
  cb_jumptable[0x1][0x1] = RL_C;

  cb_jumptable[0x7][0xC] = BIT_7_H;

  cb_jumptable[0x3][0xF] = SRL_A;

  cb_jumptable[0x5][0xF] = BIT_3_A;
  cb_jumptable[0xF][0xE] = SET_7_INDRHL;
}
