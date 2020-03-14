#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

void test_fail(void ** state) {
  assert_true(false);
}

void test_setCF(void ** state) {
  CPU cpu = initCPU();
  setCF(&cpu, true);
  
  unsigned char *f = getRegister(&cpu,F);
  
  assert_true(*f == 0x10);
  
}
