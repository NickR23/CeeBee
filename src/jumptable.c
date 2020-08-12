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

void callCC(CPU *cpu, Op_info *info, bool condition) {
  if (condition) {
    cpu->sp -= 2;
    writeNN(cpu, cpu->sp, cpu->pc + 3);
  
    // Get addr to jump to
    cpu->pc = readNN(cpu, cpu->pc + 1);
    info->cycles = 24;
  } else {
    info->cycles = 12;
  }
}


void jumpCC(CPU *cpu, Op_info *info, uint16_t offset, bool condition) {
  // Explicitly leaving info->size as zero
  // We do not want to advance past the address we jumped to
  if (condition) {
    cpu->pc = offset; 
    info->cycles = 16;
  } else {
    info->cycles = 12;
    info->size = 3;
  }
}

// Pop two bytes off of stack & jump to that address
void returnCC(CPU *cpu, Op_info *info, bool condition) {
  if (condition) {
    uint16_t addr = readNN(cpu, cpu->sp);
    cpu->pc = addr;
    cpu->sp += 2;
    info->cycles = 20;
  } else {
    info->cycles = 8;
  }
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

void add_8_n(CPU *cpu, Op_info *info, uint16_t dest_reg, uint8_t val) {
  uint8_t *dest = getRegister(cpu, dest_reg);
  setZF(cpu, ((*dest + val) & 0xff) == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (val & 0xf)) & 0x10);
  setCF(cpu, *dest + val < *dest);

  *dest = *dest + val;
  
  info->cycles = 8;
  info->size = 2;
}  


void load_indir_hl_n(CPU *cpu, Op_info *info, uint8_t n) {
  uint16_t addr = read_r16(cpu, HL);
  writeN(cpu, addr, n);
  info->cycles = 8;
  info->size = 1;
}

void adc_n_reg(CPU *cpu, Op_info *info, uint16_t dest_reg, uint8_t val) {
  uint8_t *dest = getRegister(cpu, dest_reg);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t newVal = *dest + val + carry;
  setZF(cpu, newVal == 0x00);
  setNF(cpu, false);
  setHF(cpu, ((*dest & 0xf) + (val & 0xf) + (carry & 0xf)) & 0x10);
  setCF(cpu, newVal < *dest);

  *dest = newVal;
  
  info->cycles = 8;
  info->size = 2;
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

void comp_n(CPU *cpu, Op_info *info, uint8_t n) {
  uint8_t *a = getRegister(cpu, A);
  uint8_t res = *a - n;
  setZF(cpu, res == 0);
  setNF(cpu, true);
  setHF(cpu, ((*a & 0xf) - (n & 0xf)) < 0x00 );
  setCF(cpu, *a < n);
  info->cycles = 4;
  info->size = 1;
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

void SUB_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *c = getRegister(cpu, C);
  sub_n(cpu_ptr, info, *c); 
}

void SUB_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *d = getRegister(cpu, D);
  sub_n(cpu_ptr, info, *d); 
}

void SUB_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *e = getRegister(cpu, E);
  sub_n(cpu_ptr, info, *e); 
}

void SUB_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *h = getRegister(cpu, H);
  sub_n(cpu_ptr, info, *h); 
}

void SUB_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *l = getRegister(cpu, L);
  sub_n(cpu_ptr, info, *l); 
}

void SUB_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu, A);
  sub_n(cpu_ptr, info, *a); 
}

void SUB_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  sub_n(cpu_ptr, info, val); 
  info->cycles = 8;
  info->size = 1;
}

void SBC_A_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *c = getRegister(cpu, C);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *c + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *b = getRegister(cpu, B);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *b + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *d = getRegister(cpu, D);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *d + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *e = getRegister(cpu, E);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *e + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *h = getRegister(cpu, H);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *h + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *l = getRegister(cpu, L);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *l + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *a = getRegister(cpu, A);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = *a + carry;
  sub_n(cpu_ptr, info, val); 
}

