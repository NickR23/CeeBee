#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

#define TESTCART "PokemonRed.gb"

CPU cpu;

int setup(void ** state) {
  cpu = initCPU();
  cpu.a = 0x12;
  cpu.f = 0x34;
  cpu.b = 0x87;
  cpu.c = 0x65;
  cpu.d = 0x5F;
  cpu.e = 0xA1;
  cpu.h = 0x7D;
  cpu.l = 0xD1;
  cpu.sp = 0x5766;
  cpu.pc = 0xABCD;
  return 0;
}

int teardown(void ** state) {
  freeCPU(&cpu);
  return 0;
}

void test_initCPU(void ** state) {
  CPU cpu = initCPU();
  assert_true(cpu.a == 0x0000);
  assert_true(cpu.f == 0x0000);
  assert_true(cpu.b == 0x0000);
  assert_true(cpu.c == 0x0000);
  assert_true(cpu.d == 0x0000);
  assert_true(cpu.e == 0x0000);
  assert_true(cpu.h == 0x0000);
  assert_true(cpu.l == 0x0000);
  assert_true(cpu.sp == 0x0000);
  assert_true(cpu.pc == 0x0000);

  freeCPU(&cpu);
}

void test_mmu_load_boot_rom(void ** state) {
  unsigned char *mmu = (unsigned char*) malloc(sizeof(unsigned char) * (0x10000));
  
  mmu_load_boot_rom(mmu);
  unsigned char expected = 0x31; 
  
  assert_true(mmu[0] == expected);

}

void test_getRegister(void ** state) {
  assert_true(*getRegister(&cpu,A) == 0x12);
  assert_true(*getRegister(&cpu,F) == 0x34);
  assert_true(*getRegister(&cpu,B) == 0x87);
  assert_true(*getRegister(&cpu,C) == 0x65);
  assert_true(*getRegister(&cpu,D) == 0x5F);
  assert_true(*getRegister(&cpu,E) == 0xA1);
  assert_true(*getRegister(&cpu,H) == 0x7D);
  assert_true(*getRegister(&cpu,L) == 0xD1);
  assert_null(getRegister(&cpu,9));
}

void test_read_r16(void ** state) {
  cpu.a = 0xAA;
  assert_true(read_r16(&cpu,AF) == 0xAA34);
  assert_true(read_r16(&cpu,BC) == 0x8765);
  assert_true(read_r16(&cpu,DE) == 0x5FA1);
  assert_true(read_r16(&cpu,HL) == 0x7DD1);
  assert_true(read_r16(&cpu,SP) == 0x5766);
  assert_true(read_r16(&cpu,PC) == 0xABCD);
  assert_true(read_r16(&cpu,99) == 0x0000);
}

void test_write_r16(void ** state) {
  write_r16(&cpu,AF,0x9088);
  write_r16(&cpu,BC,0xFE89);
  write_r16(&cpu,DE,0xAAAA);
  write_r16(&cpu,HL,0x8999);
  write_r16(&cpu,SP,0xBBBB);
  write_r16(&cpu,PC,0xCCCC);
  
  assert_true(read_r16(&cpu,AF) == 0x9088);
  assert_true(read_r16(&cpu,BC) == 0xFE89);
  assert_true(read_r16(&cpu,DE) == 0xAAAA);
  assert_true(read_r16(&cpu,HL) == 0x8999);
  assert_true(read_r16(&cpu,SP) == 0xBBBB);
  assert_true(read_r16(&cpu,PC) == 0xCCCC);
  assert_true(read_r16(&cpu,99) == 0x0000);
}

void test_getNN(void ** state) {
  unsigned int nn = getNN(&cpu,0); 
  assert_true(nn == 0xfe31);
}

void test_getByte(void ** state) {
  unsigned int n = getByte(&cpu,0); 
  assert_true(n == 0x31);
}

void test_runCycle(void ** state) {
  cpu = initCPU();
  cpu.pc = 0x0000;
  Op_info info = run_cycle(&cpu);
   
  assert_true(cpu.pc == 0x03);
  assert_true(info.size == 3);
  assert_true(info.cycles == 12);

  freeCPU(&cpu);
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test(test_initCPU),
    cmocka_unit_test(test_runCycle),
    cmocka_unit_test(test_mmu_load_boot_rom),
    cmocka_unit_test_setup_teardown(test_getRegister,setup,teardown),
    cmocka_unit_test_setup_teardown(test_read_r16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_write_r16,setup,teardown),
    cmocka_unit_test_setup_teardown(test_getNN,setup,teardown),
    cmocka_unit_test_setup_teardown(test_getByte,setup,teardown),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
