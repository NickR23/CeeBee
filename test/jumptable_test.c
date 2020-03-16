#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

CPU cpu;

int setup(void ** state) {
  cpu = initCPU();
  return 0;
}

int teardown(void ** state) {
  freeCPU(&cpu);
  return 0;
}

void test_setCF(void ** state) {
  setCF(&cpu, true);
  unsigned char *f = getRegister(&cpu,F);
  assert_true(*f == 0x10);
  setCF(&cpu, false);
  assert_true(*f == 0x00);
}

void test_setHF(void ** state) {
  setHF(&cpu, true);
  unsigned char *f = getRegister(&cpu,F);
  assert_true(*f == 0x20);
  setHF(&cpu, false);
  assert_true(*f == 0x00);
}

void test_setNF(void ** state) {
  setNF(&cpu, true);
  unsigned char *f = getRegister(&cpu,F);
  assert_true(*f == 0x40);
  setNF(&cpu, false);
  assert_true(*f == 0x00);
}

void test_setZF(void ** state) {
  setZF(&cpu, true);
  unsigned char *f = getRegister(&cpu,F);
  assert_true(*f == 0x80);
  setZF(&cpu, false);
  assert_true(*f == 0x00);
}

void test_NOP(void ** state) {
  Op_info info;
  NOP(&cpu, &info);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_LD_SP_d16(void ** state) {
  Op_info info;
  LD_SP_d16(&cpu, &info);
  unsigned int expected = 0xfffe;
  assert_true(cpu.sp == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}
  
void test_LD_BC_d16(void ** state) {
  Op_info info;
  LD_BC_d16(&cpu, &info);
  unsigned short expected = 0xfffe;
  unsigned short bc_val = read_r16(&cpu, BC);
  printf("bc: 0%04x\n",bc_val);
  assert_true(bc_val == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}

void test_LDINDR_BC_A(void ** state) {
  Op_info info;
  cpu.mmu[0xA78D] = 0x98;
  write_r16(&cpu,BC,0xA78D);
  write_r16(&cpu,AF,0xFFEE);
  LDINDR_BC_A(&cpu, &info);
  assert_true(cpu.mmu[0xA78D] == 0xEE);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_BC(void ** state) {
  Op_info info;
  write_r16(&cpu,BC,0xA78D);
  INC_BC(&cpu, &info);
  unsigned short bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == 0xA78E);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_B(void ** state) {
  Op_info info;
  write_r16(&cpu,BC,0x8D00);
  INC_B(&cpu, &info);
  assert_true(cpu.b == 0x8E);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu,BC,0xFF00);
  INC_B(&cpu, &info);
  assert_true(cpu.b == 0x00);
  assert_true(*f == 0xA0);
}

void test_DEC_B(void ** state) {
  Op_info info;
  write_r16(&cpu,BC,0x8D00);
  DEC_B(&cpu, &info);
  assert_true(cpu.b == 0x8C);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu,BC,0x0000);
  DEC_B(&cpu, &info);
  assert_true(cpu.b == 0xFF);
  assert_true(*f == 0x60);
}

void test_LD_B_d8(void ** state) {
  Op_info info;
  unsigned char *b = getRegister(&cpu,B);
  LD_B_d8(&cpu, &info);
  unsigned int expected = 0xfe;
  assert_true(*b == expected);
  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_RLCA(void ** state) {
  Op_info info;
  unsigned char *a = getRegister(&cpu,A);
  unsigned char *f = getRegister(&cpu,F);
  *a = 0xF0;
  RLCA(&cpu, &info);
  unsigned int expected = 0xe1;
  assert_true(*a == expected);
  assert_true(*f == 0x10);
  
  *a = 0x61;
  expected = 0xC2;
  RLCA(&cpu, &info);
  assert_true(*a == expected);
  assert_true(*f == 0x00);
  
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_a16_SP(void ** state) {
  Op_info info;
  cpu.sp = 0x8FE4;
  cpu.pc = 0x0000;
  LD_a16_SP(&cpu, &info);
  unsigned short addr = getNN(&cpu, cpu.pc + 1);
  unsigned short res = *((short*)(cpu.mmu + addr));
  assert_true(res == cpu.sp);
  assert_true(info.cycles == 20);
  assert_true(info.size == 3);
}

void test_ADD_HL_BC(void ** state) {
  Op_info info;
  write_r16(&cpu, BC, 0xF6DD);
  write_r16(&cpu, HL, 0x4CDE);
  unsigned char *f = getRegister(&cpu,F);
  *f = 0x00;
  
  ADD_HL_BC(&cpu, &info);
  unsigned short expected = 0x43BB;
  unsigned short hl_val = read_r16(&cpu, HL);
  assert_true(hl_val == expected);
  assert_true(*f == 0x30);
 
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_LD_A_INDIR_BC(void ** state) {
  Op_info info;
  
  unsigned char expected = getByte(&cpu, 8);
  write_r16(&cpu, BC, 0x0008);
  
  LD_A_INDIR_BC(&cpu, &info);
  unsigned char *a = getRegister(&cpu, A);
  assert_true(*a == expected);
  
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_DEC_BC(void ** state) {
  Op_info info;
  write_r16(&cpu, BC, 0x00C4);
  unsigned short expected = 0x00C3;
  
  DEC_BC(&cpu, &info);
  unsigned short bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == expected);
 
  /* Test underflow */
  write_r16(&cpu, BC, 0x0000);
  expected = 0xFFFF;
  DEC_BC(&cpu, &info);
  bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == expected);
  
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_C(void ** state) {
  Op_info info;
  write_r16(&cpu, BC, 0x568D);
  INC_C(&cpu, &info);
  unsigned short bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == 0x578D);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu, BC, 0xFF00);
  INC_C(&cpu, &info);
  bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == 0x0000);
  assert_true(*f == 0xA0);
}

void test_DEC_C(void ** state) {
  Op_info info;
  write_r16(&cpu, BC, 0x328D);
  DEC_C(&cpu, &info);
  unsigned short bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == 0x318D);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu, BC, 0x0000);
  DEC_C(&cpu, &info);
  bc_val = read_r16(&cpu, BC);
  assert_true(bc_val == 0xFF00);
  assert_true(*f == 0x60);
}