void SBC_A_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t hl_val = readN(cpu_ptr, addr);
  uint8_t carry = ((*getRegister(cpu, F) > 4) & 0x01);
  uint8_t val = hl_val + carry;
  sub_n(cpu_ptr, info, val); 
  info->cycles = 8;
  info->size = 1;
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

void LD_C_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  cpu_ptr->c = readN(cpu_ptr, addr);

  info->cycles = 8;
  info->size = 1;
}

void LD_L_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  cpu_ptr->l = readN(cpu_ptr, addr);

  info->cycles = 8;
  info->size = 1;
}

void LD_H_INDRHL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  cpu_ptr->h = readN(cpu_ptr, addr);

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

void LD_E_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, E);
}

// Load A into E
void LD_E_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, A);
}

void LD_E_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, D);
}

void LD_E_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, H);
}

void LD_E_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, E, L);
}

void LD_E_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  cpu_ptr->e = val;

  info->cycles = 8;
  info->size = 1;
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
}

void JP_NZ_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t offset = readNN(cpu_ptr, cpu_ptr->pc + 1);
  jumpCC(cpu_ptr, info, offset, !check_flag(cpu_ptr, ZF)); 
}

void JP_NC_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t offset = readNN(cpu_ptr, cpu_ptr->pc + 1);
  jumpCC(cpu_ptr, info, offset, !check_flag(cpu_ptr, CF)); 
}

void JP_Z_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t offset = readNN(cpu_ptr, cpu_ptr->pc + 1);
  jumpCC(cpu_ptr, info, offset, check_flag(cpu_ptr, ZF)); 
}

void JP_C_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t offset = readNN(cpu_ptr, cpu_ptr->pc + 1);
  jumpCC(cpu_ptr, info, offset, check_flag(cpu_ptr, CF)); 
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

void XOR_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, B);
}

void XOR_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, C);
}

void XOR_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, D);
}

void XOR_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *dest = getRegister(cpu_ptr, A); 
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  uint8_t result = val ^ *dest;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);
  
  *dest = result;  
  
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 2;
}

void XOR_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, E);
}

void XOR_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, H);
}

void XOR_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  xor_reg(cpu_ptr, info, A, L);
}

void XOR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t *dest = getRegister(cpu_ptr, A); 
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t result = val ^ *dest;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);
  
  *dest = result;  
  
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
}

void OR_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  uint8_t *dest = getRegister(cpu_ptr, A); 
  uint8_t result = val | *dest;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);

  info->cycles = 8;
  info->size = 2;
}

void OR_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, B);
}

void OR_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, C);
}

void OR_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, D);
}

void OR_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, E);
}

void OR_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, H);
}

void OR_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, L);
}

void OR_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  or_reg(cpu_ptr, info, A, A);
}

void OR_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL); 
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t *dest = getRegister(cpu, A); 
  uint8_t result = *dest | val;

  setZF(cpu, result == 0);
  setNF(cpu, false);
  setHF(cpu, false);
  setCF(cpu, false);
  
  *dest = result;
  
  // Provide the info for the instruction
  info->cycles = 8;
  info->size = 1;
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

void LDINDR_INDRHL_B(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *b = getRegister(cpu_ptr, B);
 load_indir_hl_n(cpu_ptr, info, *b);
}

void LDINDR_INDRHL_C(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *c = getRegister(cpu_ptr, C);
 load_indir_hl_n(cpu_ptr, info, *c);
}

void LDINDR_INDRHL_D(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *d = getRegister(cpu_ptr, D);
 load_indir_hl_n(cpu_ptr, info, *d);
}

void LDINDR_INDRHL_E(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *e = getRegister(cpu_ptr, E);
 load_indir_hl_n(cpu_ptr, info, *e);
}

void LDINDR_INDRHL_H(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *h = getRegister(cpu_ptr, H);
 load_indir_hl_n(cpu_ptr, info, *h);
}

