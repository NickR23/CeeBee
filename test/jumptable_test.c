#include <stdarg.h>
#include <stddef.h>
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

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test_setup_teardown(test_setCF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setHF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setNF,setup,teardown),
    cmocka_unit_test_setup_teardown(test_setZF,setup,teardown),
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
