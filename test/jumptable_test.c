#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

CPU cpu;
unsigned char *cart;

int setup(void ** state) {
  unsigned int cartsize;
  cart = loadCart("boot.gb", &cartsize); 
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
  NOP(cart, &cpu, &info);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_LD_SP_d16(void ** state) {
  Op_info info;
  LD_SP_d16(cart, &cpu, &info);
  unsigned int expected = 0xfffe;
  assert_true(cpu.sp == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}
  
void test_LD_BC_d16(void ** state) {
  Op_info info;
  LD_BC_d16(cart, &cpu, &info);
  unsigned int expected = 0xfffe;
  assert_true(cpu.bc == expected);
  assert_true(info.cycles == 12);
  assert_true(info.size == 3);
}

void test_LDINDR_BC_A(void ** state) {
  Op_info info;
  cpu.ram[0xA78D] = 0x98;
  cpu.bc = 0xA78D;
  cpu.af = 0xFFEE;
  LDINDR_BC_A(cart, &cpu, &info);
  assert_true(cpu.ram[cpu.bc] == 0xEE);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_BC(void ** state) {
  Op_info info;
  cpu.bc = 0xA78D;
  INC_BC(cart, &cpu, &info);
  assert_true(cpu.bc == 0xA78E);
  assert_true(info.cycles == 8);
  assert_true(info.size == 1);
}

void test_INC_B(void ** state) {
  Op_info info;
  cpu.bc = 0x008D;
  INC_B(cart, &cpu, &info);
  assert_true(cpu.bc == 0x008E);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to overflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  cpu.bc = 0x00FF;
  INC_B(cart, &cpu, &info);
  assert_true(cpu.bc == 0x0000);
  assert_true(*f == 0xA0);
}

void test_DEC_B(void ** state) {
  Op_info info;
  cpu.bc = 0x008D;
  DEC_B(cart, &cpu, &info);
  assert_true(cpu.bc == 0x008C);
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
  
  // Try to underflow
  unsigned char *f = getRegister(&cpu,F);
  *f = 0xE0;
  cpu.bc = 0x0000;
  DEC_B(cart, &cpu, &info);
  assert_true(cpu.bc == 0x00FF);
  assert_true(*f == 0x60);
}

void test_LD_B_d8(void ** state) {
  Op_info info;
  unsigned char *b = getRegister(&cpu,B);
  LD_B_d8(cart, &cpu, &info);
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
  RLCA(cart, &cpu, &info);
  unsigned int expected = 0xe1;
  assert_true(*a == expected);
  assert_true(*f == 0x10);
  
  *a = 0x61;
  expected = 0xC2;
  RLCA(cart, &cpu, &info);
  assert_true(*a == expected);
  assert_true(*f == 0x00);
  
  assert_true(info.cycles == 4);
  assert_true(info.size == 1);
}

void test_a16_SP(void ** state) {
  Op_info info;
  cpu.sp = 0x8FE4;
  cpu.pc = 0x0000;
  LD_a16_SP(cart, &cpu, &info);
  unsigned short addr = getNN(cart, cpu.pc + 1);
  unsigned short res = *((short*)(cpu.ram + addr));
  assert_true(res == cpu.sp);
  assert_true(info.cycles == 20);
  assert_true(info.size == 3);
}

void test_ADD_HL_BC(void ** state) {
  Op_info info;
  cpu.hl = 0x4CDE;
  cpu.bc = 0xF6DD; 
  unsigned char *f = getRegister(&cpu,F);
  *f = 0x00;
  
  ADD_HL_BC(cart, &cpu, &info);
  unsigned short expected = 0x43BB;
  assert_true(cpu.hl == expected);
  assert_true(*f == 0x30);
 
  assert_true(info.cycles == 8);
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
    cmocka_unit_test_setup_teardown(test_LD_SP_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_BC_d16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LDINDR_BC_A,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_BC,setup,teardown),
    cmocka_unit_test_setup_teardown(test_INC_B,setup,teardown),
    cmocka_unit_test_setup_teardown(test_DEC_B,setup,teardown),
    cmocka_unit_test_setup_teardown(test_LD_B_d8,setup,teardown),
    cmocka_unit_test_setup_teardown(test_RLCA,setup,teardown),
    cmocka_unit_test_setup_teardown(test_a16_SP,setup,teardown),
    cmocka_unit_test_setup_teardown(test_ADD_HL_BC,setup,teardown),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