void LDINDR_INDRHL_L(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint8_t *l = getRegister(cpu_ptr, L);
 load_indir_hl_n(cpu_ptr, info, *l);
}

  
void LDINDR_HL_A(void *cpu, Op_info *info) {
 CPU *cpu_ptr = (CPU*) cpu;
 uint16_t hl_val = read_r16(cpu_ptr, HL); 
 writeN(cpu_ptr, hl_val, cpu_ptr->a);
 info->cycles = 8;
 info->size = 1;
}

void LD_SP_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t hlVal = read_r16(cpu_ptr, HL);
  write_r16(cpu_ptr, SP, hlVal);
  info->cycles = 8;
  info->size = 1;
}

void LD_A_INDRNN(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->pc + 1);
  uint8_t val = readN(cpu_ptr, addr);
  cpu_ptr->a = val;
  info->cycles = 16;
  info->size = 3;
}

// This may have a problem with the carry flag
void LDHL_SP_N(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;

  uint16_t val = read_r16(cpu_ptr, SP);
  val += (int16_t) readN(cpu_ptr, cpu_ptr->pc + 1);
  uint16_t spVal = read_r16(cpu_ptr, SP);
  
  setZF(cpu_ptr, false);
  setNF(cpu_ptr, false);
  setHF(cpu_ptr, ((spVal & 0xf) + (val & 0xf)) & 0x10);

  write_r16(cpu_ptr, HL, val);

  info->cycles = 12;
  info->size = 2;
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

void LD_B_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, B);
}

void LD_B_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, C);
}

void LD_B_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, D);
}

void LD_B_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, E);
}

void LD_B_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, H);
}

void LD_B_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, L);
}

void LD_B_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, B, A);
}

void LD_C_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, B);
}

void LD_C_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, C);
}

void LD_C_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, D);
}

void LD_C_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, E);
}

void LD_C_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, A);
}

void LD_C_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, H);
}

void LD_C_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, C, L);
}

void LD_A_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, E);
}

void LD_A_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, L);
}

void LD_A_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, A, A);
}

void LD_H_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, B);
}

void LD_H_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, C);
}

void LD_H_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, D);
}

void LD_H_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, E);
}

void LD_H_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, H);
}

void LD_L_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, B);
}

void LD_L_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, C);
}

void LD_L_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, D);
}

void LD_L_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, E);
}

void LD_L_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, H);
}

void LD_L_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, L);
}

void LD_H_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, L);
}

void LD_H_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, H, A);
}

void LD_L_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, L, A);
}

void LD_D_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, B);
}

void LD_D_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  int8_t val = readN(cpu_ptr, addr);
  cpu_ptr->d = val;

  info->cycles = 8;
  info->size = 1;
}

void LD_D_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, A);
}

void LD_D_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, L);
}

void LD_D_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, H);
}

void LD_D_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, E);
}

void LD_D_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, D);
}

void LD_D_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  move(cpu_ptr, info, D, C);
}

void LDINDR_a16_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  
  uint16_t addr = readNN(cpu_ptr, cpu_ptr->pc + 1);
  writeN(cpu_ptr, addr, cpu_ptr->a);

  info->cycles = 16;
  info->size = 3;
}

void CP_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, B);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, C);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, D);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, E);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, H);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, L);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = *getRegister(cpu_ptr, A);
  comp(cpu_ptr, d8);
  info->cycles = 4;
  info->size = 1;
}

void CP_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t d8 = readN(cpu_ptr, addr);
  comp(cpu_ptr, d8);
  info->cycles = 8;
  info->size = 1;
}

void CP_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = readN(cpu_ptr, cpu_ptr->pc + 1);
  comp(cpu_ptr, d8);
  info->cycles = 8;
  info->size = 2;
}

void AND_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->c;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
  info->cycles = 4;
  info->size = 1;
}

void AND_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->d;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
}

void AND_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->e;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
}

void AND_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->h;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
}

void AND_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->l;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
}

void AND_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->a;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
}

void AND_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t res = cpu_ptr->a & cpu_ptr->b;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  
  cpu_ptr->a = res;
  info->cycles = 4;
  info->size = 1;
}

