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

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test (test_initCPU),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
