#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include <stdio.h>
#include "ceebee/mmu.h"
#include "ceebee/cpu.h"

#define TESTCART "Tetris.gb"

CPU cpu;

int setup(void ** state) {
  cpu = initCPU();
  unsigned int cartSize;
  loadCart(&cpu, TESTCART, &cartSize);
  return 0;
}

int teardown(void ** state) {
  freeCPU(&cpu);
  return 0;
}

void test_mmu_load_boot_rom(void ** state) {
  int16_t val = cpu.mmu->BIOS[0];
  assert_true(val == 0x31);
}

void test_loadCart(void ** state) {
  cpu = initCPU();
  unsigned int cartSize;
  loadCart(&cpu, TESTCART, &cartSize);
  assert_true(cartSize == 32768);
  assert_true(cpu.mmu->ram[0] == 0xC3);
  assert_true(cpu.mmu->ram[0x120] == 0xdd);
  freeCPU(&cpu);
} 
  

void test_readNN(void ** state) {
  assert_true(*cpu.mmu->finishedBIOS == 0);
  uint16_t val = readNN(&cpu, 0x0152); 
  assert_true(val == 0xCD02);
  val = readNN(&cpu, 0x00fe); 
  assert_true(val == 0x50e0);
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test_setup_teardown(test_mmu_load_boot_rom,setup,teardown),
    cmocka_unit_test_setup_teardown(test_readNN,setup,teardown),
    cmocka_unit_test(test_loadCart),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