void AND_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t d8 = readN(cpu_ptr, addr);
  uint8_t res = cpu_ptr->a & d8;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
  info->cycles = 8;
  info->size = 2;
}

void AND_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t d8 = readN(cpu_ptr, cpu_ptr->pc + 1);
  uint8_t res = cpu_ptr->a & d8;
  setZF(cpu_ptr, res == 0);
  setNF(cpu_ptr, false); 
  setHF(cpu_ptr, true); 
  setCF(cpu_ptr, false); 
  cpu_ptr->a = res;
  info->cycles = 8;
  info->size = 2;
}

void ADD_SP_r8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  add_8_n(cpu_ptr, info, SP, val); 
  setZF(cpu_ptr, false);
  setNF(cpu_ptr, false);
  info->cycles = 16;
  info->size = 2;
}

void RST_20H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x20);
}

void RST_30H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x20);
}

void RST_10H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x10);
}

void RST_00H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  restarts(cpu_ptr, info, 0x00);
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

void ADD_A_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  add_8_n(cpu_ptr, info, A, val);
}

void ADC_A_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  adc_n_reg(cpu_ptr, info, A, val);
}

void SUB_A_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  sub_n(cpu_ptr, info, val);
  info->cycles = 8;
  info->size = 2;
}

void SBC_A_d8(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint8_t val = readN(cpu_ptr, cpu_ptr->pc + 1);
  val += check_flag(cpu_ptr, CF);
  sub_n(cpu_ptr, info, val);
  info->cycles = 8;
  info->size = 2;
}

// Push next instruction onto stack and jump to nn
void CALL_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  callCC(cpu_ptr, info, true);
}

void CALL_NZ_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  callCC(cpu_ptr, info, !check_flag(cpu_ptr, ZF));
}

void CALL_Z_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  callCC(cpu_ptr, info, check_flag(cpu_ptr, ZF));
}

void CALL_NC_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  callCC(cpu_ptr, info, !check_flag(cpu_ptr, CF));
}

void CALL_C_a16(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  callCC(cpu_ptr, info, check_flag(cpu_ptr, CF));
}

// Pop two bytes off of stack & jump to that address
void RET(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, true);
  info->cycles = 16;
}

void RETI(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, true);
  printf("Interrupts not yet enabled!\n");
  info->cycles = 16;
}

void RETNZ(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, !check_flag(cpu_ptr, ZF));
}

void RETZ(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, check_flag(cpu_ptr, ZF));
}

void RETNC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, !check_flag(cpu_ptr, CF));
}

void RETC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  returnCC(cpu_ptr, info, check_flag(cpu_ptr, CF));
}

// Push BC on the stack
void PUSH_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  push_r16(cpu_ptr, info, BC);
} 

void PUSH_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  push_r16(cpu_ptr, info, DE);
} 

void PUSH_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  push_r16(cpu_ptr, info, HL);
} 

void PUSH_AF(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  push_r16(cpu_ptr, info, AF);
} 

void POP_BC(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  pop_r16(cpu_ptr, info, BC);
}  

void POP_DE(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  pop_r16(cpu_ptr, info, DE);
}  

void POP_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  pop_r16(cpu_ptr, info, HL);
}  

void POP_AF(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  pop_r16(cpu_ptr, info, AF);
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

void HALT(void *cpu, Op_info *info) {
  info->cycles = 4;
  info->size = 1;
}

void EI(void *cpu, Op_info *info) {
    printf("Interrupt enabling not implemented!\n");
    info->cycles = 4;
    info->size = 1;
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

// Set bit b in reg
void set_bit(CPU *cpu, Op_info *info, uint8_t b, uint16_t reg) {
  uint8_t *dst = getRegister(cpu, reg);
  uint8_t mask = 0x01;
  mask = mask << b;
  *dst = *dst | mask;
  info->cycles = 8;
  info->size = 1;
}

void swap(CPU *cpu, Op_info *info, uint16_t reg) {
  uint8_t *dst = getRegister(cpu, reg);
  uint8_t upper = *dst & 0xF0;
  *dst = *dst << 4;
  *dst &= upper >> 4;
  setZF(cpu, *dst == 0x00);
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

void RES_6_HL(void *cpu, Op_info *info ) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t mask = 0x01;
  mask = mask << 6;
  val = val | mask;
  writeN(cpu_ptr, addr, val);
  
  info->cycles = 16;
  info->size = 1;
}

void RES_7_HL(void *cpu, Op_info *info ) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t mask = 0x01;
  mask = mask << 7;
  val = val | mask;
  writeN(cpu_ptr, addr, val);
  
  info->cycles = 16;
  info->size = 1;
}



void SWAP_A(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, A);
}

