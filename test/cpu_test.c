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
  assert_true(*getRegister16(&cpu,A) == 0x1234);
  assert_true(*getRegister16(&cpu,B) == 0x8765);
  assert_true(*getRegister16(&cpu,D) == 0x5FA1);
  assert_true(*getRegister16(&cpu,H) == 0x7DD1);
  assert_true(*getRegister16(&cpu,SP) == 0x5766);
  assert_true(*getRegister16(&cpu,PC) == 0xABCD);
  assert_null(getRegister16(&cpu,99));
}

void test_getRegister(void ** state) {
  CPU cpu = initCPU();
  
  cpu.af = 0x1234;
  cpu.bc = 0x8765;
  cpu.de = 0x5FA1;
  cpu.hl = 0x7DD1;
  cpu.sp = 0x5766;
  cpu.pc = 0xABCD;
  
  assert_true(*getRegister(&cpu,A) == 0x34);
  assert_true(*getRegister(&cpu,F) == 0x12);
  assert_true(*getRegister(&cpu,B) == 0x65);
  assert_true(*getRegister(&cpu,C) == 0x87);
  assert_true(*getRegister(&cpu,D) == 0xA1);
  assert_true(*getRegister(&cpu,E) == 0x5F);
  assert_true(*getRegister(&cpu,H) == 0xD1);
  assert_true(*getRegister(&cpu,L) == 0x7D);
  assert_null(getRegister(&cpu,9));
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test(test_initCPU),
    cmocka_unit_test(test_getRegister16),
    cmocka_unit_test(test_getRegister),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
