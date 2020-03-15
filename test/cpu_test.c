#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

void test_initCPU(void ** state) {
  CPU cpu = initCPU();
  assert_true(cpu.af == 0x0000);
  assert_true(cpu.bc == 0x0000);
  assert_true(cpu.de == 0x0000);
  assert_true(cpu.hl == 0x0000);
  assert_true(cpu.sp == 0x0000);
  assert_true(cpu.pc == 0x0000);
}

void test_getRegister16(void ** state) {
  CPU cpu = initCPU();
  
  cpu.af = 0x1234;
  cpu.bc = 0x8765;
  cpu.de = 0x5FA1;
  cpu.hl = 0x7DD1;
  cpu.sp = 0x5766;
  cpu.pc = 0xABCD;
  assert_true(cpu.af == 0x1234);
  assert_true(cpu.bc == 0x8765);
  assert_true(cpu.de == 0x5FA1);
  assert_true(cpu.hl == 0x7DD1);
  assert_true(cpu.sp == 0x5766);
  assert_true(cpu.pc == 0xABCD);
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test(test_initCPU),
    cmocka_unit_test(test_getRegister16),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