void SWAP_B(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, B);
}

void SWAP_C(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, C);
}

void SWAP_D(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, D);
}

void SWAP_E(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, E);
}

void SWAP_H(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, H);
}

void SWAP_L(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  swap(cpu_ptr, info, L);
}

void SWAP_HL(void *cpu, Op_info *info) {
  CPU *cpu_ptr = (CPU*) cpu;
  uint16_t addr = read_r16(cpu_ptr, HL);
  uint8_t val = readN(cpu_ptr, addr);
  uint8_t upper = val & 0xF0;
  val = val << 4;
  val &= upper >> 4;
  writeN(cpu_ptr, addr, val);
  
  setZF(cpu_ptr, val == 0x00);

  info->cycles = 16;
  info->size = 2;
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
  jumptable[0x3][0x9] = ADD_HL_SP;
  jumptable[0x3][0xA] = LDDEC_A_INDR_HL;
  jumptable[0x3][0xB] = DEC_SP;
  jumptable[0x3][0xC] = INC_A;
  jumptable[0x3][0xD] = DEC_A;
  jumptable[0x3][0xE] = LD_A_d8;
  jumptable[0x3][0xF] = CCF;

  jumptable[0x4][0x0] = LD_B_B;
  jumptable[0x4][0x1] = LD_B_C;
  jumptable[0x4][0x2] = LD_B_D;
  jumptable[0x4][0x3] = LD_B_E;
  jumptable[0x4][0x4] = LD_B_H;
  jumptable[0x4][0x5] = LD_B_L;
  jumptable[0x4][0x6] = LD_B_INDRHL;
  jumptable[0x4][0x7] = LD_B_A;
  jumptable[0x4][0x8] = LD_C_B;
  jumptable[0x4][0x9] = LD_C_C;
  jumptable[0x4][0xA] = LD_C_D;
  jumptable[0x4][0xB] = LD_C_E;
  jumptable[0x4][0xC] = LD_C_H;
  jumptable[0x4][0xD] = LD_C_L;
  jumptable[0x4][0xE] = LD_C_INDRHL;
  jumptable[0x4][0xF] = LD_C_A;


  /* jumptable[0x5][0x0] = LD_D_C; */
  jumptable[0x5][0x1] = LD_D_C;
  jumptable[0x5][0x2] = LD_D_D;
  jumptable[0x5][0x3] = LD_D_E;
  jumptable[0x5][0x4] = LD_D_H;
  jumptable[0x5][0x5] = LD_D_L;
  jumptable[0x5][0x6] = LD_D_HL;
  jumptable[0x5][0x7] = LD_D_A;
  jumptable[0x5][0xA] = LD_E_D;
  jumptable[0x5][0xB] = LD_E_E;
  jumptable[0x5][0xC] = LD_E_H;
  jumptable[0x5][0xD] = LD_E_L;
  jumptable[0x5][0xE] = LD_E_HL;
  jumptable[0x5][0xF] = LD_E_A;

  jumptable[0x6][0x0] = LD_H_B;
  jumptable[0x6][0x1] = LD_H_C;
  jumptable[0x6][0x2] = LD_H_D;
  jumptable[0x6][0x3] = LD_H_E;
  jumptable[0x6][0x4] = LD_H_H;
  jumptable[0x6][0x5] = LD_H_L;
  jumptable[0x6][0x6] = LD_H_INDRHL;
  jumptable[0x6][0x7] = LD_H_A;
  jumptable[0x6][0x8] = LD_L_B;
  jumptable[0x6][0x9] = LD_L_C;
  jumptable[0x6][0xA] = LD_L_D;
  jumptable[0x6][0xB] = LD_L_E;
  jumptable[0x6][0xC] = LD_L_H;
  jumptable[0x6][0xD] = LD_L_L;
  jumptable[0x6][0xE] = LD_L_INDRHL;
  jumptable[0x6][0xF] = LD_L_A;


  jumptable[0x7][0x0] = LDINDR_INDRHL_B;
  jumptable[0x7][0x1] = LDINDR_INDRHL_C;
  jumptable[0x7][0x2] = LDINDR_INDRHL_D;
  jumptable[0x7][0x3] = LDINDR_INDRHL_E;
  jumptable[0x7][0x4] = LDINDR_INDRHL_H;
  jumptable[0x7][0x5] = LDINDR_INDRHL_L;
  jumptable[0x7][0x6] = HALT;
  jumptable[0x7][0x7] = LDINDR_HL_A;
  jumptable[0x7][0x8] = LD_A_B;
  jumptable[0x7][0x9] = LD_A_C;
  jumptable[0x7][0xA] = LD_A_D;
  jumptable[0x7][0xB] = LD_A_E;
  jumptable[0x7][0xC] = LD_A_H;
  jumptable[0x7][0xD] = LD_A_L;
  jumptable[0x7][0xE] = LD_A_INDRHL;
  jumptable[0x7][0xF] = LD_A_A;

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
  jumptable[0x9][0x1] = SUB_C;
  jumptable[0x9][0x2] = SUB_D;
  jumptable[0x9][0x3] = SUB_E;
  jumptable[0x9][0x4] = SUB_H;
  jumptable[0x9][0x5] = SUB_L;
  jumptable[0x9][0x6] = SUB_HL;
  jumptable[0x9][0x7] = SUB_A;
  jumptable[0x9][0x8] = SBC_A_B;
  jumptable[0x9][0x9] = SBC_A_C;
  jumptable[0x9][0xA] = SBC_A_D;
  jumptable[0x9][0xB] = SBC_A_E;
  jumptable[0x9][0xC] = SBC_A_H;
  jumptable[0x9][0xD] = SBC_A_L;
  jumptable[0x9][0xE] = SBC_A_HL;
  jumptable[0x9][0xF] = SBC_A_A;

  jumptable[0xA][0x0] = AND_B;
  jumptable[0xA][0x1] = AND_C;
  jumptable[0xA][0x2] = AND_D;
  jumptable[0xA][0x3] = AND_E;
  jumptable[0xA][0x4] = AND_H;
  jumptable[0xA][0x5] = AND_L;
  jumptable[0xA][0x6] = AND_HL;
  jumptable[0xA][0x7] = AND_A;
  jumptable[0xA][0x8] = XOR_B;
  jumptable[0xA][0x9] = XOR_C;
  jumptable[0xA][0xA] = XOR_D;
  jumptable[0xA][0xB] = XOR_E;
  jumptable[0xA][0xC] = XOR_H;
  jumptable[0xA][0xD] = XOR_L;
  jumptable[0xA][0xE] = XOR_HL;
  jumptable[0xA][0xF] = XOR_A;

  jumptable[0xB][0x0] = OR_B;
  jumptable[0xB][0x1] = OR_C;
  jumptable[0xB][0x2] = OR_D;
  jumptable[0xB][0x3] = OR_E;
  jumptable[0xB][0x4] = OR_H;
  jumptable[0xB][0x5] = OR_L;
  jumptable[0xB][0x6] = OR_HL;
  jumptable[0xB][0x7] = OR_A;
  jumptable[0xB][0x8] = CP_B;
  jumptable[0xB][0x9] = CP_C;
  jumptable[0xB][0xA] = CP_D;
  jumptable[0xB][0xB] = CP_E;
  jumptable[0xB][0xC] = CP_H;
  jumptable[0xB][0xD] = CP_L;
  jumptable[0xB][0xE] = CP_HL;
  jumptable[0xB][0xF] = CP_A;
  
  jumptable[0xC][0x0] = RETNZ;
  jumptable[0xC][0x1] = POP_BC;
  jumptable[0xC][0x2] = JP_NZ_a16;
  jumptable[0xC][0x3] = JP_a16;
  jumptable[0xC][0x4] = CALL_NZ_a16;
  jumptable[0xC][0x5] = PUSH_BC;
  jumptable[0xC][0x6] = ADD_A_d8;
  jumptable[0xC][0x7] = RST_00H;
  jumptable[0xC][0x8] = RETZ;
  jumptable[0xC][0x9] = RET;
  jumptable[0xC][0xA] = JP_Z_a16;
  jumptable[0xC][0xC] = CALL_Z_a16;
  jumptable[0xC][0xD] = CALL_a16;
  jumptable[0xC][0xE] = ADC_A_d8;
  jumptable[0xC][0xF] = RST_08H;

  jumptable[0xD][0x0] = RETNC;
  jumptable[0xD][0x1] = POP_DE;
  jumptable[0xD][0x2] = JP_NC_a16;
  jumptable[0xD][0x4] = CALL_NC_a16;
  jumptable[0xD][0x5] = PUSH_DE;
  jumptable[0xD][0x6] = SUB_d8;
  jumptable[0xD][0x7] = RST_10H;
  jumptable[0xD][0x8] = RETC;
  jumptable[0xD][0x9] = RETI;
  jumptable[0xD][0xA] = JP_C_a16;
  jumptable[0xD][0xC] = CALL_C_a16;
  jumptable[0xD][0xE] = SBC_A_d8;
  jumptable[0xD][0xF] = RST_18H;
  
  jumptable[0xE][0x0] = LDH_a8_A;
  jumptable[0xE][0x1] = POP_HL;
  jumptable[0xE][0x2] = LDINDR_C_A;
  jumptable[0xE][0x5] = PUSH_HL;
  jumptable[0xE][0x6] = AND_d8;
  jumptable[0xE][0x7] = RST_20H;
  jumptable[0xE][0x8] = ADD_SP_r8;
  jumptable[0xE][0x9] = JP_INDRHL;
  jumptable[0xE][0xA] = LDINDR_a16_A;
  jumptable[0xE][0xE] = XOR_d8;
  jumptable[0xE][0xF] = RST_28H;

  jumptable[0xF][0x0] = LDH_A_a8;
  jumptable[0xF][0x1] = POP_AF;
  jumptable[0xF][0x2] = LD_A_INDR_C;
  jumptable[0xF][0x3] = DI;
  jumptable[0xF][0x5] = PUSH_AF;
  jumptable[0xF][0x6] = OR_d8;
  jumptable[0xF][0x7] = RST_30H;
  jumptable[0xF][0x8] = LDHL_SP_N;
  jumptable[0xF][0x9] = LD_SP_HL;
  jumptable[0xF][0xA] = LD_A_INDRNN;
  jumptable[0xF][0xB] = EI;
  jumptable[0xF][0xE] = CP_d8;
  jumptable[0xF][0xF] = RST_38H;
 
  /* CB JUMPTABLE */
  cb_jumptable[0x1][0x1] = RL_C;

  cb_jumptable[0x7][0xC] = BIT_7_H;

  cb_jumptable[0x3][0x7] = SWAP_A;
  cb_jumptable[0x3][0xF] = SRL_A;


  cb_jumptable[0x5][0xF] = BIT_3_A;

  cb_jumptable[0xB][0x6] = RES_6_HL;
  cb_jumptable[0xB][0xE] = RES_7_HL;

  cb_jumptable[0xF][0xE] = SET_7_INDRHL;
}