void test_LD_C_d8(void ** state) {
  Op_info info;
  unsigned char *c = getRegister(&cpu,C);
  LD_C_d8(&cpu, &info);
  unsigned int expected = 0xfe;
  assert_true(*c == expected);
  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_RRCA(void ** state) {
  Op_info info;
  unsigned char *a = getRegister(&cpu,A);
  unsigned char *f = getRegister(&cpu,F);
  *a = 0xF0;
  RRCA(&cpu, &info);
  unsigned int expected = 0x78;
  assert_true(*a == expected);
  assert_true(*f == 0x00);
  
  *a = 0x01;
  expected = 0x80;
  RRCA(&cpu, &info);
  assert_true(*a == expected);
  assert_true(*f == 0x10);
  
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_LD_DE_d16(void ** state) {
  Op_info info;
  LD_DE_d16(&cpu, &info);
  unsigned int expected = 0xfffe;
  unsigned short de_val = read_r16(&cpu, DE);
  assert_true(de_val == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}

void test_LD_HL_d16(void ** state) {
  Op_info info;
  LD_HL_d16(&cpu, &info);
  unsigned int expected = 0xfffe;
  unsigned short hl_val = read_r16(&cpu, HL);
  assert_true(hl_val == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}

void test_LDINDR_DE_A(void ** state) {
  Op_info info;
  LDINDR_DE_A(&cpu, &info);
  unsigned short de_val = read_r16(&cpu, DE);
  unsigned short af_val = read_r16(&cpu, AF);
  assert_true(cpu.mmu[de_val] == (af_val & 0xFF));
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_LDINC_HL_A(void ** state) {
  Op_info info;
  unsigned char pre_hl = read_r16(&cpu, HL);
  LDINC_HL_A(&cpu, &info);
  unsigned short af_val = read_r16(&cpu, AF);
  assert_true(cpu.mmu[pre_hl] == (af_val & 0xFF));
  assert_true(read_r16(&cpu,HL) == pre_hl + 1); 
  
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_LDDEC_HL_A(void ** state) {
  Op_info info;
  write_r16(&cpu, HL, 0xCE);
  write_r16(&cpu, AF, 0xBC79);
  unsigned short pre_hl = read_r16(&cpu, HL);
  LDDEC_HL_A(&cpu, &info);
  unsigned char *a = getRegister(&cpu,A);
  assert_true(cpu.mmu[pre_hl] == *a);
  assert_true(read_r16(&cpu,HL) == pre_hl - 1); 
  
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_DE(void ** state) {
  Op_info info;
  write_r16(&cpu, DE, 0xA78D);
  INC_DE(&cpu, &info);
  unsigned char de_val = read_r16(&cpu, HL);
  assert_true(de_val == 0xA78E);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_HL(void ** state) {
  Op_info info;
  write_r16(&cpu, HL, 0xA78D);
  INC_HL(&cpu, &info);
  unsigned char hl_val = read_r16(&cpu, HL);
  assert_true(hl_val == 0xA78E);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_SP(void ** state) {
  Op_info info;
  cpu.sp = 0xA78D;
  INC_SP(&cpu, &info);
  assert_true(cpu.sp == 0xA78E);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_D(void ** state) {
  Op_info info;
  cpu.d = 0x8D;
  INC_D(&cpu, &info);
  assert_true(cpu.d == 0x8E);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  cpu.d = 0xFF;
  INC_D(&cpu, &info);
  assert_true(cpu.d == 0x00);
  assert_true(*f == 0xA0);
}

void test_INC_H(void ** state) {
  Op_info info;
  cpu.h = 0x8D;
  INC_H(&cpu, &info);
  assert_true(cpu.h == 0x8E);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  cpu.h = 0xFF;
  INC_H(&cpu, &info);
  assert_true(cpu.h == 0x0000);
  assert_true(*f == 0xA0);
}

void test_INCINDR_HL(void ** state) {
  Op_info info;
  write_r16(&cpu,DE,0xCC8D);
  cpu.mmu[0xCC8D] = 0xFB;
  INCINDR_HL(&cpu, &info);
  assert_true(cpu.mmu[0xCC8D] == 0xFC);
  assert_true(info.cycles == 12);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu,DE,0x00FF);
  cpu.mmu[0x00FF] = 0xFF;
  INCINDR_HL(&cpu, &info);
  assert_true(cpu.mmu[0x00FF] == 0x00);
  assert_true(*f == 0xA0);
}

void test_DEC_D(void ** state) {
  Op_info info;
  write_r16(&cpu,DE,0x008D);
  DEC_D(&cpu, &info);
  unsigned char de_val = read_r16(&cpu,DE);
  assert_true(de_val == 0x008C);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu,DE,0x008D);
  DEC_D(&cpu, &info);
  de_val = read_r16(&cpu,DE);
  assert_true(de_val == 0x00FF);
  assert_true(*f == 0x60);
}

void test_DEC_H(void ** state) {
  Op_info info;
  write_r16(&cpu, HL, 0x008D);
  DEC_H(&cpu, &info);
  unsigned char hl_val = read_r16(&cpu, HL);
  assert_true(hl_val == 0x008C);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu, HL, 0x0000);
  DEC_H(&cpu, &info);
  hl_val = read_r16(&cpu, HL);
  assert_true(hl_val == 0x00FF);
  assert_true(*f == 0x60);
}

void test_DECINDR_HL(void ** state) {
  Op_info info;
  write_r16(&cpu,HL,0xCC8D);
  cpu.mmu[0xCC8D] = 0xFB;
  DECINDR_HL(&cpu, &info);
  assert_true(cpu.mmu[0xCC8D] == 0xFA);
  assert_true(info.cycles == 12);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  write_r16(&cpu,HL,0x00FF);
  cpu.mmu[0x00FF] = 0x00;
  DECINDR_HL(&cpu, &info);
  assert_true(cpu.mmu[0x00FF] == 0xFF);
  assert_true(*f == 0x60);
}

void test_LD_D_d8(void ** state) {
  Op_info info;
  unsigned char *d = getRegister(&cpu,D);
  LD_D_d8(&cpu, &info);
  unsigned int expected = 0xfe;
  assert_true(*d == expected);
  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_LD_H_d8(void ** state) {
  Op_info info;
  unsigned char *h = getRegister(&cpu,H);
  LD_H_d8(&cpu, &info);
  unsigned int expected = 0xfe;
  assert_true(*h == expected);
  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_LDINDR_HL_d8(void ** state) {
  Op_info info;
  write_r16(&cpu,HL,0xCC80);
  LDINDR_HL_d8(&cpu, &info);
  unsigned int expected = 0xfe;
  assert_true(cpu.mmu[0xCC80] == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 2);
}

void test_RLA(void ** state) {
  Op_info info;
  write_r16(&cpu,AF,0x0080);
  RLA(&cpu, &info);
  unsigned char *a = getRegister(&cpu, A);
  unsigned char *f = getRegister(&cpu, F);

  unsigned char expected = 0x00;
  assert_true(*a == expected); 
  assert_true(*f == 0x10);
  
  write_r16(&cpu,AF,0x107C);
  expected = 0xF9;
  RLA(&cpu,&info);
  assert_true(*a == expected); 
  assert_true(*f == 0x00);

  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_RRA(void ** state) {
  Op_info info;
  write_r16(&cpu,AF,0x0080);
  RRA(&cpu, &info);
  unsigned char *a = getRegister(&cpu, A);
  unsigned char *f = getRegister(&cpu, F);

  unsigned char expected = 0x40;
  assert_true(*a == expected); 
  assert_true(*f == 0x00);
  
  write_r16(&cpu, AF, 0x1071);
  expected = 0xB8;
  RRA(&cpu,&info);
  assert_true(*a == expected); 
  assert_true(*f == 0x10);

  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_CPL(void ** state) { 
  Op_info info;
  
  write_r16(&cpu, AF, 0x00FF);
  CPL(&cpu,&info);
  unsigned char *a = getRegister(&cpu, A);
  unsigned char expected = 0x00;
  assert_true(*a == expected);

  write_r16(&cpu, AF, 0x00C9);
  expected = 0x36;
  assert_true(*a == expected);

  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_CCF(void ** state) { 
  Op_info info;

  write_r16(&cpu, AF, 0x90FF);
  CCF(&cpu,&info);
  unsigned char *f = getRegister(&cpu, F);
  assert_true(*f == 0x80);

  write_r16(&cpu, AF, 0xE0FF);
  CCF(&cpu,&info);
  assert_true(*f == 0x90);

  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
} 

void test_SCF(void ** state) {
  Op_info info;
  write_r16(&cpu,AF,0xF000);
  SCF(&cpu, &info);
  unsigned char expected = 0x90;
  unsigned char *f = getRegister(&cpu,F);
  assert_true(*f == expected);

  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_JR_r8(void ** state) {
  Op_info info;
  cpu.pc = 0x0004;
 
  JR_r8(&cpu,&info);

  assert_true(cpu.pc == 0x0003);

  assert_true(info.cycles == 12);
  assert_true(info.size == 2);
}

void test_JR_Z_r8(void ** state) {
  Op_info info;
  cpu.pc = 0x0004;
  write_r16(&cpu,AF,0x8000);
 
  JR_Z_r8(&cpu,&info);

  assert_true(cpu.pc == 0x0003);

  assert_true(info.cycles == 12);
  assert_true(info.size == 2);
  
  write_r16(&cpu,AF,0x0000);
  JR_Z_r8(&cpu,&info);
  assert_true(cpu.pc == 0x0003);

  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_JR_C_r8(void ** state) {
  Op_info info;
  cpu.pc = 0x0004;
  write_r16(&cpu,AF,0x1000);
 
  JR_C_r8(&cpu,&info);

  assert_true(cpu.pc == 0x0003);

  assert_true(info.cycles == 12);
  assert_true(info.size == 2);
  
  write_r16(&cpu,AF,0x0000);
  JR_C_r8(&cpu,&info);
  assert_true(cpu.pc == 0x0003);

  assert_true(info.cycles == 8);
  assert_true(info.size == 2);
}

void test_XOR_A(void ** state) {
  Op_info info;
  unsigned char *a = getRegister(&cpu, A);
  unsigned char *f = getRegister(&cpu, F);
  
  *a = 0x88;
  *f = 0xF0;
  XOR_A(&cpu,&info);
  printf("a: 0x%02x\n",*a);
  assert_true(*a == 0x00);
  assert_true(*f == 0x80);
  
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test_setup_teardown(test_setCF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setHF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setNF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setZF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_NOP,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_BC_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDINDR_BC_A,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_BC,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_B,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_B,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_B_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_RLCA,setup,teardown),
    cmocka_unit_test_setup_teardown(test_a16_SP,setup,teardown),
    cmocka_unit_test_setup_teardown(test_ADD_HL_BC,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_A_INDIR_BC,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_BC,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_C,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_C,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_C_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_RRCA,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_DE_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_HL_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_SP_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDINDR_DE_A,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDINC_HL_A,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDDEC_HL_A,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_DE,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_HL,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_SP,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_D,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_H,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INCINDR_HL,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_D,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_H,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DECINDR_HL,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_D_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_H_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDINDR_HL_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_RLA,setup,teardown),
    cmocka_unit_test_setup_teardown(test_RRA,setup,teardown),
    cmocka_unit_test_setup_teardown(test_SCF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_JR_r8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_JR_Z_r8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_JR_C_r8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_CCF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_XOR_A,setup,teardown),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
